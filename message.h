/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/message.h,v $
** $Author: cheesey $
** $Date: 1992/06/07 00:37:27 $
** $Revision: 1.6 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#ifndef My_MESSAGE_H
#define My_MESSAGE_H

#include "player.h"

extern void global_message(char *s);
extern void player_message(struct player *p,char *s);

#endif
