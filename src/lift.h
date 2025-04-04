/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/lift.h,v $
** $Author: cheesey $
** $Date: 1992/06/07 00:37:27 $
** $Revision: 1.4 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#ifndef My_LIFT_H
#define My_LIFT_H

#define LIFT_PASS_LENGTH 8

struct lift {
	struct lift *next;
	int x,y;
	int l,t; // current, to
	int id;
	int clk;
	char pass[LIFT_PASS_LENGTH];
};

extern struct lift *firstlift;

#define LIFT_NONE     0
#define LIFT_CALLED   1
#define LIFT_BUSY     2

void add_lift(int d,int x,int y);
struct lift *find_lift(int x, int y);
struct lift *scan_lift(int i);
void move_lifts();
int move_lift_up(struct lift *l);
int move_lift_down(struct lift *l);
int can_lift_ascend(struct lift *l);
int can_lift_descend(struct lift *l);
int summon_lift(int l,int x,int y);

#endif
