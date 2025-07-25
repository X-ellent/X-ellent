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
#include <sys/time.h>  // For gettimeofday

#include "fix.h"
#include "message.h"
#include "debug.h"
#include "player.h"
#include "particle.h"
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
#include "mines.h"	   // For load_mines, save_mines
#include "beam.h"      // For free_beams
#include "starburst.h" // For fire_starbursts
#include "map.h"	   // For update_teleports
#include "mymath.h"	   // For sintable

// Global variables
static int lastsave, sleepsave, saveall;
extern int players;  // Defined in player.h
extern int frame;    // Defined in player.h

// Function declarations
void load_mines(void);
void save_mines(void);
void fire_starbursts(void);
void update_teleports(void);

static void setsave() { saveall=-2; return; }
static void setquit() { saveall=-1; return; }

void build_sintable() {
	int i;
	DL("Building sin table");
	for (i=1;i<90;i++) {
		sintable[i]=sin(((double) i)/180.0*M_PI);
		sintable[180-i]=sintable[i];
	}
	sintable[0]=0; sintable[90]=1;
	for (i=0;i<180;i++) sintable[i+180]=-sintable[i];
	for (i=0;i<90;i++) sintable[i+360]=sintable[i];
	sn=sintable; cs=&sintable[90];
}

int main(int argc, char *argv[]) {
	(void)argc; (void)argv;  // Mark unused parameters
	struct timeval start_time, end_time;  // For frame timing
	long frame_time_us;  // Frame time in microseconds
	const long target_frame_time_us = 59000;  // Target ~17fps (59ms per frame)

	struct player *p;

	if (chdir(LIB) != 0) {
		fprintf(stderr, "Warning: Failed to change directory to %s\n", LIB);
		exit(1);
	}
	DL("Entering main");
	if ((signal(SIGPIPE,SIG_IGN))==SIG_ERR) {
		printf("Error: Signals handling failure!\n");
		exit(1);
	}
	if ((signal(SIGUSR1,setsave))==SIG_ERR) {
		printf("Error: Signals handling failure!\n");
		exit(1);
	}
	if ((signal(SIGTERM,setquit))==SIG_ERR) {
		printf("Error: Signals handling failure!\n");
		exit(1);
	}
	int ti;
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
	load_mines();
	setup_error_handler();

	// Main loop
	while (1) {
		gettimeofday(&start_time, NULL);  // Get start time of frame

		for (p=playone;p;p=p->next) if (p->connected) draw_all(p);
		free_beams();
		fire_starbursts();
		for (p=playone;p;p=p->next) if (p->playing) update_player(p);
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

		// Frame rate limiter
		gettimeofday(&end_time, NULL);
		frame_time_us = (end_time.tv_sec - start_time.tv_sec) * 1000000 +
						(end_time.tv_usec - start_time.tv_usec);

		if (frame_time_us < target_frame_time_us)
			usleep(target_frame_time_us - frame_time_us);
	} // Main game loop

	DL("Exiting");
	return 0;
}

