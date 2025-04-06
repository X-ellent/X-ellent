/* Global Variable Definitions */

#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "player.h"
#include "terminal.h"
#include "map.h"
#include "object.h"
#include "mymath.h"
#include "xbits.h"
#include "particle.h"
#include "newton.h"
#include "turret.h"
#include "shop.h"
#include "lift.h"
#include "addon.h"
#include "beam.h"
#include "home.h"
#include "starburst.h"
#include "message.h"
#include "xsetup.h"

/* From player.h */
struct player *playone = NULL, *freeplay = NULL;
char *weap_name[MAX_WEAPS];
char *own_name[MAX_OTHERS];
char own_dam[MAX_OTHERS];
int frame = 0, players = 0;

/* From mymath.h */
double sintable[720];
double *sn = NULL, *cs = NULL;

/* From xbits.h */
XPoint pc[PCACHE];

/* From map.h */
struct map map;
struct teleport *firsttel = NULL;
struct checkpoint *firstcheck = NULL;
int tpcount = 0, cpcount = 0, sbcount = 0;
struct starburst *firststar = NULL;

/* From object.h */
int obj_free = 0, obj_used = 0;
struct object *obj_first = NULL, *obj_freepool = NULL;
struct trolley *firsttrol = NULL;

/* From particle.h */
int part_count = 0;
struct particle *parts[MAXDEPTH], *part_free = NULL;
struct explosion *bang_first = NULL, *bang_free = NULL;

/* From xsetup.h */
int jumpable = 0;
jmp_buf jmpenv;

/* From newton.h */
struct body *firstbody = NULL;

/* From turret.h */
struct turret *firstturret = NULL;

/* From shop.h */
struct menu m_root;

/* From lift.h */
struct lift *firstlift = NULL;

/* From addon.h */
struct addtype *firstaddtype = NULL;
struct addon *freeaddon = NULL;

/* From beam.h */
struct beam *firstbeam = NULL, *freebeam = NULL;

/* From home.h */
struct home *firsthome = NULL;
