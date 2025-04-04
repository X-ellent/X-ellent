/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/turret.h,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.7 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#ifndef My_TURRET_H
#define My_TURRET_H

#include "player.h"

struct turret {
	int x,y,d;
	int rot;
	int targ;
	int shield;
	int frame;
	int count;
	int flags;
	struct turret *next;
	struct player *victim;
	char pass[6];
};

extern struct turret *firstturret;

struct turret *add_turret(int d,int x,int y);
void generate_pass(char *p,int n);
void damage_turret(struct turret *t,int d,struct player *who);
void update_turrets();
struct turret *find_turret(int l,int x,int y);

#define TFLG_ACTIVE     (1<<0)
#define TFLG_DESTROYED  (1<<1)
#define TFLG_VINDICTIVE (1<<2)

#endif
