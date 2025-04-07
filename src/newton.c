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

struct body *firstbody = NULL;

void add_body(struct body *b) {
	if (b->on) return;
	b->next=firstbody;
	b->last=0;
	firstbody=b;
	if (b->next) b->next->last=b;
	b->on=1;
}

void add_pbody(struct player *p) {
	p->rv=p->rt=0;
	p->flags&=~(FLG_THRUST|FLG_BRAKING|FLG_ROTCLOCK|FLG_ROTACLOCK|FLG_FIRING);
	add_body(&p->body);
}

void remove_body(struct body *b) {
	if (!b->on) return;
	if (b->last) b->last->next=b->next;
	else firstbody=b->next;
	if (b->next) b->next->last=b->last;
	b->last=b->next=b->on=0;
}

int is_stopped(struct body *b) {
	int x,y,xx,yy;
	if ((b->xv<.1)&&(b->xv>-0.1)&&(b->yv<0.1)&&(b->yv>-0.1)) {
		x=(int) b->x;
		y=(int) b->y;
		xx=x&127;yy=y&127;
		x=x/128;y=y/128;
		if ((xx>32)&&(xx<=96)&&(yy>32)&&(yy<=96))
			return 1;
	}
	return 0;
}

void do_collisions() {
	for (struct body *b=firstbody;b;b=b->next) if (b->on) {
		int bx=b->x,by=b->y; double dx,dy,dr;
		for (struct body *c=b->next;c;c=c->next) if ((c->on)&&(b!=c)&&(b->l==c->l)) {
			int dist = b->radius + c->radius;
			dx=c->x-b->x;dy=c->y-b->y;dr=dx*dx+dy*dy;
			if (dr < (dist*dist)) {
				double st,ct,ub,uc,sep,gap,vb,vc,qa,qb,qc,discrim;
				double bm = b->mass, cm = c->mass;
				/* Move objects to exact point of bounce. */
				double rxv = b->xv - c->xv, ryv = b->yv - c->yv;
				double time = 2; /* (Fix 'cos jon doesnt initialise it) */
				if (!(rxv == 0 && ryv == 0)) {
					qa = rxv*rxv + ryv*ryv;
					qb = 2*(rxv*c->x - rxv*b->x + ryv*c->y - ryv*b->y);
					qc = b->x*b->x + c->x*c->x + b->y*b->y + c->y*c->y -
							2*b->x*c->x - 2*b->y*c->y - dist*dist;
					discrim = qb*qb - 4*qa*qc;
					if (discrim >= 0) { // Only proceed if roots are real
						double sqrt_discrim = sqrt(discrim);
						double t1=(-qb-sqrt_discrim)/(2*qa),t2=(-qb+sqrt_discrim)/(2*qa);
						// Select the smallest valid time within the current step [0, 1]
						if (t1>=0 && t1<=1 && (t2<0 || t1<=t2)) {
							fprintf(stderr, "Collision new t1=%f (old=%f) discrim=%f\n",
									t1, t2, discrim);
							time = t1;
						} else if (t2>=0 && t2<=1) {
							fprintf(stderr, "Collision orig %f discrim=%f\n", t2, discrim);
							time = t2;
						}
					}
				}
				if ((rxv == 0 && ryv == 0) || time > 1) {
					sep = sqrt(dr); gap = dist - sep;
					if (sep == 0) {
						b->x+=gap*cm/(bm+cm); c->x-=gap*bm/(bm+cm);
					} else {
						ct = dx / sep; st = dy / sep;
						b->x -= (gap * cm / (bm + cm)) * ct;
						b->y -= (gap * cm / (bm + cm)) * st;
						c->x += (gap * bm / (bm + cm)) * ct;
						c->y += (gap * bm / (bm + cm)) * st;
					}
				} else {
					b->x-=b->xv*time; b->y-=b->yv*time;
					c->x-=c->xv*time; c->y-=c->yv*time;
					dx=c->x-b->x; dy=c->y-b->y;
					sep = dist;
					if (sep == 0) { /* Er, this is totally impossible*/
						ct=1;st=0;
					} else {
						ct=dx/sep;st=dy/sep;
					}
					ub=b->xv*ct+b->yv*st;uc=c->xv*ct+c->yv*st;
					double momentum=ub*bm+uc*cm,sepspeed=(ub-uc)*RESTITUTION;
					vb=(momentum-sepspeed*cm)/(bm+cm);vc=vb+sepspeed;
#ifdef VERBOSE_DEBUG
					fprintf(stderr,"Value is %d\n",debugval);
					if (b->type==BODY_PLAYER)
						fprintf(stderr,"B is a player -> %s\n",b->is.player->name);
					if (c->type==BODY_PLAYER)
						fprintf(stderr,"C is a player -> %s\n",c->is.player->name);
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
					vb-=ub;vc-=uc;
					b->xv+=ct*vb;b->yv+=st*vb;c->xv+=ct*vc;c->yv+=st*vc;
					/* Do bit of moving for rest of time interval after bounce */
					b->x+=(1-time)*b->xv;b->y+=(1-time)*b->yv;
					c->x+=(1-time)*c->xv;c->y+=(1-time)*c->yv;
				}

				if (b->x<20) b->x=20;
				if (b->y<20) b->y=20;
				if (b->x>=(map.wid*128-20)) b->x=map.wid*128-21;
				if (b->y>=(map.hgt*128-20)) b->y=map.hgt*128-21;

				if (c->x<20) c->x=20;
				if (c->y<20) c->y=20;
				if (c->x>=(map.wid*128-20)) c->x=map.wid*128-21;
				if (c->y>=(map.hgt*128-20)) c->y=map.hgt*128-21;
			} // if dr < dist*dist
		} // c loop

		if (bx<20) bx=20;
		if (by<20) by=20;
		if (bx>=(map.wid*128-20)) bx=map.wid*128-21;
		if (by>=(map.hgt*128-20)) by=map.hgt*128-21;

		switch(rd2(b->l,bx/128,by/128)) {
			case 'O':
			struct turret *tur=find_turret(b->l,bx/128,by/128);
			if (tur->flags&TFLG_DESTROYED) break;
			if (!(dx=(bx&127)-64)) dx=1;
			if (!(dy=(by&127)-64)) dy=1;
			if ((dx*dx+dy*dy)>(45*45)) break;
			b->xf+=16000/dx;b->yf+=16000/dy;
		}
	} // b loop
}

void apply_forces(struct body *b, bool braking) {
	int xc=(int)(b->x/128),yc=(int)(b->y/128);
	switch(rd2(b->l,xc,yc)) {
		case '8':b->yf-=PUSH_FORCE;break;
		case '2':b->yf+=PUSH_FORCE;break;
		case '4':b->xf-=PUSH_FORCE;break;
		case '6':b->xf+=PUSH_FORCE;break;
		case '7':b->xf-=PUSH_FORCES;b->yf-=PUSH_FORCES;break;
		case '1':b->xf-=PUSH_FORCES;b->yf+=PUSH_FORCES;break;
		case '3':b->xf+=PUSH_FORCES;b->yf+=PUSH_FORCES;break;
		case '9':b->xf+=PUSH_FORCES;b->yf-=PUSH_FORCES;break;
		case '5':
			if ((((int)b->x)&127)<50) b->xf-=PUSH_FORCES;
			if ((((int)b->x)&127)>78) b->xf+=PUSH_FORCES;
			if ((((int)b->y)&127)<50) b->yf-=PUSH_FORCES;
			if ((((int)b->y)&127)>78) b->yf+=PUSH_FORCES;
			break;
		case '0':
			if ((((int)b->x)&127)>78) b->xf-=PUSH_FORCES;
			if ((((int)b->x)&127)<50) b->xf+=PUSH_FORCES;
			if ((((int)b->y)&127)>78) b->yf-=PUSH_FORCES;
			if ((((int)b->y)&127)<50) b->yf+=PUSH_FORCES;
			break;
		default:break;
	}
	double oxv=b->xv,oyv=b->yv;
	b->xv+=b->xf/b->mass;b->yv+=b->yf/b->mass;
	b->xf=0;b->yf=0;
	double over=(b->xv*b->xv+b->yv*b->yv)/(MAXVEL*MAXVEL)*1.5;
	if (over>1.0) {b->xv/=over; b->yv/=over;}
	if ((b->height==0)&&(rd2(b->l,xc,yc)&&rd(b->l,xc,yc)!='+')) {
		double friction=FRICTION/500;
		if (rd2(b->l,xc,yc)=='-') friction*=4;
		if (braking) friction*=2;
		b->xv*=(1-friction);b->yv*=(1-friction);
	}
	b->x+=(b->xv+oxv)/2;b->y+=(b->yv+oyv)/2;
	if (b->x<20) b->x=20;
	if (b->y<20) b->y=20;
	if (b->x>=(map.wid*128-20)) b->x=map.wid*128-21;
	if (b->y>=(map.hgt*128-20)) b->y=map.hgt*128-21;
}
