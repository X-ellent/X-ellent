/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/player.c,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.7 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#include <math.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <setjmp.h>
#include <X11/Xauth.h>

#include "fix.h"
#include "constants.h"
#include "mymath.h"
#include "player.h"
#include "debug.h"
#include "particle.h"
#include "telnet.h"
#include "message.h"
#include "turret.h"
#include "home.h"
#include "events.h"
#include "terminal.h"
#include "xsetup.h"
#include "beam.h"
#include "addon.h"
#include "ip_user.h"

#define inthang(f,o) if (strcmp(chop[0],"f")==0) p->owned|=o

extern struct ip_user ip_users[]; // TODO - extern?
extern int num_ip_users;
struct player *freeplay;
struct player *playone;
int players;
char *weap_name[MAX_WEAPS];

static char inbuf[1024];
static char txt[256];
static char *chop[16];

static struct player *alloc_player();
static void lose_items(struct player *p);
static void away_stuff(struct player *p);
static void quit_player(struct player *p);
static int mygets(FILE *pd);
static void mychop();

static struct player *alloc_player()
{
    struct player *p;
    p=freeplay;
    if (p) {
	freeplay=p->next;
    } else {
	p=(struct player *) calloc(1,sizeof(struct player));
    } 
    p->body.is.player=p;
    p->body.type=BODY_PLAYER;
    p->body.radius=20;
    p->channel=-1;
    return p;
}

extern void load_ip_users();
extern time_t last_ip_users_mtime;

extern int setup_player(char *calling_ip)
{
    load_ip_users(); // Ensure we have the latest version
    char* calling_user = NULL;
    for (int i = 0; i < num_ip_users; i++) {
        if (strncmp(ip_users[i].ip, calling_ip, INET_ADDRSTRLEN) == 0) {
            calling_user = ip_users[i].username;
            break;
        }
    }

    if (calling_user == NULL) {
        fprintf(stderr, "Unauthorized connection attempt from %s\n", calling_ip);
        return 0;
    }

    struct player *p;
    char txt[1024];
    char *nm = tread(); // Read painintheass
    if (strcmp(nm,"painintheass")) {
        ctwrite("You must use the new game client");
        return 0;
    }
    nm = tread(); // Read requested user
    char requested_user[32];
    strncpy(requested_user, nm, sizeof(requested_user));

    nm = tread(); // Read requested DISPLAY

    printf("Connect from %s %s (client: user=%s display=%s)\n",
        calling_user, calling_ip, requested_user, nm);

    char display[30];
    char *username;
    if (strcmp(calling_user, "rebroad") == 0) {
        username = requested_user;
        snprintf(display, sizeof(display), "%s", nm);
    } else {
        username = calling_user;
        char *display_number = strchr(nm, ':');
        if (display_number == NULL)
            display_number = ":0";  // Default to :0 if no colon found
        snprintf(display, sizeof(display), "%s%s", calling_ip, display_number);
    }

    nm = tread(); // Read Xauth cookie
    char xauth_cookie[33];
    strncpy(xauth_cookie, nm, sizeof(xauth_cookie));

    // Create a temporary Xauthority file for this connection
    char temp_xauth_file[] = "/tmp/xauth_XXXXXX";
    int fd = mkstemp(temp_xauth_file);
    if (fd != -1) {
        FILE *fp = fdopen(fd, "w");
        if (fp) {
            Xauth auth_entry;
            auth_entry.family = FamilyLocal;
            auth_entry.address = display;
            auth_entry.address_length = strlen(display);
            auth_entry.number = "0";
            auth_entry.name = "MIT-MAGIC-COOKIE-1";
            auth_entry.name_length = strlen(auth_entry.name);
            auth_entry.data = xauth_cookie;
            auth_entry.data_length = 32;

            XauWriteAuth(fp, &auth_entry);
            fclose(fp);

            setenv("XAUTHORITY", temp_xauth_file, 1);
        }
    }

    if (p = find_player(username)) {
	if (p->connected) {
	    ctwrite("You are ALREADY connected...");
	    return 0;
	}
    } else {
	p=alloc_player();
	strncpy(p->user, username, 15);
	strncpy(p->name, username, 31);

	/* Setup brand new player */
	
/*	p->maxrng=16;*/
	p->maxfuel=200000;
	p->fuel=150000;
	p->shield=1000;
	p->maxshield=1000;
	p->weap=WEP_RIF;
	p->weap_mask=(1<<0);
	p->ammo[0]=-1;
	p->cash=START_CASH;
	p->score=0;
	p->immune=START_IMMUNE;
	p->deaths=0;
	p->kills=0;
	p->tkills=0;
	p->rating=1;

	p->next=playone;
	playone=p;
    }
    if ((p->oldhome)&&(!p->oldhome->owner)) {
	p->home=p->oldhome;
    } else {
	if (!p->home) {
	    if (!(p->home=pick_home())) {
		ctwrite("The map is too full right now, sorry");
		return 0;
	    }
	}
    }

    if (!init_player_display(p, display)) {
        unlink(temp_xauth_file);
        ctwrite("init_player_display() failed");
	return 0;
    }
    unlink(temp_xauth_file); // No longer needed now that connection established

    p->connected=-1;
    p->draw_interval=1;  // TODO - work out latency first
    
    if (p->playing) {
	sprintf(txt,"%s has rejoined us.",p->name);
	global_message(txt);
	players++;
	p->immune=START_IMMUNE_REJ;
	return -1;
    }
    /* Set up for rejoining or new player */
    Setup_string(p,"name",p->name,31);
    {
	char *c;
	if (c=ctpass())
	    if (*c) strncpy(p->pass,c,8);
    }
    {
	struct player *op;
	for (op=playone;op;op=op->next)
	    if (op!=p) {
		if (strcmp(op->name,p->name)==0) strcpy(p->name,p->user);
		if (strcmp(op->user,p->name)==0) strcpy(p->name,p->user);
	    }
    }
    p->rot=0;p->rv=0;p->rt=0;
    p->body.mass=500;
    if ((p->homefuel+p->fuel)<MIN_FUEL) p->homefuel=MIN_FUEL;
    p->home->owner=p;
    p->body.x=p->home->x*128+64;
    p->body.y=p->home->y*128+64;
    p->body.l=p->home->l;
    p->body.xv=0;p->body.yv=0;p->body.xf=0;p->body.yf=0;
/*    p->range=p->maxrng;*/
    p->qflags=0;
    p->playing=-1;
    remove_body(&p->body);
    p->flags=FLG_HOME;
    p->slot=0;
    p->immune=DEATH_IMMUNE;
    p->ptarg=0;
    p->scount=0;
    p->onground=0;
    p->msg[0][0]=0;
    p->msg[1][0]=0;
    p->msg[2][0]=0;
    p->msg[3][0]=0;
    for (int i=0; i < map.depth; i++) p->mapmem[i]=0;
    players++;

    /* Set configurable things */

    p->flags|=Setup_flag(p,"ident",FLG_IDENT,0,1);
    p->flags|=Setup_flag(p,"status",FLG_STATUS,0,1);
    p->flags|=Setup_flag(p,"message",0,FLG_NOMSG,1);
    p->flags|=Setup_flag(p,"slots",0,FLG_NOWEP,1);
    p->flags|=Setup_flag(p,"instruments",0,FLG_NOINSTR,1);

    p->thrust=Setup_value(p,"thrust",40,0,100);
    p->spin=Setup_value(p,"spin",75,0,100);
    p->ethrust=Setup_value(p,"altthrust",70,0,100);
    p->espin=Setup_value(p,"altspin",100,0,100);
    p->step=Setup_value(p,"step",5,0,50);
    p->fuelmin=200*Setup_value(p,"fuel",30,0,p->maxfuel/200);
    
    p->delay=0;

    init_home(p);
    sprintf(txt,"%s has just joined us.",p->name);
    global_message(txt);
    return -1;
}

extern void update_player(struct player *p)
{
    int xc,yc;
    double oxv,oyv;
    int t,i;
    int bigfall;
    char tx[60];
    double friction,inertia;
    jumpable=-1;
    switch (setjmp(jmpenv)) {
    case 0:break;
    case 1:return;
    case 2:
	bloody_errors(p);
	return;
    }
    bigfall=0;
    if (p->immune) p->immune--;
    process_events(p);
    if (!p->body.on) {
	away_stuff(p);
	jumpable=0;
	longjmp(jmpenv,1);
    }
    if (p->qflags==7) quit_player(p);
    if (p->homing) {
	p->homing--;
	if (!p->homing)
	    player_message(p,"Home base now available...");
    }
    if ((p->flags&FLG_FUELLING)&&(rd2(p->body.l,(int) p->body.x/128,
				     (int) p->body.y/128)=='F')) {
	int x,y;
	x=(int) p->body.x;y=(int) p->body.y;
	x&=127;y&=127;
	if ((x>32)&&(x<=96)&&(y>32)&&(y<=96)) {
	    if (p->cash>=FUEL_COST) {
		p->cash-=FUEL_COST;
		p->fuel+=FUEL_PLUS;
		if (p->fuel>p->maxfuel) p->fuel=p->maxfuel;
	    }
	}
    }
    if ((p->flags&FLG_THRUST)&&(p->fuel)) {
	t=4000*p->thrust/100;
	p->fuel-=p->thrust;
	if (p->fuel<0) p->fuel=0;
	p->body.xf+=t*sn[(int) p->rot];
	p->body.yf-=t*cs[(int) p->rot];
    }
    if (p->flags&FLG_CLOAKING) {
	p->fuel-=CLOAK_FUEL_DRAIN;
	if (p->fuel<=0) {
	    p->flags&=~FLG_CLOAKING;
	    p->fuel=0;
	}
    }
    if (p->flags&FLG_ANTICLOAK) {
	p->fuel-=ANTICLOAK_FUEL_DRAIN;
	if (p->fuel<=0) {
	    p->flags&=~FLG_ANTICLOAK;
	    p->fuel=0;
	}
    }
    if (p->flags&FLG_MINESWEEP) {
	p->fuel-=SWEEP_FUEL_DRAIN;
	if (p->fuel<=0) {
	    p->flags&=~FLG_MINESWEEP;
	    p->fuel=0;
	}
    }
    if (p->flags&FLG_INVIS) {
	p->fuel-=INVIS_FUEL_DRAIN;
	if (p->fuel<=0) {
	    p->flags&=~FLG_INVIS;
	    p->fuel=0;
	}
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
    if (p->body.xv>MAXVEL) p->body.xv=MAXVEL;
    if (p->body.yv>MAXVEL) p->body.yv=MAXVEL;

    if (p->body.xv<-MAXVEL) p->body.xv=-MAXVEL;
    if (p->body.yv<-MAXVEL) p->body.yv=-MAXVEL;
    p->body.x+=(p->body.xv+oxv)/2;
    p->body.y+=(p->body.yv+oyv)/2;
    xc=((int)p->body.x)/128;
    yc=((int)p->body.y)/128;
    if (p->body.height==0) {
	double vel;
	if (is_hole(p->body.l,p->body.x,p->body.y)) {
	    p->onground=GROUND_TIME;
	    if ((p->body.fallen==0)&&
		(rd2(p->body.l,(int)p->body.x/128,(int)p->body.y/128)!='l')) {
		vel=(p->body.xv*p->body.xv+p->body.yv*p->body.yv);
		if (vel<400) {p->body.fallen=1;p->body.height++;};
		if (vel<200) {p->body.fallen=2;};
		if (vel<100) {p->body.fallen=2;p->body.height--;};
		if (vel<50) {p->body.fallen=3;}
		if (vel>=400) {p->body.fallen=0;p->body.height++;};
	    } else {
		p->body.l++;
		if (p->body.l==map.depth) {
		    sprintf(tx,"%s just fell off",p->name);
		    global_message(tx);
		    psend(p,"#FELL OFF\n");
		    p->body.fallen=0;
		    bigfall=-1;
		} else {
		    p->body.fallen++;
		    p->body.height=3;
		}
	    }
} else {
	    if (p->body.fallen>9) {
		sprintf(tx,"%s just fell off",p->name);
		global_message(tx);
		bigfall=-1;
	    }
	    p->body.fallen=0;
	    if (p->onground) {
		if (p->onground==GROUND_TIME) {
		    psend(p,(sprintf(tx,"#LAND %d:%d,%d\n",p->body.l,
				     (int)p->body.x,(int)p->body.y),txt));
		    p->loctime=LOCATE_TIME;
		}
		p->onground--;
	    } else {
		p->ox=p->oox;
		p->oy=p->ooy;
		p->olvl=p->oolvl;
		p->oox=p->body.x;
		p->ooy=p->body.y;
		p->oolvl=p->body.l;
	    }
	    if (p->locate&&(--p->loctime<0)) {
		p->loctime=LOCATE_TIME;
		psend(p,(sprintf(tx,"#LOCATE %d %d:%d,%d\n",(int)p->rot,
				 p->body.l,(int)p->body.x,(int)p->body.y),
			 tx));
	    }
	    if (p->locate&&(--p->tartime<0)) {
		struct addon *tarad;
		p->tartime=LOCATE_TARGET_TIME;
		tarad=find_addon(p->firstadd,ADD_TARGET);
		
		if ((tarad)&&(tarad->level>2)&&(p->ptarg)&&
		    ((!(p->ptarg->flags&FLG_CLOAKING))||
		     can_locate(p,p->ptarg))) {
		    if (p->ptarg->body.l==p->body.l) {
			psend(p,(sprintf(txt,"#TARGET %s %d:%d,%d\n",
					p->ptarg->user,
					p->ptarg->body.l,
					(int) p->ptarg->body.x,
					 (int) p->ptarg->body.y),txt));
		    } else {
			if (((p->body.l+tarad->info[1])==p->ptarg->body.l)&&
			    ((!(p->ptarg->flags&FLG_CLOAKING))||
			     can_locate(p,p->ptarg))) {
			    psend(p,(sprintf(txt,"#TARGET %s %s\n",
					    p->ptarg->user,
					    (tarad->info[1]>0)?
					    "Below":"Above"),txt));
			}
		    }
		}
	    }
	}
    } else {
	if (p->body.fallen<3) {
	    if (p->body.fallen==1) p->body.height++;
	} else {
	    p->body.height--;
	}
	p->body.fallen++;
    }
    if (bigfall!=-1) {
	inertia=1-(INERTIA/((double) p->body.mass));
	if ((p->body.height==0)&&(rd(p->body.l,xc,yc)&MAP_SOLID)) {
	    friction=1-(FRICTION/500);
	    if (rd2(p->body.l,xc,yc)=='-') friction/=4;
	    if (rd2(p->body.l,xc,yc)!='+') {
		if (p->flags&FLG_BRAKING) {
		    p->body.xv*=(friction/2);
		    p->body.yv*=(friction/2);
		} else {
		    p->body.xv*=friction;
		    p->body.yv*=friction;
		}
	    }
	}
	p->rv*=inertia;
	t=SPINSPEED*p->spin/100;
	switch (p->flags&(FLG_ROTCLOCK|FLG_ROTACLOCK)) {
	case FLG_ROTCLOCK:
	    p->rv+=(t/((double)p->body.mass));
	    break;
	case FLG_ROTACLOCK:
	    p->rv-=(t/((double)p->body.mass));
	    break;
	}
	p->rot+=p->rv;
	if (p->rot<0) p->rot+=360;
	if (p->rot>=360) p->rot-=360;
	p->body.xf=0;p->body.yf=0;
	if (p->ammo[p->weap]==0) p->flags&=(~FLG_FIRING);
	if ((p->flags&FLG_FIRING)&&(!p->recharge)) {
	    double v;
	    int d;
	    v=random()%50;
	    p->immune=0;
	    v=v/10;
	    switch(p->weap) {
	    case WEP_RIF:
		fire_particle(p,p->body.l,p->body.x+v*sn[(int) p->rot],
			      p->body.y-v*cs[(int) p->rot],(int) p->rot,30.0,10,15,
			      16);
		p->flags&=(~FLG_FIRING);
		break;
	    case WEP_LMG:
		fire_particle(p,p->body.l,p->body.x+v*sn[(int) p->rot],
			      p->body.y-v*cs[(int) p->rot],(int) p->rot,30.0,10,18,
			      20);
		p->ammo[p->weap]--;
		break;
	    case WEP_HMG:
		fire_particle(p,p->body.l,p->body.x+v*sn[(int) p->rot],
			      p->body.y-v*cs[(int) p->rot],(int) p->rot,28.0,20,25,
			      100);
		p->ammo[p->weap]--;
		break;
	    case WEP_MORTAR:
		v=v+18.0;
		fire_particle(p,p->body.l,p->body.x+v*sn[(int) p->rot],
			      p->body.y-v*cs[(int) p->rot],(int) p->rot,10.0,100,
			      100,2000);
		p->ammo[p->weap]--;
		p->flags&=(~FLG_FIRING);
		break;
	    case WEP_FLAME:
		for (i=0;i<5;i++) {
		    v=random()%200/10+5.0;
		    fire_particle(p,p->body.l,p->body.x+v*sn[(int) p->rot],
				  p->body.y-v*cs[(int) p->rot],
				  (int) p->rot-9+random()%19
				  ,20.0,20,200,20);
		}
		p->ammo[p->weap]--;
		break;
	    case WEP_RMG:
		v/=4;
		fire_particle(p,p->body.l,p->body.x-v*sn[(int) p->rot],
			      p->body.y+v*cs[(int) p->rot],
			      (((int) p->rot)+180)%360,
			      28.0,20,25,100);
		p->ammo[p->weap]--;
		break;
	    case WEP_PRONG:
		fire_particle(p,p->body.l,p->body.x+v*sn[(int) p->rot],
			      p->body.y-v*cs[(int) p->rot],(int) p->rot,
			      28.0,20,25,100);
		fire_particle(p,p->body.l,p->body.x+v*sn[(int) p->rot],
			      p->body.y-v*cs[(int) p->rot],(int) p->rot+5,
			      28.0,20,25,100);
		fire_particle(p,p->body.l,p->body.x+v*sn[(int) p->rot],
			      p->body.y-v*cs[(int) p->rot],(int) p->rot-5,
			      28.0,20,25,100);
		p->ammo[p->weap]--;
		break;
	    case WEP_TARGLASER:
		if (d=fire_beam_weapon(p,&p->body,p->body.x,p->body.y,
				       (int)p->rot,0)) {
		    player_message(p,(sprintf(tx,"Target encountered %d",d),tx));
		    psend(p,(sprintf(tx,"#LASER TARGET %d %d:%d,%d %d\n",
				    (int)p->rot,p->body.l,(int)p->body.x,
				    (int)p->body.y,d),tx));
		} else {
		    player_message(p,(sprintf(tx,"No target encountered"),tx));
		}
		p->ammo[p->weap]--;
		p->recharge=RECHARGE_TARGLASER;
		break;
	    case WEP_LASER:
		fire_beam_weapon(p,&p->body,p->body.x,p->body.y,(int)p->rot,
				 200);
		p->ammo[p->weap]--;
		p->recharge=RECHARGE_LASER;
		break;
	    }
	} else {
	    if (p->recharge) p->recharge--;
	}
	if (++p->scount>=10) {
	    int rate;
	    rate=addon_level(p->firstadd,ADD_ENERGY);
	    if ((p->shield<p->maxshield)&&(p->fuel)) {
		p->shield+=(1<<rate)*3;
		if (p->shield>p->maxshield) p->shield=p->maxshield;
		p->fuel-=(1<<rate);
		if (p->fuel<0) p->fuel=0;
	    }
	    p->scount=0;
	}
    }
    if (p->body.x<20) p->body.x=20;
    if (p->body.y<20) p->body.y=20;
    if (p->body.x>=(map.wid*128-20)) p->body.x=map.wid*128-21;
    if (p->body.y>=(map.hgt*128-20)) p->body.y=map.hgt*128-21;
    if (bigfall==-1) {
	p->flags|=(FLG_DEAD|FLG_FALLEN);
	p->delay=DEATH_DELAY;
	p->shield=p->shield*9/10;
	if (p->shield<FALL_MIN_SHIELD) p->shield=FALL_MIN_SHIELD;
	if (p->body.l<map.depth)
	    explode(p->body.l,p->body.x,p->body.y,30,0,0,0);
	p->fuel=p->fuel*9/10;
	if (p->fuel<FALL_MIN_FUEL) p->fuel=FALL_MIN_FUEL;
	p->cash=p->cash*19/20;
	p->body.xv=0;p->body.xf=0;
	p->body.fallen=0;
	p->body.yv=0;p->body.yf=0;
	remove_body(&p->body);
    }
    jumpable=0;
    longjmp(jmpenv,1);
}

extern void damage_player(struct player *p,int d,struct player *who,int how)
{
    char txt[1024];
    int rat;
    if ((who)&&(how==DAM_SHOT)&&((who->rating*2)<p->rating)) return;
    if (p->shield<(d-100)) d=p->shield+100;
    p->shield-=d;
    if (who&&(who!=p)) {
	who->score+=(d+9)/10;
    }
    if (p->shield<0) {
	p->shield=0;
	remove_body(&p->body);
	if ((who)&&(who!=p)) {
	    who->score+=DEATH_BONUS;
	    if (who->rating<=p->rating) {
		who->cash+=DEATH_CASH;
	    }
	    if (2*who->rating<=p->rating) {
		who->cash+=DEATH_CASH-DEATH_CASH_MIN;
	    }
	    if (who->rating<=(p->rating*2)) {
		who->cash+=p->cash/10;
		who->cash+=DEATH_CASH_MIN;
	    } else {
		if (how==DAM_SHOT) {
		    int i;
		    p->cash+=who->cash/4;
		    who->cash=who->cash*3/4;
		    p->fuel+=who->fuel/4;
		    if (p->fuel>p->maxfuel) p->fuel=p->maxfuel;
		    who->fuel-=who->fuel/4;
		    for (i=0;i<MAX_WEAPS;i++) {
			p->ammo[i]+=who->ammo[i]/4;
			who->ammo[i]-=who->ammo[i]/4;
		    }
		    p->cash=p->cash*10/9;
		}
	    }
	    who->kills++;
	    if (how==DAM_SHOT) {
		sprintf(txt,"%s was killed by %s.",p->name,who->name);
		if (2*p->rating!=who->rating) {
		    rat=(2*p->rating-who->rating);
		    if (rat>0) rat+=3;
		    if (rat<0) rat-=3;
		    rat=rat/4;
		    if (rat>who->rating) rat=who->rating;
		    if (rat>4) rat=4; /* Limit rating change to 4 */
		    who->rating+=rat;
		    if (rat>0) p->rating-=rat;
		    if (rat>0) {
			player_message(who,"Your rating has risen....");
		    } else {
			if (rat<0) {
			    player_message(who,"Your rating has fallen....");
			}
		    }
		} else {
		    who->rating++;
		    player_message(who,"Your rating has risen....");
		}
	    } else {
		psend(p,(sprintf(txt,"#DIED %s\n",who->name),txt));
		sprintf(txt,"%s was killed due to %s.",p->name,who->name);
	    }
	} else {
	    switch(how) {
	    case DAM_FALL:
		psend(p,"#DIED FALL\n");
		sprintf(txt,"%s has died due to falling.",p->name);break;
	    case DAM_CRASH:
		psend(p,"#DIED CRASH\n");
		sprintf(txt,"%s has died due to a collision.",p->name);break;
	    case DAM_EXPL:
		psend(p,"#DIED EXPLOSION\n");
		sprintf(txt,"%s has died due to an explosion.",p->name);break;
	    case DAM_BOGUS:
		psend(p,"#DIED BOGUS\n");
		sprintf(txt,"%s has died due to a BOGUS.",p->name);break;
	    case DAM_LASER:
		psend(p,"#DIED LASER\n");
		sprintf(txt,"%s has been pierced nicely.",p->name);break;
	    default:
		psend(p,"#DIED BANG\n");
		sprintf(txt,"%s has just exploded.",p->name);break;
	    }
	}
	global_message(txt);
	*txt=0;
	explode(p->body.l,p->body.x,p->body.y,30,10000,100,0);
	p->flags|=FLG_DEAD;
	p->delay=DEATH_DELAY;
	p->body.xv=0;p->body.xf=0;
	p->body.yv=0;p->body.yf=0;
	p->deaths++;
	lose_items(p);
	p->shield=500;
	if ((p->homefuel+p->fuel)<MIN_FUEL) p->homefuel=MIN_FUEL;
    }
}

static void lose_items(struct player *p) {
    int i;
    struct addon *ad;
    for (i=0;i<map.depth;i++) p->mapmem[i]=0;
    for (i=0;i<9;i++) if (p->slots[i]&&(!p->slotobj[i])) {
	p->slots[i]=OBJ_EMPTY;
	p->size[i]=0;
	p->mode[i]=0;
    }
    for (ad=p->firstadd;ad;ad=ad->next)
	ad->damage--;
    if (p->firstadd) p->firstadd=strip_addons(p,p->firstadd);
}

static void away_stuff(struct player *p)
{
    int i;
    if (!(p->connected)) {
	p->playing=0;
	p->home->owner=0;
	p->home=0;
	lose_items(p);
	for (i=0;i<map.depth;i++) p->mapmem[i]=0;
	p->body.xv=0;p->body.yv=0;p->body.xf=0;p->body.yf=0;
	p->rv=0;p->rt=0;
	remove_body(&p->body);
	return;
    }
    if (p->flags&FLG_DEAD) {
	p->delay--;
	if (p->delay==0) {
	    p->flags&=~FLG_DEADCLR;
	    p->body.x=p->home->x*128+64;
	    p->body.y=p->home->y*128+64;
	    p->body.xv=0;p->body.xf=0;
	    p->body.yv=0;p->body.yf=0;
	    p->body.l=p->home->l;
	    p->rv=0;p->rt=0;p->rot=0;p->cash=p->cash*9/10;p->fuel=p->fuel*9/10;
	    p->shield=p->maxshield/2;
	    p->immune=DEATH_IMMUNE;
	    remove_body(&p->body);
	    p->body.fallen=0;
	    if ((p->homefuel+p->fuel)<MIN_FUEL) p->homefuel=MIN_FUEL;
	    p->body.height=0;
	    init_home(p);
	    return;
	}
	if ((p->delay==(DEATH_SHOW+1))&&(p->flags&FLG_FALLEN)) {
	    p->delay=0;
	    p->flags&=~FLG_DEADCLR;
	    p->body.x=p->ox;
	    p->body.y=p->oy;
	    p->body.xv=0;p->body.xf=0;
	    p->body.yv=0;p->body.yf=0;
	    p->body.l=p->olvl;
	    p->rv=0;p->rt=0;
	    p->immune=FALL_IMMUNE;
	    add_body(&p->body);
	    p->body.fallen=0;
	    p->body.height=0;
	    return;
	}
    }
    if (p->flags&FLG_TERMINAL)
        if (!run_program(p))
            player_message(p,"Terminal crashed - We're working on it!");
    if (p->flags&FLG_HOME) do_home(p);
    return;
}

extern void init_all_weap() {
    weap_name[WEP_RIF]="Rifle";
    weap_name[WEP_LMG]="Light Machine Gun";
    weap_name[WEP_HMG]="Heavy Machine Gun";
    weap_name[WEP_MORTAR]="Mortar Launcher";
    weap_name[WEP_FLAME]="Flame Thrower";
    weap_name[WEP_RMG]="Rear Machine Gun";
    weap_name[WEP_PRONG]="Forked Machine Gun";
    weap_name[WEP_TARGLASER]="Targetting Laser";
    weap_name[WEP_LASER]="Laser Cannon";
}

static void quit_player(struct player *p) {
    char txt[1024];
    p->flags&=~FLG_DEADCLR;
    XAutoRepeatOn(p->d.disp);
    p->qflags=0;
    shutdown_display(p);
    players--;
    psend(p,"#DISCONNECT\n");
    sprintf(txt,"%s has just disconnected",p->name);
    global_message(txt);
}

extern struct player *find_player(char *n) {
    struct player *p;
    for (p=playone;p;p=p->next) 
	if (strcmp(p->user,n)==0) return p;
    return 0;
}

extern void save_players() {
    struct player *p;
    struct addon *ao;
    char ft[128];
    int f;
    FILE *pd;

    if (!(pd=fopen("player.data.file","w"))) {
	global_message("!!!ERROR - Cannot open player file.!!!");
	return;
    }

    fprintf(pd,"Player data file....\n");
    fprintf(pd,"user/name/password/score/cash/rating\n");
    fprintf(pd,"kills/turrets/deaths\n");
    fprintf(pd,"shield/max/fuel/max/radar-range/homefuel\n");
    fprintf(pd,"thrust/spin/altthrust/altspin/step/flags/weapon\n");
    fprintf(pd,"weapons list\nmines list\nowned list\n");
    fprintf(pd,"\n");
    
    for (p=playone;p;p=p->next) {
	fprintf(pd,"%s/%s/%s/%d/%d/%d\n",p->user,p->name,p->pass,p->score,
		p->cash,p->rating);
	fprintf(pd,"%d/%d/%d\n",p->kills,p->tkills,p->deaths);
	fprintf(pd,"%d/%d/%d/%d/%d\n",p->shield,p->maxshield,p->fuel,
		p->maxfuel,p->homefuel);
	f=0;
	if (p->flags&FLG_STATUS) ft[f++]='S';
	if (p->flags&FLG_IDENT) ft[f++]='N';
	if (p->flags&FLG_NOWEP) ft[f++]='W';
	if (p->flags&FLG_NOINSTR) ft[f++]='I';
	if (p->flags&FLG_NOMSG) ft[f++]='M';
	ft[f]=0;
	fprintf(pd,"%d/%d/%d/%d/%d/%s/%d\n",p->thrust,p->spin,p->ethrust,
		p->espin,p->step,ft,p->weap);
	for (f=1;f<MAX_WEAPS;f++)
	    if (p->weap_mask&(1<<f))
		fprintf(pd,"W%d/%d\n",f,p->ammo[f]);
	for (f=0;f<9;f++)
	    if (p->slots[f]&&!p->slotobj[f])
		fprintf(pd,"M%d/%d/%d\n",p->slots[f],p->size[f],p->mode[f]);
	for (f=0;f<9;f++)
	    if (p->homeslots[f])
		fprintf(pd,"S%d/%d/%d\n",p->homeslots[f],p->homesize[f],
			p->homemode[f]);
	for (ao=p->firstadd;ao;ao=ao->next)
	    fprintf(pd,"A%d/%d/%d\n",ao->is->type,ao->level,
		    ao->damage);
	fprintf(pd,"\n");
    }
    fprintf(pd,"END\n");
    fclose(pd);
}

extern void load_players() {
    struct player *p;
    int i;
    struct addon *ao;
    FILE *pd;

    if (!(pd=fopen("player.data.file","rb"))) {
	fprintf(stderr,"Cannot open player file for reading\n");
	return;
    }

    while (mygets(pd));
    while (mygets(pd),strcmp(inbuf,"END")) {
	p=alloc_player();
	p->next=playone;
	playone=p;

	p->flags=0;
	p->body.xv=0;p->body.yv=0;p->body.xf=0;p->body.yf=0;p->rv=0;p->rt=0;
	p->playing=0;
	remove_body(&p->body);
	p->weap_mask=(1<<0);
	p->ammo[0]=-1;

	mychop();
	strncpy(p->user,chop[0],16);
	strncpy(p->name,chop[1],31);
	strncpy(p->pass,chop[2],8);
	p->score=atoi(chop[3]);
	p->cash=atoi(chop[4]);
	p->rating=atoi(chop[5]);
	mygets(pd);mychop();
	p->kills=atoi(chop[0]);
	p->tkills=atoi(chop[1]);
	p->deaths=atoi(chop[2]);
	mygets(pd);mychop();
	p->shield=atoi(chop[0]);
	p->maxshield=atoi(chop[1]);
	p->fuel=atoi(chop[2]);
	p->maxfuel=atoi(chop[3]);
	p->homefuel=atoi(chop[4]);
	mygets(pd);mychop();
	p->thrust=atoi(chop[0]);
	p->spin=atoi(chop[1]);
	p->ethrust=atoi(chop[2]);
	p->espin=atoi(chop[3]);
	p->step=atoi(chop[4]);
	for (i=0;chop[5][i];i++)
	    switch (chop[5][i]) {
	    case 'S':p->flags|=FLG_STATUS;break;
	    case 'N':p->flags|=FLG_IDENT;break;
	    case 'W':p->flags|=FLG_NOWEP;break;
	    case 'I':p->flags|=FLG_NOINSTR;break;
	    case 'M':p->flags|=FLG_NOMSG;break;
	    }
	p->weap=atoi(chop[6]);
	while (mygets(pd)) {
	    mychop();
	    chop[0]++;
	    switch(*inbuf) {
	    case 'W':
		i=atoi(chop[0]);
		p->ammo[i]=atoi(chop[1]);
		p->weap_mask|=(1<<i);
		break;
	    case 'M':
		for (i=0;i<9;i++)
		    if (p->slots[i]==OBJ_EMPTY) {
			p->slots[i]=atoi(chop[0]);
			p->size[i]=atoi(chop[1]);
			p->mode[i]=atoi(chop[2]);
			p->slotobj[i]=0;
			i=10;
		    }
		break;
	    case 'S':
		for (i=0;i<9;i++)
		    if (p->homeslots[i]==OBJ_EMPTY) {
			p->homeslots[i]=atoi(chop[0]);
			p->homesize[i]=atoi(chop[1]);
			p->homemode[i]=atoi(chop[2]);
			i=10;
		    }
		break;
	    case 'A':
		i=atoi(chop[0]);
		ao=add_addon(i);
		if (ao) {
		    ao->next=p->firstadd;
		    p->firstadd=ao;
		    ao->level=atoi(chop[1]);
		    ao->damage=atoi(chop[2]);
		    new_addon_level(p,ao);
		}
		break;
	    }
	}
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

static void mychop() {
    char *c;
    int n;
    c=(chop[0]=inbuf);
    n=1;
    while(c=(char *)strchr(c,'/')) {
	*c=0;
	c++;
	chop[n++]=c;
    }
}

extern int can_locate(struct player *p,struct player *q) {
    struct addon *anticlk;
    struct addon *othclk;
    if (!p||!q) return 0;
    if (!q->body.on) return 0;
    if (!(q->flags&FLG_CLOAKING)) return -1;
    othclk=find_addon(q->firstadd,ADD_CLOAKING);
    if (!othclk) return -1;
    if (!(p->flags&FLG_ANTICLOAK)) return 0;
    anticlk=find_addon(p->firstadd,ADD_ANTICLOAK);
    if (othclk->level>anticlk->level) return 0;
    return -1;
}
