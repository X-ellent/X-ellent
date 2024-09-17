/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/map.c,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.7 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>

#include "fix.h"
#include "map.h"
#include "player.h"
#include "debug.h"
#include "home.h"
#include "lift.h"
#include "turret.h"
#include "terminal.h"
#include "starburst.h"
#include "message.h"

static FILE *mdata;
static char buf[1024];

static int readin(char *b);

extern void create_map()
{
    struct checkpoint *cp;
    struct teleport *tp;
    struct starburst *sb;
    int l,x,y;
    char *c;
    DL("Loading in map....");
    if (!(c=(char *)getenv("MAPTHANG"))) c="testmap";
    if (!(mdata=fopen(c,"rb"))) {
	perror("Cannot open mapfile");
	exit(1);
    }
    cpcount=0;
    readin(buf);strncpy(map.name,buf,63);
    readin(buf);strncpy(map.author,buf,63);
    readin(buf);strncpy(map.comments,buf,255);
    readin(buf);map.wid=atoi(buf);
    readin(buf);map.hgt=atoi(buf);
    readin(buf);map.depth=atoi(buf);
    printf("Name    :%s\n",map.name);
    printf("Author  :%s\n",map.author);
    printf("Comments:%s\n",map.comments);
    printf("Size    :%d x %d\n",map.wid,map.hgt);
    printf("Depth   :%d\n",map.depth);
    map.wid+=2;map.hgt+=2;
    if (map.depth>MAXDEPTH) {
	fprintf(stderr,"Map is too deep for current configuration.\n");
	exit(2);
    }
    /* First read in the simple data, ie, solidity, and extra data... */
    for (l=0;l<map.depth;l++) {
	map.data[l]=(char *) calloc(map.wid,map.hgt);
	map.data2[l]=(char *) calloc(map.wid,map.hgt);
	map.data3[l]=(char *) calloc(map.wid,map.hgt);
	readin(buf);
	for (y=1;y<(map.hgt-1);y++) {
	    readin(buf);
	    for (x=1;x<(map.wid-1);x++) {
		map.data3[l][x+map.wid*y]|=MAP_NET_NODE;
		switch (buf[x-1]) {
		case ' ':
		case  0 :
		case '.':
		    map.data3[l][x+map.wid*y]&=~MAP_NET_NODE;
		    break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '0':
		case '+':
		case '-':
		    map.data[l][x+map.wid*y]|=MAP_FRICT|MAP_SOLID;
		    map.data2[l][x+map.wid*y]=buf[x-1];
		    break;
		case '*':
		    map.data[l][x+map.wid*y]|=MAP_SOLID;
		    rd2(l,x,y)='*';
 		    break;
		case 'l':
		    rd(l,x,y)|=MAP_SOLID;
		    rd2(l,x,y)='l';
		    break;
		case 'L':
		    rd(l,x,y)|=MAP_SOLID;
		    rd2(l,x,y)='L';
		    add_lift(l,x,y);
		    break;
		case 'O':
		    rd(l,x,y)|=MAP_SOLID;
		    rd2(l,x,y)='O';
		    add_turret(l,x,y);
		    break;
		case 'H':
		    rd(l,x,y)|=MAP_SOLID;
		    rd2(l,x,y)='H';
		    add_home(l,x,y);
		    break;
		case 'T':
		    rd(l,x,y)|=MAP_SOLID;
		    rd2(l,x,y)='T';
		    break;
		case 'X':
		    rd(l,x,y)|=MAP_SOLID;
		    rd2(l,x,y)='X';
		    tp=(struct teleport*)calloc(1,sizeof(struct teleport));
		    tp->next=firsttel;
		    firsttel=tp;
		    tp->x=x;tp->l=l;tp->y=y;
		    tp->num=tpcount++;
		    generate_pass(tp->pass,4);
		    break;
		case 'Z':
		    rd(l,x,y)|=MAP_SOLID;
		    rd2(l,x,y)='Z';
		    sb=(struct starburst*)calloc(1,sizeof(struct starburst));
		    sb->next=firststar;
		    firststar=sb;
		    sb->body.x=(double) x*128+64;
		    sb->body.y=(double) y*128+64;
		    sb->body.l=l;
		    sb->sleep=random()%360;
		    sb->num=sbcount++;
		    sb->rot=random()%360;
		    break;
		case 'C':
		    rd(l,x,y)|=MAP_SOLID;
		    rd2(l,x,y)='C';
		    cp=(struct checkpoint*)calloc(1,sizeof(struct checkpoint));
		    cp->next=firstcheck;
		    firstcheck=cp;
		    cp->x=x;cp->l=l;cp->y=y;cp->num=cpcount++;
		    break;
		case 'S':
		    rd(l,x,y)|=MAP_SOLID;
		    rd2(l,x,y)='S';
		    break;
		case 'F':
		    rd(l,x,y)|=MAP_SOLID;
		    rd2(l,x,y)='F';
		    break;
		default:
		    map.data[l][x+map.wid*y]|=MAP_SOLID;
		    fprintf(stderr,"Unknown map symbol at %d:%d,%d '%c'\n",
			    l,x,y,buf[x-1]);
		}
	    }
	}
    }
    /* Now build up the 'first-order' wall data from the solid/non solid info*/
    for (l=0;l<map.depth;l++) {
	for (y=1;y<(map.hgt-1);y++)
	    for (x=1;x<map.wid;x++)
		if ((rd(l,x,y)^rd(l,x-1,y))&MAP_SOLID) 
		    rd(l,x,y)|=MAP_LWALL;
	for (x=1;x<(map.wid-1);x++)
	    for (y=1;y<map.hgt;y++)
		if ((rd(l,x,y)^rd(l,x,y-1))&MAP_SOLID) 
		    rd(l,x,y)|=MAP_TWALL;
    }
    /* Then do the 'second-order' scan for walls observed from above */
    for (l=0;l<map.depth-1;l++) 
	for (y=1;y<(map.hgt-1);y++)
	    for (x=1;x<(map.wid-1);x++)
		if (!(rd(l,x,y)&MAP_SOLID)) {
		    if (rd(l+1,x,y)&MAP_SOLID) rd(l,x,y)|=MAP_OBSC;
		    if (rd(l+1,x,y)&MAP_LWALL) rd(l,x,y)|=MAP_LWALL1;
		    if (rd(l+1,x,y)&MAP_TWALL) rd(l,x,y)|=MAP_TWALL1;
		    if (rd(l+1,x+1,y)&MAP_LWALL) rd(l,x+1,y)|=MAP_LWALL1;
		    if (rd(l+1,x,y+1)&MAP_TWALL) rd(l,x,y+1)|=MAP_TWALL1;
		}
#ifdef SHOW_THIRD_LEVEL
    /* THEN do the 'third-order' scan for walls observed from above */
    for (l=0;l<map.depth-2;l++) 
	for (y=1;y<(map.hgt-1);y++)
	    for (x=1;x<(map.hgt-1);x++)
		if (!(rd(l,x,y)&(MAP_SOLID|MAP_OBSC))) {
		    if ((rd(l+2,x,y)&MAP_LWALL)&&(!(rd(l,x,y)&MAP_LWALL1)))
			rd(l,x,y)|=MAP_LWALL0;
		    if ((rd(l+2,x,y)&MAP_TWALL)&&(!(rd(l,x,y)&MAP_TWALL1)))
			rd(l,x,y)|=MAP_TWALL0;
		    if ((rd(l+2,x+1,y)&MAP_LWALL)&&(!(rd(l,x+1,y)&MAP_LWALL1)))
		        rd(l,x+1,y)|=MAP_LWALL0;
		    if ((rd(l+2,x,y+1)&MAP_TWALL)&&(!(rd(l,x,y+1)&MAP_TWALL1)))
			rd(l,x,y+1)|=MAP_TWALL0;
		}
#endif
    /* Set random destinations for teleports */
    for(tp=firsttel;tp;tp=tp->next) {
	tp->dest=find_teleport(random()%tpcount);
	if (tp->dest==tp) tp->dest=tp->next;
	if (!tp->dest) tp=firsttel;
    }
}

static int readin(char *b)
{
    int l;
    *b=0;
    fgets(b,1024,mdata);
    if (l=strlen(b)) b[l-1]=0;
    return (l-1);
}

extern int is_hole(int l,int x,int y)
{
    int xx,yy;
    char c;
    xx=x/128;
    yy=y/128;
    if (l>=map.depth) return -1;
    if (!(rd(l,xx,yy)&MAP_SOLID)) return -1;
    x&=127;
    y&=127;
    if ((x<=32)||(x>96)||(y<=32)||(y>96)) return 0;
    c=rd2(l,xx,yy);
    if (c=='l') return -1;
    return 0;
}

extern struct checkpoint *pick_check(struct checkpoint *cpo) {
    int n,m;
    struct checkpoint *cp;
    m=0;
    for (cp=firstcheck;cp;cp=cp->next)
	if ((!cpo)||(cpo->l!=cp->l)) m++;
    n=random()%m;
    for (cp=firstcheck;cp;cp=cp->next)
	if ((!cpo)||(cpo->l!=cp->l))
	    if (!(n--)) return cp;
    return firstcheck;
}

extern struct teleport *find_teleport(int n) {
    struct teleport *tp;
    for(tp=firsttel;tp;tp=tp->next)
	if (tp->num==n) return tp;
    return 0;
}

extern struct teleport *locate_teleport(int l,int x,int y) {
    struct teleport *tp;
    for(tp=firsttel;tp;tp=tp->next)
	if ((tp->l==l)&&(tp->x==x)&&(tp->y==y)) return tp;
    return 0;
}

extern void use_teleport(struct teleport *t) {
    char txt[256];
    int minx,miny,maxx,maxy;
    int dx,dy;
    struct body *b;
    struct object *o;
    if (!t) return;
    if (t->dest==t) return;
    minx=t->x*128+31;
    miny=t->y*128+31;
    maxx=t->x*128+97;
    maxy=t->y*128+97;
    dx=(t->dest->x-t->x)*128;
    dy=(t->dest->y-t->y)*128;
    for (b=firstbody;b;b=b->next) {
	if (b->l==t->l) {
	    if ((b->x>minx)&&(b->x<maxx)&&(b->y>miny)&&(b->y<maxy)) {
		b->l=t->dest->l;
		b->x+=dx;b->y+=dy;
		if (b->type==BODY_PLAYER) {
		    player_message(b->is.player,"Pling!!!!");
		    psend(b->is.player,
			  (sprintf(txt,"#TELE %d DEST %d %d:%d,%d\n"
				  ,t->num,t->dest->num,b->l,
				   (int)b->x,(int)b->y),txt));
		}
	    }
	}
    }
    for (o=obj_first;o;o=o->next) {
	if (o->l==t->l) {
	    if ((o->x>minx)&&(o->x<maxx)&&(o->y>miny)&&(o->y<maxy)) {
		o->l=t->dest->l;
		o->x+=dx;o->y+=dy;
	    }
	}
    }
}

extern void update_teleports() {
    struct teleport *tp;
    for (tp=firsttel;tp;tp=tp->next) {
	if (tp->clk) {
	    tp->clk++;
	    if (tp->clk==29) tp->clk=-5;
	    if (tp->clk<0)
		use_teleport(tp);
	}
    }
}
