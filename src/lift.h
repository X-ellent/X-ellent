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
struct lift {
    struct lift *next;
    int x,y;
    int l,t;
    int id;
    int clk;
    char pass[8];
};

struct lift *firstlift;

#define LIFT_NONE     0
#define LIFT_CALLED   1
#define LIFT_BUSY     2

extern void add_lift(int d,int x,int y);
extern struct lift *find_lift(int x, int y);
extern struct lift *scan_lift(int i);
extern void move_lifts();
extern int move_lift_up(struct lift *l);
extern int move_lift_down(struct lift *l);
extern void move_lift_to(struct lift *l,int d);
extern int can_lift_ascend(struct lift *l);
extern int can_lift_descend(struct lift *l);
extern int summon_lift(int l,int x,int y);

#endif
