/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/debug.h,v $
** $Author: cheesey $
** $Date: 1992/06/07 00:37:27 $
** $Revision: 1.6 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#ifndef My_DEBUG_H
#define My_DEBUG_H

#ifdef EBUG

#define DL(s) logmsg(s)
void logmsg();
#else
#define DL(s) /* s */
#endif // EBUG

#endif // My_DEBUG_H
