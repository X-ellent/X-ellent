/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/constants.h,v $
** $Author: cheesey $
** $Date: 1993/07/01 14:05:02 $
** $Revision: 1.8 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#ifndef My_CONSTANTS_H
#define My_CONSTANTS_H

 /* Sometimes i really mourn the non-availablility of such wonderful */
 /* features such as being able to place comments after #defines just */
 /* so that I can say what all of the things in here do on the line */
 /* that they do them in, ah well, i guess we cant have everything in */
 /* this life can we??? *wishful sigh* */

#define FRICTION 50.0
#define INERTIA 100.0
#define SPINSPEED 1000
#define MAXVEL 40.0
#define FORCE 1000
#define PUSH_FORCE 2000
#define PUSH_FORCES 1414

#define GAME_SOCK 8765
#define TERM_SOCK 8766

#define START_CASH       180
#define START_IMMUNE     100
#define START_IMMUNE_REJ 50

#define DEATH_BONUS    200
#define DEATH_CASH     500
#define DEATH_CASH_MIN 100
#define DEATH_DELAY    100
#define DEATH_SHOW     70
#define DEATH_IMMUNE   60
#define DEATH_MSG      "You Have Died.. Bogus."

#define GROUND_TIME 16
#define FALL_MIN_SHIELD 5
#define FALL_MIN_FUEL 50*200
#define FALL_IMMUNE 3

#define TROLLEY_FALL_TIME 30
#define STRING_LENGTH 80
#define STRING_FORCE 15
#define STRING_MAX_LENGTH 500

#define MIN_FUEL  20000

#define FUEL_COST   2
#define FUEL_PLUS   1500

#define SWEEP_FUEL_DRAIN 25
#define CLOAK_FUEL_DRAIN 50
#define INVIS_FUEL_DRAIN 100
#define ANTICLOAK_FUEL_DRAIN 200

 /* Defines for turrets: Shield level, Pause before regeneration, Delay */
 /* between shots, Maximum updates in one frame */

#define TURRET_SHIELD 500
#define TURRET_DIE_COUNT 4500
#define TURRET_DELAY 20
#define TURRET_MAXNUM 8
#define TURRET_DELAY_VINDICTIVE 6
#define TURRET_VICTIM_BIAS 4

#define BONUS_RAND 200
#define BONUS_TIME 1000
#define BONUS_AGE 10000
#define BONUS_SHOUT 1000

#define SAVE_TIME 1000
#define HOME_TAKE_TIME 2000

#define TERM_NUMBER 8
#define TERM_ROMSIZE 32000

#define MAX_WEAPS   10
#define MAX_OTHERS  10
#define MAX_UPGRADE 10
#define MAX_INFO    4
#define LOCATE_TIME 1
#define LOCATE_TARGET_TIME 2

#define RECHARGE_TARGLASER 2
#define RECHARGE_LASER     10

#define DEFNAME "xellent"

#endif

