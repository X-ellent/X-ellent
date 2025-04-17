/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/draw.c,v $
** $Author: cheesey $
** $Date: 1993/07/01 14:05:02 $
** $Revision: 1.8 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#include <stdio.h>
#include <string.h>
#include <setjmp.h>

#include "fix.h"
#include "player.h"
#include "map.h"
#include "turret.h"
#include "mymath.h"
#include "debug.h"
#include "particle.h"
#include "constants.h"
#include "xsetup.h"
#include "draw.h"
#include "lift.h"
#include "beam.h"
#include "addon.h"

static void draw_me(struct player *p);
static void draw_others(struct player *p);
static void draw_falling(struct player *p);
static void draw_msg(struct player *p);

static void draw_map(struct player *p) {
	int x=(int)p->body.x,y=(int)p->body.y,l=p->body.l;
	XPoint rc[25];
	Display *d=p->d.disp;
	Pixmap *w=p->d.backing;
	GC red=p->d.gc_red, blue=p->d.gc_blue, white=p->d.gc_white;
	GC grey=p->d.gc_grey, dgrey=p->d.gc_dgrey, yellow=p->d.gc_yellow;
	int a=p->rot, xx=x/128, yy=y/128, dx=x&127, dy=y&127;
	double si=sn[a], co=cs[a], tsi, tco;
	int mx=WINWID/2-dx*co-dy*si, my=WINHGT/2+dx*si-dy*co;
	int sx=xx-3;if (sx<0) sx=0;
	int sy=yy-3;if (sy<0) sy=0;
	int ex=xx+4;if (ex>map.wid) ex=map.wid;
	int ey=yy+4;if (ey>map.hgt) ey=map.hgt;
	dx=128*co;dy=-128*si;
	int ddx=dx/4,ddy=dy/4,dddx=dx*3/4,dddy=dy*3/4;
	XFillRectangle(d,w,p->d.gc_black,0,0,WINWID,WINHGT);
	if (l>=map.depth) {
		for (struct explosion *e=bang_first;e;e=e->next) if (e->d==p->body.l) {
			dx=e->x-p->body.x; dy=e->y-p->body.y;
			ddx=dx*co+dy*si; ddy=dy*co-dx*si;
			if (e->r>20)
				XDrawArc(d,w,red,ddx+mx-e->r+20,ddy-e->r+20+my,
						e->r*2-40,e->r*2-40,0,360*64);
			if (e->r>10)
				XDrawArc(d,w,yellow,ddx+mx-e->r+10,ddy-e->r+10+my,
						e->r*2-20,e->r*2-20,0,360*64);
			XDrawArc(d,w,white,ddx+mx-e->r,ddy-e->r+my,e->r*2,e->r*2,0,360*64);
		} return;
	}
	for (int iy=sy;iy<ey;iy++) {
		int xc=mx+(sx-xx)*dx-(iy-yy)*dy, yc=my+(sx-xx)*dy+(iy-yy)*dx;
		for (int ix=sx;ix<ex;ix++) {
			if (rd(l,ix,iy)&MAP_SOLID) {
				int cx=xc+dx/2-dy/2, cy=yc+dy/2+dx/2;
				rc[0].x=xc+ddx-ddy;rc[0].y=yc+ddx+ddy;
				rc[1].x=xc+dddx-ddy;rc[1].y=yc+ddx+dddy;
				rc[2].x=xc+dddx-dddy;rc[2].y=yc+dddx+dddy;
				rc[3].x=xc+ddx-dddy;rc[3].y=yc+dddx+ddy;
				rc[4].x=rc[0].x;rc[4].y=rc[0].y;
				switch (rd2(l,ix,iy)) {
				case 'L':
					XDrawLine(d,w,p->d.gc_dred,rc[0].x,rc[0].y,rc[2].x,rc[2].y);
					XDrawLine(d,w,p->d.gc_dred,rc[1].x,rc[1].y,rc[3].x,rc[3].y);
					// fallthrough
				case 'l':
					XDrawLines(d,w,p->d.gc_dred,rc,5,CoordModeOrigin);break;
				case 'H':
					XDrawLines(d,w,blue,rc,5,CoordModeOrigin);break;
				case 'X':
					XDrawLines(d,w,dgrey,rc,5,CoordModeOrigin);
					struct teleport *tp=locate_teleport(l,ix,iy);
					XDrawArc(d,w,dgrey,cx-28,cy-28,56,56,0,360*64);
					if (tp->clk) {
						int temp;
						if (tp->clk<0) {
							temp=-28*tp->clk/5;
							XDrawArc(d,w,yellow,cx-temp,cy-temp,temp*2,
									 temp*2,0,360*64);
						} else {
							temp=tp->clk;
							XDrawArc(d,w,red,cx-temp,cy-temp,temp*2,
									 temp*2,0,360*64);
						}
					} break;
				case 'Z':
					XDrawArc(d,w,blue,cx-28,cy-28,56,56,0,360*64); break;
				case 'T':
					XFillPolygon(d,w,p->d.gc_fdgrey,rc,5,Convex,CoordModeOrigin);
					XDrawLines(d,w,white,rc,5,CoordModeOrigin); break;
				case 'S':
					XFillPolygon(d,w,p->d.gc_fblue,rc,5,Convex,CoordModeOrigin);
					XDrawLines(d,w,white,rc,5,CoordModeOrigin); break;
				case 'F':
					XFillPolygon(d,w,p->d.gc_fred,rc,5,Convex,CoordModeOrigin);
					XDrawLines(d,w,white,rc,5,CoordModeOrigin);break;
				case 'C':
					XDrawArc(d,w,red,cx-25,cy-25,50,50,0,60*64);
					XDrawArc(d,w,red,cx-25,cy-25,50,50,120*64,60*64);
					XDrawArc(d,w,red,cx-25,cy-25,50,50,240*64,60*64);
					break;
				case 'O':
					struct turret *tur=find_turret(l,ix,iy);
					if (tur->flags&TFLG_DESTROYED)
						XDrawArc(d,w,dgrey,cx-25,cy-25,50,50,0,360*64);
					else {
						tco=cs[(360+tur->rot-a)%360];tsi=sn[(360+tur->rot-a)%360];
						XDrawArc(d,w,(p==tur->victim)?red:white,cx-25,
								 cy-25,50,50,0,360*64);
						XDrawLine(d,w,white,cx+25*tsi,cy-25*tco,
								  cx+(40-5*tur->frame)*tsi,
								  cy-(40-5*tur->frame)*tco);
					} break;
				case '+':
					XDrawLine(d,w,p->d.gc_ice,xc,yc,xc+dx-dy,yc+dx+dy);
					XDrawLine(d,w,p->d.gc_ice,xc+dx,yc+dy,xc-dy,yc+dx);
					break;
				case '-':
					XDrawLines(d,w,p->d.gc_ice,rc,5,CoordModeOrigin);break;
				case '7':
					XDrawLine(d,w,p->d.gc_ice,rc[0].x,rc[0].y,rc[1].x,rc[1].y);
					XDrawLine(d,w,p->d.gc_ice,rc[0].x,rc[0].y,rc[3].x,rc[3].y);
					break;
				case '9':
					XDrawLine(d,w,p->d.gc_ice,rc[0].x,rc[0].y,rc[1].x,rc[1].y);
					XDrawLine(d,w,p->d.gc_ice,rc[1].x,rc[1].y,rc[2].x,rc[2].y);
					break;
				case '3':
					XDrawLine(d,w,p->d.gc_ice,rc[2].x,rc[2].y,rc[3].x,rc[3].y);
					XDrawLine(d,w,p->d.gc_ice,rc[1].x,rc[1].y,rc[2].x,rc[2].y);
					break;
				case '1':
					XDrawLine(d,w,p->d.gc_ice,rc[2].x,rc[2].y,rc[3].x,rc[3].y);
					XDrawLine(d,w,p->d.gc_ice,rc[0].x,rc[0].y,rc[3].x,rc[3].y);
					break;
				case '8':
					XDrawLine(d,w,p->d.gc_ice,rc[0].x,rc[0].y,rc[1].x,rc[1].y);
					break;
				case '6':
					XDrawLine(d,w,p->d.gc_ice,rc[2].x,rc[2].y,rc[1].x,rc[1].y);
					break;
				case '2':
					XDrawLine(d,w,p->d.gc_ice,rc[2].x,rc[2].y,rc[3].x,rc[3].y);
					break;
				case '4':
					XDrawLine(d,w,p->d.gc_ice,rc[0].x,rc[0].y,rc[3].x,rc[3].y);
					break;
				case '0':
					XDrawLine(d,w,p->d.gc_ice,rc[0].x,rc[0].y,rc[2].x,rc[2].y);
					XDrawLine(d,w,p->d.gc_ice,rc[1].x,rc[1].y,rc[3].x,rc[3].y);
					break;
				case '5':
					XDrawLine(d,w,p->d.gc_ice,rc[0].x,rc[0].y,rc[1].x,rc[1].y);
					XDrawLine(d,w,p->d.gc_ice,rc[2].x,rc[2].y,rc[3].x,rc[3].y);
					break;
				default:
					XDrawLines(d,w,dgrey,rc,5,CoordModeOrigin);break;
				} // switch
			} else if (rd(l,ix,iy)&MAP_OBSC) {
				for (int a=0;a<5;a++) for (int b=0;b<5;b++) {
					rc[a+5*b].x=xc+dx*a/4-dy*b/4;
					rc[a+5*b].y=yc+dx*b/4+dy*a/4;
				}
				XDrawPoints(d,w,white,rc,25,CoordModeOrigin);
			}
			xc=xc+dx; yc=yc+dy;
		} // ix for
	} // iy for
	for (int iy=sy;iy<ey;iy++) {
		int xc=mx+(sx-xx)*dx-(iy-yy)*dy, yc=my+(sx-xx)*dy+(iy-yy)*dx;
		for (int ix=sx;ix<ex;ix++) {
			switch (rd(l,ix,iy)&MAP_LWALL) {
			case MAP_LWALL:XDrawLine(d,w,white,xc,yc,xc-dy,yc+dx);break;
			case MAP_LWALL1:XDrawLine(d,w,grey,xc,yc,xc-dy,yc+dx);break;
			case MAP_LWALL0:XDrawLine(d,w,dgrey,xc,yc,xc-dy,yc+dx);break;
			default:break;
			}
			switch (rd(l,ix,iy)&MAP_TWALL) {
			case MAP_TWALL:XDrawLine(d,w,white,xc,yc,xc+dx,yc+dy);break;
			case MAP_TWALL1:XDrawLine(d,w,grey,xc,yc,xc+dx,yc+dy);break;
			case MAP_TWALL0:XDrawLine(d,w,dgrey,xc,yc,xc+dx,yc+dy);break;
			default:break;
			}
			xc=xc+dx;yc=yc+dy;
		}
	}
	/* Draw in objects */
	{
		struct object *u;
		GC mc;
		mx=WINWID/2;my=WINHGT/2;
		for (u=obj_first;u;u=u->next) if (p->body.l==u->l) {
			dx=(u->x)-x; dy=(u->y)-y;
			if (!((dx>WINWID)||(dx<-WINWID)||(dy>WINHGT)||(dy<-WINHGT))) {
				ddx=dx*co+dy*si; ddy=dy*co-dx*si;
				switch(u->type) {
				case OBJ_MINE_TRIG:
				case OBJ_MINE_TIME:
				case OBJ_MINE_PROX:
				case OBJ_MINE_VELY:
				case OBJ_MINE_SMART:
					if (u->flags&OBJ_F_ARM) {
						if ((!(u->flags&OBJ_F_FLASH))&&(u->flags&OBJ_F_TRIG)) {
							mc=red;
							u->flags|=OBJ_F_FLASH;
						} else mc=dgrey;
						if (p->flags&FLG_MINESWEEP)
							XDrawArc(d,w,mc,mx+ddx-8,my+ddy-8,16,16,0,360*64);
					} else XDrawArc(d,w,blue,mx+ddx-8,my+ddy-8,16,16,0,360*64);
					break;
				case OBJ_BONUS:
					XDrawArc(d,w,white,mx+ddx-8,my+ddy-8,16,16,0,360*64);
				default:
					XDrawArc(d,w,yellow,mx+ddx-8,my+ddy-8,16,16,0,360*64);
					break;
				} // switch
			}
		}
	}
	/* OK Now draw particles too */
	{
		mx=WINWID/2;my=WINHGT/2;
		int n=0;
		for (struct particle *u=parts[l];u;u=u->next) {
			dx=(u->x)-x; dy=(u->y)-y;
			if (!((dx>WINWID)||(dx<-WINWID)||(dy>WINHGT)||(dy<-WINHGT))) {
				ddx=dx*co+dy*si; ddy=dy*co-dx*si;
				if (u->life!=-1) {
					pc[n].x=mx+ddx; pc[n++].y=my+ddy;
					pc[n].x=mx+ddx+1; pc[n++].y=my+ddy+1;
					pc[n].x=mx+ddx+1; pc[n++].y=my+ddy;
					pc[n].x=mx+ddx; pc[n++].y=my+ddy+1;
					if (n==PCACHE) {
						XDrawPoints(d,w,white,pc,PCACHE,CoordModeOrigin);
						n=0;
					}
				} else {
					int r=random()%5+1;
					XDrawArc(d,w,yellow,mx+ddx-r,my+ddy-r,r*2,r*2,0,360*64);
				}
			}
		}
		if (n) XDrawPoints(d,w,white,pc,n,CoordModeOrigin);
	}
	/* Draw in them laserbeam thingsp */
	{
		for(struct beam *b=firstbeam;b;b=b->next) if (p->body.l==b->l) {
			dx=(b->x)-p->body.x; dy=(b->y)-p->body.y;
			int sx=dx*co+dy*si+mx, sy=dy*co-dx*si+my;
			dx=(b->xx)-p->body.x; dy=(b->yy)-p->body.y;
			int ex=dx*co+dy*si+mx, ey=dy*co-dx*si+my;
			switch(b->type) {
			case BEAM_RED:
				XDrawLine(d,w,red,sx,sy,ex,ey);
				break;
			case BEAM_BLUE:
				XDrawLine(d,w,blue,sx,sy,ex,ey);
				break;
			default:
				XDrawLine(d,w,dgrey,sx,sy,ex,ey);
				break;
			}
		}
	}
	/* Then draw explosions */
	{
		for (struct explosion *e=bang_first;e;e=e->next) if (e->d==p->body.l) {
			dx=e->x-p->body.x; dy=e->y-p->body.y;
			ddx=dx*co+dy*si; ddy=dy*co-dx*si;
			if (e->r>20) XDrawArc(d,w,red,ddx+mx-e->r+20,ddy-e->r+20+my,
					 e->r*2-40,e->r*2-40,0,360*64);
			if (e->r>10) XDrawArc(d,w,yellow,ddx+mx-e->r+10,ddy-e->r+10+my,
					 e->r*2-20,e->r*2-20,0,360*64);
			XDrawArc(d,w,white,ddx+mx-e->r,ddy-e->r+my,e->r*2,e->r*2,0,360*64);
		}
	}
}

void draw_all(struct player *p) {
	jumpable=-1;
	switch (setjmp(jmpenv)) {
	case 0:break;
	case 1:return;
	case 2:
		bloody_errors(p);
		return;
	}
	if (p->body.on) { draw_map(p);draw_others(p);draw_me(p); }
	else if (p->flags&FLG_DEAD) {
		if (p->delay==DEATH_SHOW) {
			int l=strlen(DEATH_MSG);
			XFillRectangle(p->d.disp,p->d.backing,p->d.gc_black,0,0,
						   WINWID,WINHGT);
			for (int a=-1;a<2;a++) for (int b=-1;b<2;b++)
				XDrawString(p->d.disp,p->d.backing,p->d.gc_yellow,
						WINWID/2-p->d.bw*l/2+a,WINHGT/2-p->d.bh+b,DEATH_MSG,l);
			XDrawString(p->d.disp,p->d.backing,p->d.gc_black,WINWID/2-
						p->d.bw*l/2,WINHGT/2-p->d.bh,DEATH_MSG,l);
		} else if (p->delay>DEATH_SHOW) {
			if (p->flags&FLG_FALLEN && p->body.l>=map.depth) draw_falling(p);
			else { draw_map(p);draw_others(p);draw_msg(p); }
		} else XDrawArc(p->d.disp,p->d.backing,p->d.gc_red,20,20,
					WINWID-40,WINHGT-40,360*64*(p->delay-1)/DEATH_SHOW,
					360*128/DEATH_SHOW);
	}
	XCopyArea(p->d.disp,p->d.backing,p->d.gamewin,p->d.gc,0,0,WINWID,WINHGT,0,0);
	if (frame>=16) XFlush(p->d.disp);
	jumpable=0;
	longjmp(jmpenv,1);
}

void draw_map_level(struct player *p,int l) {
	char txt[64];
	Display *d=p->d.disp;
	Pixmap w=p->d.backing;
	GC red=p->d.gc_dred, blue=p->d.gc_blue, white=p->d.gc_white;
	GC grey=p->d.gc_grey, dgrey=p->d.gc_dgrey, yellow=p->d.gc_yellow;
	XFillRectangle(d,w,p->d.gc_black,0,0,WINWID,WINHGT);
	sprintf(txt,"Map of level #%d",l);
	if (p->mapmem[l]) strcat(txt," [Mem]");
	XDrawString(d,w,p->d.gc_red,20,p->d.fh,txt,strlen(txt));
	int wx=WINWID/((map.wid>map.hgt)?map.wid:map.hgt);
	int mx=WINWID/2-wx*(map.wid)/2, my=WINHGT/2-wx*(map.hgt)/2;
	int wwx=wx/2;
	int pl=p->body.l;
	int px=(int)p->body.x/128, py=(int)p->body.y/128;
	for (int y=1;y<(map.hgt-1);y++) for (int x=1;x<(map.wid-1);x++) {
		if ((l==pl)&&(x==px)&&(y==py))
			{ XFillRectangle(d,w,yellow,mx+x*wx,my+y*wx,wx+1,wx+1);continue; }
		switch(rd(l,x,y)&(MAP_SOLID|MAP_OBSC)) {
		case MAP_SOLID:
			switch(rd2(l,x,y)) {
			case '-':
				XDrawRectangle(d,w,p->d.gc_ice,mx+x*wx+wx/4,my+y*wx+wx/4,wwx+1,wwx+1);
				break;
			case '+':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				XDrawLine(d,w,p->d.gc_ice,mx+x*wx,my+y*wx,mx+x*wx+wx,my+y*wx+wx);
				XDrawLine(d,w,p->d.gc_ice,mx+x*wx+wx,my+y*wx,mx+x*wx,my+y*wx+wx);
				break;
			case 'L':
				XDrawLine(d,w,red,mx+x*wx+wx/4,my+y*wx+wx/4,
						  mx+x*wx+wx*3/4,my+y*wx+wx*3/4);
				XDrawLine(d,w,red,mx+x*wx+wx/4,my+y*wx+wx*3/4,
						  mx+x*wx+wx*3/4,my+y*wx+wx/4);
			case 'l':
				XDrawRectangle(d,w,red,mx+x*wx+wx/4,my+y*wx+wx/4,wwx+1,wwx+1);
				break;
			case 'S':
				XFillRectangle(d,w,p->d.gc_fblue,mx+x*wx+wx/4,
							   my+y*wx+wx/4,wwx+1,wwx+1);
				XDrawRectangle(d,w,white,mx+x*wx+wx/4,my+y*wx+wx/4,wwx+1,wwx+1);
				break;
			case 'H':
				XDrawRectangle(d,w,blue,mx+x*wx+wx/4,my+y*wx+wx/4,wwx+1,wwx+1);
				break;
			case 'X':
				XDrawRectangle(d,w,dgrey,mx+x*wx+wx/4,my+y*wx+wx/4,wwx+1,wwx+1);
				XDrawArc(d,w,dgrey,mx+x*wx+wx/3,my+y*wx+wx/3,wx/3,wx/3,0,360*64);
				break;
			case 'Z':
				XDrawArc(d,w,blue,mx+x*wx+wx/3,my+y*wx+wx/3,wx/3,wx/3,0,360*64);
				break;
			case 'F':
				XFillRectangle(d,w,p->d.gc_fred,mx+x*wx+wx/4,
							   my+y*wx+wx/4,wwx+1,wwx+1);
				XDrawRectangle(d,w,white,mx+x*wx+wx/4,my+y*wx+wx/4,wwx+1,wwx+1);
				break;
			case 'T':
				XFillRectangle(d,w,p->d.gc_fdgrey,mx+x*wx+wx/4,my+y*wx+wx/4,wwx+1,wwx+1);
				XDrawRectangle(d,w,white,mx+x*wx+wx/4,my+y*wx+wx/4,wwx+1,wwx+1);
				break;
			case 'C':
				XDrawArc(d,w,red,mx+x*wx+wx/4,my+y*wx+wx/4,wx/2+1,wx/2+1,0,360*64);
				break;
			case 'O':
				struct turret *tur=find_turret(l,x,y);
				if (!(tur->flags&TFLG_DESTROYED))
					XDrawArc(d,w,white,mx+x*wx+wx/4,my+y*wx+wx/4,wx/2+1,wx/2+1,0,360*64);
				else
					XDrawArc(d,w,dgrey,mx+x*wx+wx/4,my+y*wx+wx/4,wx/2+1,wx/2+1,0,360*64);
				break;
			default:
				XDrawRectangle(d,w,dgrey,mx+x*wx+wx/4,my+y*wx+wx/4,wwx+1,wwx+1);
				break;
			}
			break;
		case MAP_OBSC:
			for (int a=0;a<5;a++) for (int b=0;b<5;b++)
				XDrawPoint(d,w,grey,mx+x*wx+wx*a/4,my+y*wx+wx*b/4);
			break;
		default:
			break;
		}
	} // for x (and y)
	for (int y=0;y<map.hgt;y++) for (int x=0;x<map.wid;x++) {
		switch (rd(l,x,y)&(MAP_LWALL)) {
		case MAP_LWALL:
			XDrawLine(d,w,white,mx+x*wx,my+y*wx,mx+x*wx,my+(y+1)*wx);
			break;
		case MAP_LWALL1:
			XDrawLine(d,w,grey,mx+x*wx,my+y*wx,mx+x*wx,my+(y+1)*wx);
			break;
/*		case MAP_LWALL0:
			XDrawLine(d,w,dgrey,mx+x*wx,my+y*wx,mx+x*wx,my+(y+1)*wx);
			break;*/
		default:
			break;
		}
		switch (rd(l,x,y)&(MAP_TWALL)) {
		case MAP_TWALL:
			XDrawLine(d,w,white,mx+x*wx,my+y*wx,mx+(x+1)*wx,my+y*wx);
			break;
		case MAP_TWALL1:
			XDrawLine(d,w,grey,mx+x*wx,my+y*wx,mx+(x+1)*wx,my+y*wx);
			break;
/*		case MAP_TWALL0:
			XDrawLine(d,w,dgrey,mx+x*wx,my+y*wx,mx+(x+1)*wx,my+y*wx);
			break;*/
		default:
			break;
		}
	}
	XCopyArea(p->d.disp,p->d.backing,p->d.gamewin,p->d.gc,0,0,WINWID,WINHGT,0,0);
	XFlush(p->d.disp);
}

static void draw_me(struct player *p) {
	int rx,ry, dx,dy, i;
	struct addon *radad, *tarad, *visad, *aclad;
	int antilev;
	int cansee=0;/* can see target?? (if have one)*/
	char txt[80];
	Display *d=p->d.disp;
	Pixmap w=p->d.backing;
	GC red=p->d.gc_red,blue=p->d.gc_blue,white=p->d.gc_white;
	GC yellow=p->d.gc_yellow,dgrey=p->d.gc_dgrey;
	int mx=WINWID/2,my=WINHGT/2;
	int rd=p->body.radius+2*p->body.height;
	XDrawRectangle(d,w,(p->fuel<p->fuelmin)?red:blue,mx-100,my-100,200,200);
	XDrawArc(d,w,white,mx-rd/4,my-rd/4,rd/2,rd/2,0,360*64);
	XDrawArc(d,w,p->immune?yellow:white,mx-rd,my-rd,rd*2,rd*2,0,360*64);
	XDrawLine(d,w,white,mx,my-rd/4,mx,my-3*rd/4);
	double si=sn[360-(int) p->rot], co=cs[360-(int) p->rot];
	radad=find_addon(p->firstadd,ADD_RADAR);
	tarad=find_addon(p->firstadd,ADD_TARGET);
	visad=find_addon(p->firstadd,ADD_VISUAL);
	aclad=find_addon(p->firstadd,ADD_ANTICLOAK);
	if (aclad) antilev=aclad->level;
	else antilev=0;
	if (visad&&(visad->info[0])) {
		int tx,ox,oy;
		ox=(visad->info[2]-p->body.x)/visad->info[1];
		oy=(visad->info[3]-p->body.y)/visad->info[1];
		for(i=0;i<64;i++) if (p->lines[i][0]) {
			dx=(p->lines[i][0]-128)*4; dy=(p->lines[i][1]-128)*4;
			rx=(p->lines[i][2]-128)*4; ry=(p->lines[i][3]-128)*4;
			switch(visad->info[0]) {
				case 3: dx+=ox;dy+=oy;rx+=ox;ry+=oy; // fallthrough
				case 2: tx=dx*co-dy*si;dy=dy*co+dx*si;dx=tx;
						tx=rx*co-ry*si;ry=ry*co+rx*si;rx=tx; // fallthrough
				case 1:
				default:
					XDrawLine(d,w,p->d.gc_yellow,mx+dx,my+dy,mx+rx,my+ry);
			}
		}
	}
	if (radad) {
		int rng=(1<<radad->info[0]);
		for (struct player *o=playone;o;o=o->next)
			if ((!(o->flags&FLG_CLOAKING))&&(o->body.on)&&(o!=p)&&
				((p->body.l+radad->info[1])==o->body.l)) {
				dx=(o->body.x-p->body.x); dy=(o->body.y-p->body.y);
				int t=dx*co-si*dy;dy=dy*co+si*dx;dx=t;
				rx=dx/rng;ry=dy/rng;
				if ((rx<100)&&(rx>-100)&&(ry<100)&&(ry>-100)) {
					if ((o==p->ptarg)&&(tarad)) {
						XDrawPoint(d,w,red,mx+rx,my+ry);
						XDrawPoint(d,w,red,mx+rx,my+ry+1);
						XDrawPoint(d,w,red,mx+rx+1,my+ry+1);
						XDrawPoint(d,w,red,mx+rx+1,my+ry);
						if ((tarad->level>=3)&&
							(dx<mx)&&(dx>-mx)&&(dy<my)&&(dy>-my))
							XDrawRectangle(d,w,red,mx+dx-25,my+dy-25,50,50);
					} else {
						XDrawPoint(d,w,blue,mx+rx,my+ry);
						XDrawPoint(d,w,blue,mx+rx,my+ry+1);
						XDrawPoint(d,w,blue,mx+rx+1,my+ry+1);
						XDrawPoint(d,w,blue,mx+rx+1,my+ry);
					}
				}
			}
	}
	cansee=0;
	if (tarad && radad && p->ptarg && p->ptarg->body.on) {
		if (!(p->ptarg->flags&FLG_CLOAKING)) cansee=1;
		else if (p->flags&FLG_ANTICLOAK) {
			struct addon *othclk;
			othclk=find_addon(p->ptarg->firstadd,ADD_CLOAKING);
			cansee=(othclk->level<=antilev)?1:0;
		}
	}
	if (tarad && radad) {
		if (cansee&&((p->body.l+radad->info[1])==p->ptarg->body.l)) {
			dx=(p->ptarg->body.x-p->body.x); dy=(p->ptarg->body.y-p->body.y);
			int t=100*(1<<radad->info[2]);
			if ((p->body.l==p->ptarg->body.l)||((dx<t)&&(dx>-t)&&(dy<t)&&(dy>-t))) {
				t=dx*co-si*dy;dy=dy*co+si*dx;dx=t;
				double r=sqrt(dx*dx+dy*dy);
				dx=(90*dx)/r;dy=(90*dy)/r;
				XDrawArc(d,w,red,mx+dx-5,my+dy-5,10,10,0,64*360);
				if (tarad->level>=2) {
					sprintf(txt,"%d",(int)(r/1.28));
					XDrawString(d,w,red,mx-96,my+98,txt,strlen(txt));
				}
			}
		}
	}
	if (!(p->flags&FLG_NOINSTR)) {
		sprintf(txt,"X:%d",(int) p->body.x/128);
		XDrawString(d,w,red,mx-100,my-102-p->d.fh,txt,strlen(txt));
		sprintf(txt,"Y:%d",(int) p->body.y/128);
		XDrawString(d,w,red,mx-20,my-102-p->d.fh,txt,strlen(txt));
		sprintf(txt,"L:%d",p->body.l);
		XDrawString(d,w,red,mx+80,my-102-p->d.fh,txt,strlen(txt));
		if (radad) {
			sprintf(txt,"Radar x%d",(1<<radad->info[0]));
			XDrawString(d,w,red,mx-100,my+100+p->d.fh,txt,strlen(txt));
		}
		XDrawString(d,w,red,mx-100,my-104,weap_name[p->weap],strlen(weap_name[p->weap]));
		if (p->weap) {
			sprintf(txt,"%d",p->ammo[p->weap]);
			XDrawString(d,w,red,mx-96,my-100+p->d.fh,txt,strlen(txt));
		}
		if (p->recharge) {
			sprintf(txt,"Recharging... %d",p->recharge);
			XDrawString(d,w,red,mx-96,my-100+p->d.fh*2,txt,strlen(txt));
		}
		if (p->homing) {
			sprintf(txt,"Home available in %d",p->homing);
			XDrawString(d,w,red,mx-96,my-100+p->d.fh*3,txt,strlen(txt));
		}
		if (radad) {
			switch (radad->info[1]) {
				case 0:strcpy(txt,"Scanning Level");break;
				case 1:strcpy(txt,"Scanning Below");break;
				case -1:strcpy(txt,"Scanning Above");break;
				default:strcpy(txt,"Scanning Errrr... somewhere");break;
			}
			XDrawString(d,w,red,mx-10,my+100+p->d.fh,txt,strlen(txt));
		}
		sprintf(txt,"Score:%d",(int) p->score);
		XDrawString(d,w,red,mx-100,my-102-p->d.fh*2,txt,strlen(txt));
		sprintf(txt,"Cash:%d",(int) p->cash);
		XDrawString(d,w,red,mx-100,my-102-p->d.fh*3,txt,strlen(txt));
		sprintf(txt,"Rating:%d",(int) p->rating);
		XDrawString(d,w,red,mx+25,my-102-p->d.fh*3,txt,strlen(txt));
		if (tarad&&(p->ptarg)) {
			if (!cansee) sprintf(txt,"Target: No Signal");
			else if (p->body.l==p->ptarg->body.l)
				sprintf(txt,"Target: At %d,%d -> %s",
						(int)p->ptarg->body.x/128,(int)p->ptarg->body.y/128,
						(p->ptarg->flags&FLG_IDENT)?p->ptarg->name:"NO ID");
			else if (radad&&((p->body.l+radad->info[1])==p->ptarg->body.l))
				sprintf(txt,"Target: Located %s -> %s",
						(radad->info[1]>0)?"Below":"Above",
						(p->ptarg->flags&FLG_IDENT)?p->ptarg->name:"NO ID");
			else sprintf(txt,"Target: Scanning -> %s",
					(p->ptarg->flags&FLG_IDENT)?p->ptarg->name:"NO ID");
			XDrawString(d,w,red,mx-100,my+100+p->d.fh*2,txt,strlen(txt));
		}
	}
	if (p->flags&FLG_STATUS) {
		char ontop=rd2(p->body.l,(int)p->body.x/128,(int)p->body.y/128);
		int n=10;
		if (p->flags&FLG_IDENT) XDrawString(d,w,red,32,p->d.fh*n++,"Identifier",10);
		if (p->flags&FLG_NOMSG) XDrawString(d,w,red,32,p->d.fh*n++,"No Messages",11);
		if (p->flags&FLG_NOWEP) XDrawString(d,w,red,32,p->d.fh*n++,"No Slotlist",11);
		if (p->flags&FLG_CLOAKING) XDrawString(d,w,red,32,p->d.fh*n++,"Cloaking",8);
		if (p->flags&FLG_ANTICLOAK) XDrawString(d,w,red,32,p->d.fh*n++,"Anti-Cloaking",13);
		if (p->flags&FLG_INVIS) XDrawString(d,w,red,32,p->d.fh*n++,"Invisible",9);
		if (p->flags&FLG_MINESWEEP) XDrawString(d,w,red,32,p->d.fh*n++,"Mine Sweeping",13);
		if ((p->ptarg)&&(tarad)) XDrawString(d,w,red,32,p->d.fh*n++,"Tracking",8);
		if ((ontop=='L')||(ontop=='l')) {
			struct lift *l;
			if ((l=find_lift(p->body.x/128,p->body.y/128))) {
				sprintf(txt,"Lift #%d L:%d",l->id,l->l);
				XDrawString(d,w,red,32,p->d.fh*n++,txt,strlen(txt));
			}
		}
		if ((ontop=='X')) {
			struct teleport *t;
			if ((t=locate_teleport(p->body.l,p->body.x/128,p->body.y/128))) {
				sprintf(txt,"Teleport #%d->%d",t->num,t->dest->num);
				XDrawString(d,w,red,32,p->d.fh*n++,txt,strlen(txt));
			}
		}
		XDrawString(d,w,red,50,p->d.fh,"Fuel",4);
		XDrawString(d,w,red,50,p->d.fh*2,"Thrust",6);
		XDrawString(d,w,red,50,p->d.fh*3,"Spin",4);
		XDrawString(d,w,red,50,p->d.fh*5,"Shield",6);
		DrawMeter(p,50+p->d.fw*7,p->d.fh,WINWID/2,4,p->maxfuel/200,p->fuel/200);
		DrawMeter(p,50+p->d.fw*7,p->d.fh*2,WINWID/2,4,100,p->thrust);
		DrawMeter(p,50+p->d.fw*7,p->d.fh*3,WINWID/2,4,100,p->spin);
		DrawMeter(p,50+p->d.fw*7,p->d.fh*5,WINWID/2,4,p->maxshield/10,p->shield/10);
	}
	if (!(p->flags&FLG_NOMSG)) for (int i=0;i<4;i++)
		XDrawString(d,w,blue,16,WINHGT-p->d.fh*(i+1),p->msg[i],strlen(p->msg[i]));
	if (!(p->flags&FLG_NOWEP)) {
		XDrawString(d,w,dgrey,mx+110,p->d.fh*(10),"#",1);
		XDrawString(d,w,dgrey,mx+140,p->d.fh*(10),"Contains",8);
		for (int i=0;i<9;i++) {
			GC sl=(p->slot==(i+1))?p->d.gc_fred:dgrey;
			txt[0]='1'+i;
			XDrawString(d,w,sl,mx+110,p->d.fh*(i+12),txt,1);
			if (p->slotobj[i]) XDrawString(d,w,sl,mx+123,p->d.fh*(i+12),
						(p->slotobj[i]->flags&OBJ_F_ARM)?"A":"D",1);
			switch(p->slots[i]) {
				case OBJ_EMPTY:strcpy(txt,"Slot Empty");break;
				case OBJ_MINE_TRIG:strcpy(txt,"Mine (Trig)");break;
				case OBJ_MINE_TIME:strcpy(txt,"Mine (Time)");break;
				case OBJ_MINE_PROX:strcpy(txt,"Mine (Prox)");break;
				case OBJ_MINE_VELY:strcpy(txt,"Mine (Vely)");break;
				case OBJ_MINE_SMART:strcpy(txt,"Mine (Smart)");break;
			}
			XDrawString(d,w,sl,mx+140,p->d.fh*(i+12),txt,strlen(txt));
		}
	}
	if ((p->slot)&&(p->slots[p->slot-1]!=OBJ_EMPTY)) {
		switch(p->slots[p->slot-1]) {
			case OBJ_MINE_TRIG:strcpy(txt,"Triggered Mine");break;
			case OBJ_MINE_TIME:strcpy(txt,"Timed Mine");break;
			case OBJ_MINE_PROX:strcpy(txt,"Proximity Mine");break;
			case OBJ_MINE_VELY:strcpy(txt,"Velocity Mine");break;
			case OBJ_MINE_SMART:strcpy(txt,"Smart Mine");break;
		}
		switch(p->size[p->slot-1]) {
			case 1:strcat(txt," <Small>");break;
			case 2:strcat(txt," <Medium>");break;
			case 3:strcat(txt," <Large>");break;
			case 4:strcat(txt," <Extra>");break;
		}
		if (p->slotobj[p->slot-1]) {
			if (p->slotobj[p->slot-1]->flags&OBJ_F_ARM)
				strcat(txt," (Armed)");
			else if (p->slotobj[p->slot-1]->flags&OBJ_F_ARMING)
				strcat(txt," (Arming)");
			XDrawString(d,w,red,mx-100,my+100+p->d.fh*3,txt,strlen(txt));
		} else {
			if (p->mode[p->slot-1])
				DrawMeter(p,mx-100,my+100+p->d.fh*4,200,4,100,
						  (p->mode[p->slot-1]==-1)?0:p->mode[p->slot-1]);
			XDrawString(d,w,dgrey,mx-100,my+100+p->d.fh*3,txt,strlen(txt));
		}
	}
	if (find_addon(p->firstadd,ADD_COMPASS)) {
		int dx=-25*si, dy=-25*co;
		XDrawLine(d,w,red,25+dx,25+dy,25,25);
		XDrawArc(d,w,blue,0,0,51,51,(int) ((p->rot+180)*64)%(360*64),(int)180*64);
	}
}

extern void DrawMeter(struct player *p,int x,int y,int l,int h,int max,int val) {
	char v[32];
	Display *d=p->d.disp;
	Pixmap w=p->d.backing;
	GC red=p->d.gc_red,white=p->d.gc_white,grey=p->d.gc_grey;
	sprintf(v,"%d",val);
	XDrawString(d,w,red,x+l+10,y,v,strlen(v));
	y=y-p->d.fh/2+2;
	XDrawLine(d,w,white,x+l/4,y+h/2,x+l/4,y-h/2);
	XDrawLine(d,w,white,x+l/2,y+h,x+l/2,y-h);
	XDrawLine(d,w,white,x+3*l/4,y+h/2,x+3*l/4,y-h/2);
	XDrawLine(d,w,grey,x+l,y,x+(l*val/max),y);
	XDrawLine(d,w,red,x,y,x+(l*val/max),y);
	XDrawLine(d,w,white,x,y+h,x,y-h);
	XDrawLine(d,w,white,x+l,y+h,x+l,y-h);
}

static void draw_others(struct player *p) {
	int mx,my, cx,cy, rx,ry,dr, rd;
	char txt[256];
	Display *d=p->d.disp; Pixmap w=p->d.backing;
	GC red=p->d.gc_red,blue=p->d.gc_blue,white=p->d.gc_white,yellow=p->d.gc_yellow;
	double ms=-sn[(int) p->rot], mc=cs[(int) p->rot];
	struct player *o;
	for (o=playone;o;o=o->next) if (!(o->flags&FLG_INVIS)&&(o->body.on)&&
				(o!=p)&&(o->body.l==p->body.l)) {
		int l,mx=(o->body.x-p->body.x), my=(o->body.y-p->body.y);
		dr=(int)(o->rot-p->rot+720)%360;
		cx=WINWID/2+mx*mc-my*ms; cy=WINHGT/2+my*mc+mx*ms;
		rd=o->body.radius+2*o->body.height;
		rx=rd*3*sn[dr]/4; ry=-rd*3*cs[dr]/4;
		XDrawArc(d,w,white,cx-rd/4,cy-rd/4,rd/2,rd/2,0,360*64);
		XDrawArc(d,w,o->immune?yellow:white,cx-rd,cy-rd,rd*2,rd*2,0,360*64);
		XDrawLine(d,w,white,cx+rx/3,cy+ry/3,cx+rx,cy+ry);
		if (o->flags&FLG_IDENT) {
			l=strlen(o->name);
			XDrawString(d,w,white,cx-p->d.tfont->max_bounds.width*l/2,
					cy+rd+8+p->d.tfont->max_bounds.ascent,o->name,l);
		}
		sprintf(txt,"%d",o->rating); l=strlen(txt);
		if (p->rating<=(o->rating*2))
			XDrawString(d,w,white,cx-p->d.tfont->max_bounds.width*l/2,
					cy-rd-8-p->d.tfont->max_bounds.descent,txt,l);
		else XDrawString(d,w,p->d.gc_fred,
					cx-p->d.tfont->max_bounds.width*l/2,
					cy-rd-8-p->d.tfont->max_bounds.descent,txt,l);
	}
	for (struct trolley *tr=firsttrol;tr;tr=tr->next) if (tr->body.l==p->body.l) {
		int mx=tr->body.x-p->body.x, my=tr->body.y-p->body.y;
		cx=WINWID/2+mx*mc-my*ms; cy=WINHGT/2+my*mc+mx*ms;
		int r=tr->body.radius;
		XDrawArc(d,w,white,cx-r,cy-r,r*2,r*2,tr->ang*64,120*64);
		XDrawArc(d,w,red,cx-r,cy-r,r*2,r*2,((tr->ang+120)%360)*64,120*64);
		XDrawArc(d,w,blue,cx-r,cy-r,r*2,r*2,((tr->ang+240)%360)*64,120*64);
		for (o=tr->holder;o;o=o->nexthold) {
			mx=o->body.x-p->body.x; my=o->body.y-p->body.y;
			int px=WINWID/2+mx*mc-my*ms, py=WINHGT/2+my*mc+mx*ms;
			XDrawLine(d,w,p->d.gc_yellow,cx,cy,px,py);
		}
	}
}

static void draw_falling(struct player *p) {
	Display *d=p->d.disp;
	Pixmap w=p->d.backing;
	GC white=p->d.gc_white;
	int mx=WINWID/2,my=WINHGT/2;
	int rd=800/((p->body.fallen++)+20);
	XFillRectangle(d,w,p->d.gc_black,0,0,WINWID,WINHGT);
	XDrawRectangle(d,w,p->d.gc_blue,mx-100,my-100,200,200);
	XDrawArc(d,w,white,mx-rd/8,my-rd/8,rd/4,rd/4,0,360*64);
	XDrawArc(d,w,white,mx-rd/2,my-rd/2,rd,rd,0,360*64);
	XDrawLine(d,w,white,mx,my-rd/16,mx,my-3*rd/8);
	if (!(p->flags&FLG_NOMSG)) for (int i=0;i<4;i++)
		XDrawString(d,w,p->d.gc_blue,16,WINHGT-p->d.fh*(i+1),p->msg[i],
				strlen(p->msg[i]));
}


static void draw_msg(struct player *p) {
	if (!(p->flags&FLG_NOMSG)) for (int i=0;i<4;i++)
		XDrawString(p->d.disp,p->d.backing,p->d.gc_blue,16,
					WINHGT-p->d.fh*(i+1),p->msg[i],strlen(p->msg[i]));
}
