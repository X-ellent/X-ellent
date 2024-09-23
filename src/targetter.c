/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/targetter.c,v $
** $Author: cheesey $
** $Date: 1993/05/17 13:27:13 $
** $Revision: 1.2 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#include <string.h>
#include <stdio.h>

#include "addon.h"
#include "player.h"
#include "terminal.h"
#include "targetter.h"

static char txt[256];

extern void targetter_command(struct player *p,struct addon *a,char *s) {
    if (strcmp(s,"HELP")==0) {
	psend(p,"=TAR HELP\n=HELP\n");
	psend(p,"=INFO\n");
	psend(p,"=STATUS\n");
	psend(p,"=<n>NEXT<type>\n");
	psend(p,"=<n>SET<type><targ>\n");
	psend(p,"=<n>DATA\n");
	psend(p,"=<type>=P,player\n");
	return;
    }
    if (strcmp(s,"STATUS")==0) {
	psend(p,"=TAR STATUS\n");
	if (p->ptarg) {
	    psend(p,(sprintf(txt,"=T1P%s %s\n",p->ptarg->user,
			     p->ptarg->name),txt));
	} else {
	    psend(p,"=T1-\n");
	}
	return;
    }
    if (strcmp(s,"1NEXTP")==0) {
	psend(p,"=TAR NEXT\n");
	next_target(p);
	if (p->ptarg) {
	    psend(p,(snprintf(txt, sizeof(txt), "=T1P%s\n",p->ptarg->user),txt));
	} else {
	    psend(p,"=T1-\n");
	}
	return;
    }
    if (strncmp(s,"1SETP",5)==0) {
	struct player *pl;
	psend(p,"=TAR SET\n");
	if (!(pl=find_player(&s[5]))) {
	    psend(p,"!Cannot find player\n");
	    return;
	}
	p->ptarg=pl;
	psend(p,(snprintf(txt, sizeof(txt), "=T1P%s\n",p->ptarg->user),txt));
	return;
    }
    if (strcmp(s,"1DATA")==0) {
	psend(p,"=TAR DATA\n");
	if (!p->ptarg) {
	    psend(p,"=No Target");
	} else {
	    psend(p,"=Service withdrawn by popular demand\n");
	}
	return;
    }
    psend(p,(snprintf(txt, sizeof(txt), "!Unknown command to targetter\n"),txt));
    return;
}

extern void next_target(struct player *p)
{
    if (!addon_level(p->firstadd,ADD_TARGET)) {
	p->ptarg=0;
	return;
    }
    if (p->ptarg) {
	p->ptarg=p->ptarg->next;
    } else {
	p->ptarg=p->next;
    }
    if (p->ptarg==0) p->ptarg=playone;
    if (p->ptarg==p) p->ptarg=0;
    if (p->ptarg&&!p->ptarg->body.on) next_target(p);
    if (p->ptarg&&!can_locate(p,p->ptarg)) next_target(p);
}


