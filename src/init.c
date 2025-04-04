/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/init.c,v $
** $Author: cheesey $
** $Date: 1992/06/07 00:37:27 $
** $Revision: 1.6 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#include <math.h>

#include "init.h"
#include "mymath.h"
#include "debug.h"

extern void build_sintable()
{
	int i;
	DL("Building sin table");
	for (i=1;i<90;i++) {
	sintable[i]=sin(((double) i)/180.0*PI);
	sintable[180-i]=sintable[i];
	}
	sintable[0]=0;
	sintable[90]=1;
	for (i=0;i<180;i++)
	sintable[i+180]=-sintable[i];
	for (i=0;i<360;i++)
	sintable[i+360]=sintable[i];
	sn=sintable;
	cs=&sintable[90];
}
