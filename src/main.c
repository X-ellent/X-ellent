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

    #define NUM_SECTIONS 16  // Number of sections to be timed

    // Section labels
    const char* section_names[NUM_SECTIONS] = {
        "draw_all", "free_beams", "fire_starbursts", "update_players", "update_turrets", "update_bonus",
        "update_teleports", "move_particles", "move_explosions", "move_objects", "update_trolleys",
        "do_collisions", "move_lifts", "save_players", "do_login", "timings" };

    double cumulative_section_times[NUM_SECTIONS] = {0};
    int fps = 0;

    struct timespec section_start_time; // Store the start time of current function
    struct timespec now; // Store the current time

    void start_timing(int current_section_index) {
        static int previous_section_index = -1;

        clock_gettime(CLOCK_MONOTONIC, &now);

        if (previous_section_index != -1) {
            // Stop timing the previous section
            double time_us = (now.tv_sec - section_start_time.tv_sec) * 1000000.0 +
                         (now.tv_nsec - section_start_time.tv_nsec) / 1e3;
            cumulative_section_times[previous_section_index] += time_us;
        }

        // Start timing the current section
        section_start_time = now;
        previous_section_index = current_section_index;
    }

    struct timespec loop_start_time, second_timer_start;
    double target_loop_time_ms = 60.0;  // Target loop time in milliseconds
    double work_time_ms = 0.0, sleep_time_ms = 0.0, utilization = 0.0;
    double cumulative_work_time = 0.0, cumulative_sleep_time = 0.0;

    clock_gettime(CLOCK_MONOTONIC, &loop_start_time);
    second_timer_start = loop_start_time;

    FILE *csv_file;
    csv_file = fopen("performance_log.csv", "w");
    fprintf(csv_file, "Timestamp,");
    for (int i = 0; i < NUM_SECTIONS; i++)
        fprintf(csv_file, "%s,", section_names[i]);

    // Main loop
    while (1) {
        fps++;

        start_timing(0);
	for (p=playone;p;p=p->next) 
	    if (p->connected) draw_all(p);
        start_timing(1);
	free_beams();
        start_timing(2);
	fire_starbursts();
        start_timing(3);
	if (players) for (p=playone;p;p=p->next)
	    if (p->playing) update_player(p);
        start_timing(4);
	update_turrets();
        start_timing(5);
	update_bonus();
        start_timing(6);
	update_teleports();
        start_timing(7);
	move_particles();
        start_timing(8);
	move_explosions();
        start_timing(9);
	move_objects();
        start_timing(10);
	update_trolleys();
        start_timing(11);
	if (players) do_collisions();
        start_timing(12);
	move_lifts();
	frame++;
	lastsave++;
        start_timing(13);
	if (!players) {
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
                fprintf(stderr, "saveall==-1\n");
	        exit(0);
	}
        start_timing(14);
	do_login();
        start_timing(15); // "now" is used for this section

        work_time_ms = (now.tv_sec - loop_start_time.tv_sec) * 1000.0 +
                       (now.tv_nsec - loop_start_time.tv_nsec) / 1e6 - sleep_time_ms;

        if (sleepsave)  // Non-zero when no players
            sleep_time_ms = 1000;
        else
            sleep_time_ms = target_loop_time_ms - work_time_ms;

        // Track cumulative work and sleep times
        cumulative_work_time += work_time_ms;
        cumulative_sleep_time += sleep_time_ms;

        // Every second, print average debug info
        double elapsed_time = (now.tv_sec - second_timer_start.tv_sec) +
                              (now.tv_nsec - second_timer_start.tv_nsec) / 1e9;
        if (elapsed_time >= 1.0) {
            // Print average timings for all sections
            for (int i = 0; i < NUM_SECTIONS; i += 3) {
                printf("%-16s: %-7.2f us   %-16s: %-7.2f us   %-16s: %-7.2f us\n",
                        section_names[i], cumulative_section_times[i] / fps,
                        (i+1 < NUM_SECTIONS) ? section_names[i+1] : "", (i+1 < NUM_SECTIONS) ? cumulative_section_times[i+1] / fps : 0,
                        (i+2 < NUM_SECTIONS) ? section_names[i+2] : "", (i+2 < NUM_SECTIONS) ? cumulative_section_times[i+2] / fps : 0);
            }
            fprintf(csv_file, "%ld.%09ld,", now.tv_sec, now.tv_nsec);
            for (int i = 0; i < NUM_SECTIONS; i++)
                fprintf(csv_file, "%.2f,", cumulative_section_times[i] / fps);

            // Print utilization info
            double avg_work_time_ms = cumulative_work_time / fps;
            double avg_sleep_time_ms = cumulative_sleep_time / fps;
            utilization = (avg_work_time_ms / target_loop_time_ms) * 100;
            printf("FPS=%d Avg work time: %.2f ms, Avg sleep time: %.2f ms, Utilization: %.2f%%\n",
                    fps, avg_work_time_ms, avg_sleep_time_ms, utilization);

            fflush(csv_file);  // Ensure data is written immediately

            // Reset counters for the next second
            cumulative_work_time = 0.0;
            cumulative_sleep_time = 0.0;
            fps = 0;
            for (int i = 0; i < NUM_SECTIONS; i++)
                cumulative_section_times[i] = 0.0;

            second_timer_start = now;  // Reset the timer
        }
        loop_start_time = now;
        start_timing(-1); // Sleep doesn't have a section
        if (sleep_time_ms > 0)
            usleep(sleep_time_ms * 1000);  // usleep takes microseconds
    } // Main game loop

    sleep(5);
    DL("Exiting");
    return 0;
}

