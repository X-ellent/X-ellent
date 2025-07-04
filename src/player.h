/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/player.h,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.7 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/


#ifndef My_PLAYER_H
#define My_PLAYER_H

#include "constants.h"
#include "xbits.h"
#include "object.h"
#include "map.h"
#include "newton.h"
#include "terminal.h"
#include "damages.h"
#include "fix.h"

struct player {
	char name[32];			// What name do I want to go by?
	char user[16];			// Whats my usercode
	char pass[9];			// Whats the password
	struct player_display d;// What display am I on?
	double rot;				// How far am i rotated?
	struct body body;		// Details of me
	double ox,oy;			// Next to last place on ground
	double oox,ooy;			// Last place on ground
	int olvl;				// Next to last level on ground
	int oolvl;				// Last level on ground on
	int onground;			// How long since last on ground
	double rv;				// Rotational velocity
	double rt;				// Target rotational velocity
	int rating;				// What is my rating??
	int score;				// What is my score?
	int cash;				// How much cash do i have
	int step;				// What step do i alter things in
	int thrust,ethrust;		// What are my settings
	int spin,espin;
	int fuel,maxfuel;		// How much fuel do i have from what
	int flags;				// What flags do i have set
	struct addon *firstadd;
	struct player *next;
	struct home *home;
	struct home *oldhome;
	int shield;
	int maxshield;
	int scount;
	int weap;
	int ammo[MAX_WEAPS];
	int weap_mask;
	int playing;
	int connected;
	int qflags;
	int slots[9];
	int size[9];
	int mode[9];
	struct object *slotobj[9];
	int slot;
	int delay;			// Time until we respawn from death */
	struct menu *shopmenu;
	struct login *term;
	int shopsel;
	int immune;
	struct player *ptarg;
	char msg[4][60];
	int deaths;
	int kills;
	int tkills;
	int homefuel;
	int homeslots[9];
	int homesize[9];		// TODO - what is this?
	int homemode[9];		// TODO - what is this?
	int fuelmin;
	struct player *lasthold;
	struct player *nexthold;
	struct trolley *holding;
	int channel;			// For the telnet connection thing
	unsigned char lines[64][4];
	int locate,loctime,tartime;
	int homing;				// Time for nicking home base
	int recharge;			// Time until can fire again
};

extern struct player *playone;
extern struct player *freeplay;

extern char *weap_name[MAX_WEAPS];

extern int frame;
extern int players;

#define FLG_THRUST			(1<<0)
#define FLG_BRAKING			(1<<1)
#define FLG_ROTCLOCK		(1<<2)
#define FLG_ROTACLOCK		(1<<3)
#define FLG_HOME			(1<<4)
#define FLG_IDENT			(1<<5)
#define FLG_STATUS			(1<<6)
#define FLG_CLOAKING		(1<<7)
#define FLG_NOMSG			(1<<8)
#define FLG_FIRING			(1<<9)
#define FLG_NOWEP			(1<<10)
#define FLG_MINESWEEP		(1<<11)
#define FLG_NOINSTR			(1<<12)
#define FLG_DEAD			(1<<12)
#define FLG_INVIS			(1<<13)
#define FLG_TERMINAL		(1<<14)
#define FLG_SHOPPING		(1<<15)
#define FLG_FUELLING		(1<<16)
#define FLG_INWINDOW		(1<<17)
#define FLG_FALLEN			(1<<18)
#define FLG_CTRL			(1<<19)
#define FLG_ANTICLOAK		(1<<20)

#define FLG_DEADCLR			(FLG_THRUST|FLG_BRAKING|FLG_ROTCLOCK|FLG_FIRING|\
							 FLG_ROTACLOCK|FLG_CLOAKING|FLG_MINESWEEP|\
							 FLG_HOME|FLG_INVIS|FLG_TERMINAL|FLG_SHOPPING|\
							 FLG_FUELLING|FLG_FALLEN|FLG_DEAD|FLG_ANTICLOAK)

#define WEP_RIF		  0
#define WEP_LMG		  1
#define WEP_HMG		  2
#define WEP_MORTAR	  3
#define WEP_FLAME	  4
#define WEP_RMG		  5
#define WEP_PRONG	  6
#define WEP_TARGLASER 7
#define WEP_LASER	  8

void damage_player(struct player *p,int d,struct player *who,int typ);
int setup_player();
void update_player(struct player *p);
void init_all_weap();
struct player *find_player(char *n);
void save_players();
void load_players();
void next_target(struct player *p);
bool can_locate(struct player *p,struct player *t);

#endif
