/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/xsetup.c,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.7 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <X11/Xatom.h>

#include "fix.h"
#include "xbits.h"
#include "player.h"
#include "debug.h"
#include "constants.h"
#include "telnet.h"
#include "xsetup.h"
#include "message.h"

static XColor xc,ex;
jmp_buf jmpenv;
int jumpable;
char why[2048];

int measure_x11_latency(struct player *p) {
    struct timeval start, end;
    long long start_ms, end_ms;
    unsigned char *prop_return;
    unsigned long nitems, bytes_after;
    int actual_format;
    Atom actual_type;

    gettimeofday(&start, NULL);

    // Set the property
    long data = start.tv_sec * 1000000 + start.tv_usec;
    XChangeProperty(p->d.disp, p->d.gamewin, p->latency_atom, XA_INTEGER, 32,
                    PropModeReplace, (unsigned char*)&data, 1);

    // Immediately read it back
    XGetWindowProperty(p->d.disp, p->d.gamewin, p->latency_atom, 0, 1, False, XA_INTEGER,
                       &actual_type, &actual_format, &nitems, &bytes_after, &prop_return);

    gettimeofday(&end, NULL);

    if (prop_return) XFree(prop_return);

    start_ms = (long long)start.tv_sec * 1000 + start.tv_usec / 1000;
    end_ms = (long long)end.tv_sec * 1000 + end.tv_usec / 1000;

    return (int)(end_ms - start_ms);
}

static int my_error_handler(Display *d) {
    DL("XIOErrorHandler called!!!! Someone just did something nasty");
    if (jumpable) longjmp(jmpenv,2);
    DL("ARGH!!! I Can't recover from the situation either!!!");
    save_players();
    DL("PANIC!!! Saved players and exiting....");
    return 0;
}

static int my_other_error_handler(Display *d,XErrorEvent *e) {
    DL("XErrorHandler called!!!! Something very nasty happened");
    fprintf(stderr,"Serial %d Error %d Request %d Minor %d\n",(int)e->serial,
	    (int)e->error_code,(int)e->request_code,(int)e->minor_code);
    if (jumpable) longjmp(jmpenv,2);
    DL("ARGH!!! I Can't recover from the situation either!!!");
    save_players();
    DL("PANIC!!! Saved players and exiting....");
    return 0;
}

extern void bloody_errors(struct player *p) {
    char txt[1024];
    fprintf(stderr, "Emergency code called to chuck off offending player: %s\n", p->user);
    XAutoRepeatOn(p->d.disp);
    p->qflags=0;
    p->connected=0;
    players--;
    if (!p->body.on) {
	if (p->flags&FLG_SHOPPING) {
	    add_body(&p->body);
	}
	if (p->flags&FLG_TERMINAL) {
	    add_body(&p->body);
	    p->term->p=0;
	    p->term=0;
	    return;
	}
    }
    p->flags&=~FLG_DEADCLR;
    sprintf(txt,"%s has just disconnected nastily",p->name);
    global_message(txt);
}

extern void setup_error_handler() {
    DL("Setting up error handler for X things");
    jumpable=0;
    XSetIOErrorHandler(my_error_handler);
    XSetErrorHandler(my_other_error_handler);
}

extern int init_player_display(struct player *p,char *d)
{
    unsigned long vm;
    int fail;
    /*    DL("Initialising player display");*/
    if ((p->d.disp=XOpenDisplay(d))==0) {
	ctwrite("Could not open display, sorry");
	ctwrite(d);
	fprintf(stderr,"Could not open display for %s (%s)\n",d,p->user);
	return 0;
    }
    switch (setjmp(jmpenv)) {  // TODO - what is this?
    case 0:break;
    case 1:return -1;
    case 2:
	bloody_errors(p);  // TODO - what is this?
    case 3:return 0;
    }
    p->d.screen=DefaultScreen(p->d.disp);
    p->d.gc=DefaultGC(p->d.disp,p->d.screen);
    vm=GCForeground|GCBackground|GCFont|GCLineStyle|GCFillStyle;
    strncpy(p->d.name,d,19);
    p->d.gamewin=XCreateSimpleWindow(p->d.disp,
				     RootWindow(p->d.disp,p->d.screen),0,0,
				     WINWID,WINHGT,0,
				     WhitePixel(p->d.disp,p->d.screen),
				     BlackPixel(p->d.disp,p->d.screen));
    if (!p->d.gamewin) {
	ctwrite("Could not bring up window");
	XCloseDisplay(p->d.disp);
	jumpable=0;longjmp(jmpenv,3);
    }
    p->flags|=FLG_INWINDOW;
    fail=0;
    ctwrite("Loading tiny");
    if (!(p->d.tfont=XLoadQueryFont(p->d.disp,TINYFONT))) fail=-1;
    ctwrite("Loading huge");
    if (!(p->d.bfont=XLoadQueryFont(p->d.disp,HUGEFONT))) fail=-1;
    ctwrite("Loading game");
    if (!(p->d.font=XLoadQueryFont(p->d.disp,GAMEFONT))) fail=-1;
    ctwrite("Loading term");
    if (!(p->d.lfont=XLoadQueryFont(p->d.disp,TERMFONT))) fail=-1;
    if (fail) {
	ctwrite("Could not load font");
	XCloseDisplay(p->d.disp);
	jumpable=0;longjmp(jmpenv,3);
    }
    p->d.fw=p->d.font->max_bounds.width;
    p->d.fo=p->d.font->max_bounds.ascent;
    p->d.fh=p->d.fo+p->d.font->max_bounds.descent;
    p->d.bw=p->d.bfont->max_bounds.width;
    p->d.bo=p->d.bfont->max_bounds.ascent;
    p->d.bh=p->d.bfont->max_bounds.descent+p->d.bo/2;
    p->d.tw=p->d.lfont->max_bounds.width;
    p->d.to=p->d.lfont->max_bounds.ascent;
    p->d.th=p->d.to+p->d.lfont->max_bounds.descent;
    XSetFont(p->d.disp,p->d.gc,p->d.font->fid);
    p->d.gc_black=XCreateGC(p->d.disp,p->d.gamewin,0,0);
    p->d.gc_white=XCreateGC(p->d.disp,p->d.gamewin,0,0);
    p->d.gc_ice=XCreateGC(p->d.disp,p->d.gamewin,0,0);
    p->d.gc_red=XCreateGC(p->d.disp,p->d.gamewin,0,0);
    p->d.gc_grey=XCreateGC(p->d.disp,p->d.gamewin,0,0);
    p->d.gc_blue=XCreateGC(p->d.disp,p->d.gamewin,0,0);
    p->d.gc_dgrey=XCreateGC(p->d.disp,p->d.gamewin,0,0);
    p->d.gc_dred=XCreateGC(p->d.disp,p->d.gamewin,0,0);
    p->d.gc_fred=XCreateGC(p->d.disp,p->d.gamewin,0,0);
    p->d.gc_fblue=XCreateGC(p->d.disp,p->d.gamewin,0,0);
    p->d.gc_fdgrey=XCreateGC(p->d.disp,p->d.gamewin,0,0);
    p->d.gc_yellow=XCreateGC(p->d.disp,p->d.gamewin,0,0);
    p->d.gc_termhi=XCreateGC(p->d.disp,p->d.gamewin,0,0);
    p->d.gc_termlo=XCreateGC(p->d.disp,p->d.gamewin,0,0);
    XCopyGC(p->d.disp,p->d.gc,vm,p->d.gc_black);
    XCopyGC(p->d.disp,p->d.gc,vm,p->d.gc_white);
    XCopyGC(p->d.disp,p->d.gc,vm,p->d.gc_ice);
    XCopyGC(p->d.disp,p->d.gc,vm,p->d.gc_red);
    XCopyGC(p->d.disp,p->d.gc,vm,p->d.gc_grey);
    XCopyGC(p->d.disp,p->d.gc,vm,p->d.gc_blue);
    XCopyGC(p->d.disp,p->d.gc,vm,p->d.gc_dgrey);
    XCopyGC(p->d.disp,p->d.gc,vm,p->d.gc_yellow);
    XCopyGC(p->d.disp,p->d.gc,vm,p->d.gc_termhi);
    XCopyGC(p->d.disp,p->d.gc,vm,p->d.gc_termlo);
    Setup_color(p,"white","white");
    XSetForeground(p->d.disp,p->d.gc_white,xc.pixel);
    Setup_color(p,"ice","white");
    XSetForeground(p->d.disp,p->d.gc_ice,xc.pixel);
    Setup_color(p,"black","black");
    XSetForeground(p->d.disp,p->d.gc_black,xc.pixel);
    XSetForeground(p->d.disp,p->d.gc,xc.pixel);
    Setup_color(p,"red","red");
    XSetForeground(p->d.disp,p->d.gc_red,xc.pixel);
    Setup_color(p,"grey","grey60");
    XSetForeground(p->d.disp,p->d.gc_grey,xc.pixel);
    Setup_color(p,"blue","DodgerBlue");
    XSetForeground(p->d.disp,p->d.gc_blue,xc.pixel);
    Setup_color(p,"green","SeaGreen");
    XSetForeground(p->d.disp,p->d.gc_dgrey,xc.pixel);
    Setup_color(p,"yellow","yellow");
    XSetForeground(p->d.disp,p->d.gc_yellow,xc.pixel);
    Setup_color(p,"termhi","green1");
    XSetForeground(p->d.disp,p->d.gc_termhi,xc.pixel);
    Setup_color(p,"termlo","green3");
    XSetForeground(p->d.disp,p->d.gc_termlo,xc.pixel);
    XSetFont(p->d.disp,p->d.gc_white,p->d.tfont->fid);
    XSetFont(p->d.disp,p->d.gc_yellow,p->d.bfont->fid);
    XSetFont(p->d.disp,p->d.gc_black,p->d.bfont->fid);
    XSetFont(p->d.disp,p->d.gc_termlo,p->d.lfont->fid);
    XSetFont(p->d.disp,p->d.gc_termhi,p->d.lfont->fid);
    XStoreName(p->d.disp,p->d.gamewin,"X-ellent 8-)");
    XCopyGC(p->d.disp,p->d.gc_red,vm,p->d.gc_dred);
    XCopyGC(p->d.disp,p->d.gc_red,vm,p->d.gc_fred);
    XCopyGC(p->d.disp,p->d.gc_blue,vm,p->d.gc_fblue);
    XCopyGC(p->d.disp,p->d.gc_dgrey,vm,p->d.gc_fdgrey);
    if (DefaultDepth(p->d.disp,p->d.screen)==1) {
	Pixmap greytile;
	Pixmap bluetile;
	Pixmap greentile;
	greytile=XCreatePixmap(p->d.disp,p->d.gamewin,2,2,1);
	bluetile=XCreatePixmap(p->d.disp,p->d.gamewin,2,2,1);
	greentile=XCreatePixmap(p->d.disp,p->d.gamewin,2,2,1);
	XDrawPoint(p->d.disp,greytile,p->d.gc_white,0,0);
	XDrawPoint(p->d.disp,greytile,p->d.gc_black,0,1);
	XDrawPoint(p->d.disp,greytile,p->d.gc_white,1,1);
	XDrawPoint(p->d.disp,greytile,p->d.gc_black,1,0);

	XDrawPoint(p->d.disp,bluetile,p->d.gc_black,1,1);
	XDrawPoint(p->d.disp,bluetile,p->d.gc_white,0,1);
	XDrawPoint(p->d.disp,bluetile,p->d.gc_white,1,0);
	XDrawPoint(p->d.disp,bluetile,p->d.gc_white,0,0);

	XDrawPoint(p->d.disp,greentile,p->d.gc_white,1,1);
	XDrawPoint(p->d.disp,greentile,p->d.gc_black,0,1);
	XDrawPoint(p->d.disp,greentile,p->d.gc_black,1,0);
	XDrawPoint(p->d.disp,greentile,p->d.gc_black,0,0);

	XSetLineAttributes(p->d.disp,p->d.gc_dred,1,LineOnOffDash,0,0);
	XSetLineAttributes(p->d.disp,p->d.gc_blue,1,LineOnOffDash,0,0);
	XSetLineAttributes(p->d.disp,p->d.gc_grey,1,LineOnOffDash,0,0);
	XSetDashes(p->d.disp,p->d.gc_dred,0,"\002\004",2);
	XSetDashes(p->d.disp,p->d.gc_blue,0,"\007\002",2);
	XSetDashes(p->d.disp,p->d.gc_grey,0,"\005\001",2);

	XSetFillStyle(p->d.disp,p->d.gc_fred,FillStippled);
	XSetFillStyle(p->d.disp,p->d.gc_fblue,FillStippled);
	XSetFillStyle(p->d.disp,p->d.gc_fdgrey,FillStippled);
	XSetStipple(p->d.disp,p->d.gc_fred,greytile);
	XSetStipple(p->d.disp,p->d.gc_fblue,bluetile);
	XSetStipple(p->d.disp,p->d.gc_fdgrey,greentile);
    }
    p->d.backing=XCreatePixmap(p->d.disp,p->d.gamewin,WINWID,WINHGT,
			       DefaultDepth(p->d.disp,p->d.screen));
    XSelectInput(p->d.disp,p->d.gamewin,KeyPressMask|KeyReleaseMask|
		 EnterWindowMask|LeaveWindowMask|PropertyChangeMask);
    XMapRaised(p->d.disp,p->d.gamewin);
    p->latency_atom = XInternAtom(p->d.disp, "GAME_LATENCY_CHECK", False);
    XFlush(p->d.disp);
    jumpable=0;longjmp(jmpenv,1);

    /* This is actually never reached but who cares? Well, gcc certainly */
    /* seems to so here goes, if I get this far something isnt right so */
    /* i'll return 0 which happens to mean fail in this instance */
    
    return 0;
}

extern void shutdown_display(struct player *p) {
    int y=jumpable;
    if (!y) {
	jumpable=-1;
	switch (setjmp(jmpenv)) {
	case 0:break;
	case 1:return;
	case 2:
	    bloody_errors(p);
	    return;
	}
    }
    XUnmapWindow(p->d.disp,p->d.gamewin);
    XFreePixmap(p->d.disp,p->d.backing);
    XDestroyWindow(p->d.disp,p->d.gamewin);
    XFreeGC(p->d.disp,p->d.gc_black);
    XFreeGC(p->d.disp,p->d.gc_white);
    XFreeGC(p->d.disp,p->d.gc_yellow);
    XFreeGC(p->d.disp,p->d.gc_red);
    XFreeGC(p->d.disp,p->d.gc_grey);
    XFreeGC(p->d.disp,p->d.gc_dgrey);
    XFreeGC(p->d.disp,p->d.gc_blue);
    XFreeGC(p->d.disp,p->d.gc_termhi);
    XFreeGC(p->d.disp,p->d.gc_termlo);
    XCloseDisplay(p->d.disp);
    //XFlush(p->d.disp);
    p->d.disp = NULL;
    p->connected=0;
    if (!y) {
	jumpable=0;
	longjmp(jmpenv,1);
    }
    fprintf(stderr, "Display shutdown completed for player %s\n", p->user);
    return;
}

extern void Setup_color(struct player *p,char *dname,char *dcol) {
    char *str;
    char err[1024];
    if (str=(char *) ctquery(dname)) {
	if (XAllocNamedColor(p->d.disp,DefaultColormap(p->d.disp,p->d.screen),
			     str,&ex,&xc)) return;
	sprintf(err,"Cannot find color %s\n",str);
	ctwrite(err);
    }
    XAllocNamedColor(p->d.disp,DefaultColormap(p->d.disp,p->d.screen),dcol,
		     &ex,&xc);
}

extern int Setup_value(struct player *p,char *dname,int val,int min,int max) {
    char *str;
    int v;
    if (str=(char *) ctquery(dname)) {
	v=atoi(str);
	if (v<min) v=min;
	if (v>max) v=max;
	return v;
    }
    return val;
}

extern int Setup_flag(struct player *p,char *dname,int on,int off,int def) {
    char *str;
    int v;
    v=def?on:off;
    if (str=(char *) ctquery(dname)) {
	if (strcmp(str,"on")==0) v=on;
	if (strcmp(str,"yes")==0) v=on;
	if (strcmp(str,"off")==0) v=off;
	if (strcmp(str,"no")==0) v=off;
    }
    return v;
}

extern void Setup_string(struct player *p,char *dname,char *s,int l) {
    char *str;
    if (str=(char *) ctquery(dname))
	strncpy(s,str,l);
}

