/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/newton.c,v $
** $Author: cheesey  (and Jonathan ;) )$
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.1 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** (Apart from the bits written by Jonathan Sloman (maucl@csv) to make it work)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) (and me!) 8-)
*/

/* coefficient of restitution in collisions, should be <= 1 */
#define RESTITUTION 0.98

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "fix.h"
#include "newton.h"
#include "mymath.h"
#include "damages.h"
#include "player.h"
#include "turret.h"

extern void add_body(struct body *b) {
    if (b->on) return;
    b->next=firstbody;
    b->last=0;
    firstbody=b;
    if (b->next) b->next->last=b;
    b->on=-1;
}

extern void remove_body(struct body *b) {
    if (!b->on) return;
    if (b->last) {
	b->last->next=b->next;
    } else {
	firstbody=b->next;
    }
    if (b->next) {
	b->next->last=b->last;
    }
    b->last=0;
    b->next=0;
    b->on=0;
}

extern int is_stopped(struct body *b) {
    int x,y,xx,yy;
    if ((b->xv<.1)&&(b->xv>-0.1)&&(b->yv<0.1)&&(b->yv>-0.1)) {
	x=(int) b->x;
	y=(int) b->y;
	xx=x&127;yy=y&127;
	x=x/128;y=y/128;
	if ((xx>32)&&(xx<=96)&&(yy>32)&&(yy<=96))
	    return -1;
    }
    return 0;
}

extern void do_collisions()
{
    struct body *b;
    struct body *c;
    struct turret *tur;
    int cx,cy;
    int dist;
    double dx,dy,dr;
    double st,ct;
    double ub,uc;
    double sep;
    double vb,vc;
    double qa,qb,qc;
    double discrim;
    double maxval,minval;
    double momentum,sepspeed;
    double gap,gb,gc;
    /* new vars */
    double dif1, dif2;
    double time;
    double rxv, ryv;
    int bm, cm;
    int debugval;
    for (b = firstbody; b; b = b->next)
	if (b->on) {
	    cx = b->x;
	    cy = b->y;
	    for (c = b->next; c; c = c->next)
		if ((b != c) && (c->on) && (b->l == c->l)) {
		    dist = b->radius + c->radius;
		    dx = c->x - b->x;
		    dy = c->y - b->y;
		    dr = dx * dx + dy * dy;
		    if (dr < (dist * dist)) {
			bm = b->mass;
			cm = c->mass;
			/* Move objects to exact point of bounce. */
			rxv = b->xv - c->xv;
			ryv = b->yv - c->yv;
			time = 2; /* (Fix 'cos jon doesnt initialise it) */
			if (!(rxv == 0 && ryv == 0)) {
			    qa = rxv * rxv + ryv * ryv;
			    qb = 2*(rxv*c->x - rxv*b->x + ryv*c->y - ryv*b->y);
			    qc = b->x*b->x + c->x*c->x + b->y*b->y + c->y*c->y - 
				2*b->x*c->x - 2*b->y*c->y - dist*dist;
			    discrim = qb * qb - 4 * qa * qc;
			    if (discrim < 0)
				time = 2; /* random value which is > 1 */
			    else
				time = (-qb + sqrt(discrim)) / (2*qa);
			}
			if ((rxv == 0 && ryv == 0) || time > 1) {
			    sep = sqrt(dr);
			    gap = dist - sep + 1;
			    if (sep == 0) {
				b->x += gap * cm / (bm + cm);
				c->x -= gap * bm / (bm + cm);
			    } else {
				ct = dx / sep;
				st = dy / sep;
				b->x -= (gap * cm / (bm + cm)) * ct;
				b->y -= (gap * cm / (bm + cm)) * st;
				c->x += (gap * bm / (bm + cm)) * ct;
				c->y += (gap * bm / (bm + cm)) * st;
			    }
			} else {
			    b->x -= b->xv * time;
			    b->y -= b->yv * time;
			    c->x -= c->xv * time;
			    c->y -= c->yv * time;
			    dx = c->x - b->x;
			    dy = c->y - b->y;
			    sep = dist;
			    if (sep == 0) { /* Er, this is totally impossible*/
				ct=1;
				st=0;
			    } else {
				ct = dx / sep;
				st = dy / sep;
			    }
			    ub = b->xv * ct + b->yv * st;
			    uc = c->xv * ct + c->yv * st;
			    momentum = ub * bm + uc * cm;
			    sepspeed = - (ub - uc) * RESTITUTION;
			    vb = (momentum + sepspeed*cm) / (bm + cm);
			    vc = vb - sepspeed;
#ifdef VERBOSE_DEBUG
			    fprintf(stderr,"Value is %d\n",debugval);
			    if (b->type==BODY_PLAYER)
				fprintf(stderr,"B is a player -> %s\n",
					b->is.player->name);
			    if (c->type==BODY_PLAYER)
				fprintf(stderr,"C is a player -> %s\n",
					c->is.player->name);
			    fprintf(stderr,"Collision: B x  : %f\n",b->x);
			    fprintf(stderr,"             y  : %f\n",b->x);
			    fprintf(stderr,"             xv : %f\n",b->xv);
			    fprintf(stderr,"             yv : %f\n",b->yv);
			    fprintf(stderr,"Collision: C x  : %f\n",c->x);
			    fprintf(stderr,"             y  : %f\n",c->x);
			    fprintf(stderr,"             xv : %f\n",c->xv);
			    fprintf(stderr,"             yv : %f\n",c->yv);
			    fprintf(stderr,"\n");
			    fprintf(stderr,"Difference   x  : %f\n",dx);
			    fprintf(stderr,"             y  : %f\n",dy);
			    fprintf(stderr,"             l  : %f\n",sep);
			    fprintf(stderr,"\n");
			    fprintf(stderr,"Velocities   B  : %f\n",ub);
			    fprintf(stderr,"             C  : %f\n",uc);
			    fprintf(stderr,"Momentum        : %f\n",momentum);
			    fprintf(stderr,"Energy          : %f\n",energy);
			    fprintf(stderr,"\n");
			    fprintf(stderr,"Quadratic     a : %f\n",qa);
			    fprintf(stderr,"              b : %f\n",qb);
			    fprintf(stderr,"              c : %f\n",qc);
			    fprintf(stderr,"   Discriminant : %f\n",discrim);
			    fprintf(stderr,"       Root min : %f\n",minval);
			    fprintf(stderr,"       Root max : %f\n",maxval);
	      
			    fprintf(stderr,"\n");
			    fprintf(stderr,"New Velocity vb : %f\n",vb);
			    fprintf(stderr,"             vc : %f\n",vc);
			    fprintf(stderr,"\n");
			    exit(1);
#endif
			    vb -= ub;
			    vc -= uc;
			    b->xv += ct * vb;
			    b->yv += st * vb;
			    c->xv += ct * vc;
			    c->yv += st * vc;
			    /* Do bit of moving for rest of time interval after bounce */
			    b->x += (1 - time) * b->xv;
			    b->y += (1 - time) * b->yv;
			    c->x += (1 - time) * c->xv;
			    c->y += (1 - time) * c->yv;
		  
			}

			if (b->x<20) b->x=20;
			if (b->y<20) b->y=20;
			if (b->x>=(map.wid*128-20)) b->x=map.wid*128-21;
			if (b->y>=(map.hgt*128-20)) b->y=map.hgt*128-21;

			if (c->x<20) c->x=20;
			if (c->y<20) c->y=20;
			if (c->x>=(map.wid*128-20)) c->x=map.wid*128-21;
			if (c->y>=(map.hgt*128-20)) c->y=map.hgt*128-21;
		    }
		}

	    if (cx<20) cx=20;
	    if (cy<20) cy=20;
	    if (cx>=(map.wid*128-20)) cx=map.wid*128-21;
	    if (cy>=(map.hgt*128-20)) cy=map.hgt*128-21;

	    switch(rd2(b->l,cx/128,cy/128)) {
	    case 'O':
		tur=find_turret(b->l,cx/128,cy/128);
		if (tur->flags&TFLG_DESTROYED) break;
		if (!(dx=(cx&127)-64)) dx=1;
		if (!(dy=(cy&127)-64)) dy=1;
		if ((dx*dx+dy*dy)>(45*45)) break;
		b->xf+=16000/dx;
		b->yf+=16000/dy;
	    }
	}
}

