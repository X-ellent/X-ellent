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

extern int jumpable;
extern jmp_buf jmpenv;

void setup_error_handler();
void bloody_errors(struct player *p);
int init_player_display(struct player *p,char *d);
void shutdown_display(struct player *p);
int Setup_value(struct player *p,char *dname,int val,int min,int max);
void Setup_color(struct player *p,char *dname,char *dcol);
void Setup_string(struct player *p,char *dname,char *s,int l);
int Setup_flag(struct player *p,char *dname,int on,int off,int def);

#endif
