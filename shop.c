/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/shop.c,v $
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
#include <stdlib.h>

#include "fix.h"
#include "player.h"
#include "map.h"
#include "shop.h"
#include "object.h"
#include "addon.h"
#include "constants.h"

static FILE *pdata;
static char buf[2048];
static int pos;
static char str[2048];

static int readin();
static void next_bit();
static void show_shop_menu(struct player *p);
static void buy_mine(struct player *p,struct item *it);
static void buy_ammo(struct player *p,struct item *it);
static void buy_weap(struct player *p,struct item *it);
static void buy_othr(struct player *p,struct item *it);
static void buy_repr(struct player *p,struct item *it);

extern void init_all_shop() {

	if (!(pdata=fopen("price.data","rb"))) {
		perror("Cannot open price file");
		exit(1);
	}

	read_menu(&m_root);

	fclose(pdata);
}

static int readin() {
	int l;
	*buf=0;
	if (fgets(buf, 2048, pdata) == NULL) {
		return 0;
	}
	if (!*buf) return 0;
	l=strlen(buf);
	if (buf[l-1]=='\n') buf[l-1]=0;
	pos=0;
	return 1;
}

static void next_bit() {
	char *n;
	if (pos==-1) {
		*str=0;
		return;
	}
	n=(char *) strchr(&buf[pos],',');
	if (!n) {
		strcpy(str,&buf[pos]);
		pos=-1;
		return;
	} else {
		*n=0;
		strcpy(str,&buf[pos]);
		pos=n-buf+1;
	}
}

extern void read_menu(struct menu *m) {
	int n;
	readin();
	next_bit();
	if (strlen(str) >= 63) {
		memcpy(m->name, str, 63);
	} else {
		strcpy(m->name, str);
	}
	next_bit();switch(*str) {
	case 'M':m->show=SHOW_MINES;break;
	case 'W':m->show=SHOW_WEAPS;break;
	case 'O':m->show=SHOW_OTHER;break;
	}
	for (n=0;n<10;n++) {
		readin();
		if (*buf=='.') return;
		m->item[n]=(struct item *) calloc(1,sizeof(struct item));
		next_bit();
		if (strlen(str) >= 63) {
			memcpy(m->item[n]->name, str, 63);
		} else {
			strcpy(m->item[n]->name, str);
		}
		next_bit();
		if (*str=='>') {
			m->item[n]->type=ITEM_NEXT;
			m->item[n]->sel.next=(struct menu *) calloc(1,sizeof(struct menu));
			m->item[n]->sel.next->prev=m;
			read_menu(m->item[n]->sel.next);
		} else {
			switch (*str) {
			case 'M':stock_mine(m->item[n]);break;
			case 'A':stock_ammo(m->item[n]);break;
			case 'W':stock_weap(m->item[n]);break;
			case 'O':stock_othr(m->item[n]);break;
			case 'R':stock_repr(m->item[n]);break;
			}
			next_bit();m->item[n]->sel.price=atoi(str);
		}
	}
}

extern void stock_mine(struct item *i) {
	i->type=ITEM_MINE;
	next_bit();i->obj.mine.type=atoi(str);
	next_bit();i->obj.mine.charge=atoi(str);
}

extern void stock_ammo(struct item *i) {
	i->type=ITEM_AMMO;
	next_bit();i->obj.ammo.type=atoi(str);
	next_bit();i->obj.ammo.qty=atoi(str);
}

extern void stock_weap(struct item *i) {
	i->type=ITEM_WEAP;
	next_bit();i->obj.weap.type=atoi(str);
	next_bit();i->obj.weap.ammo=atoi(str);
}

extern void stock_othr(struct item *i) {
	i->type=ITEM_OTHR;
	next_bit();i->obj.othr.type=atoi(str);
}

extern void stock_repr(struct item *i) {
	i->type=ITEM_REPR;
	next_bit();i->obj.repr.type=atoi(str);
}

extern void init_shop(struct player *p) {
	p->flags|=FLG_SHOPPING;
	remove_body(&p->body);
	p->shopmenu=&m_root;
	psend(p,"#SHOP ENTER\n");
	show_shop_menu(p);
}

extern void exit_shop(struct player *p) {
	if (p->shopmenu->prev) {
		p->shopmenu=p->shopmenu->prev;
		show_shop_menu(p);
	} else {
		add_body(&p->body);
		p->flags&=~(FLG_SHOPPING|FLG_BRAKING);
		psend(p,"#SHOP EXIT\n");
	}
}

static void show_shop_menu(struct player *p)
{
	Display *d;
	Pixmap w;
	GC red,dgrey,yellow;
	int i,n;
	char prc[80];
	d=p->d.disp;
	w=p->d.backing;
	red=p->d.gc_red;
	dgrey=p->d.gc_dgrey;
	yellow=p->d.gc_yellow;
	XFillRectangle(p->d.disp,p->d.backing,p->d.gc_black,0,0,WINWID,WINHGT);

	if (p->shopmenu->name[0]) {
		i=strlen(p->shopmenu->name);
		XDrawString(p->d.disp,p->d.backing,p->d.gc_yellow,WINWID/2-p->d.bw*i/2,
					p->d.bo,p->shopmenu->name,i);
	} else {
		XDrawString(p->d.disp,p->d.backing,p->d.gc_yellow,WINWID/2-p->d.bw*2,
					p->d.bo,"Shop",4);
	}

	snprintf(prc, sizeof(prc), "Cash: %d", p->cash);
	XDrawString(d,w,yellow,40,p->d.bo*2+p->d.bh/2,prc,strlen(prc));
	for (i=1;(i<10)&&(p->shopmenu->item[i-1]);i++) {
		char c;
		c='0'+i;
		XDrawString(d,w,red,20,100+p->d.fh*i,&c,1);
		if ((p->shopmenu->item[i-1]->type!=ITEM_OTHR)&&
			(p->shopmenu->item[i-1]->type!=ITEM_REPR)) {
			XDrawString(d,w,red,20+p->d.fw*3,100+p->d.fh*i,
						p->shopmenu->item[i-1]->name,
						strlen(p->shopmenu->item[i-1]->name));
			if (p->shopmenu->item[i-1]->type!=ITEM_NEXT) {
				snprintf(prc, sizeof(prc), "$%d", p->shopmenu->item[i-1]->sel.price);
				XDrawString(d,w,red,WINWID*3/4,100+p->d.fh*i,prc,strlen(prc));
			} else {
				XDrawString(d,w,red,WINWID*3/4,100+p->d.fh*i,"<More>",6);
			}
		} else {
			int typ;
			struct addtype *adt;
			int l;
			typ=p->shopmenu->item[i-1]->obj.othr.type;
			if ((adt=find_addtyp(typ))) {
				if (p->shopmenu->item[i-1]->type!=ITEM_REPR) {
					l=addon_level(p->firstadd,typ)+1;
					if (l>adt->upgrades) {
						snprintf(prc, sizeof(prc), "%s", adt->name);
					} else {
						char fmt[120];
						snprintf(fmt, sizeof(fmt), "%s Mark %%d", adt->name);
						snprintf(prc, sizeof(prc), fmt, l);
					}
					XDrawString(d,w,red,20+p->d.fw*3,100+p->d.fh*i,
								prc,strlen(prc));
					if (l<=adt->upgrades) {
						snprintf(prc, sizeof(prc), "$%d", adt->cost[l-1]);
						XDrawString(d,w,red,WINWID*3/4,100+p->d.fh*i,prc,
									strlen(prc));
					} else {
						XDrawString(d,w,red,WINWID*3/4,100+p->d.fh*i,
									"-- None --",10);
					}
				} else {
					l=addon_level(p->firstadd,typ);
					if (!l) {
						snprintf(prc, sizeof(prc), "%s", adt->name);
					} else {
						char fmt[120];
						snprintf(fmt, sizeof(fmt), "%s Mark %%d", adt->name);
						snprintf(prc, sizeof(prc), fmt, l);
					}
					XDrawString(d,w,red,20+p->d.fw*3,100+p->d.fh*i,
								prc,strlen(prc));
					if (l) {
						if (adt->damage!=(find_addon(p->firstadd,adt->type)->damage)) {
							snprintf(prc, sizeof(prc), "$%d", adt->repcost*l);
						} else {
							snprintf(prc, sizeof(prc), "Repaired");
						}
						XDrawString(d,w,red,WINWID*3/4,100+p->d.fh*i,prc,
									strlen(prc));
					}
				}
			}
		}
	}
	i++;
	XDrawString(d,w,red,20,100+p->d.fh*i,"Esc",3);
	if (p->shopmenu->prev) {
		XDrawString(d,w,red,20+p->d.fw*3,100+p->d.fh*i,"Previous menu",13);
	} else {
		XDrawString(d,w,red,20+p->d.fw*3,100+p->d.fh*i,"Exit the shop.",14);
	}
	n=14;
	switch (p->shopmenu->show) {
	case SHOW_MINES:
		XDrawString(d,w,red,20,100+p->d.fh*n++,"Drop-Slots Filled:",18);
		for (i=0;i<9;i++)
			if (p->slots[i]!=OBJ_EMPTY) {
				snprintf(prc, sizeof(prc), "%d   ",i+1);
				switch(p->slots[i]) {
				case OBJ_MINE_TRIG:strncat(prc, "Mine (Trig)", sizeof(prc) - strlen(prc) - 1);break;
				case OBJ_MINE_TIME:strncat(prc, "Mine (Time)", sizeof(prc) - strlen(prc) - 1);break;
				case OBJ_MINE_PROX:strncat(prc, "Mine (Prox)", sizeof(prc) - strlen(prc) - 1);break;
				case OBJ_MINE_VELY:strncat(prc, "Mine (Vely)", sizeof(prc) - strlen(prc) - 1);break;
				default:strncat(prc, "Unknown", sizeof(prc) - strlen(prc) - 1);break;
				}
				switch(p->size[i]) {
				case 1:strncat(prc, " <Small>", sizeof(prc) - strlen(prc) - 1);break;
				case 2:strncat(prc, " <Medium>", sizeof(prc) - strlen(prc) - 1);break;
				case 3:strncat(prc, " <Large>", sizeof(prc) - strlen(prc) - 1);break;
				case 4:strncat(prc, " <Extra>", sizeof(prc) - strlen(prc) - 1);break;
				}
				if (p->slotobj[i])
					strncat(prc, " (Dropped)", sizeof(prc) - strlen(prc) - 1);
				XDrawString(d,w,dgrey,40,100+p->d.fh*n++,prc,strlen(prc));
			} else {
				snprintf(prc, sizeof(prc), "%d   Slot Empty", i+1);
				XDrawString(d,w,dgrey,40,100+p->d.fh*n++,prc,14);
			}
		break;
	case SHOW_WEAPS:
		XDrawString(d,w,red,20,100+p->d.fh*n++,"Weapon Systems Fitted:",22);
		for (i=0;i<MAX_WEAPS;i++)
			if(p->weap_mask&(1<<i)) {
				XDrawString(d,w,dgrey,40,100+p->d.fh*n,weap_name[i],
							strlen(weap_name[i]));
				if (i) {
					snprintf(prc, sizeof(prc), "%d Ammo", p->ammo[i]);
				} else {
					snprintf(prc, sizeof(prc), "Inf. Ammo");
				}
					XDrawString(d,w,dgrey,WINWID/2+90,100+p->d.fh*n++,prc,
								strlen(prc));
			}
		break;
	case SHOW_OTHER:
		XDrawString(d,w,red,20,100+p->d.fh*n++,"Other Systems Installed:",24);
		{
			struct addon *ao;
			for (ao=p->firstadd;ao;ao=ao->next) {
				snprintf(prc, sizeof(prc), "   %s Mk.%d <%d/%d>", ao->is->name,
						ao->level, ao->damage, ao->is->damage);
				XDrawString(d,w,dgrey,20,100+p->d.fh*n++,prc,strlen(prc));
			}
		}
		break;
	};
}

extern void buy_shop(struct player *p,int n)
{
	if (!p->shopmenu->item[n]) return;
	if (p->shopmenu->item[n]->type==ITEM_NEXT) {
		p->shopmenu=p->shopmenu->item[n]->sel.next;
		p->shopsel=n;
		show_shop_menu(p);
	} else {
		struct item *it;
		it=p->shopmenu->item[n];
		if (it->sel.price==0) return;
		if (it->sel.price>p->cash) return;
		switch (it->type) {
		case ITEM_MINE:buy_mine(p,it);break;
		case ITEM_AMMO:buy_ammo(p,it);break;
		case ITEM_WEAP:buy_weap(p,it);break;
		case ITEM_OTHR:buy_othr(p,it);break;
		case ITEM_REPR:buy_repr(p,it);break;
		}
	}
}

static void buy_mine(struct player *p,struct item *it) {
	int s;
	for (s=0;s<9;s++)
		if (p->slots[s]==OBJ_EMPTY) {
			p->cash-=it->sel.price;
			p->slots[s]=it->obj.mine.type;
			p->size[s]=it->obj.mine.charge;
			p->slotobj[s]=0;
			switch(it->obj.mine.type) {
			case OBJ_MINE_TRIG:p->mode[s]=0;break;
			case OBJ_MINE_TIME:p->mode[s]=10;break;
			case OBJ_MINE_PROX:p->mode[s]=20+5*it->obj.mine.charge;break;
			case OBJ_MINE_VELY:p->mode[s]=50;break;
			}
			show_shop_menu(p);
			return;
		}
}

static void buy_ammo(struct player *p,struct item *it) {
	p->cash-=it->sel.price;
	p->ammo[it->obj.ammo.type]+=it->obj.ammo.qty;
	show_shop_menu(p);
}

static void buy_weap(struct player *p,struct item *it) {
	if (p->weap_mask&(1<<it->obj.weap.type)) return;
	p->cash-=it->sel.price;
	p->weap_mask|=(1<<it->obj.weap.type);
	p->ammo[it->obj.weap.type]=it->obj.weap.ammo;
	show_shop_menu(p);
}

static void buy_othr(struct player *p,struct item *it) {
	int j,l;
	struct addon *ao;
	struct addtype *atp;
	j=it->obj.othr.type;
	atp=find_addtyp(j);
	if (!atp) return;
	ao=find_addon(p->firstadd,j);
	l=1;
	if (ao) l=ao->level+1;
	if (l>atp->upgrades) return;
	if (p->cash<atp->cost[l-1]) return;
	p->cash-=atp->cost[l-1];
	if (!ao) {
		ao=add_addon(j);
		ao->next=p->firstadd;
		p->firstadd=ao;
	} else {
		ao->level++;
		ao->damage=ao->is->damage;
	}
	new_addon_level(p,ao);
	show_shop_menu(p);
}

static void buy_repr(struct player *p,struct item *it) {
	int i,l;
	struct addon *ao;
	i=it->obj.repr.type;
	ao=find_addon(p->firstadd,i);
	if (!ao) return;
	if (ao->damage>=ao->is->damage) return;
	l=ao->level*ao->is->repcost;
	if (p->cash<l) return;
	p->cash-=l;
	ao->damage++;
	show_shop_menu(p);
}


extern void enter_shop(struct player *p) {
	int x,y;
	if ((p->body.xv<.1)&&(p->body.xv>-0.1)&&
		(p->body.yv<0.1)&&(p->body.yv>-0.1)) {
		x=(int) p->body.x;
		y=(int) p->body.y;
		x&=127;y&=127;
		if ((x>32)&&(x<=96)&&(y>32)&&(y<=96)) {
			p->body.xv=0;p->body.yv=0;p->body.xf=0;p->body.yf=0;
			p->rv=0;p->rt=0;
			p->flags&=(~(FLG_THRUST|FLG_BRAKING|FLG_ROTCLOCK|FLG_ROTACLOCK|
						 FLG_FIRING));
			remove_body(&p->body);
			p->flags|=FLG_SHOPPING;
			init_shop(p);
		}
	}
}
