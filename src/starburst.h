/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/starburst.h,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.1 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#ifndef My_STARBURST_H
#define My_STARBURST_H

#include "newton.h"

struct starburst {
    struct body body;
    int rot;
    int mode;
    int wait;
    int num;
    int wid;
    int dir;
    int sleep;
    struct starburst *next;
};

struct starburst *firststar;
int sbcount;

#define STAR_SCANNING 0
#define STAR_LOCKING  1
#define STAR_LOCKED   2

#endif

