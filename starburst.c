/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/starburst.c,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.1 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#include "starburst.h"
#include "beam.h"
#include "mymath.h"

#define STAR_SLEEP_TIME    3000
#define STAR_RECHARGE_TIME 50

extern void fire_starbursts() {
	double x,y;
	int tx,ty;
	int n,n1,n2;
	double dist;
	struct starburst *sb;
	for (sb=firststar;sb;sb=sb->next) {
		if (sb->wait)
			sb->wait--;
		x=sb->body.x+28*sn[sb->rot];
		y=sb->body.y-28*cs[sb->rot];
		switch(sb->mode) {
		case STAR_SCANNING:
			if (sb->sleep<0) {
				sb->sleep++;
				break;
			}
			if (sb->sleep>360) {
				sb->sleep=-STAR_SLEEP_TIME;
				break;
			}
			sb->sleep++;
			if ((n=fire_beam_weapon(0,&sb->body,x,y,sb->rot,0))) {
				dist=n*128/100;
				tx=(x+dist*sn[sb->rot])/128;
				ty=(y-dist*cs[sb->rot])/128;
				if (rd(sb->body.l,tx,ty)!='O') {
					sb->mode=STAR_LOCKING;
					sb->wid=5;
				}
				break;
			}
			if (sb->dir>3) sb->dir=3;
			if (sb->dir<3) sb->dir=-3;
			if (sb->rot<3) sb->rot++;
			sb->rot=(sb->rot+sb->dir+360)%360;
			break;
		case STAR_LOCKING:
			sb->sleep=0;
			n1=fire_beam_weapon(0,&sb->body,x,y,(sb->rot+360-sb->wid)%360,0);
			n=fire_beam_weapon(0,&sb->body,x,y,sb->rot,0);
			n2=fire_beam_weapon(0,&sb->body,x,y,(sb->rot+sb->wid)%360,0);
			if ((n1&&n&&n2)||((!n1)&&n&&(!n2))) {

				if (!sb->wait)
					sb->mode=STAR_LOCKED;
				break;
			}
			if (n1) {
				sb->rot=(sb->rot+((sb->wid>1)?358:359))%360;
				if (sb->wid>1) sb->wid--;
				sb->dir=-3;
				break;};
			if (n2) {
				sb->rot=(sb->rot+((sb->wid>1)?2:1))%360;
				if (sb->wid>1) sb->wid--;
				sb->dir=3;
				break;
			};
			if (!n) {
				if (sb->wid>30) {
					sb->mode=STAR_SCANNING;
					break;
				}
				sb->wid++;
				sb->rot=(sb->rot+sb->dir+360)%360;
			}
			break;
		case STAR_LOCKED:
			fire_beam_weapon(0,&sb->body,x,y,sb->rot,100);
			sb->wait=STAR_RECHARGE_TIME;
			sb->mode=STAR_LOCKING;
			break;
		}
	}
}


