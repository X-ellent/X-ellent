/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/turret.c,v $
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
#include <malloc.h>
#include <string.h>
#include <stdio.h>

#include "fix.h"
#include "constants.h"
#include "turret.h"
#include "player.h"
#include "map.h"
#include "mymath.h"
#include "message.h"
#include "particle.h"

static struct turret *tup;

static void do_active_turret();

struct turret *alloc_turret()
{
	return ((struct turret *) calloc(1,sizeof(struct turret)));
}

extern struct turret *add_turret(int d,int x,int y) {
	struct turret *t;
	t=alloc_turret();
	t->next=firstturret;
	firstturret=t;
	t->x=x;t->y=y;t->d=d;
	t->shield=TURRET_SHIELD;
	t->frame=0;
	t->count=1+random()%TURRET_DELAY;
	t->rot=random()%360;
	t->targ=random()%360;
	t->flags=0;
	generate_pass(t->pass,6);
	return t;
}

extern void generate_pass(char *p,int n) {
	int i;
	for (i=0;i<n;i++)
		p[i]=random()%10+'0';
}

extern void damage_turret(struct turret *t,int d,struct player *who) {
	char txt[80];
	t->shield-=d;
	if (who) who->score+=(d+9)/10;
	if (t->shield<0) {
		t->shield=0;
		t->flags|=TFLG_DESTROYED;
		explode(t->d,t->x*128+64,t->y*128+64,20,8000,150,0);
		t->frame=-TURRET_DIE_COUNT;
		if (who) {
			sprintf(txt,"%s has just destroyed a turret",who->name);
			global_message(txt);
			if ((t->flags&TFLG_VINDICTIVE)&&(t->victim==who)) {
				who->cash+=150;
			} else {
				who->cash+=55;
			}
			who->score+=100;
			who->tkills++;
			if (t->victim==who) {
				if (who->rating<10) who->rating++;
			} else {
				if (who->rating<15) who->rating++;
			}
			t->victim=who;
		}
	}
}

extern void update_turrets() {
	struct turret *t;
	int rd;
	int maxspin;
	int xo,yo;
	if (!firstturret) return; /* No turrets! */
	do_active_turret();
	for (t=firstturret;t;t=t->next) {
		if (t->frame<0) {
			if (!(++t->frame)) {
				t->flags&=~TFLG_DESTROYED;
				t->shield=TURRET_SHIELD;
				t->count=TURRET_DELAY;
			}
		} else {
			maxspin=(t->flags&TFLG_VINDICTIVE)?12:4;
			rd=(t->targ-t->rot+360)%360;
			if (rd>180) {
				if (rd<(360-maxspin)) {
					t->rot+=(360-maxspin);
					t->rot=t->rot%360;
				} else {
					t->rot=t->targ;
				}
			} else {
				if (rd>maxspin) {
					t->rot+=maxspin;
					t->rot=t->rot%360;
				} else {
					t->rot=t->targ;
				}
			}
			if (t->frame>0) {
				t->frame++;
				if (t->frame>=3) {
					t->frame=0;
					if (t->flags&TFLG_VINDICTIVE) {
						t->count=TURRET_DELAY_VINDICTIVE;
					} else {
						t->count=TURRET_DELAY;
					}
				}
			} else {
				if (t->count>0) t->count--;
				if ((t->count<=0)&&(t->flags&TFLG_ACTIVE)) {
					xo=(int) 25.0*sn[t->rot];
					yo=(int) 25.0*cs[t->rot];
					fire_particle(0,t->d,t->x*128+64+xo,t->y*128+64-yo,
								  t->rot,28.0,35,25,750);
					t->frame=1;
					if (t->flags&TFLG_VINDICTIVE) {
						t->count=TURRET_DELAY_VINDICTIVE;
					} else {
						t->count=TURRET_DELAY;
					}
				}
			}
		}
	}
}

static void do_active_turret() {
	struct turret *t;
	struct turret *ot;
	struct player *p;
	struct player *pt;
	int num;
	int td;
	int f,dx,dy;
	double an;
	if (!tup) tup=firstturret;
	ot=tup;
	num=0;
	do {
		t=tup->next;
		if (!t) t=firstturret;
		f=0;
		while (f!=1) {
			if (t==tup) {
				f=1;
			} else {
				if (t->frame>=0) {
					f=1;
				} else {
					t=t->next;
					if (!t) t=firstturret;
				}
			}
		}
		if (t->frame<0) return; /* All turrets destroyed */
		tup=t;
		td=0;
		pt=0;
		for (p=playone;p;p=p->next)
			if ((p->playing)&&(p->body.on)&&(p->body.l==t->d)) {
				t->flags&=~(TFLG_ACTIVE|TFLG_VINDICTIVE);
				dx=(int) p->body.x-t->x*128-64;
				dy=(int) p->body.y-t->y*128-64;
				f=dx*dx+dy*dy;
				if (p==t->victim) f=f/TURRET_VICTIM_BIAS;
				if ((!td)||(f<td)) {
					td=f;
					if (f!=0) {
						an=atan2((double)dx,(double)-dy);
						an=an*180/PI;
						t->targ=(int) an;
						t->targ+=360;
						t->targ=t->targ%360;
						pt=p;
						t->flags|=TFLG_ACTIVE;
					}
				}
			}
		if (pt&&(pt==t->victim)) t->flags|=TFLG_VINDICTIVE;
	} while ((++num<TURRET_MAXNUM)&&(tup!=ot));
}


extern struct turret *find_turret(int l,int x,int y) {
	struct turret *t;
	for (t=firstturret;t;t=t->next)
		if ((t->x==x)&&(t->y==y)&&(t->d==l)) return t;
	return 0;
}
