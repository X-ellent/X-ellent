/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/targetter.h,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.1 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#ifndef My_TARGETTER_H
#define My_TARGETTER_H

struct coord {
    int l,x,y;
};

struct ptarg {
    struct player *player;
    struct body *body;
};

struct ttarg {
    struct trolley *trolley;
    struct body *body;
};

struct target {
    int type;
    union {
	struct ptarg play;
	struct ttarg trol;
	struct coord coord;
    } is;
};

#define TARG_NONE;
#define TARG_PLAYER;
#define TARG_TROLLEY;
#define TARG_COORD;

#endif
