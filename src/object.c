/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/object.c,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.7 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "constants.h"
#include "object.h"
#include "player.h"
#include "message.h"
#include "bonus.h"
#include "mines.h"
#include "map.h"
#include "fix.h"

static void move_trolley(struct trolley *p);
static int mygets(FILE *pd);

static char inbuf[1024];
static struct trolley *spottrol;
struct trolley *firsttrol;
struct object *obj_freepool;
int obj_free;
int obj_used;
struct object *obj_first;

struct object *alloc_object()
{
    struct object *p;
    p=obj_freepool;
    if (p) {
	obj_freepool=p->next;
	obj_free--;
    } else {
	p=(struct object *) calloc(1,sizeof(struct object));
    } 
    obj_used++;
    return p;
}

extern void move_objects()
{
    struct object *o;
    struct object *n;
    struct player *p;
    double friction;
    double dx,dy;
    int rng,det,det2;
    if (!obj_first) return;
    for (o=obj_first,n=o->next;o;o=n,o?n=o->next:0) {
	if (o->flags&OBJ_F_ARMING)
	    if (!(--o->count)) {
		o->flags|=OBJ_F_ARM;
		o->flags&=(~OBJ_F_ARMING);
	    }
	if (o->flags&OBJ_F_TRIG) {
	    switch (o->type) {
	    case OBJ_MINE_TRIG:
		o->flags|=OBJ_F_EXPLODE;
		break;
	    case OBJ_MINE_TIME:
		if (!(--o->mode))
		    o->flags|=OBJ_F_EXPLODE;
		break;
	    case OBJ_MINE_PROX:
		det=o->mode*2;
		det2=det-100;
		if (det2<0) det2=0;
		det*=det;
		det2=det2*det2;
		for (p=playone;p;p=p->next)
		    if (p->body.l==o->l) {
			dx=(p->body.x-o->x);
			dy=(p->body.y-o->y);
			rng=(int) (dx*dx)+(dy*dy);
			if ((rng<=det)&&(rng>det2))
			    o->flags|=OBJ_F_EXPLODE;
		    }
		break;
	    case OBJ_MINE_VELY:
		for (p=playone;p;p=p->next)
		    if (p->body.l==o->l) {
			dx=(p->body.x-o->x);
			dy=(p->body.y-o->y);
			rng=(int) (dx*dx)+(dy*dy);
			if (rng<=10000) {
			    dx=(double) o->mode/10;
			    dy=p->body.xv*p->body.xv+p->body.yv*p->body.yv;
			    if ((dy>(dx*dx))&&(dy<((dx+5)*(dx+5))))
				o->flags|=OBJ_F_EXPLODE;
			}
		    }
		
		break;
	    case OBJ_BONUS:
		for (p=playone;p;p=p->next)
		    if (p->body.l==o->l) {
			dx=(p->body.x-o->x);
			dy=(p->body.y-o->y);
			rng=(int) (dx*dx)+(dy*dy);
			if (rng<=400) {
			    get_bonus(p);
			    o->flags|=OBJ_F_EXPLODE;
			}
		    }
	    default:
		break;
	    }
	}
	if (!(o->flags&OBJ_F_EXPLODE)) {
	    friction=1-(FRICTION/1000);
	    if (!(rd(o->l,(int) (o->x/128),(int) (o->y/128))&MAP_FRICT)) {
		o->xv*=friction;
		o->yv*=friction;
	    }
	    o->x+=o->xv;
	    o->y+=o->yv;
	    if (is_hole(o->l,o->x,o->y)) {
		o->l++;
		if (o->l>=map.depth) {
		    if (o->slot) {
			o->has.owner->slots[o->slot-1]=OBJ_EMPTY;
			o->has.owner->size[o->slot-1]=0;
			o->has.owner->mode[o->slot-1]=0;
			o->has.owner->slotobj[o->slot-1]=0;
			o->slot=0;
		    }
		    o->flags|=OBJ_F_EXPLODE;
		}
	    }
	}
    }
    rng=1;
    while(rng==1) {
	rng=0;
	for (o=obj_first;o;o?o=o->next:0) 
	    if (o->flags&OBJ_F_EXPLODE) {
		explode_mine(o);
		o=0;rng=1;
	    } else {
		if (o->flags&OBJ_F_CHAIN) o->flags|=OBJ_F_EXPLODE;
	    }
    }
}

extern void init_all_trolleys() {
    int n;
    for (n=0;n<12;n++)
	create_trolley(0);
}

extern void create_trolley(struct trolley *tr) {
    struct trolley *t;
    if (firstcheck==0) return;
    if (tr) {
	t=tr;
    } else {
	t=(struct trolley *) calloc(1,sizeof(struct trolley));
    }
    t->body.is.trolley=t;
    t->body.type=BODY_TROLLEY;
    t->body.radius=20;
    t->cp=pick_check(0);
    t->body.l=t->cp->l;
    t->body.x=t->cp->x*128+32+random()%64;
    t->body.y=t->cp->y*128+32+random()%64;
    t->cp=pick_check(t->cp);
    t->body.mass=200;
    t->body.xv=0;t->body.yv=0;t->body.xf=0;t->body.yf=0;
    if (!tr) {
	t->next=firsttrol;
	firsttrol=t;
    }
    add_body(&t->body);
}


static void move_trolley(struct trolley *p) {
    int xc,yc;
    double oxv,oyv;
    double friction;
    int l;
    struct player *o;
    struct player *n;
    char str[256];
    p->ang=(p->ang+17)%360;
    if (p->holder)
	for (o=p->holder,n=o->nexthold;o;o=n,n=n?n->nexthold:0) 
	    if ((!o->body.on)||(o->body.l!=p->body.l)) {
		if (o->lasthold) {
		    o->lasthold->nexthold=o->nexthold;
		} else {
		    p->holder=o->nexthold;
		}
		if (o->nexthold) o->nexthold->lasthold=o->lasthold;
		o->lasthold=0;
		o->nexthold=0;
		o->holding=0;
	    }
    xc=((int)p->body.x)/128;
    yc=((int)p->body.y)/128;
    if (rd(p->body.l,xc,yc)&MAP_FRICT)
	switch(rd2(p->body.l,xc,yc)) {
	case '8':p->body.yf-=PUSH_FORCE;break;
	case '2':p->body.yf+=PUSH_FORCE;break;
	case '4':p->body.xf-=PUSH_FORCE;break;
	case '6':p->body.xf+=PUSH_FORCE;break;
	case '7':p->body.xf-=PUSH_FORCES;p->body.yf-=PUSH_FORCES;break;
	case '1':p->body.xf-=PUSH_FORCES;p->body.yf+=PUSH_FORCES;break;
	case '3':p->body.xf+=PUSH_FORCES;p->body.yf+=PUSH_FORCES;break;
	case '9':p->body.xf+=PUSH_FORCES;p->body.yf-=PUSH_FORCES;break;
	case '5':
	    if ((((int)p->body.x)&127)<50) p->body.xf-=PUSH_FORCES;
	    if ((((int)p->body.x)&127)>78) p->body.xf+=PUSH_FORCES;
	    if ((((int)p->body.y)&127)<50) p->body.yf-=PUSH_FORCES;
	    if ((((int)p->body.y)&127)>78) p->body.yf+=PUSH_FORCES;
	    break;
	case '0':
	    if ((((int)p->body.x)&127)>78) p->body.xf-=PUSH_FORCES;
	    if ((((int)p->body.x)&127)<50) p->body.xf+=PUSH_FORCES;
	    if ((((int)p->body.y)&127)>78) p->body.yf-=PUSH_FORCES;
	    if ((((int)p->body.y)&127)<50) p->body.yf+=PUSH_FORCES;
	    break;
	default:break;
	}
    oxv=p->body.xv;
    oyv=p->body.yv;
    p->body.xv+=p->body.xf/p->body.mass;
    p->body.yv+=p->body.yf/p->body.mass;
    p->body.xf=0;p->body.yf=0;
    if (p->body.xv>MAXVEL) p->body.xv=MAXVEL;
    if (p->body.yv>MAXVEL) p->body.yv=MAXVEL;
    if (p->body.xv<-MAXVEL) p->body.xv=-MAXVEL;
    if (p->body.yv<-MAXVEL) p->body.yv=-MAXVEL;
    p->body.x+=(p->body.xv+oxv)/2;
    p->body.y+=(p->body.yv+oyv)/2;
    xc=((int)p->body.x)/128;
    yc=((int)p->body.y)/128;
    
    if (is_hole(p->body.l,p->body.x,p->body.y)) {
	if (p->falltime) {
	    p->falltime--;
	} else {
	    char txt[256];
	    p->body.l++;
	    if (p->body.l>=map.depth) {
		create_trolley(p);
	    }
	    sprintf(txt,"Thingy at %d:%d,%d",p->body.l,(int)p->body.x/128,(int)p->body.y/128);
	    global_message(txt);
	    p->falltime=TROLLEY_FALL_TIME;
	}
    } else {
	int ux,uy,sx,sy;
	p->falltime=TROLLEY_FALL_TIME;
	if (p->body.l==p->cp->l) {
	    ux=(int) p->body.x;
	    uy=(int) p->body.y;
	    sx=ux-p->cp->x*128-64;
	    sy=uy-p->cp->y*128-64;
	    ux=sx*sx+sy*sy;
	    if (ux<(25*25)) {
		struct player *pl;
		p->cp=pick_check(p->cp);
		p->body.mass*=2;
		sprintf(str,"Mass %d    Take to check at %d:%d,%d",p->body.mass,
			p->cp->l,p->cp->x,p->cp->y);
		for (pl=p->holder;pl;pl=pl->nexthold) {
		    player_message(pl,str);
		    pl->cash+=100;
		}
	    }
	}
    }
    friction=1-(FRICTION/500);
    if (rd2(p->body.l,xc,yc)=='-') friction/=4;
    if ((rd(p->body.l,xc,yc)&MAP_SOLID)&&(rd2(p->body.l,xc,yc)!='+')) {
	p->body.xv*=friction;
	p->body.yv*=friction;
    }
    if (p->body.x<20) p->body.x=20;
    if (p->body.y<20) p->body.y=20;
    if (p->body.x>=(map.wid*128-20)) p->body.x=map.wid*128-21;
    if (p->body.y>=(map.hgt*128-20)) p->body.y=map.hgt*128-21;

    for (o=p->holder;o;o=o->nexthold) {
	int dx,dy;
	dx=o->body.x-p->body.x;
	dy=o->body.y-p->body.y;
	l=dx*dx+dy*dy;
	if (l>(STRING_LENGTH*STRING_LENGTH)) {
	    double ll,el;
	    double fx,fy;
	    ll=sqrt((double) l);
	    el=ll-STRING_LENGTH;
	    fx=STRING_FORCE*dx*el/ll;
	    fy=STRING_FORCE*dy*el/ll;
	    p->body.xf+=fx;p->body.yf+=fy;
	    o->body.xf-=fx;o->body.yf-=fy;
	}
    }
}

extern void update_trolleys() {
    struct trolley *t;
    for (t=firsttrol;t;t=t->next) 
	move_trolley(t);
}


extern void take_hold(struct player *p) {
    struct trolley *tr;
    struct trolley *mtr;
    int len,minlen;
    char str[256];
    if (p->holding) {
	if (p->lasthold) {
	    p->lasthold->nexthold=p->nexthold;
	} else {
	    p->holding->holder=p->nexthold;
	} 
	if (p->nexthold) p->nexthold->lasthold=p->lasthold;
	p->lasthold=0;
	p->nexthold=0;
	p->holding=0;
	return;
    }
    mtr=0;
    minlen=0;
    for (tr=firsttrol;tr;tr=tr->next)
	if (tr->body.l==p->body.l) {
	    len=((p->body.x-tr->body.x)*(p->body.x-tr->body.x)+
		 (p->body.y-tr->body.y)*(p->body.y-tr->body.y));
	    if (len<STRING_MAX_LENGTH*STRING_MAX_LENGTH) {
		if ((!minlen)||(len<minlen)) {
		    minlen=len;
		    mtr=tr;
		}
	    }
	}
    if (!mtr) {
	if (firsttrol) {
	    if (spottrol)
		spottrol=spottrol->next;
	    if (!spottrol) spottrol=firsttrol;
	    sprintf(str,"The object in question is at %d:%d,%d",
		    spottrol->body.l,(int) spottrol->body.x/128,
		    (int) spottrol->body.y/128);
	    player_message(p,str);
	}
	return;
    }
    p->holding=mtr;
    p->lasthold=0;
    p->nexthold=mtr->holder;
    if (p->nexthold) p->nexthold->lasthold=p;
    mtr->holder=p;
    sprintf(str,"Mass %d    Take to check at %d:%d,%d",mtr->body.mass,
	    mtr->cp->l,mtr->cp->x,mtr->cp->y);
    player_message(p,str);
}

extern void save_mines() {
    struct object *o;
    FILE *pd;

    if (!(pd=fopen("mines.data.file","w"))) {
	global_message("!!!ERROR - Cannot open mines file.!!!");
	return;
    }

    for (o=obj_first;o;o=o->next) {
	switch(o->type) {
	case OBJ_MINE_TRIG:
	case OBJ_MINE_TIME:
	case OBJ_MINE_VELY:
	case OBJ_MINE_PROX:
	case OBJ_MINE_SMART:
	    if ((o->flags&OBJ_F_ARM)&&(o->has.owner)) {
		fprintf(pd,"%d/%d/%f/%f/%f/%f/%d/%d/%d/%d/%s/\n",o->type,
			o->l,o->x,o->y,o->xv,o->yv,o->count,
			o->mode,o->flags,o->charge,o->has.owner->user);
	    }
	    break;
	default:
	case OBJ_BONUS:
	    break;
	}
    }
    fprintf(pd,"END\n");
    fclose(pd);
}

extern void load_mines() {
    struct object *o;
    FILE *pd;
    char *nb,*onb;

    if (!(pd=fopen("mines.data.file","rb"))) {
	fprintf(stderr,"!!!ERROR - Cannot open mines file for reading.!!!");
	return;
    }
    while(mygets(pd),strcmp(inbuf,"END")) {
	nb=inbuf;
	o=alloc_object();
	o->next=obj_first;
	obj_first=o;
	onb=nb;nb=strchr(nb,'/');*nb++=0;o->type=atoi(onb);
	onb=nb;nb=strchr(nb,'/');*nb++=0;o->l=atoi(onb);
	onb=nb;nb=strchr(nb,'/');*nb++=0;o->x=atof(onb);
	onb=nb;nb=strchr(nb,'/');*nb++=0;o->y=atof(onb);
	onb=nb;nb=strchr(nb,'/');*nb++=0;o->xv=atof(onb);
	onb=nb;nb=strchr(nb,'/');*nb++=0;o->yv=atof(onb);
	onb=nb;nb=strchr(nb,'/');*nb++=0;o->count=atoi(onb);
	onb=nb;nb=strchr(nb,'/');*nb++=0;o->mode=atoi(onb);
	onb=nb;nb=strchr(nb,'/');*nb++=0;o->flags=atoi(onb);
	onb=nb;nb=strchr(nb,'/');*nb++=0;o->charge=atoi(onb);
	onb=nb;nb=strchr(nb,'/');*nb++=0;o->has.owner=find_player(onb);
    }
    fclose(pd);
}
    


static int mygets(FILE *pd) {
    int l;
    *inbuf=0;
    fgets(inbuf,2048,pd); // TODO - validate input
    if (l=strlen(inbuf))
	if (inbuf[l-1]=='\n') inbuf[--l]=0;
    return l;
}
