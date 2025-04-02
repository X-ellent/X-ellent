/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/xbits.h,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.7 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#ifndef My_XBITS_H
#define My_XBITS_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "fonts.h"

struct player_display {
    Display *disp;
    int screen;
    Window gamewin;
    Pixmap backing;
    GC gc;

    GC gc_black;
    GC gc_white;
    GC gc_ice;
    GC gc_red;
    GC gc_dred;   /* Dashed red line for lifts */
    GC gc_blue;
    GC gc_dgrey;
    GC gc_fred;   /* For filling */
    GC gc_fblue;
    GC gc_fdgrey;
    GC gc_grey;
    GC gc_yellow;
    GC gc_termhi;
    GC gc_termlo;

    XFontStruct *font;
    XFontStruct *tfont;
    XFontStruct *bfont;
    XFontStruct *lfont;
    int fh,fw,fo;
    int bh,bw,bo;
    int th,tw,to;
    char name[64];
};

#define PCACHE 8000
extern XPoint pc[PCACHE];

#define WINWID 512
#define WINHGT 512

#define BLKWID 128
#define BLKHGT 128

#endif
