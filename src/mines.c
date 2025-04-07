/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/mines.c,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.7 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#include "player.h"
#include "map.h"
#include "mymath.h"
#include "bonus.h"
#include "particle.h"
#include "constants.h"

void activate_slot(struct player *p) {
	struct object *o;
	p->immune=0;
	if ((!p->slot)||(p->slots[p->slot-1]==OBJ_EMPTY)) return;
	if (p->slotobj[p->slot-1]) return;
	switch (p->slots[p->slot-1]) {
	case OBJ_MINE_TRIG:
	case OBJ_MINE_TIME:
	case OBJ_MINE_VELY:
	case OBJ_MINE_PROX:
		o=(p->slotobj[p->slot-1]=alloc_object());
		o->x=p->body.x;
		o->y=p->body.y;
		o->xv=p->body.xv;
		o->yv=p->body.yv;
		o->xv+=2*sn[(int)p->rot]; // Throw it in the direction we're facing
		o->yv-=2*cs[(int)p->rot];
		o->flags=0;
		o->l=p->body.l;
		o->type=p->slots[p->slot-1];
		o->mode=p->mode[p->slot-1];
		if (p->mode[p->slot-1]==-1) o->mode=0;
		if (o->type==OBJ_MINE_TIME) o->mode=o->mode*10+1;
		o->next=obj_first;
		o->slot=p->slot;
		o->has.owner=p;
		o->charge=5*p->size[p->slot-1];
		obj_first=o;
		break;
	}
}

void arm_slot(struct player *p) {
	struct object *o;
	if ((!p->slot)||(p->slots[p->slot-1]==OBJ_EMPTY)) return;
	o=p->slotobj[p->slot-1];
	switch (p->slots[p->slot-1]) {
	case OBJ_MINE_TRIG:
	case OBJ_MINE_TIME:
	case OBJ_MINE_VELY:
	case OBJ_MINE_PROX:
		if ((o)&&(!(o->flags&(OBJ_F_ARM|OBJ_F_ARMING)))) {
			o->flags|=OBJ_F_ARMING;
			o->count=20;
		}
		break;
	default:
		break;
	}
}

void disarm_slot(struct player *p) {
	struct object *o;
	if ((!p->slot)||(p->slots[p->slot-1]==OBJ_EMPTY)) return;
	o=p->slotobj[p->slot-1];
	switch (p->slots[p->slot-1]) {
	case OBJ_MINE_TRIG:
	case OBJ_MINE_TIME:
	case OBJ_MINE_VELY:
	case OBJ_MINE_PROX:
		if (o) o->flags&=(~(OBJ_F_ARMING|OBJ_F_ARM));
		break;
	default:
		break;
	}
}

void detonate_slot(struct player *p) {
	struct object *o;
	if ((!p->slot)||(p->slots[p->slot-1]==OBJ_EMPTY)) return;
	o=p->slotobj[p->slot-1];
	switch (p->slots[p->slot-1]) {
	case OBJ_MINE_TRIG:
	case OBJ_MINE_TIME:
	case OBJ_MINE_VELY:
	case OBJ_MINE_PROX:
		if (o&&(o->flags&OBJ_F_ARM)) {
			o->flags|=OBJ_F_TRIG;
			p->slotobj[p->slot-1]=0;
			o->slot=0;
			p->slots[p->slot-1]=OBJ_EMPTY;
			p->size[p->slot-1]=0;
			p->mode[p->slot-1]=0;
		}
		break;
	}
}

void explode_mine(struct object *o) {
	struct object *n;
	if (obj_first==o) {
		obj_first=o->next;
		o->next=obj_freepool;
		obj_freepool=o;
	} else {
		for (n=obj_first;(n)&&(n->next!=o);n=n->next);
		if (n->next==o) {
			n->next=o->next;
			o->next=obj_freepool;
			obj_freepool=o;
		}
	}
	if (o->type==OBJ_BONUS) {vape_bonus(o);return;}
	if (o->l>=map.depth) return;
	explode(o->l,(int)o->x,(int)o->y,5+o->charge,400*o->charge,20*o->charge,o->has.owner);
}
