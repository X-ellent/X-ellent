/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/xsetup.h,v $
** $Author: cheesey $
** $Date: 1992/06/07 00:37:27 $
** $Revision: 1.6 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#ifndef My_XSETUP_H
#define My_XSETUP_H

#include <setjmp.h>

extern void setup_error_handler();
extern void bloody_errors(struct player *p);
extern int init_player_display(struct player *p,char *d);
extern void shutdown_display(struct player *p);
extern int Setup_value(struct player *p,char *dname,int val,int min,int max);
extern void Setup_color(struct player *p,char *dname,char *dcol);
extern void Setup_string(struct player *p,char *dname,char *s,int l);
extern int Setup_flag(struct player *p,char *dname,int on,int off,int def);

jmp_buf jmpenv;
int jumpable;

#endif
