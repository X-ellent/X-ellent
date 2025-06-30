/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/addon.h,v $
** $Author: cheesey $
** $Date: 1993/07/01 14:05:02 $
** $Revision: 1.2 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#ifndef My_ADDON_H
#define My_ADDON_H

#include "constants.h"
#include "player.h"
#include "fix.h"

struct addtype {
	struct addtype *next;	  /* Pointer to next one */
	int type;				  /* What addon type is this? */
	char name[64];			  /* What's it called */
	char subs[4];			  /* What subsystem name is this? */
	int upgrades;			  /* What level of upgrade does it have */
	int damage;				  /* How many damage points can it take */
	int cost[MAX_UPGRADE];	  /* How much does it cost per level */
	int repcost;			  /* What is the repair cost */
};

extern struct addtype *firstaddtype;

struct addon {
	struct addon *next;		 /* Pointer to next one */
	struct addtype *is;		 /* Which kind of thing is this? */
	int level;				 /* What level is it at? */
	int damage;				 /* How many damage points does it have left? */
	int info[MAX_INFO];		 /* What info does it have about it eg. coords*/
};

extern struct addon *freeaddon;

#define ADD_COMPASS			1
#define ADD_CLOAKING		2
#define ADD_INVIS			3
#define ADD_MINESWEEP		4
#define ADD_ENERGY			5
#define ADD_RADAR			6
#define ADD_TARGET			7
#define ADD_FUELTANK		8
#define ADD_SHIELD			9
#define ADD_VISUAL			10
#define ADD_ANTICLOAK		11

void			init_addons();
struct addtype*	find_addtyp(int typ);
struct addon*	find_addon(struct addon *ad,int typ);
int				addon_level(struct addon *ad,int typ);
struct addon*	add_addon(int typ);
void			free_addon(struct addon *a);
struct addon*	strip_addons(struct player *p,struct addon *a);
void			new_addon_level(struct player *p,struct addon *a);
int				addon_command(struct player *p,struct addon *a,uchar *s);

#endif
