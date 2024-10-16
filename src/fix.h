/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/fix.h,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.7 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#ifndef My_FIX_H
#define My_FIX_H

#ifdef NON_ANSI_HEADERS

extern void bzero();
extern int  socket();
extern void perror();
extern int  setsockopt();
extern int  bind();
extern int  listen();
extern int  ioctl();
extern int  select();
extern int  accept();
extern int  close();
extern int  read();
extern int  write();
extern int  printf();
extern int  fprintf();
extern int  connect();
extern int  usleep();
extern void time();
extern void srandom();
extern int  random();
extern int  fclose();
extern int  atoi();
extern int  ungetc();
extern int  _filbuf();

#endif

#include <stdlib.h>

#endif
