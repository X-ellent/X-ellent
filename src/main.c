/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/main.c,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.7 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ip_user.h"
#include "fix.h"
#include "message.h"
#include "debug.h"
#include "player.h"
#include "particle.h"
#include "init.h"
#include "constants.h"
#include "telnet.h"
#include "xsetup.h"
#include "shop.h"
#include "terminal.h"
#include "turret.h"
#include "object.h"
#include "bonus.h"
#include "draw.h"
#include "lift.h"
#include "addon.h"

#define MAX_USERS 100

struct ip_user ip_users[MAX_USERS];
int num_ip_users = 0;

static int lastsave;
static int sleepsave;
static int saveall;

extern int main();

void load_ip_users() {
    FILE *fp = fopen("ip_users.txt", "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: Cannot open ip_users.txt\n");
        exit(1);
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        char ip[16], username[32];
        if (sscanf(line, "%15s %31s", ip, username) == 2) {
            strcpy(ip_users[num_ip_users].ip, ip);
            strcpy(ip_users[num_ip_users].username, username);
            num_ip_users++;
            if (num_ip_users >= MAX_USERS) {
                break;
            }
        }
    }

    fclose(fp);
}

static void setsave(int sig) {
    saveall=-2;
    return;
}

static void setquit(int sig) {
    saveall=-1;
    return;
}

extern int main(int argc,char *argv[])
{
    int ti;
    struct player *p;
    saveall=0;
    chdir(LIB);
    DL("Entering main");
    if ((signal(SIGPIPE,SIG_IGN))==SIG_ERR) {
	(void) printf("Error: Signals handling failure!\n");
        exit(1);
    }
    if ((signal(SIGUSR1,setsave))==SIG_ERR) {
	(void) printf("Error: Signals handling failure!\n");
        exit(1);
    }
    if ((signal(SIGUSR2,setquit))==SIG_ERR) {
	(void) printf("Error: Signals handling failure!\n");
        exit(1);
    }
    time((time_t*)&ti);
    srandom(ti);
    build_sintable();
    open_sockets();
    create_map();
    init_addons();
    init_all_shop();
    init_all_weap();
    init_all_term();
    init_all_trolleys();
    load_players();
    load_ip_users();
    load_mines();
    players=0;
    lastsave=0;
    sleepsave=0;
    setup_error_handler();
    while (1) {
	for (p=playone;p;p=p->next) 
	    if (p->connected) draw_all(p);
	free_beams();
	fire_starbursts();
	for (p=playone;p;p=p->next) 
	    if (p->playing) update_player(p);
	update_turrets();
	update_bonus();
	update_teleports();
	move_particles();
	move_explosions();
	move_objects();
	update_trolleys();
	do_collisions();
	move_lifts();
	frame++;
	lastsave++;
	if (!players) {
	    sleep(1);
	    sleepsave++;
	    if (sleepsave==2) {
		save_players();
		save_mines();
	    }
	} else {
	    sleepsave=0;
	    if (lastsave>SAVE_TIME) {
		save_players();
		save_mines();
		lastsave=0;
	    }
	}
	if (saveall==-2) {
	    saveall=0;
	    global_message("Signal recieved: Saving all data...");
	    save_players();
	    save_mines();
	}
	if (saveall==-1) {
	    global_message("Signal recieved: Shutting down.");
	    save_players();
	    save_mines();
	    for (p=playone;p;p=p->next)
		if (p->connected) {
		    XAutoRepeatOn(p->d.disp);
		    shutdown_display(p);
		}
	    exit(0);
	}
	do_login();
    }
    sleep(5);
    DL("Exiting");
    return 0;
}

