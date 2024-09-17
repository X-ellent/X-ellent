/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/shop.h,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.7 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#ifndef My_SHOP_H
#define My_SHOP_H

struct menu {
    char name[64];
    struct item *item[10];
    struct menu *prev;
    int show;
};

struct sh_mine {
    int type;
    int charge;
};

struct sh_ammo {
    int type;
    int qty;
};

struct sh_weap {
    int type;
    int ammo;
};

struct sh_othr {
    int type;
};

struct sh_repr {
    int type;
};

struct item {
    char name[64];
    int type;
    union {
	struct menu *next;
	int price;
    } sel;
    union {
	struct sh_mine mine;
	struct sh_ammo ammo;
	struct sh_weap weap;
	struct sh_othr othr;
	struct sh_repr repr;
    } obj;
};

struct menu m_root;

#define ITEM_NEXT 1
#define ITEM_MINE 2
#define ITEM_AMMO 3
#define ITEM_WEAP 4
#define ITEM_OTHR 5
#define ITEM_REPR 6

#define SHOW_NOTHING   0
#define SHOW_MINES     1
#define SHOW_WEAPS     2
#define SHOW_OTHER     3

extern void init_all_shop();
extern void read_menu(struct menu *m);
extern void stock_mine(struct item *i);
extern void stock_ammo(struct item *i);
extern void stock_weap(struct item *i);
extern void stock_othr(struct item *i);
extern void stock_repr(struct item *i);
extern void init_shop(struct player *p);
extern void exit_shop(struct player *p);
extern void enter_shop(struct player *p);
extern void buy_shop(struct player *p,int n);


#endif
