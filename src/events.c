/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/events.c,v $
** $Author: cheesey $
** $Date: 1993/07/01 14:05:02 $
** $Revision: 1.8 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#include <stdio.h> // For stderr
//#include <X11/keysym.h>
#include <X11/XKBlib.h>  // For XkbKeycodeToKeysym

#include "player.h"
//#include "debug.h"
//#include "constants.h"
//#include "map.h"
#include "home.h" // For go_home
//#include "terminal.h"
#include "shop.h" // For enter_shop
//#include "newton.h"
#include "mines.h" // For detonate_slot
#include "message.h"
#include "lift.h"
#include "addon.h"

//static void key_press(struct player *p,int k);
//static void key_unpress(struct player *p,int k);

static char c;
static struct addon *ad;

static void key_press(struct player *p,int k) {
	int t;
	if (!p->body.on) {
		if (p->flags&FLG_HOME) switch (k) {
			case XK_Escape:exit_home(p);break;
			case XK_Control_L:
			case XK_Control_R:p->flags|=FLG_CTRL;break;
			case XK_f:p->flags|=FLG_FUELLING;break;
			case XK_1:home_mine(p,0);break;
			case XK_2:home_mine(p,1);break;
			case XK_3:home_mine(p,2);break;
			case XK_4:home_mine(p,3);break;
			case XK_5:home_mine(p,4);break;
			case XK_6:home_mine(p,5);break;
			case XK_7:home_mine(p,6);break;
			case XK_8:home_mine(p,7);break;
			case XK_9:home_mine(p,8);break;
			case XK_q:p->qflags|=16;break;
			case XK_F1:psend(p,"KF1\n");break;
			case XK_F2:psend(p,"KF2\n");break;
			case XK_F3:psend(p,"KF3\n");break;
			case XK_F4:psend(p,"KF4\n");break;
			case XK_F5:psend(p,"KF5\n");break;
			case XK_F6:psend(p,"KF6\n");break;
			case XK_F7:psend(p,"KF7\n");break;
			case XK_F8:psend(p,"KF8\n");break;
			case XK_F9:psend(p,"KF9\n");break;
			case XK_F10:psend(p,"KF10\n");break;
			case XK_F11:psend(p,"KF11\n");break;
			case XK_F12:psend(p,"KF12\n");break;
			case XK_KP_F1:psend(p,"KPF1\n");break;
			case XK_KP_F2:psend(p,"KPF2\n");break;
			case XK_KP_F3:psend(p,"KPF3\n");break;
			case XK_KP_F4:psend(p,"KPF4\n");break;
		}
		if (p->flags&FLG_SHOPPING) switch (k) {
			case XK_Escape:exit_shop(p);break;
			case XK_1:buy_shop(p,0);break;
			case XK_2:buy_shop(p,1);break;
			case XK_3:buy_shop(p,2);break;
			case XK_4:buy_shop(p,3);break;
			case XK_5:buy_shop(p,4);break;
			case XK_6:buy_shop(p,5);break;
			case XK_7:buy_shop(p,6);break;
			case XK_8:buy_shop(p,7);break;
			case XK_9:buy_shop(p,8);break;
			case XK_F1:psend(p,"KF1\n");break;
			case XK_F2:psend(p,"KF2\n");break;
			case XK_F3:psend(p,"KF3\n");break;
			case XK_F4:psend(p,"KF4\n");break;
			case XK_F5:psend(p,"KF5\n");break;
			case XK_F6:psend(p,"KF6\n");break;
			case XK_F7:psend(p,"KF7\n");break;
			case XK_F8:psend(p,"KF8\n");break;
			case XK_F9:psend(p,"KF9\n");break;
			case XK_F10:psend(p,"KF10\n");break;
			case XK_F11:psend(p,"KF11\n");break;
			case XK_F12:psend(p,"KF12\n");break;
			case XK_KP_F1:psend(p,"KPF1\n");break;
			case XK_KP_F2:psend(p,"KPF2\n");break;
			case XK_KP_F3:psend(p,"KPF3\n");break;
			case XK_KP_F4:psend(p,"KPF4\n");break;
		}
		if (p->flags&FLG_TERMINAL) switch (k) {
			case XK_Escape:term_option(p,10);break;
			case XK_0:term_option(p,'0');break;
			case XK_1:term_option(p,'1');break;
			case XK_2:term_option(p,'2');break;
			case XK_3:term_option(p,'3');break;
			case XK_4:term_option(p,'4');break;
			case XK_5:term_option(p,'5');break;
			case XK_6:term_option(p,'6');break;
			case XK_7:term_option(p,'7');break;
			case XK_8:term_option(p,'8');break;
			case XK_9:term_option(p,'9');break;
			case XK_a:term_option(p,'a');break;
			case XK_b:term_option(p,'b');break;
			case XK_c:term_option(p,'c');break;
			case XK_d:term_option(p,'d');break;
			case XK_e:term_option(p,'e');break;
			case XK_f:term_option(p,'f');break;
			case XK_g:term_option(p,'g');break;
			case XK_h:term_option(p,'h');break;
			case XK_i:term_option(p,'i');break;
			case XK_j:term_option(p,'j');break;
			case XK_k:term_option(p,'k');break;
			case XK_l:term_option(p,'l');break;
			case XK_m:term_option(p,'m');break;
			case XK_n:term_option(p,'n');break;
			case XK_o:term_option(p,'o');break;
			case XK_p:term_option(p,'p');break;
			case XK_q:term_option(p,'q');break;
			case XK_r:term_option(p,'r');break;
			case XK_s:term_option(p,'s');break;
			case XK_t:term_option(p,'t');break;
			case XK_u:term_option(p,'u');break;
			case XK_v:term_option(p,'v');break;
			case XK_w:term_option(p,'w');break;
			case XK_x:term_option(p,'x');break;
			case XK_y:term_option(p,'y');break;
			case XK_z:term_option(p,'z');break;
			case XK_space:term_option(p,' ');break;
			case XK_Return:term_option(p,1);break;
			case XK_Up:term_option(p,2);break;
			case XK_Down:term_option(p,3);break;
			case XK_F1:psend(p,"KF1\n");break;
			case XK_F2:psend(p,"KF2\n");break;
			case XK_F3:psend(p,"KF3\n");break;
			case XK_F4:psend(p,"KF4\n");break;
			case XK_F5:psend(p,"KF5\n");break;
			case XK_F6:psend(p,"KF6\n");break;
			case XK_F7:psend(p,"KF7\n");break;
			case XK_F8:psend(p,"KF8\n");break;
			case XK_F9:psend(p,"KF9\n");break;
			case XK_F10:psend(p,"KF10\n");break;
			case XK_F11:psend(p,"KF11\n");break;
			case XK_F12:psend(p,"KF12\n");break;
			case XK_KP_F1:psend(p,"KPF1\n");break;
			case XK_KP_F2:psend(p,"KPF2\n");break;
			case XK_KP_F3:psend(p,"KPF3\n");break;
			case XK_KP_F4:psend(p,"KPF4\n");break;
		} // in terminal
	} else switch(k) {
		case XK_z:p->flags|=FLG_ROTACLOCK;break;
		case XK_x:p->flags|=FLG_ROTCLOCK;break;
		case XK_minus:if ((ad=find_addon(p->firstadd,ADD_RADAR)) && ad->info[0]>0) ad->info[0]--;
			p->immune=0;break;
		case XK_equal:if ((ad=find_addon(p->firstadd,ADD_RADAR)) && ad->info[2]>ad->info[0]) ad->info[0]++;
			p->immune=0;break;
		case XK_KP_7:
		case XK_KP_Subtract:
		case XK_KP_Divide:
		case XK_KP_Multiply:
		case XK_r:if ((ad=find_addon(p->firstadd,ADD_RADAR)) && ad->level>1) ad->info[1]=-1;
			p->immune=0;break;
		case XK_KP_4:
		case XK_KP_Add:
		case XK_KP_Separator:
		case XK_g:if ((ad=find_addon(p->firstadd,ADD_RADAR)) && ad->level>1) ad->info[1]=0;
			p->immune=0;break;
		case XK_KP_1:
		case XK_KP_Enter:
		case XK_v:if ((ad=find_addon(p->firstadd,ADD_RADAR)) && ad->level>1) ad->info[1]=1;
			p->immune=0;break;
		case XK_Shift_L:
		case XK_Shift_R:p->flags|=FLG_THRUST;break;
		case XK_space:p->qflags|=4;p->flags|=FLG_BRAKING;break;
		case XK_s:p->flags^=FLG_STATUS;break;
		case XK_n:p->flags^=FLG_IDENT;break;
		case XK_m:p->flags^=FLG_NOMSG;break;
		case XK_w:p->flags^=FLG_NOWEP;break;
		case XK_f:p->immune=0;p->flags|=FLG_FUELLING;break;
		case XK_Delete:p->flags^=FLG_NOINSTR;break;
		case XK_b:p->immune=0;take_hold(p);break;
		case XK_i:if (find_addon(p->firstadd,ADD_INVIS)) p->flags^=FLG_INVIS;
			p->immune=0;break;
		case XK_o:if (find_addon(p->firstadd,ADD_CLOAKING)) p->flags^=FLG_CLOAKING;
			p->immune=0;break;
		case XK_y:if (find_addon(p->firstadd,ADD_ANTICLOAK)) p->flags^=FLG_ANTICLOAK;
			p->immune=0;break;
		case XK_p:if (find_addon(p->firstadd,ADD_MINESWEEP)) p->flags^=FLG_MINESWEEP;
			p->immune=0;break;
		case XK_c:p->immune=0;p->weap=(p->weap+1)%MAX_WEAPS;
			while (!((p->weap_mask&(1<<p->weap))&&(p->ammo[p->weap]))) p->weap=(p->weap+1)%MAX_WEAPS;
			break;
		case XK_t:p->immune=0;next_target(p);break;
		case XK_h:p->qflags|=2;p->immune=0;
			if (rd2(p->body.l,(int)p->body.x/128,(int)p->body.y/128)=='H')
				if (is_stopped(&p->body)) take_home(p);
			break;
		case XK_e:c=rd2(p->body.l,(int)p->body.x/128,(int)p->body.y/128);
			if (c=='X') {
				struct teleport *tp;
				tp=locate_teleport(p->body.l,(int)p->body.x/128,(int)p->body.y/128);
				if (!tp->clk) tp->clk=1;
				p->immune=0;break;
			}
			if (!is_stopped(&p->body)) break;
			int x=(int)p->body.x&127, y=(int)p->body.y&127;if (x<32||x>96||y<32||y>96) break;
			switch (c) {
			case 'S': init_shop(p);break;
			case 'T': init_term(p);break;
			case 'H': go_home(p);break;
			} break;
		case XK_l:p->immune=0;
			if (rd2(p->body.l,(int)p->body.x/128,(int)p->body.y/128)=='l')
			switch (summon_lift(p->body.l,(int)p->body.x/128,(int)p->body.y/128)) {
				case LIFT_NONE:player_message(p,"No lift here!");break;
				case LIFT_BUSY:player_message(p,"Lift Busy...");break;
				case LIFT_CALLED:player_message(p,"Lift Called...");break;
				default:player_message(p,"Lift knackered...");break;
			} break;
		case XK_Up:
		case XK_Down:p->immune=0;
			if (rd2(p->body.l,(int) p->body.x/128,(int) p->body.y/128)=='L') {
				int x=(int)p->body.x&127, y=(int)p->body.y&127;
				if (x<32||x>96||y<32||y>96) break;
				struct lift *l=find_lift((int)p->body.x/128,(int)p->body.y/128);
				if (l->t==l->l)
					if ((k==XK_Up && can_lift_ascend(l)) || (k==XK_Down && can_lift_descend(l))) {
						player_message(p,"Lift Activated...");
						l->clk=0;
						k==XK_Up ? l->t-- : l->t++;
					} else k==XK_Up ? player_message(p,"Lift Cannot Go Up.")
						: player_message(p,"Lift Cannot Go Down.");
				else player_message(p,"Lift Busy...");
			} break;
		case XK_KP_8:
		case XK_bracketleft:
			p->thrust-=p->step; if (p->thrust<0) p->thrust=0;
			break;
		case XK_KP_9:
		case XK_bracketright:
			p->thrust+=p->step; if (p->thrust>100) p->thrust=100;
			break;
		case XK_q: p->qflags|=1;break;
		case XK_KP_5:
		case XK_semicolon:
			p->spin-=p->step; if (p->spin<0) p->spin=0;
			break;
		case XK_KP_6:
		case XK_apostrophe:
			p->spin+=p->step; if (p->spin>100) p->spin=100;
			break;
		case XK_KP_2:
		case XK_period:p->immune=0;
			if ((p->slot)&&(p->mode[p->slot-1])) {
				p->mode[p->slot-1]-=p->step;
				if (p->mode[p->slot-1]<=0) p->mode[p->slot-1]=0;
				if (p->mode[p->slot-1]==0) p->mode[p->slot-1]=-1;
			}
			break;
		case XK_KP_3:
		case XK_slash:p->immune=0;
			if ((p->slot)&&(p->mode[p->slot-1])) {
				if (p->mode[p->slot-1]==-1) p->mode[p->slot-1]=0;
				p->mode[p->slot-1]+=p->step;
				if (p->mode[p->slot-1]>100) p->mode[p->slot-1]=100;
			}
			break;
		case XK_Escape:
			t=p->spin;p->spin=p->espin;p->espin=t;
			t=p->thrust;p->thrust=p->ethrust;p->ethrust=t;
			break;
		case XK_Return:p->immune=0;p->flags|=FLG_FIRING;break;
		case XK_0:p->slot=0;break;
		case XK_1:p->slot=1;break;
		case XK_2:p->slot=2;break;
		case XK_3:p->slot=3;break;
		case XK_4:p->slot=4;break;
		case XK_5:p->slot=5;break;
		case XK_6:p->slot=6;break;
		case XK_7:p->slot=7;break;
		case XK_8:p->slot=8;break;
		case XK_9:p->slot=9;break;
		case XK_Tab:p->immune=0;activate_slot(p);break;
		case XK_a:p->immune=0;arm_slot(p);break;
		case XK_u:p->immune=0;disarm_slot(p);break;
		case XK_d:p->immune=0;detonate_slot(p);break;
		case XK_F1:psend(p,"KF1\n");break;
		case XK_F2:psend(p,"KF2\n");break;
		case XK_F3:psend(p,"KF3\n");break;
		case XK_F4:psend(p,"KF4\n");break;
		case XK_F5:psend(p,"KF5\n");break;
		case XK_F6:psend(p,"KF6\n");break;
		case XK_F7:psend(p,"KF7\n");break;
		case XK_F8:psend(p,"KF8\n");break;
		case XK_F9:psend(p,"KF9\n");break;
		case XK_F10:psend(p,"KF10\n");break;
		case XK_F11:psend(p,"KF11\n");break;
		case XK_F12:psend(p,"KF12\n");break;
		case XK_KP_F1:psend(p,"KPF1\n");break;
		case XK_KP_F2:psend(p,"KPF2\n");break;
		case XK_KP_F3:psend(p,"KPF3\n");break;
		case XK_KP_F4:psend(p,"KPF4\n");break;
	}
}

static void key_unpress(struct player *p,int k) {
	if (!p->body.on) {
		if (p->flags&FLG_HOME) switch (k) {
			case XK_Control_L:
			case XK_Control_R:p->flags&=~FLG_CTRL;break;
			case XK_f:p->flags&=~FLG_FUELLING;break;
		}
	} else switch(k) {
		case XK_z:p->flags&=~FLG_ROTACLOCK;break;
		case XK_x:p->flags&=~FLG_ROTCLOCK;break;
		case XK_Shift_R:
		case XK_Shift_L:p->flags&=~FLG_THRUST;break;
		case XK_space:p->qflags&=~4;p->flags&=~FLG_BRAKING;break;
		case XK_Return:p->flags&=~FLG_FIRING;break;
		case XK_q:p->qflags&=~1;break;
		case XK_h:p->qflags&=~2;break;
		case XK_f:p->flags&=~FLG_FUELLING;break;
	}
}

void process_events(struct player *p) {
	XEvent xev;
	if (!p->connected) return;
	XSync(p->d.disp,False);
	if (p->flags&FLG_INWINDOW) {
		XAutoRepeatOff(p->d.disp);
		XSync(p->d.disp,False);
	}
	while (XPending(p->d.disp)) {
		XNextEvent(p->d.disp,&xev);
		switch(xev.xany.type) {
		case KeyPress: // 2
			key_press(p,XkbKeycodeToKeysym(p->d.disp,xev.xkey.keycode,0,0));
			break;
		case KeyRelease: // 3
			key_unpress(p,XkbKeycodeToKeysym(p->d.disp,xev.xkey.keycode,0,0));
			break;
		case EnterNotify: // 7
			printf("%s: EnterNotify\n", __func__);
			break;
		case LeaveNotify: // 8
			printf("%s: LeaveNotify\n", __func__);
			break;
		case FocusIn: // 9
			printf("%s: FocusIn\n", __func__);
			XAutoRepeatOff(p->d.disp);p->flags|=FLG_INWINDOW;break;
		case FocusOut: // 10
			printf("%s: FocusOut\n", __func__);
			p->flags&=~(FLG_THRUST|FLG_BRAKING|FLG_ROTCLOCK|FLG_ROTACLOCK|
						FLG_CTRL|FLG_FIRING);
			XAutoRepeatOn(p->d.disp);
			p->flags&=~FLG_INWINDOW;
			break;
		case NoExpose: // 14
			printf("%s: NoExpose\n", __func__);
			break;
		case DestroyNotify: // 17
			printf("%s: DestroyNotify\n", __func__);
			p->qflags=7;
			break;
		case ColormapNotify: // 32
			printf("%s: ColormapNotify\n", __func__); break;
		case ClientMessage: // 33
			printf("%s: ClientMessage\n", __func__); break;
		case MappingNotify: // 34
			printf("%s: MappingNotify\n", __func__); break;
		case GenericEvent: // 35
			printf("%s: GenericEvent\n", __func__); break;
		default:
/*	    fprintf(stderr,"Unknown event: Code %d\n",xev.xany.type);*/
			break;
		} // switch
	}
}
