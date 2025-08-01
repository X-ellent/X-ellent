/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/mines.h,v $
** $Author: cheesey $
** $Date: 1992/06/07 00:37:27 $
** $Revision: 1.6 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#ifndef My_MINES_H
#define My_MINES_H

void activate_slot(struct player *p);
void arm_slot(struct player *p);
void disarm_slot(struct player *p);
void detonate_slot(struct player *p);
void explode_mine(struct object *o);

#endif
