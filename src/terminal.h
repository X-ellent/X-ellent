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

void psend(struct player *,char *);
void init_term(struct player *);
void exit_term(struct player *);
void init_all_term();
void run_program(struct player *);
void term_option(struct player *,int);
int terminal_input(int,int,char *);

#endif
