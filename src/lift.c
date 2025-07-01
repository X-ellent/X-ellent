/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/lift.c,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.5 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "lift.h"
#include "map.h"
#include "turret.h"
#include "terminal.h"

static int lid=0;

void add_lift(int d,int x,int y) {
	struct lift *l;
	if (find_lift(x,y))
		rd2(d,x,y)='l';
	else {
		l=(struct lift *) calloc(1,sizeof(struct lift));
		l->next=firstlift;
		firstlift=l;
		l->id=++lid;
		generate_pass(l->pass,6);
		l->x=x;l->y=y;l->t=d;l->l=d;
	}
}

struct lift *find_lift(int x,int y) {
	struct lift *e;
	for (e=firstlift;e;e=e->next)
		if ((x==e->x)&&(y==e->y)) return e;
	return 0;
}

struct lift *scan_lift(int i) {
	struct lift *e;
	for (e=firstlift;e;e=e->next)
		if (i==e->id) return e;
	return 0;
}

int summon_lift(int l,int x,int y) {
	struct lift *e;
	e=find_lift(x,y);
	if (!e) return LIFT_NONE;
	if (e->t!=e->l)
		return LIFT_BUSY;
	else {
		e->t=l;
		e->clk=0;
		return LIFT_CALLED;
	}
}

void move_lifts() {
	struct lift *l;
	for (l=firstlift;l;l=l->next) if (++l->clk>20) {
		l->clk=0;
		if (l->t!=l->l) {
			if (l->t<l->l) {
				move_lift_up(l);
				if (l->t>l->l) l->t=l->l;
			} else {
				move_lift_down(l);
				if (l->t<l->l) l->t=l->l;
			}
		}
	}
}

void move_lift_to(struct lift *l,int d) {
	char txt[256];
	rd2(l->l,l->x,l->y)='l';
	int minx=l->x*128+31,miny=l->y*128+31,maxx=l->x*128+97,maxy=l->y*128+97;
	for (struct player *p=playone;p;p=p->next) if (p->body.l==l->l)
		if ((p->body.x>minx)&&(p->body.x<maxx)&&(p->body.y>miny)&&(p->body.y<maxy)) {
			p->body.l=d;
			psend(p,(sprintf(txt,"#LIFT %d %d:%d,%d\n",l->id,p->body.l,
							 (int)p->body.x,(int)p->body.y),txt));
		}
	for (struct object *o=obj_first;o;o=o->next) if (o->l==l->l)
		if ((o->x>minx)&&(o->x<maxx)&&(o->y>miny)&&(o->y<maxy)) o->l=d;
	for (struct trolley *tr=firsttrol;tr;tr=tr->next) if (tr->body.l==l->l)
		if ((tr->body.x>minx)&&(tr->body.x<maxx)&&
			(tr->body.y>miny)&&(tr->body.y<maxy)) tr->body.l=d;
	l->l=d;
	rd2(l->l,l->x,l->y)='L';
	l->clk=0;
}

int move_lift_up(struct lift *l) {
	if (l->l==0) {
		l->t=0;
		return 1;
	}
	int d;
	for (d=(l->l-1);(d>=0)&&(!(rd(d,l->x,l->y)&MAP_SOLID));d--);
	if (d<0) {
		l->t=l->l;
		return 1;
	}
	if (rd2(d,l->x,l->y)!='l') {
		l->t=l->l;
		return 1;
	}
	move_lift_to(l,d);
	return 0;
}

int move_lift_down(struct lift *l) {
	if (l->l==(map.depth-1)) {
		l->t=l->l;
		return 1;
	}
	int d;
	for (d=(l->l+1);(d<map.depth)&&(!(rd(d,l->x,l->y)&MAP_SOLID));d++);
	if (d==map.depth) {
		l->t=l->l;
		return 1;
	}
	if (rd2(d,l->x,l->y)!='l') {
		l->t=l->l;
		return 1;
	}
	move_lift_to(l,d);
	return 0;
}

int can_lift_descend(struct lift *l) {
	if (l->l==(map.depth-1)) return 0;
	int d;
	for (d=(l->l+1);(d<map.depth)&&(!(rd(d,l->x,l->y)&MAP_SOLID));d++);
	if (d==map.depth) return 0;
	if (rd2(d,l->x,l->y)!='l') return 0;
	return 1;
}

int can_lift_ascend(struct lift *l) {
	if (l->l==0) return 0;
	int d;
	for (d=(l->l-1);(d>=0)&&(!(rd(d,l->x,l->y)&MAP_SOLID));d--);
	if (d<0) return 0;
	if (rd2(d,l->x,l->y)!='l') return 0;
	return 1;
}

int lift_command(struct player *p,char *com) {
	struct lift *li;
	int n;
	char *ch;
	int a,correct;
	if (strcmp(com,"HELP")==0) {
		psend(p,"=LIF HELP\n=HELP\n=MOVE<num> <pass> <dest>\n=PASS<num> <pass> <newpass>\n");
		return 3;
	}
	if (strncmp(com,"MOVE",4)==0) {
		n=atoi(&com[4]);
		li=scan_lift(n);
		if (!li) {
			psend(p,"!Invalid lift number\n");
			return 3;
		}
		ch=strchr(&com[4],' ');
		if (!ch) {
			psend(p,"!Invalid format\n");
			return 3;
		}
		ch++;
		correct=0;
		for (a=0;a<6;a++)
			if (ch[a]==li->pass[a]) correct++;
		if (correct==0) {
			psend(p,"=LIF MOVE\n=PASS INCORRECT\n");
			return 3;
		}
		if (correct!=6) {
			psend(p,"=LIF MOVE\n=PASS WRONG\n");
			return 3;
		}
		ch=strchr(ch,' ');
		if (!ch) {
			psend(p,"!Invalid format\n");
			return 3;
		}
		if (li->l!=li->t) {
			psend(p,"=LIF MOVE\n=Lift busy\n");
			return 3;
		}
		n=atoi(ch);
		if ((n<0)||(n>=map.depth)) {
			psend(p,"!Level out of range\n");
			return 3;
		}
		psend(p,"=LIF MOVE\n=Lift activated\n");
		li->t=n;
		return 3;

	}
	if (strncmp(com,"PASS",4)==0) {
		n=atoi(&com[4]);
		li=scan_lift(n);
		if (!li) {
			psend(p,"!Invalid lift number\n");
			return 3;
		}
		ch=strchr(&com[4],' ');
		if (!ch++) {
			psend(p,"!Invalid format\n");
			return 3;
		}
		correct=0;
		for (a=0;a<6;a++) if (ch[a]==li->pass[a]) correct++;
		if (correct==0) {
			psend(p,"=LIF PASS\n=PASS INCORRECT\n");
			return 3;
		}
		if (correct!=4) {
			psend(p,"=LIF PASS\n=PASS WRONG\n");
			return 3;
		}
		ch=strchr(ch,' ');
		if (!ch++) {
			psend(p,"!Invalid format\n");
			return 3;
		}
		for (a=0;a<6;a++) if (!isdigit(ch[a])) {
			psend(p,"!Invalid password format\n");
			return 3;
		}
		for (a=0;a<6;a++) li->pass[a]=ch[a];
		psend(p,"=LIF PASS\n=Password Set\n");
		return 3;
	}
	psend(p,"!Invalid command for lift.\n");
	return 3;
}
