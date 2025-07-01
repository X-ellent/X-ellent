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

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef unsigned char uchar;

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
#endif // NON_ANSI_HEADERS

static inline void safe_strcpy(char *dest, const char *src, size_t dest_size) {
	size_t copy_len = strlen(src); if (copy_len >= dest_size) copy_len = dest_size - 1;
	memcpy(dest, src, copy_len); dest[copy_len] = '\0';
}

#endif // My_FIX_H
