/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/draw.h,v $
** $Author: cheesey $
** $Date: 1992/06/07 00:37:27 $
** $Revision: 1.6 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#ifndef My_DRAW_H
#define My_DRAW_H

extern void draw_all(struct player *p);
extern void draw_map_level(struct player *p,int l);
extern void DrawMeter(struct player *p,int x,int y,int l,int h,int mx,int val);

#endif
