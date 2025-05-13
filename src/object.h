/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/object.h,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.7 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#ifndef My_OBJECT_H
#define My_OBJECT_H

#include "newton.h"

struct object {
	int type;
	struct object *next;
	int l;
	double x,y;
	double xv,yv;
	double xf,yf;
	int count;
	int mode;
	int flags;
	int charge;
	union {
		struct player *owner;
		struct object *same;
	} has;
	int slot;
};

struct trolley {
	struct body body;
	struct trolley *next;
	struct player *holder;
	int falltime;
	int ang;
	struct checkpoint *cp;
};

extern struct trolley *firsttrol;
extern struct trolley thetrol;
extern struct trolley thetrolb;
extern struct trolley thetrolc;
extern struct trolley thetrold;

extern struct object *obj_first;
extern struct object *obj_freepool;

extern int obj_used;
extern int obj_free;

#define OBJ_EMPTY			0
#define OBJ_MINE_TRIG		1
#define OBJ_MINE_TIME		2
#define OBJ_MINE_PROX		3
#define OBJ_MINE_VELY		4
#define OBJ_MINE_SMART		5
#define OBJ_BONUS			6

#define OBJ_F_ARM		 (1<<0)
#define OBJ_F_ARMING	 (1<<1)
#define OBJ_F_TRIG		 (1<<2)
#define OBJ_F_FLASH		 (1<<3)
#define OBJ_F_EXPLODE	 (1<<4)
#define OBJ_F_CHAIN		 (1<<5)

struct object *alloc_object();
void move_objects();
void take_hold(struct player *p);
void update_trolleys();
void init_all_trolleys();
void create_trolley(struct trolley *tr);

#endif
