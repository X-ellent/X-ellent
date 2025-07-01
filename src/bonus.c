/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/bonus.c,v $
** $Author: cheesey $
** $Date: 1993/07/01 14:05:02 $
** $Revision: 1.8 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#include <stdio.h> // For sprintf()
#include <stdlib.h> // For random()

#include "map.h"
#include "player.h"
#include "message.h"
#include "particle.h"

static struct object *firstbonus;
static int bonuscount;
static char txt[256];

static void add_bonus() {
	struct object *o=alloc_object();
	o->next=obj_first;
	obj_first=o;
	o->type=OBJ_BONUS;
	o->l=random()%map.depth;
	o->x=(double)(random()%(map.wid-2)*128+192);
	o->y=(double)(random()%(map.hgt-2)*128+192);
	o->xv=(random()%100-50)/10;
	o->yv=(random()%100-50)/10;
	o->xf=0;o->yf=0;
	o->flags=OBJ_F_TRIG;
	o->count=BONUS_AGE+random()%BONUS_RAND;
	o->has.same=firstbonus;
	firstbonus=o;
}

void vape_bonus(struct object *o) {
	struct object *n;
	if (firstbonus==o)
		firstbonus=o->has.same;
	else {
		for (n=firstbonus;(n)&&(n->has.same!=o);n=n->has.same);
		if ((n)&&(n->has.same==o)) n->has.same=o->has.same;
	}
	bonuscount=BONUS_TIME;
}

void update_bonus() {
	for (struct object *o=firstbonus;o;o=o->has.same) {
		o->count--;
		if (o->count<0) o->flags|=OBJ_F_EXPLODE;
		if (o->count==BONUS_SHOUT) {
			sprintf(txt,"** BONUS On level %d at %d,%d!! **",o->l,
					(int)o->x/128,(int)o->y/128);
			global_message(txt);
		}
	}
	bonuscount++;
	if (bonuscount>BONUS_TIME) {
		add_bonus();
		bonuscount=random()%BONUS_RAND;
	}
}

static void get_bogus(struct player *p) {
	int s; char *txt;
	if (random()%10==1) {
		txt="Most heinous! Mega BOGUS!";
		damage_player(p,1000,0,DAM_BOGUS);
		s=2;
	} else {
		if ((random()%12)==1) {
			damage_player(p,p->shield,0,DAM_BOGUS);
			txt="BOGUS!!!! Most un-excellent dude!";
			s=1;
		} else {
			damage_player(p,100+random()%200,0,DAM_BOGUS);
			txt="Oh no dude! BOGUS!!!!";
			s=0;
		}
	}
	explode(p->body.l,(int) p->body.x,(int) p->body.y,8+8*s,400*s,0,0);
	if (txt) player_message(p,txt);
}

void get_bonus(struct player *);

static void get_bonus_mine(struct player *p) {
	for (int i=0;i<9;i++) if (p->slots[i]==OBJ_EMPTY) {
		switch(random()%16) {
			case 0:p->size[i]=4;break;
			case 1:
			case 2:p->size[i]=3;break;
			case 3:
			case 4:
			case 5:
			case 6:p->size[i]=2;break;
			default:p->size[i]=1;break;
		}
		switch(random()%4) {
			case 0:p->slots[i]=OBJ_MINE_TRIG;p->mode[i]=0;break;
			case 1:p->slots[i]=OBJ_MINE_TIME;p->mode[i]=10;break;
			case 2:p->slots[i]=OBJ_MINE_PROX;p->mode[i]=20+5*p->size[i];break;
			case 3:p->slots[i]=OBJ_MINE_VELY;p->mode[i]=50;break;
		}
		player_message(p,"A bonus mine!!");
		return;
	}
	get_bonus(p);
}

static void get_bonus_ammo(struct player *p) {
	int i=p->weap;
	if (i) {
		int n=random()%200+random()%200+random()%200;
		n=n/i;
		p->ammo[i]+=n;
		player_message(p,"Bonus Ammunition!");
		return;
	}
	get_bonus(p);
}

void get_bonus(struct player *p) {
	char *txt=0;
	int r=random()%20;
	switch (r) {
	case 0:p->cash+=500;txt="BONUS!!!! Unprecedented cash!";break;
	case 1:p->cash+=80;txt="BONUS!!!! Cash!";break;
	case 2:p->cash+=90;txt="BONUS!!!! Cash!";break;
	case 3:p->cash+=100;txt="Excellent! Bonus Cash!";break;
	case 4:p->cash+=110;txt="Excellent! Bonus Cash!";break;
	case 5:p->cash+=120;txt="Excellent! Bonus Cash!";break;
	case 6:p->cash+=150;txt="Excellent! Bonus Cash!";break;
	case 7:p->cash+=200;txt="Excellent! Bonus Cash!";break;
	case 8:p->cash+=(random()%500+100);txt="BONUS!!!! A stash of cash!";break;
	case 9:p->fuel+=20*200;txt="Wow! Some fuel!";break;
	case 10:p->fuel+=80*200;txt="Wow! Some fuel!";break;
	case 11:p->fuel+=150*200;txt="Wow! Some fuel!";break;
	case 12:p->fuel+=400*200;txt="Wow Excellent! Load of fuel!";break;
	case 13:get_bonus_mine(p);break;
	case 14:
	case 15:get_bonus_ammo(p);break;
	case 16:
	case 17:get_bonus(p);break;
	case 18:
	case 19:get_bogus(p);break;
	}
	if (p->fuel>p->maxfuel) p->fuel=p->maxfuel;
	if (txt) player_message(p,txt);
	bonuscount=BONUS_TIME;
}