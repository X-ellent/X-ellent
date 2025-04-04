/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/map.h,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.7 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#ifndef My_MAP_H
#define My_MAP_H

#define MAXDEPTH 64

struct map {
	char name[64];
	char author[64];
	char comments[256];
	int  wid,hgt,depth;
	char *data[MAXDEPTH];
	char *data2[MAXDEPTH];
	char *data3[MAXDEPTH];
};

struct checkpoint {
	struct checkpoint *next;
	int l,x,y;
	int num;
};

struct teleport {
	struct teleport *next;
	int l,x,y;
	int num;
	int clk;
	struct teleport *dest;
	char pass[5];
};

extern struct map map;
extern struct checkpoint *firstcheck;
extern struct teleport *firsttel;
extern int cpcount;
extern int tpcount;

#define MAP_SOLID	  (1<<0)
#define MAP_TWALL0	  (1<<1)
#define MAP_TWALL1	  (1<<2)
#define MAP_LWALL0	  (1<<3)
#define MAP_LWALL1	  (1<<4)
#define MAP_OBSC	  (1<<5)
#define MAP_FRICT	  (1<<6)
//#define MAP_XTRA	  (1<<7)

#define MAP_NET_NODE  (1<<0)
#define MAP_NET_SHUT  (1<<1)

#define MAP_TWALL	(MAP_TWALL0|MAP_TWALL1)
#define MAP_LWALL	(MAP_LWALL0|MAP_LWALL1)

#define rd(l,x,y) (map.data[l][(x)+(y)*map.wid])
#define rd2(l,x,y) (map.data2[l][(x)+(y)*map.wid])
#define rd3(l,x,y) (map.data3[l][(x)+(y)*map.wid])

int is_hole(int l,int x,int y);
void create_map();
struct checkpoint *pick_check();
struct teleport *find_teleport(int n);
struct teleport *locate_teleport(int l,int x,int y);
void use_teleport(struct teleport *t);

#endif
