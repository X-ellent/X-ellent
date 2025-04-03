/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/telnet.h,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.7 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#ifndef My_TELNET_H
#define My_TELNET_H

void  do_connect();
void  open_sockets();
void  do_login();
char* tread();
void  ctwrite();
char* ctquery(char *s);
char* ctpass();

#endif
