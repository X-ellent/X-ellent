/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/debug.c,v $
** $Author: cheesey $
** $Date: 1992/06/07 00:37:27 $
** $Revision: 1.6 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#include <stdio.h>

#ifdef EBUG

extern void logmsg(char *s) { fprintf(stderr,"%s\n",s); }

#endif
