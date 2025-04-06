/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/terminal.h,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.8 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#ifndef My_TERMINAL_H
#define My_TERMINAL_H

#include "constants.h"

void init_term(struct player *p);
void exit_term(struct player *p);
void init_all_term();
void run_program(struct player *p);
void term_option(struct player *p,int n);
int terminal_input(int chan,int state,char *s);
void terminal_disconnect(int n);
void tsend(int n,char *s);
void psend(struct player *p,char *s);
int terminal_operand(struct player *p,char *s);
int terminal_command(struct player *p,char *sys,char *com);
int system_command(struct player *p,char *com);
int login_command(struct player *p,char *com);
int teleport_command(struct player *p,char *com);
int lift_command(struct player *p,char *com);
int weapons_command(struct player *p,char *com);

#endif
