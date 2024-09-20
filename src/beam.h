/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/beam.h,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.1 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#ifndef My_BEAM_H
#define My_BEAM_H

#include "player.h"

struct beam {
    int x,y,l;
    int xx,yy;
    int type;
    struct beam *next;
};

#define BEAM_BLUE 0
#define BEAM_RED  1

extern struct beam *firstbeam;
extern struct beam *freebeam;

extern struct beam *alloc_beam();
extern void free_beams();
extern int fire_beam_weapon(struct player *p,struct body *src,double x,
			    double y,int rot,int dam);

#endif
