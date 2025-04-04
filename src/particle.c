/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/particle.c,v $
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

#include "particle.h"
#include "map.h"
#include "turret.h"
#include "constants.h"
#include "mymath.h"
#include "player.h"
#include "debug.h"

struct particle *alloc_particle() {
	struct particle *p=part_free;
	if (p) part_free=p->next;
	else p=(struct particle *) calloc(1,sizeof(struct particle));
	part_count++;
	return p;
}

void free_particle(struct particle *p) {
	p->next=part_free;
	part_free=p;
	part_count--;
}

void fire_particle(struct player *pl,int l,int x,int y,int a,double v,
						  int d,int r,int m) {
	struct particle *p;
	if (a<0) a=(a+360)%360;
	if (pl) {
		pl->body.xf-=v*m*sn[a]/10;pl->body.yf+=v*m*cs[a]/10;
	}
	p=alloc_particle();
	p->x=x;
	p->y=y;
	p->rot=a;
	p->vely=v;
	p->life=r;
	p->dam=d;
	p->mass=m;
	p->owner=pl;
	p->next=parts[l];
	parts[l]=p;
}

void move_particles() {
	struct particle *p; // previous?
	struct particle *t; // this?
	struct particle *n; // next
	struct player *pl;
	struct turret *tu;
	struct trolley *tr;
	double x,y;
	int dx,dy;
	for(int l=0;l<map.depth;l++) {
		if (parts[l]) for (p=0,t=parts[l],n=t->next;t;p=t,t=n,t?n=t->next:0) {
			if ((--t->life)>0) {
				t->x+=t->vely*sn[t->rot];t->y-=t->vely*cs[t->rot];
				if ((t->x<0)||(t->x>=map.wid*128)||(t->y<0)||(t->y>=map.hgt*128))
					t->life=0;
				else {
					switch (rd2(l,(int) t->x/128,(int) t->y/128)) {
					case 'O':
						if (!((tu=find_turret(l,(int) t->x/128,(int) t->y/128))
							  ->flags&TFLG_DESTROYED)) {
							dx=(int) (t->x);dx&=127;dx-=64;
							dy=(int) (t->y);dy&=127;dy-=64;
							if ((dx*dx+dy*dy)<=(25*25)) {
								t->life=-1;
								damage_turret(tu,t->dam,t->owner);
							}
						}
						break;
					}
				}
			} else {
				if (p) p->next=n;
				else parts[l]=n;
				free_particle(t);
				t=p; // Move pointer to previous
			}
		} // loop through parts[]
	} // loop through map levels
	for (pl=playone;pl;pl=pl->next)
		if (pl->body.on) {
			x=pl->body.x;y=pl->body.y;
			for (p=parts[pl->body.l];p;p=p->next) {
				dx=(int) (x-p->x);
				dy=(int) (y-p->y);
				if ((dx*dx+dy*dy)<400) {
					/* Do bullet collision */
					p->life=-1;
					if (!pl->immune) {
						damage_player(pl,p->dam,p->owner,DAM_SHOT); // TODO money here
						pl->body.xf+=p->vely*p->mass*sn[p->rot]/10;
						pl->body.yf-=p->vely*p->mass*cs[p->rot]/10;
					}
				}
			}
		}
	for (tr=firsttrol;tr;tr=tr->next) {
		x=tr->body.x;y=tr->body.y;
		for (p=parts[tr->body.l];p;p=p->next) {
			dx=(int) (x-p->x);
			dy=(int) (y-p->y);
			if ((dx*dx+dy*dy)<400) {
				/* Do bullet collision */
				p->life=-1;
				tr->body.xf+=p->vely*p->mass*sn[p->rot]/10;
				tr->body.yf-=p->vely*p->mass*cs[p->rot]/10;
			}
		}
	}
}

struct explosion *alloc_bang() {
	struct explosion *p=bang_free;
	if (p) bang_free=p->next;
	else p=(struct explosion *) calloc(1,sizeof(struct explosion));
	return p;
}

void explode(int l,int x, int y,int s,int f,int d,struct player *who) {
	struct player *p;
	int dx,dy;
	double r;
	struct explosion *e=alloc_bang();
	e->next=bang_first;bang_first=e;
	e->x=x; /* X pos */
	e->y=y; /* Y pos */
	e->d=l; /* Level */
	e->r=5; /* Current Radius */
	e->a=0; /* Current Age */
	e->s=s; /* Size */
	f=f*s*10;
	for (p=playone;p;p=p->next) if ((!p->immune)&&(p->body.on)&&(p->body.l==l)) {
		dx=x-p->body.x;dy=y-p->body.y;
		if (r=dx*dx+dy*dy, r<=(s*10+5)*(s*10+5)) {
			r+=0.1;
			p->body.xf-=dx*f/r;p->body.yf-=dy*f/r;
			if (d) {
				int dam=d*s*s*10/r;
				if (dam>d) dam=d;
				damage_player(p,dam,who,DAM_EXPL);
			}
		}
	}
	{
		struct object *o,*n=0;
		for (o=obj_first,o?n=o->next:0;o;o=n,n?n=n->next:0)
			if ((o->l==l)&&(o->flags&OBJ_F_ARM)) {
				dx=x-o->x;dy=y-o->y;
				if (r=dx*dx+dy*dy, r<=(s*10+5)*(s*10+5) && r!=0 && f/r>30) {
					if (o->slot) {
						o->has.owner->slots[o->slot-1]=0;
						o->has.owner->slotobj[o->slot-1]=0;
						o->slot=0;
					}
					o->flags|=OBJ_F_CHAIN;
				}
			}
	}
	return;
}

void move_explosions() {
	struct explosion *e,*l=0,*n;
	if (bang_first) for (e=bang_first,n=e->next;e;l=e,e=n,e?n=e->next:0) {
		e->r+=10;
		if (e->a++>e->s) {
			if (l) l->next=e->next;
			else bang_first=e->next;
			e->next=bang_free;
			bang_free=e;
			e=l;
		}
	}
}
