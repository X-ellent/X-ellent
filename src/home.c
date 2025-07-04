/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/home.c,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.7 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#include <string.h>
#include <stdio.h>
#include <malloc.h>

#include "player.h"
#include "map.h"
#include "shop.h"
#include "fix.h"
#include "object.h"
#include "constants.h"
#include "home.h"
#include "draw.h"
#include "xsetup.h"
#include "message.h"

static void show_home(struct player *p);

void add_home(int l,int x,int y) {
	struct home *h;
	h=(struct home *) calloc(1,sizeof(struct home));
	h->next=firsthome;
	firsthome=h;
	h->x=x;h->y=y;h->l=l;
	h->owner=0;
}

void exit_home(struct player *p) {
	char txt[256];
	add_pbody(p);
	p->flags&=~FLG_HOME;
	p->immune=START_IMMUNE_REJ;
	snprintf(txt,sizeof(txt),"#HOME EXIT %d:%d,%d\n",p->body.l,(int)p->body.x,
					(int)p->body.y);
	psend(p, txt);
}

void init_home(struct player *p) {
	p->flags&=(~(FLG_THRUST|FLG_BRAKING|FLG_ROTCLOCK|FLG_ROTACLOCK|FLG_FIRING));
	p->flags|=FLG_HOME; p->flags&=~FLG_CTRL;
	int f=p->fuelmin-p->fuel;
	f = f < 0 ? 0 : (f > p->homefuel ? p->homefuel : f);
	p->fuel+=f; p->homefuel-=f;
	remove_body(&p->body);
	psend(p,"#HOME ENTER\n");
	show_home(p);
}

static void show_home(struct player *p) {
	char txt[80];
	Display *d=p->d.disp;
	Pixmap w=p->d.backing;
	GC red=p->d.gc_red,dgrey=p->d.gc_dgrey;
	XFillRectangle(p->d.disp,p->d.backing,p->d.gc_black,0,0,WINWID,WINHGT);
	XDrawString(d,w,red,50,p->d.fh,"Fuel",4);
	XDrawString(d,w,dgrey,50,p->d.fh*2,"Base",4);
	DrawMeter(p,50+p->d.fw*7,p->d.fh,WINWID/2,4,p->maxfuel/200,p->fuel/200);
	DrawMeter(p,50+p->d.fw*7,p->d.fh*2,WINWID/2,4,p->maxfuel/50,p->homefuel/200);
	XDrawString(d,w,red,10,p->d.fh*4,"#",1);
	XDrawString(d,w,red,40,p->d.fh*4,"Contains",8);
	for (int i=0;i<9;i++) {
		txt[0]='1'+i;
		XDrawString(d,w,dgrey,10,p->d.fh*(i+5),txt,1);
		if (p->slotobj[i]) XDrawString(d,w,dgrey,23,p->d.fh*(i+5),
							(p->slotobj[i]->flags&OBJ_F_ARM)?"A":"D",1);
		switch(p->slots[i]) {
			case OBJ_EMPTY:strcpy(txt,"Slot Empty");break;
			case OBJ_MINE_TRIG:strcpy(txt,"Mine (Trig)");break;
			case OBJ_MINE_TIME:strcpy(txt,"Mine (Time)");break;
			case OBJ_MINE_PROX:strcpy(txt,"Mine (Prox)");break;
			case OBJ_MINE_VELY:strcpy(txt,"Mine (Vely)");break;
			case OBJ_MINE_SMART:strcpy(txt,"Mine (Smart)");break;
		}
		switch(p->size[i]) {
			case 1:strcat(txt," <Small>");break;
			case 2:strcat(txt," <Medium>");break;
			case 3:strcat(txt," <Large>");break;
			case 4:strcat(txt," <Extra>");break;
		}
		XDrawString(d,w,dgrey,40,p->d.fh*(i+5),txt,strlen(txt));
	}
	XDrawString(d,w,red,WINWID/2+10,p->d.fh*4,"#",1);
	XDrawString(d,w,red,WINWID/2+40,p->d.fh*4,"Stored",6);
	for (int i=0;i<9;i++) {
		txt[0]='1'+i;
		XDrawString(d,w,dgrey,WINWID/2+10,p->d.fh*(i+5),txt,1);
		switch(p->homeslots[i]) {
			case OBJ_EMPTY:strcpy(txt,"Slot Empty");break;
			case OBJ_MINE_TRIG:strcpy(txt,"Mine (Trig)");break;
			case OBJ_MINE_TIME:strcpy(txt,"Mine (Time)");break;
			case OBJ_MINE_PROX:strcpy(txt,"Mine (Prox)");break;
			case OBJ_MINE_VELY:strcpy(txt,"Mine (Vely)");break;
			case OBJ_MINE_SMART:strcpy(txt,"Mine (Smart)");break;
		}
		switch(p->homesize[i]) {
			case 1:strcat(txt," <Small>");break;
			case 2:strcat(txt," <Medium>");break;
			case 3:strcat(txt," <Large>");break;
			case 4:strcat(txt," <Extra>");break;
		}
		XDrawString(d,w,dgrey,WINWID/2+40,p->d.fh*(i+5),txt,strlen(txt));
	}
}

void do_home(struct player *p) {
	int f,c=0;
	if (p->flags&FLG_FUELLING) {
		if (p->flags&FLG_CTRL) {
			if (p->homefuel<p->maxfuel*4) {
				c=1; f=p->fuel;
				if (f>p->maxfuel/100) f=p->maxfuel/100;
				p->fuel-=f; p->homefuel+=f;
				if (p->homefuel>p->maxfuel*4) p->homefuel=p->maxfuel*4;
			}
		} else if (p->fuel<p->maxfuel) {
			c=1; f=p->homefuel;
			if (f>2000) f=2000;
			p->fuel+=f; p->homefuel-=f;
			if (p->fuel>p->maxfuel) p->fuel=p->maxfuel;
		}
	}
	if (p->qflags&16) {
		home_quit(p); // TODO - Why don't we do this in process_events?
		return;
	}
	if (c) show_home(p);
}

void home_mine(struct player *p,int s) {
	int i;
	if (p->flags&FLG_CTRL) {
		if (p->slotobj[s]||(p->slots[s]==OBJ_EMPTY)) return;
		for (i=0;i<9;i++) if (p->homeslots[i]==OBJ_EMPTY) {
			p->homeslots[i]=p->slots[s];
			p->homesize[i]=p->size[s];
			p->homemode[i]=p->mode[s];
			p->slots[s]=OBJ_EMPTY;
			p->size[s]=0;
			p->mode[s]=0;
			show_home(p);
			return;
		}
	} else {
		if (p->homeslots[s]==OBJ_EMPTY) return;
		for (i=0;i<9;i++) if (p->slots[i]==OBJ_EMPTY) {
			p->slots[i]=p->homeslots[s];
			p->size[i]=p->homesize[s];
			p->mode[i]=p->homemode[s];
			p->homeslots[s]=OBJ_EMPTY;
			p->homesize[s]=0;
			p->homemode[s]=0;
			show_home(p);
			return;
		}
	}
}

void home_quit(struct player *p) {
	char txt[128];
	p->playing=0;
	p->home->owner=0;
	p->oldhome=p->home; // Only remembered as long as the server is running
	p->home=0;
	remove_body(&p->body);
	p->flags&=~FLG_DEADCLR;
	XAutoRepeatOn(p->d.disp);
	p->qflags=0;
	shutdown_display(p);
	players--; // No longer connected AND playing
	snprintf(txt,sizeof(txt),"%s has just quitted",p->name);
	global_message(txt);
	psend(p,"#HOME QUIT\n");
	return;
}

void go_home(struct player *p) {
	int x=(int)p->body.x/128,y=(int)p->body.y/128;
	if (!is_stopped(&p->body)) {
		player_message(p,"You must be stopped on your home base to go home");
		return;
	}
	if (rd2(p->body.l,x,y)!='H') {
		player_message(p,"Errr.. this isnt a home base");
		return;
	}
	if ((x!=p->home->x)||(y!=p->home->y)) {
		player_message(p,"This is not YOUR home base!");
		return;
	}
	if (p->homing) {
		player_message(p,"Patience!!!");
		return;
	}
	if (p->holding) {
		struct trolley *tr=p->holding;
		p->cash+=tr->body.mass;
		for(struct player *o=tr->holder;o;o=o->nexthold) {
			if (o->lasthold) {
				o->lasthold->nexthold=0;
				o->lasthold=0;
			}
			o->holding=0;
		}
		tr->holder=0;
		tr->body.xv=0;tr->body.yv=0;
		tr->body.xf=0;tr->body.yf=0;
		create_trolley(tr);
	}
	init_home(p);
}

struct home *pick_home() {
	struct home *h;
	int free=0,chosen;
	for (h=firsthome;h;h=h->next) if (!h->owner) free++;
	if (!free) return 0;
	chosen=random()%free;
	for (h=firsthome;h;h=h->next) if (!h->owner && !chosen--) return h;
	return 0;
}

struct home *locate_home(int l,int x,int y) {
	struct home *h;
	for (h=firsthome;h;h=h->next)
		if ((h->l==l)&&(h->x==x)&&(h->y==y)) return h;
	return 0;
}

void take_home(struct player *p) {
	struct home *h;
	char txt[256];
	h=locate_home(p->body.l,(int)p->body.x/128,(int)p->body.y/128);
	if (!h) return;
	if (p->homing) return;
	if (h->owner==p) {
		player_message(p,"This IS your home base!");
		return;
	}
	if (!h->owner) {
		p->homing=HOME_TAKE_TIME;
		player_message(p,"You take this home base..");
		p->home->owner=0;
		p->home=h;
		p->oldhome=h;
		h->owner=p;
		sprintf(txt,"#HOME NEW %d:%d,%d\n",h->l,h->x*128+64,h->y*128+64);
		psend(p, txt);
		return;
	} else {
		if (h->owner->rating>=p->rating) {
			player_message(p,"You cannot take that home base!");
			player_message(h->owner,(sprintf(txt,"%s just tried to steal your home base",p->name),txt));
			return;
		}
		p->homing=HOME_TAKE_TIME;
		player_message(p,(sprintf(txt,"You steal this home base from %s.",
				h->owner->name),txt));
		player_message(h->owner,(sprintf(txt,"%s has stolen your home base.",
				p->name),txt));
		player_message(h->owner,(sprintf(txt,"Your new home base is %d:%d,%d",
				p->home->l,p->home->x,p->home->y),txt));
		sprintf(txt,"#HOME NEW %d:%d,%d\n",p->home->l,p->home->x*128+64,
				p->home->y*128+64);
		psend(h->owner,txt);
		sprintf(txt,"#HOME NEW %d:%d,%d\n",h->l,h->x*128+64,h->y*128+64);
		psend(p,txt);
		p->home->owner=h->owner; // This owner of this home is now the owner of our home
		h->owner->home=p->home; // Our home is now the home of the owner of this home
		h->owner->oldhome=p->home; // Our home is now the preferred home for the owner of this home
		p->home=h; // Our home is this home
		p->oldhome=h; // This is our preferred home
		h->owner=p; // This home is owned by us!
		return;
	}
}


