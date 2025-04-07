/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/newton.h,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.1 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#ifndef My_NEWTON_H
#define My_NEWTON_H

struct body {
	struct body *next;
	struct body *last;
	int l;				// What level at I am
	double x,y;			// What coords am I at?
	double xv,yv;		// What velocity am i going at
	double xf,yf;		// What force is acting upon me
	int type;			// What type of body is this?
	int radius;			// How wide is this body
	int mass;			// What mass does this body have
	int height;			// Height
	int fallen;			// How much falling
	int on;				// Is this body on the map?
	union {
		struct player *player;
		struct trolley *trolley;
	} is;
};

extern struct body *firstbody;

#define BODY_PLAYER   1
#define BODY_TROLLEY  2

void add_body(struct body *b);
void add_pbody(struct player *p);
void remove_body(struct body *b);
int  is_stopped(struct body *b);
void do_collisions();

#endif

