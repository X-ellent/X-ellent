/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/message.c,v $
** $Author: cheesey $
** $Date: 1992/06/07 00:37:27 $
** $Revision: 1.6 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#include "string.h"

#include "message.h"
#include "player.h"

extern void global_message(char *s)
{
	struct player *p;
	for (p=playone;p;p=p->next) {
	strcpy(p->msg[3],p->msg[2]);
	strcpy(p->msg[2],p->msg[1]);
	strcpy(p->msg[1],p->msg[0]);
	strncpy(p->msg[0],s,59);
	}
}

extern void player_message(struct player *p,char *s)
{
	strcpy(p->msg[3],p->msg[2]);
	strcpy(p->msg[2],p->msg[1]);
	strcpy(p->msg[1],p->msg[0]);
	strncpy(p->msg[0],s,59);
}

