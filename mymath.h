/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/mymath.h,v $
** $Author: cheesey $
** $Date: 1992/06/07 00:37:27 $
** $Revision: 1.6 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#ifndef My_MYMATH_H
#define My_MYMATH_H

#include <math.h>

#define PI M_PI

double sintable[720];
double *sn;
double *cs;

#define ABS(x) (((x)>=0)?(x):-(x))

#endif
