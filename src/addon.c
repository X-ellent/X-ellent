/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/addon.c,v $
** $Author: cheesey $
** $Date: 1993/07/01 14:05:02 $
** $Revision: 1.2 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "message.h"
#include "addon.h"
#include "player.h"
#include "fix.h"
#include "terminal.h"

extern void targetter_command(struct player *p,struct addon *a,char *s);

static void addme(int n,char *s,char *ss,int u,int d,int rc,int ca,int cb,
				  int cc);

void init_addons() {
	addme(ADD_CLOAKING,"Cloaking Device","CLO",5,10,800,6000,10000,15000);
	firstaddtype->cost[3]=21000;firstaddtype->cost[4]=28000;
	addme(ADD_COMPASS,"Compass","COM",1,20,24,200,0,0);
	addme(ADD_ENERGY,"Energy Unit","NRG",3,10,1200,10000,15000,20000);
	addme(ADD_INVIS,"Invisibility Module","INV",1,10,1200,7500,0,0);
	addme(ADD_MINESWEEP,"Mine Sweeper","MIN",1,10,600,4000,0,0);
	addme(ADD_RADAR,"Radar Unit","RAD",5,25,40,800,2000,4000);
	firstaddtype->cost[3]=8000;firstaddtype->cost[4]=16000;
	addme(ADD_TARGET,"Targetter","TAR",3,20,60,1000,1500,2000);
	addme(ADD_FUELTANK,"Extra Fuel Tank","FUE",3,30,100,2500,3750,5003);
	addme(ADD_SHIELD,"Enhanced Shield","SHL",3,20,100,2000,3000,4000);
	addme(ADD_VISUAL,"Visual Display Unit","VIS",1,20,200,4000,0,0);
	addme(ADD_ANTICLOAK,"Anti-Cloaking Device","ACL",5,10,1500,15000,22000,29000);
	firstaddtype->cost[3]=36000;firstaddtype->cost[4]=43000;
}

static void addme(int n,char *s,char *ss,int u,int d,int rc,int ca,int cb, int cc) {
	struct addtype *at;
	at=(struct addtype *) calloc(1,sizeof(struct addtype));
	strcpy(at->name,s);
	strcpy(at->subs,ss);
	at->type=n;
	at->upgrades=u;
	at->damage=d;
	at->cost[0]=ca;
	at->cost[1]=cb;
	at->cost[2]=cc;
	at->repcost=rc;
	at->next=firstaddtype;
	firstaddtype=at;
}

struct addtype *find_addtyp(int typ) {
	struct addtype *at;
	for (at=firstaddtype;at;at=at->next)
		if (at->type==typ) return at;
	return 0;
}

struct addon *find_addon(struct addon *firstaddon,int typ) {
	struct addon *ao;
	for (ao=firstaddon;ao;ao=ao->next)
		if (ao->is->type==typ) return ao;
	return 0;
}

int addon_level(struct addon *firstaddon,int typ) {
	struct addon *ao;
	for (ao=firstaddon;ao;ao=ao->next)
		if (ao->is->type==typ) return ao->level;
	return 0;
}

struct addon *add_addon(int typ) {
	struct addon *a;
	struct addtype *at;
	int i;
	if (!(at=find_addtyp(typ))) return 0;
	if ((a=freeaddon))
		freeaddon=a->next;
	else
		a=(struct addon *) calloc(1,sizeof(struct addon));
	a->next=0;
	a->is=at;
	a->level=1;
	a->damage=at->damage;
	for (i=0;i<MAX_INFO;i++)
		a->info[i]=0;
	return a;
}

void free_addon(struct addon *a) {
	a->next=freeaddon;
	freeaddon=a;
	return;
}

struct addon* strip_addons(struct player *p,struct addon *a) {
	char txt[256];
	struct addon *fa;
	struct addon *pa;
	struct addon *ca;
	struct addon *na;
	pa=0;
	fa=0;
	for (ca=a;ca;ca=na) {
		na=ca->next;
		ca->next=0;
		if (ca->damage>0) {
			if (pa) {
				pa->next=ca;
			} else {
				fa=ca;
			}
			pa=ca;
		} else {
			if (ca->level>1) {
				snprintf(txt,sizeof(txt),"Your %s has been damaged.",ca->is->name);
				ca->damage=ca->is->damage;
				ca->level--;
				new_addon_level(p,ca);
				if (pa) {
					pa->next=ca;
				} else {
					fa=ca;
				}
				pa=ca;
			} else {
				ca->level=0;
				new_addon_level(p,ca);
				snprintf(txt,sizeof(txt),"Your %s has been destroyed.",ca->is->name);
				free_addon(ca);
			}
			player_message(p,txt);
		}
	}
	return fa;
}

void new_addon_level(struct player *p,struct addon *ad) {
	char txt[256];
	snprintf(txt,sizeof(txt),"#SUBSYSTEM %s %d\n",ad->is->subs,ad->level);
	psend(p,txt);
	switch (ad->is->type) {
	case ADD_RADAR:
		if (ad->level==1) ad->info[1]=0;
		ad->info[2]=1+ad->level;
		ad->info[0]=ad->info[2];
		return;
	case ADD_TARGET:
		if (ad->level==0) p->ptarg=0;
		return;
	case ADD_FUELTANK:
		p->maxfuel=(ad->level+1)*200000;
		return;
	case ADD_SHIELD:
		p->maxshield=(ad->level*200)+1000;
		return;
	case ADD_VISUAL:
		ad->info[0]=0;
		ad->info[1]=1;
		return;
	default:
		return;
	}
}

int addon_command(struct player *p,struct addon *a,uchar *s) {
	char txt[256];
	if (strcmp((const char *)s,"INFO")==0) {
		snprintf(txt, sizeof(txt), "=%s INFO\n=%s\n=%d %d/%d\n", a->is->subs,
				a->is->name, a->level, a->damage, a->is->damage);
		psend(p, txt);
		return 3;
	}
	switch(a->is->type) {
	case ADD_ANTICLOAK:
		if (strcmp((const char *)s,"HELP")==0) {
			psend(p,"=ACL HELP\n");
			psend(p,"=HELP\n=ON\n=OFF\n=STATUS\n");
			return 3;
		}
		if (strcmp((const char *)s,"ON")==0) {
			p->flags|=FLG_ANTICLOAK;
			return 3;
		}
		if (strcmp((const char *)s,"OFF")==0) {
			p->flags&=~FLG_ANTICLOAK;
			return 3;
		}
		if (strcmp((const char *)s,"STATUS")==0) {
			if (p->flags&FLG_ANTICLOAK)
				psend(p,"=ANTICLOAK ENGAGED\n");
			else
				psend(p,"=ANTICLOAK NOT ENGAGED\n");
		}
		break;
	case ADD_MINESWEEP:
		if (strcmp((const char *)s,"HELP")==0) {
			psend(p,"=MIN HELP\n");
			psend(p,"=HELP\n=ON\n=OFF\n=STATUS\n");
			return 3;
		}
		if (strcmp((const char *)s,"ON")==0) {
			p->flags|=FLG_MINESWEEP;
			return 3;
		}
		if (strcmp((const char *)s,"OFF")==0) {
			p->flags&=~FLG_MINESWEEP;
			return 3;
		}
		if (strcmp((const char *)s,"STATUS")==0) {
			if (p->flags&FLG_MINESWEEP)
				psend(p,"=MINESWEEP ENGAGED\n");
			else
				psend(p,"=MINESWEEP NOT ENGAGED\n");
		}
		break;
	case ADD_TARGET:
		targetter_command(p,a,(char *)s);
		return 3;
	case ADD_VISUAL:
		if (strcmp((const char *)s,"HELP")==0) {
			psend(p,"=VIS HELP\n");
			psend(p,"=HELP\n=STATUS\n=INFO\n=MODE<num>\n=SCALE<num>\n");
			psend(p,"=POS<num>,<num>\n=CLEAR\n=DRAW<data>\n");
			return 3;
		}
		if (strcmp((const char *)s,"STATUS")==0) {
			psend(p,"=VIS STATUS\n");
			snprintf(txt, sizeof(txt), "=M%d %d,%d S%d\n", a->info[0], a->info[2],
					 a->info[3], a->info[1]);
			psend(p, txt);
			return 3;
		}
		if (strncmp((const char *)s,"MODE",4)==0) {
			a->info[0]=atoi((const char *)&s[4]);
			return 3;
		}
		if (strncmp((const char *)s,"SCALE",5)==0) {
			a->info[1]=atoi((const char *)&s[5]);
			return 3;
		}
		if (strncmp((const char *)s,"POS",3)==0) {
			char *comma;
			if (!(comma=strchr((char *)s,','))) {
				psend(p,"!Bad command\n");
				return 3;
			}
			*comma=0;
			a->info[2]=atoi((const char *)(s+3));
			*comma=',';
			a->info[3]=atoi((const char *)(comma+1));
			return 3;
		}
		if (strcmp((const char *)s,"CLEAR")==0) {
			int i;
			for(i=0;i<64;i++) p->lines[i][0]=0;
			return 3;
		}
		if (strncmp((const char *)s,"DRAW",4)==0) {
			int i,l,n;
			l=(int)strlen((char*)s);
			for (i=4;i<l;i++) {
				if ((s[i]>63)&&(s[i]<128)) {
					n=s[i]-64;
					p->lines[n][0]=s[++i]-4;
					p->lines[n][1]=s[++i]-4;
					p->lines[n][2]=s[++i]-4;
					p->lines[n][3]=s[++i]-4;
				} else {
					if ((s[i]>127)&&(s[i]<192)) {
						n=s[i]-128;
						p->lines[n][0]=0;
					}
				}
			}
			return 3;
		}
		psend(p,"!Invalid command to visual display unit\n");
		return 3;
	}
	snprintf(txt, sizeof(txt), "!Unknown command to subsystem %s\n", a->is->subs);
	psend(p, txt);
}
