/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/home.h,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.7 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#ifndef My_HOME_H
#define My_HOME_H

extern void exit_home(struct player *p);
extern void init_home(struct player *p);
extern void do_home(struct player *p);
extern void home_mine(struct player *p,int s);
extern void home_quit(struct player *p);
extern void go_home(struct player *p);
extern void add_home(int d,int x,int y);
extern struct home *pick_home();
extern struct home *locate_home(int l,int x,int y);
extern void take_home(struct player *p);

struct home {
    int l,x,y;
    struct player *owner;
    struct home *next;
};

extern struct home* firsthome;

#endif
