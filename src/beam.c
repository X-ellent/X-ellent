/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/beam.c,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.1 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

 /* Well, snow has hassled me enough and finally got round to half */
 /* writing this so I guess here come beam weapons... Hmmmm.....   */

#include "beam.h"
#include "mymath.h"
#include "turret.h"

struct beam *alloc_beam() {
	struct beam *b;
	if (freebeam) {
		b=freebeam;
		freebeam=b->next;
		b->next=0;
	} else
		b=(struct beam *) calloc(1,sizeof(struct beam));
	return b;
}

void free_beams() {
	struct beam *b;
	if (!firstbeam) return;
	if (freebeam) {
		for (b=freebeam;b->next;b=b->next);
		b->next=firstbeam;
	} else freebeam=firstbeam;
	firstbeam=0;
}

int fire_beam_weapon(struct player *p,struct body *src,double x, double y,int r,int dam) {
	struct body *b, *targ=0;
	struct beam *be;
	struct turret *ttarg=0, *t;
	double xx,yy;
	double near=0,n,m;
	int l=src->l;
	double dx=sn[r], dy=-cs[r];
	for (b=firstbody;b;b=b->next) if ((b->on)&&(b!=src)&&(l==b->l)) {
		xx=x-b->x; yy=y-b->y;
		n=(xx*dy)-(yy*dx);
		if (ABS(n)<b->radius) {
			m=-(xx*dx+yy*dy);
			if (m>0 && ((!targ)||(m<near))) {
				targ=b;
				near=m;
			}
		}
	}
	for (t=firstturret;t;t=t->next)
		if ((!(t->flags&TFLG_DESTROYED))&&(t->d==l)) {
			xx=x-t->x*128-64; yy=y-t->y*128-64;
			n=(xx*dy)-(yy*dx);
			if (ABS(n)<25.0) {
				m=-(xx*dx+yy*dy);
				if (m>0 && (((!targ)&&(!ttarg))||(m<near))) {
					ttarg=t;
					near=m;
				}
			}
		}
	if ((!targ)&&(!ttarg)) {
		be=alloc_beam();
		be->next=firstbeam;
		firstbeam=be;
		near=((map.wid>map.hgt)?map.wid:map.hgt)*128;
		be->l=l;
		be->x=(int) x;
		be->y=(int) y;
		be->xx=(int) x+dx*near;
		be->yy=(int) y+dy*near;
		be->type=(dam)?BEAM_RED:BEAM_BLUE;
		return 0;
	} else {
		be=alloc_beam();
		be->next=firstbeam;
		firstbeam=be;
		be->l=l;
		be->x=(int) x;
		be->y=(int) y;
		be->xx=(int) x+dx*near;
		be->yy=(int) y+dy*near;
		be->type=(dam)?BEAM_RED:BEAM_BLUE;
	}
	if (targ) {
		switch(targ->type) {
		case BODY_PLAYER:
			if (dam==0) return (int)near/1.28;
			damage_player(targ->is.player,dam,p,(p)?DAM_SHOT:DAM_LASER);
			return 0;
			break;
		case BODY_TROLLEY:
			if (dam==0) return (int)near/1.28;
			fire_beam_weapon(p,targ,x+dx*near,y+dy*near,random()%360,dam/3);
			fire_beam_weapon(p,targ,x+dx*near,y+dy*near,random()%360,dam/3);
			fire_beam_weapon(p,targ,x+dx*near,y+dy*near,random()%360,dam/3);
			return 0;
			break;
		default:
			break;
		}
	} else if (ttarg) {
		if (dam==0) return (int)near/1.28;
		damage_turret(ttarg,dam,p);
		return 0;
	}
	return 0;
}
