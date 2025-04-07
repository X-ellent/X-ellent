/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/old.terminal.c,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.8 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "fix.h"
#include "player.h"
#include "map.h"
#include "home.h"
#include "draw.h"
#include "lift.h"
#include "shop.h"
#include "debug.h"
#include "object.h"
#include "constants.h"
#include "addon.h"
#include "message.h"
#include "terminal.h"

#define ROMBASE 1024

/* Define a type that can safely hold pointers on both 32-bit and 64-bit systems */
#include <stdint.h>
typedef uintptr_t ptr_int_t; /* Use this type for pointer-integer conversions */

#define OP_RTS 1
#define OP_JMP 2
#define OP_JSR 3
#define OP_CLS 4
#define OP_NEW 5
#define OP_THI 6
#define OP_TLO 7
#define OP_STR 8
#define OP_NUM 9
#define OP_POP 10
#define OP_INN 11
#define OP_INS 12
#define OP_POPA 13
#define OP_POPB 14
#define OP_PSHA 17
#define OP_PSHB 18
#define OP_INC 21
#define OP_INCA 22
#define OP_INCB 23
#define OP_DEC 24
#define OP_DECA 25
#define OP_DECB 26
#define OP_STO 27
#define OP_STOA 28
#define OP_STOB 29
#define OP_FET 30
#define OP_FETA 31
#define OP_FETB 32
#define OP_BEQ 33
#define OP_BMI 34
#define OP_CMP 35
#define OP_CMPA 36
#define OP_CMPB 37
#define OP_PSH 38
#define OP_SPC 39
#define OP_SYSTEM 40
#define OP_KEY 41
#define OP_TAB 42
#define OP_ROT 43
#define OP_SWAP 44
#define OP_NEG 45
#define OP_ADD 46
#define OP_SUB 47
#define OP_MULT 48
#define OP_DIV 49
#define OP_DUP 50
#define OP_MOD 51
#define OP_BNE 52
#define OP_BPL 53
#define OP_NOP 54
#define OP_BIT 55
#define OP_BITA 56
#define OP_BITB 57
#define OP_BAT 58
#define OP_POS 59

#define PFLG_EQ (1<<0)
#define PFLG_MI (1<<1)
#define PFLG_HI (1<<2)

#define PSTAT_ERR -1
#define PSTAT_INN 1
#define PSTAT_KEY 2
#define PSTAT_INS 3

 /* I have decided to treat the terminals as if they were all little */
 /* processorts which run their own assembly language, this WILL take */
 /* a bit of writing i guess since ive just decided to rewrite the */
 /* language they run but it should make terminals a lot more */
 /* flexible. All that i need is to use a simple stack system */
 /* involving say a value stack and a program counter stack and ive */
 /* then got a perfectly good system to be getting on with. All i have */
 /* to define then is a nice set of opcodes and I have what i came */
 /* here for (which is a nice elegant way of doing terminals (I can */
 /* waffle on quite effectively when i put my mind to it cant i?) */

 /* This login structure defines the 'architecture' of the terminal's */
 /* processor by setting up its internal registers (and its link with */
 /* the player controlling it) */

struct login {
	struct player *p;
	int x;              /* Cursor address on screen */
	int psp;            /* Position in pc stack */
	int nsp;            /* Position in num stack */
	int state;          /* Set if performing operation of some sort */
	int tmp;            /* Internal register for operations */
	int tmpb;           /* Internal register for operations */
	int pc[20];         /* My pc stack */
	ptr_int_t num[64];  /* My num stack - now uses ptr_int_t to safely store pointers */
	int a,b;            /* Number registers */
	int status;         /* Status register */
	char ram[512];      /* Terminal's ram */
};

struct label {
	char name[16];
	int adr;
	struct label *next;
};

// Global variables
struct label *firstlabel;
int startpc;
struct login tty[TERM_NUMBER];
char rom[TERM_ROMSIZE];

static void cts(struct player *p);
static void term_newline(struct player *p);
static int term_hprint(struct player *p,char *c,int l);
static int term_print(struct player *p,char *c,int l);
static void get_label(FILE *td,int pc);
static int get_string(FILE *td,int pc);
static int get_number(char c,FILE *td,int pc);
static int get_thing(char c,FILE *td,int pc);
static int find_label(char *s);
static void get_playerinfo(struct login *t,struct player *p,int a);
static ptr_int_t getnextplayer(struct player *p);
static void get_lift_info(struct login *t,int lift,char *rc);

static int comperr;
static char txt[256];
static struct player *tuser[256];

#define POP ((ptr_int_t)t->num[--t->nsp])
#define PUSH(x) (t->num[t->nsp++] = (ptr_int_t)(x))








static void get_playerinfo(struct login *t,struct player *p,int a) {
	char *rc;
	int *ri;
	a&=~3;
	if (!p) return;
	rc=&t->ram[a];
	ri=(int *) rc;
	strcpy(&rc[0],p->user);
	strcpy(&rc[16],p->name);
	ri[12]=p->rating;
	ri[13]=p->score;
	ri[14]=p->cash;
	ri[15]=p->body.l;
	ri[16]=p->body.x/128;
	ri[17]=p->body.y/128;
	ri[18]=p->fuel/200;
	ri[19]=p->shield/10;
	if (p->home) {
		ri[20]=p->home->l;
		ri[21]=p->home->x;
		ri[22]=p->home->y;
	} else {
		ri[20]=-1;
		ri[21]=0;
		ri[22]=0;
	}
	ri[23]=p->deaths;
	ri[24]=p->kills;
	ri[25]=p->tkills;
}

static void cts(struct player *p) {
	XFillRectangle(p->d.disp,p->d.backing,p->d.gc_black,0,0,WINWID,WINHGT);
	p->term->x=0;
}

void init_term(struct player *p) {
	int i;
	struct login *t;
	for (i=0;(i<TERM_NUMBER)&&(tty[i].p);i++);
	if (i==TERM_NUMBER) {
		player_message(p,"Out of tty's.... please try later...");
		p->term=0;
		return;
	} else {
		p->flags|=FLG_TERMINAL;
		remove_body(&p->body);
		p->term=&tty[i];
		p->term->p=p;
		t=p->term;
		cts(p);
		t->psp=0;
		t->nsp=0;
		t->pc[0]=startpc;
		t->state=0;
		t->status=0;
		psend(p,"#TERM ENTER\n");
		get_playerinfo(t,p,0);
		run_program(p);
	}
}

static int get_string(FILE *td,int pc) {
	char c;

	while((c=getc(td))!='"') {
		rom[pc++]=c;
	}
	rom[pc++]=0;  /* Add null terminator */
	return pc;
}

static int get_number(char c,FILE *td,int pc) {
	char d;
	ptr_int_t n;  /* Changed to ptr_int_t for 64-bit compatibility */
	n=c-'0';
	if ((c=='-')||(c=='+')) n=0;
	while (isdigit(d=getc(td))) n=n*10+d-'0';
	if (c=='-') n=-n;
	char *nc=(char *) &n;
	rom[pc++]=OP_PSH;
	rom[pc++]=nc[0];
	rom[pc++]=nc[1];
	rom[pc++]=nc[2];
	rom[pc++]=nc[3];
	return pc;
}

static int find_label(char *s) {
	struct label *l;
	for(l=firstlabel;l;l=l->next)
		if (strcmp(l->name,s)==0) return l->adr;
	return 0;
}

static int get_thing(char c,FILE *td,int pc) {
	char str[16];str[0]=c;
	int n;
	for (n=1;n<15;n++) if (isspace(str[n]=getc(td))) {
		str[n]=0; n=16;
	}
	if (n==15) while(!isspace(getc(td)));

/* DO NOT REMOVE OR ALTER THIS LINE **START**/
	if (strcmp(str,"RTS")==0) {rom[pc++]=OP_RTS;return pc;};
	if (strcmp(str,"JMP")==0) {rom[pc++]=OP_JMP;return pc;};
	if (strcmp(str,"JSR")==0) {rom[pc++]=OP_JSR;return pc;};
	if (strcmp(str,"CLS")==0) {rom[pc++]=OP_CLS;return pc;};
	if (strcmp(str,"NEW")==0) {rom[pc++]=OP_NEW;return pc;};
	if (strcmp(str,"THI")==0) {rom[pc++]=OP_THI;return pc;};
	if (strcmp(str,"TLO")==0) {rom[pc++]=OP_TLO;return pc;};
	if (strcmp(str,"STR")==0) {rom[pc++]=OP_STR;return pc;};
	if (strcmp(str,"NUM")==0) {rom[pc++]=OP_NUM;return pc;};
	if (strcmp(str,"POP")==0) {rom[pc++]=OP_POP;return pc;};
	if (strcmp(str,"INN")==0) {rom[pc++]=OP_INN;return pc;};
	if (strcmp(str,"INS")==0) {rom[pc++]=OP_INS;return pc;};
	if (strcmp(str,"POPA")==0) {rom[pc++]=OP_POPA;return pc;};
	if (strcmp(str,"POPB")==0) {rom[pc++]=OP_POPB;return pc;};
	if (strcmp(str,"PSHA")==0) {rom[pc++]=OP_PSHA;return pc;};
	if (strcmp(str,"PSHB")==0) {rom[pc++]=OP_PSHB;return pc;};
	if (strcmp(str,"INC")==0) {rom[pc++]=OP_INC;return pc;};
	if (strcmp(str,"INCA")==0) {rom[pc++]=OP_INCA;return pc;};
	if (strcmp(str,"INCB")==0) {rom[pc++]=OP_INCB;return pc;};
	if (strcmp(str,"DEC")==0) {rom[pc++]=OP_DEC;return pc;};
	if (strcmp(str,"DECA")==0) {rom[pc++]=OP_DECA;return pc;};
	if (strcmp(str,"DECB")==0) {rom[pc++]=OP_DECB;return pc;};
	if (strcmp(str,"STO")==0) {rom[pc++]=OP_STO;return pc;};
	if (strcmp(str,"STOA")==0) {rom[pc++]=OP_STOA;return pc;};
	if (strcmp(str,"STOB")==0) {rom[pc++]=OP_STOB;return pc;};
	if (strcmp(str,"FET")==0) {rom[pc++]=OP_FET;return pc;};
	if (strcmp(str,"FETA")==0) {rom[pc++]=OP_FETA;return pc;};
	if (strcmp(str,"FETB")==0) {rom[pc++]=OP_FETB;return pc;};
	if (strcmp(str,"BEQ")==0) {rom[pc++]=OP_BEQ;return pc;};
	if (strcmp(str,"BMI")==0) {rom[pc++]=OP_BMI;return pc;};
	if (strcmp(str,"CMP")==0) {rom[pc++]=OP_CMP;return pc;};
	if (strcmp(str,"CMPA")==0) {rom[pc++]=OP_CMPA;return pc;};
	if (strcmp(str,"CMPB")==0) {rom[pc++]=OP_CMPB;return pc;};
	if (strcmp(str,"PSH")==0) {rom[pc++]=OP_PSH;return pc;};
	if (strcmp(str,"SPC")==0) {rom[pc++]=OP_SPC;return pc;};
	if (strcmp(str,"SYSTEM")==0) {rom[pc++]=OP_SYSTEM;return pc;};
	if (strcmp(str,"KEY")==0) {rom[pc++]=OP_KEY;return pc;};
	if (strcmp(str,"TAB")==0) {rom[pc++]=OP_TAB;return pc;};
	if (strcmp(str,"ROT")==0) {rom[pc++]=OP_ROT;return pc;};
	if (strcmp(str,"SWAP")==0) {rom[pc++]=OP_SWAP;return pc;};
	if (strcmp(str,"NEG")==0) {rom[pc++]=OP_NEG;return pc;};
	if (strcmp(str,"ADD")==0) {rom[pc++]=OP_ADD;return pc;};
	if (strcmp(str,"SUB")==0) {rom[pc++]=OP_SUB;return pc;};
	if (strcmp(str,"MULT")==0) {rom[pc++]=OP_MULT;return pc;};
	if (strcmp(str,"DIV")==0) {rom[pc++]=OP_DIV;return pc;};
	if (strcmp(str,"DUP")==0) {rom[pc++]=OP_DUP;return pc;};
	if (strcmp(str,"MOD")==0) {rom[pc++]=OP_MOD;return pc;};
	if (strcmp(str,"BNE")==0) {rom[pc++]=OP_BNE;return pc;};
	if (strcmp(str,"BPL")==0) {rom[pc++]=OP_BPL;return pc;};
	if (strcmp(str,"NOP")==0) {rom[pc++]=OP_NOP;return pc;};
	if (strcmp(str,"BIT")==0) {rom[pc++]=OP_BIT;return pc;};
	if (strcmp(str,"BITA")==0) {rom[pc++]=OP_BITA;return pc;};
	if (strcmp(str,"BITB")==0) {rom[pc++]=OP_BITB;return pc;};
	if (strcmp(str,"BAT")==0) {rom[pc++]=OP_BAT;return pc;};
	if (strcmp(str,"POS")==0) {rom[pc++]=OP_POS;return pc;};
/* DO NOT REMOVE OR ALTER THIS LINE **END**/

	if (!(n=find_label(str))) {
		if (comperr!=2) {
			fprintf(stderr,"Label not found '%s'\n",str);
			comperr=-1;
		}
	}

	char *nc=(char *) &n;
	rom[pc++]=OP_PSH;
	rom[pc++]=nc[0];rom[pc++]=nc[1];rom[pc++]=nc[2];rom[pc++]=nc[3];
	return pc;
}

void init_all_term() {
	FILE *td;
	int c; // Was char (which is unsigned on some platforms so fails for EOF)
	int pc, pass;

	DL("Assembling terminal code");

	for (pass=0;pass<2;pass++) {
		if (pass==1) DL("Second pass");

		if (!(td=fopen("term.asm","rb"))) {
			fprintf(stderr,"Cannot open terminal code file for reading\n");
			exit(1);
		}

		pc=0;
		comperr=pass?0:2;
		c=getc(td);
		while (c!=EOF) {
			switch(c) {
			case '.':
				get_label(td,pc+ROMBASE);
				break;
			case '\"':
				pc=get_string(td,pc);
				break;
			case '+':
			case '-':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				pc=get_number(c,td,pc);
				break;
			default:
				pc=get_thing(c,td,pc);
				break;
			}
			while (isspace(c=getc(td)));
		}

		fclose(td);
	}

	if (comperr) exit(1);
}

static void get_label(FILE *td,int pc) {
	char str[16];
	struct label *l;
	int n;
	for (n=0;n<15;n++) {
		int c=getc(td); // int NOT char!
		if (isspace(c)) {
			str[n]=0;
			n=16;
		} else str[n]=c;
	}
	if (find_label(str)) return;
	l=(struct label *) calloc(1,sizeof(struct label));
	l->next=firstlabel;
	firstlabel=l;
	l->adr=pc;
	strcpy(l->name,str);
	if (n==15) while(!isspace(getc(td)));
	if (strcmp(l->name,"start")==0) startpc=pc;
}

static void term_newline(struct player *p) {
	p->term->x=0;
	XCopyArea(p->d.disp,p->d.backing,p->d.backing,p->d.gc,0,p->d.th,WINWID,
			  WINHGT-p->d.th,0,0);
}

static int term_hprint(struct player *p,char *c,int l) {
	int wid;
	wid=WINWID/p->d.tw-2;
	XDrawString(p->d.disp,p->d.backing,p->d.gc_termhi,10+p->d.tw*p->term->x,
				WINHGT-p->d.th*2,c,l);
	p->term->x+=l;
	if (p->term->x>=wid) term_newline(p);
	return l;
}

static int term_print(struct player *p,char *c,int l) {
	int wid;
	wid=WINWID/p->d.tw-2;
	XDrawString(p->d.disp,p->d.backing,p->d.gc_termlo,10+p->d.tw*p->term->x,
				WINHGT-p->d.th*2,c,l);
	p->term->x+=l;
	if (p->term->x>=wid) term_newline(p);
	return l;
}

void exit_term(struct player *p) {
	add_pbody(p);
	p->term->p=NULL; p->term=NULL;
	psend(p,"#TERM EXIT\n");
}

void run_program(struct player *p) {
	struct login *t;
	char *s,*ss;
	char txt[64];
	int n,m;
	int aa,bb,cc;
	struct player *ply;
	int done;
	t=p->term;
	for (done=0;done<50;done++) {
		switch(t->state) {
		case 0:
			switch(rom[(t->pc[t->psp]++)-ROMBASE]) {
			case OP_CLS:cts(p);break;
			case OP_NEW:term_newline(p);return;
			case OP_PSH:
			{
				ptr_int_t *pval;
				pval = (ptr_int_t *)&rom[t->pc[t->psp]-ROMBASE];

				#if defined(__LP64__) || defined(_LP64) || defined(__amd64) || defined(__x86_64__) || defined(__aarch64__)
				/* On 64-bit systems, we need to read 8 bytes */
				t->pc[t->psp]+=8;
				#else
				/* On 32-bit systems, we need to read 4 bytes */
				t->pc[t->psp]+=4;
				#endif

				PUSH(*pval);
			}
				break;
			case OP_STR:
				if (!t->nsp) {
					fprintf(stderr,"Stack underflow in terminal.\n");
					t->state=-1;
					break;
				}
				n=(int)t->num[--t->nsp];
				s = (n>=ROMBASE) ? &rom[n-ROMBASE] : &t->ram[n];
				if (t->status&PFLG_HI) {
					term_hprint(p,s,strlen(s));
				} else {
					term_print(p,s,strlen(s));
				}
				break;
			case OP_NUM:
				if (!t->nsp) {
					fprintf(stderr,"Stack underflow in terminal.\n");
					t->state=-1;
					break;
				}
				n=(int)t->num[--t->nsp];
				sprintf(txt,"%d",n);
				if (t->status&PFLG_HI) {
					term_hprint(p,txt,strlen(txt));
				} else {
					term_print(p,txt,strlen(txt));
				}
				break;
			case OP_INN:t->state=PSTAT_INN;return;
			case OP_INS:
				t->tmp=POP;
				t->tmpb=POP;
				t->state=PSTAT_INS;return;
			case OP_KEY:t->state=PSTAT_KEY;return;
			case OP_RTS:
				if (t->psp) {
					t->psp--;
					break;
				} else {
					add_body(&p->body);
					p->flags&=~(FLG_TERMINAL|FLG_BRAKING);
					t->p=0;
					p->term=0;
					psend(p,"#TERM EXIT\n");
					return;
				}
			case OP_FETA:
				n=POP;
				s = (n>=ROMBASE) ? &rom[n-ROMBASE] : &t->ram[n];
				ss=(char *)&t->a;
				ss[0]=s[0];ss[1]=s[1];ss[2]=s[2];ss[3]=s[3];
				break;
			case OP_FETB:
				n=POP;
				s = (n>=ROMBASE) ? &rom[n-ROMBASE] : &t->ram[n];
				ss=(char *)&t->b;
				ss[0]=s[0];ss[1]=s[1];ss[2]=s[2];ss[3]=s[3];
				break;
			case OP_FET:
				n=(int)t->num[t->nsp-1];
				s = (n>=ROMBASE) ? &rom[n-ROMBASE] : &t->ram[n];
				ss=(char *)&t->num[t->nsp-1];
				ss[0]=s[0];ss[1]=s[1];ss[2]=s[2];ss[3]=s[3];
				break;
			case OP_STO:
				n=POP;
				aa=POP;
				s = (n>=ROMBASE) ? &rom[n-ROMBASE] : &t->ram[n];
				ss=(char *)&aa;
				s[0]=ss[0];s[1]=ss[1];s[2]=ss[2];s[3]=ss[3];
				break;
			case OP_STOA:
				n=POP;
				s = (n>=ROMBASE) ? &rom[n-ROMBASE] : &t->ram[n];
				ss=(char *)&t->a;
				s[0]=ss[0];s[1]=ss[1];s[2]=ss[2];s[3]=ss[3];
				break;
			case OP_STOB:
				n=POP;
				s = (n>=ROMBASE) ? &rom[n-ROMBASE] : &t->ram[n];
				ss=(char *)&t->b;
				s[0]=ss[0];s[1]=ss[1];s[2]=ss[2];s[3]=ss[3];
				break;
			case OP_THI:t->status|=PFLG_HI;break;
			case OP_TLO:t->status&=~PFLG_HI;break;
			case OP_SPC:term_print(p," ",1);break;
			case OP_TAB:n=POP;if (t->x<n) t->x=n;break;
			case OP_BAT:t->x=POP;break;
			case OP_POS:PUSH(t->x);break;
			case OP_JSR:
				n=(int)t->num[--t->nsp];
				t->pc[++t->psp]=n;
				break;
			case OP_JMP:
				n=(int)t->num[--t->nsp];
				t->pc[t->psp]=n;
				break;
			case OP_INC:
				aa=POP;
				PUSH(aa+1);
				break;
			case OP_DEC:
				aa=POP;
				PUSH(aa-1);
				break;
			case OP_INCA:t->a++;break;
			case OP_INCB:t->b++;break;
			case OP_DECA:t->a--;break;
			case OP_DECB:t->b--;break;
			case OP_CMPA:
				n=(int)t->num[--t->nsp];
				if (n==t->a) {
					t->status|=PFLG_EQ;
				} else {
					t->status&=~PFLG_EQ;
				}
				if (n<t->a) {
					t->status|=PFLG_MI;
				} else {
					t->status&=~PFLG_MI;
				}
				break;
			case OP_CMPB:
				n=(int)t->num[--t->nsp];
				if (n==t->b) {
					t->status|=PFLG_EQ;
				} else {
					t->status&=~PFLG_EQ;
				}
				if (n<t->b) {
					t->status|=PFLG_MI;
				} else {
					t->status&=~PFLG_MI;
				}
				break;
			case OP_CMP:
				n=(int)t->num[--t->nsp];
				m=(int)t->num[t->nsp-1];
				if (n==m) {
					t->status|=PFLG_EQ;
				} else {
					t->status&=~PFLG_EQ;
				}
				if (n<m) {
					t->status|=PFLG_MI;
				} else {
					t->status&=~PFLG_MI;
				}
				break;
			case OP_BEQ:
				n=(int)t->num[--t->nsp];
				if (t->status&PFLG_EQ) t->pc[t->psp]=n;
				break;
			case OP_BNE:
				n=(int)t->num[--t->nsp];
				if (!(t->status&PFLG_EQ)) t->pc[t->psp]=n;
				break;
			case OP_BMI:
				n=(int)t->num[--t->nsp];
				if (t->status&PFLG_MI) t->pc[t->psp]=n;
				break;
			case OP_BPL:
				n=(int)t->num[--t->nsp];
				if (!(t->status&PFLG_MI)) t->pc[t->psp]=n;
				break;
			case OP_SYSTEM:
				n=(int)t->num[--t->nsp];
				switch(n) {
				case 0:draw_map_level(p,POP);break; /* Draw this level */
				case 1:
					aa=POP;bb=POP;
					struct player *player_ptr = (struct player *)(uintptr_t)aa;
					get_playerinfo(t, player_ptr, bb);
					break;
				case 2:PUSH((ptr_int_t) playone);break; /* Put playone onto stack */
				case 3:aa=POP;
					bb=getnextplayer((struct player *)(uintptr_t)aa);
					PUSH(bb);break; /* get next player */
				case 4:aa=POP;
					PUSH((ptr_int_t) find_player(&t->ram[aa]));break;
				case 5:aa=POP;
					p->cash-=aa;
					get_playerinfo(t,p,0);break;
				case 6:aa=POP;bb=POP;
					s=(aa>=ROMBASE)?&rom[aa-ROMBASE]:&t->ram[aa];
					ss=(bb>=ROMBASE)?&rom[bb-ROMBASE]:&t->ram[bb];
					PUSH(strcmp(s,ss));break;
				case 7:aa=POP;bb=POP;
					s=(aa>=ROMBASE)?&rom[aa-ROMBASE]:&t->ram[aa];
					ss=(bb>=ROMBASE)?&rom[bb-ROMBASE]:&t->ram[bb];
					strcpy(s,ss);break;
				case 8:aa=POP;
					s=(aa>=ROMBASE)?&rom[aa-ROMBASE]:&t->ram[aa];
					PUSH(atoi(s));break;
				case 9:aa=POP;bb=POP;
					s=(aa>=ROMBASE)?&rom[aa-ROMBASE]:&t->ram[aa];
					ss=(bb>=ROMBASE)?&rom[bb-ROMBASE]:&t->ram[bb];
					strcat(s,ss);break;
				case 10:aa=POP;bb=POP;
					ss=(bb>=ROMBASE)?&rom[bb-ROMBASE]:&t->ram[bb];
					player_message((struct player *)(uintptr_t)aa,ss);break;
				case 11:aa=POP;
					s=(aa>=ROMBASE)?&rom[aa-ROMBASE]:&t->ram[aa];
					global_message(s);break;
				case 12:ply=(struct player *)(uintptr_t)POP;aa=POP;
					ply->cash+=(aa*95)/100;
					fprintf(stderr,"Money transfer from %s to %s of %d\n",
							p->user,ply->user,aa);
					break;
				case 13:aa=POP;bb=POP;
					s=(aa>=ROMBASE)?&rom[aa-ROMBASE]:&t->ram[aa];
					get_lift_info(t,bb,s);break;
				default:
					fprintf(stderr,"Illegal system call %d\n",n);break;
				}
				break;
			case OP_NOP:return;
			case OP_BITA:aa=POP;
				if (t->a&(1<<aa)) {
					t->status&=~PFLG_EQ;
				} else {
					t->status|=PFLG_EQ;
				}
				break;
			case OP_BITB:aa=POP;
				if (t->b&(1<<aa)) {
					t->status&=~PFLG_EQ;
				} else {
					t->status|=PFLG_EQ;
				}
				break;
			case OP_BIT:aa=POP;bb=POP;
				if (bb&(1<<aa)) {
					t->status&=~PFLG_EQ;
				} else {
					t->status|=PFLG_EQ;
				}
				PUSH(bb);
				break;
			case OP_POPA:t->a=POP;break;
			case OP_POPB:t->b=POP;break;
			case OP_PSHA:PUSH(t->a);break;
			case OP_PSHB:PUSH(t->b);break;
			case OP_POP:n=POP;break;
			case OP_NEG:n=POP;PUSH(-n);break;
			case OP_SWAP:aa=POP;bb=POP;PUSH(aa);PUSH(bb);break;
			case OP_ROT:aa=POP;bb=POP;cc=POP;PUSH(bb);PUSH(aa);PUSH(cc);break;
			case OP_ADD:bb=POP;aa=POP;PUSH((aa+bb));break;
			case OP_SUB:bb=POP;aa=POP;PUSH((aa-bb));break;
			case OP_MULT:bb=POP;aa=POP;PUSH((aa*bb));break;
			case OP_DIV:bb=POP;aa=POP;PUSH((aa/bb));break;
			case OP_MOD:bb=POP;aa=POP;PUSH((aa%bb));break;
			case OP_DUP:aa=POP;PUSH(aa);PUSH(aa);break;
			default:
				fprintf(stderr,"Illegal opcode %d\n",
						rom[t->pc[t->psp]-ROMBASE-1]);
				t->state=-1; /* Error */
				break;
			}
			break;
		case PSTAT_INN:
			return;
		case PSTAT_KEY:
			return;
		default:
			return;
		}
	}
}

extern void term_option(struct player *p,int n) {
	char b;
	if (p->term->state==PSTAT_INN) {
		if (n==10) {
			p->term->state=0;
			p->term->num[p->term->nsp++]=-1;
			term_hprint(p,"ESC",3);
			return;
		}
		if ((n<'0')||(n>'9')) return;
		p->term->state=0;
		p->term->num[p->term->nsp++]=n-'0';
		b=n;
		term_hprint(p,&b,1);
		return;
	}
	if (p->term->state==PSTAT_KEY) {
		p->term->state=0;
		p->term->num[p->term->nsp++]=n-'0';
		return;
	}
	if (p->term->state==PSTAT_INS) {
		if (n==1) {
			p->term->ram[p->term->tmpb]=0;
			p->term->state=0;
			return;
		} else {
			if (n>=' ') {
				if (p->term->tmp) {
					p->term->ram[p->term->tmpb++]=n;
					b=n;
					if (p->term->status&PFLG_HI) {
						term_hprint(p,&b,1);
					} else {
						term_print(p,&b,1);
					}
					p->term->tmp--;
				}
			}
		}
	}
}

static ptr_int_t getnextplayer(struct player *p) {
	if (p) return (ptr_int_t) p->next;
	return 0;
}

static void get_lift_info(struct login *t,int lift,char *rc) {
	struct lift *l;
	int *ri;
	ri=(int *)rc;
	if (!(l=scan_lift(lift))) {
		t->status&=~PFLG_EQ;
		return;
	}
	t->status|=PFLG_EQ;
	ri[0]=l->id;
	ri[1]=l->l;
	ri[2]=l->x;
	ri[3]=l->y;
	ri[4]=l->t;
	strcpy(&rc[20],l->pass);
	ri[7]=0;
	return;
}

extern int terminal_input(int n,int state,char *in) {
	struct player *p;
	p=tuser[n];
	switch (state) {
	case -1: /* User disconnecting!!! */
		terminal_disconnect(n);
		return 0;
	case 1: /* New user, just logging in */
		tuser[n]=find_player(in);
		if (!tuser[n]) {
			tsend(n,"!No such user.\n");
			return 0;
		}
		if (tuser[n]->channel!=-1) {
			tsend(n,"!That user has a connection already\n");
			tuser[n]=0;
			return 0;
		}
		tuser[n]->channel=n;
		return 2;
	case 2: /* Asking for authorisation */
		if (!(*p->pass)) {
			tsend(n,"!No Password Set!\n");
			terminal_disconnect(n);
			return 0;
		}
		if (strcmp(p->pass,in)) {
			tsend(n,"!Invalid password\n");
			terminal_disconnect(n);
			return 0;
		}
		player_message(p,"Your influence has arrived.");
		return 3;
	case 3: /* Standard I/O */
		return terminal_operand(p,in);
	default:
		fprintf(stderr,"Input: %d %d <%s>\n",n,state,in);
	}
	return state;
}

extern void terminal_disconnect(int n) {
	if (tuser[n]) {
		tuser[n]->channel=-1;
		tuser[n]=0;
	}
}

extern void tsend(int n,char *s) {
	ssize_t ret;
	ret = write(n,s,strlen(s));
	(void)ret; /* Ignore return value */
}

extern void psend(struct player *p,char *s) {
	ssize_t ret;
	if (p->channel==-1) return;
	ret = write(p->channel,s,strlen(s));
	(void)ret; /* Ignore return value */
}

extern int terminal_operand(struct player *p,char *s) {
	char sys[4];
	int ret;

	sys[0]=s[1];
	sys[1]=s[2];
	sys[2]=s[3];
	sys[3]=0;
	switch(s[0]) {
	case '>':
		sprintf(txt,"%s - %s",p->user,&s[1]);
		player_message(p,txt);
		break;
	case 'X':
		player_message(p,"Your influence has exited.");
		terminal_disconnect(p->channel);
		return 0;
	case '*':
		ret=terminal_command(p,sys,&s[4]);
		psend(p,"...\n");
		return ret;
	}
	return 3;
}

int terminal_command(struct player *p,char *sys,char *com) {
	struct addon *ad;
	if (strcmp(sys,"SYS")==0) return system_command(p,com);
	if (strcmp(sys,"TER")==0) return login_command(p,com);
	if (strcmp(sys,"TEL")==0) return teleport_command(p,com);
	if (strcmp(sys,"LIF")==0) return lift_command(p,com);
	if (strcmp(sys,"WEP")==0) return weapons_command(p,com);
	for (ad=p->firstadd;ad;ad=ad->next)
		if (strcmp(ad->is->subs,sys)==0)
			return addon_command(p,ad,(uchar *)com);
	sprintf(txt,"!Unknown subsystem %s\n",sys);
	psend(p,txt);
	return 3;
}

extern int system_command(struct player *p,char *com) {
/*    fprintf(stderr,"System command : %s %s\n",p->user,com);*/
	if (strcmp(com,"HELP")==0) {            /* Lists accepted commands */
		psend(p,"=SYS HELP\n");
		psend(p,"=HELP\n");
		psend(p,"=SUBSYSTEMS\n");
		psend(p,"=SYSTEMS\n");
		psend(p,"=STATUS\n");
		psend(p,"=NAME<name>\n");
		psend(p,"=SPIN<value>\n");
		psend(p,"=THRUST<value>\n");
		psend(p,"=LOCATE <ON/OFF>\n");
		psend(p,"=HOME\n=USERS\n");
		return 3;
	}
	if (strcmp(com,"SUBSYSTEMS")==0) {      /* Lists subsystems */
		struct addon *ad;
		psend(p,"=SYS SUBSYSTEMS\n");
		for (ad=p->firstadd;ad;ad=ad->next) {
			sprintf(txt,"=%s %s\n",ad->is->subs,ad->is->name);
			psend(p,txt);
		}
		return 3;
	}
	if (strcmp(com,"SYSTEMS")==0) {      /* Lists systems */
		psend(p,"=SYS SYSTEMS\n");
		psend(p,"=TER Terminals\n=TEL Teleports\n=LIF Lifts\n=WEP Weapons\n");
		return 3;
	}
	if (strncmp(com,"NAME",4)==0) {      /* Sets Name */
		struct player *op;
		for (op=playone;op;op=op->next)
			if (op!=p) {
				if (strcmp(&com[4],op->name)==0) return 3;
				if (strcmp(&com[4],op->user)==0) return 3;
			}
		strncpy(p->name,&com[4],31);
		return 3;
	}
	if (strncmp(com,"THRUST",5)==0) {      /* Sets Thrust */
		int n;
		n=atoi(&com[5]);
		if ((n<0)||(n>100)) {
			psend(p,"!Thrust out of range!\n");
			return 3;};
		p->thrust=n;
		return 3;
	}
	if (strncmp(com,"SPIN",4)==0) {      /* Sets Spin */
		int n;
		n=atoi(&com[4]);
		if ((n<0)||(n>100)) {
			psend(p,"!Spin out of range!\n");
			return 3;};
		p->spin=n;
		return 3;
	}
	if (strcmp(com,"STATUS")==0) {          /* Shows my status */
		psend(p,(sprintf(txt,"=SYS STATUS\n=F%d S%d $%d R%d Sc%d %s %s %s\n",
						 p->fuel/200,
						 p->shield/10,p->cash,p->rating,p->score,
						 (p->flags&FLG_IDENT)?"ID":"",
						 (p->flags&FLG_NOMSG)?"":"MSG",
						 (p->locate)?"LOC":""),txt));
		return 3;
	}
	if (strcmp(com,"LOCATE ON")==0) {          /* Shows my coordinates */
		psend(p,"=SYS LOCATE ON\n");
		p->locate=-1;
		return 3;
	}
	if (strcmp(com,"LOCATE OFF")==0) {          /* Stops my coordinates */
		psend(p,"=SYS LOCATE OFF\n");
		p->locate=0;
		return 3;
	}
	if (strcmp(com,"HOME")==0) {            /* Shows my home coordinates */
		if (p->home) {
			psend(p,(sprintf(txt,"=SYS HOME\n=%d:%d,%d\n",p->home->l,
							(int) p->home->x*128+64,(int) p->home->y*128+64),
					 txt));
		} else {
			psend(p,"=No home\n");
		}
		return 3;
	}
	if (strcmp(com,"USERS")==0) {
		struct player *pl;
		psend(p,"=SYS USERS\n");
		for(pl=playone;pl;pl=pl->next)
			if (pl->body.on)
				psend(p,(sprintf(txt,"=%s %d %s\n",pl->user,pl->rating,
								 pl->name),txt));
		return 3;
	}
	psend(p,"!Unknown command.\n");
	return 3;
}

extern int login_command(struct player *p,char *com) {
	int x,y;
	if (!(p->flags&FLG_TERMINAL)) {
		psend(p,"!Not in a terminal!\n");
		return 3;
	}
	if (strcmp(com,"HELP")==0) {
		psend(p,"=TER HELP\n=HELP\n=MAP\n=TELEPORTS\n=LIFTS\n");
		return 3;
	}
	if (strcmp(com,"MAP")==0) {
		psend(p,(sprintf(txt,"=TER MAP\n=%d %d:%d,%d\n",map.depth,p->body.l,
						 map.wid,map.hgt),txt));
		for (y=0;y<map.hgt;y++) {
			txt[0]='=';
			for (x=0;x<map.wid;x++) {
				txt[x+1]=rd2(p->body.l,x,y);
				if (txt[x+1]==0) txt[x+1]=' ';
			}
			txt[map.wid+1]='\n';
			txt[map.wid+2]=0;
			psend(p,txt);
		}
		return 3;
	}
	if (strcmp(com,"TELEPORTS")==0) {
		struct teleport *tp;
		psend(p,"=TER TELEPORTS\n");
		for (tp=firsttel;tp;tp=tp->next)
			psend(p,(sprintf(txt,"=%d %d %d:%d,%d\n",tp->num,tp->dest->num,
							 tp->l,tp->x*128+64,tp->y*128+64),txt));
		return 3;
	}
	if (strcmp(com,"LIFTS")==0) {
		struct lift *li;
		psend(p,"=TER LIFTS\n");
		for (li=firstlift;li;li=li->next)
			psend(p,(sprintf(txt,"=%d %d:%d,%d %d\n",li->id,li->l,
							li->x*128+64,li->y*128+64,li->t),txt));
		return 3;
	}
	psend(p,"!Invalid command for terminal.\n");
	return 3;
}

extern int teleport_command(struct player *p,char *com) {
	struct teleport *tp;
	struct teleport *tpb;
	int n;
	char *ch;
	int a,correct;
	if (strcmp(com,"HELP")==0) {
		psend(p,"=TEL HELP\n=HELP\n=SET<num> <pass> <dest>\n=ACTIVATE<num> <pass>\n=PASS<num> <pass> <newpass>\n");
		return 3;
	}
	if (strncmp(com,"SET",3)==0) {
		n=atoi(&com[3]);
		tp=find_teleport(n);
		if (!tp) {
			psend(p,"!Invalid teleport number\n");
			return 3;
		}
		ch=strchr(&com[3],' ');
		if (!ch) {
			psend(p,"!Invalid format\n");
			return 3;
		}
		ch++;
		correct=0;
		for (a=0;a<4;a++)
			if (ch[a]==tp->pass[a]) correct++;
		if (correct==0) {
			psend(p,"=TEL SET\n=PASS INCORRECT\n");
			return 3;
		}
		if (correct!=4) {
			psend(p,"=TEL SET\n=PASS WRONG\n");
			return 3;
		}
		ch=strchr(ch,' ');
		if (!ch) {
			psend(p,"!Invalid format\n");
			return 3;
		}
		if (tp->clk<0) {
			psend(p,"=TEL SET\n=Teleport Busy\n");
			return 3;
		}
		n=atoi(ch+1);
		tpb=find_teleport(n);
		if (!tpb) {
			psend(p,"!Invalid destination number\n");
			return 3;
		}
		tp->dest=tpb;
		psend(p,"=TEL SET\n=Destination Set\n");
		if (tp->clk>0) tp->clk=1;
		return 3;
	}
	if (strncmp(com,"ACTIVATE",8)==0) {
		n=atoi(&com[8]);
		tp=find_teleport(n);
		if (!tp) {
			psend(p,"!Invalid teleport number\n");
			return 3;
		}
		ch=strchr(&com[8],' ');
		if (!ch) {
			psend(p,"!Invalid format\n");
			return 3;
		}
		ch++;
		correct=0;
		for (a=0;a<4;a++)
			if (ch[a]==tp->pass[a]) correct++;
		if (correct==0) {
			psend(p,"=TEL ACTIVATE\n=PASS INCORRECT\n");
			return 3;
		}
		if (correct!=4) {
			psend(p,"=TEL ACTIVATE\n=PASS WRONG\n");
			return 3;
		}
		if (tp->clk<0) {
			psend(p,"=TEL ACTIVATE\n=Teleport Busy\n");
			return 3;
		}
		if (tp->clk>0) {
			psend(p,"=TEL ACTIVATE\n=Teleport Charging\n");
			return 3;
		}
		psend(p,"=TEL ACTIVATE\n=Teleport Activated\n");
		tp->clk=1;
		return 3;
	}
	if (strncmp(com,"PASS",4)==0) {
		n=atoi(&com[4]);
		tp=find_teleport(n);
		if (!tp) {
			psend(p,"!Invalid teleport number\n");
			return 3;
		}
		ch=strchr(&com[4],' ');
		if (!ch) {
			psend(p,"!Invalid format\n");
			return 3;
		}
		ch++;
		correct=0;
		for (a=0;a<4;a++)
			if (ch[a]==tp->pass[a]) correct++;
		if (correct==0) {
			psend(p,"=TEL PASS\n=PASS INCORRECT\n");
			return 3;
		}
		if (correct!=4) {
			psend(p,"=TEL PASS\n=PASS WRONG\n");
			return 3;
		}
		ch=strchr(ch,' ');
		if (!ch) {
			psend(p,"!Invalid format\n");
			return 3;
		}
		ch++;
		for (a=0;a<4;a++)
			if (!isdigit(ch[a])) {
				psend(p,"!Invalid password format\n");
				return 3;
			}
		for (a=0;a<4;a++)
			tp->pass[a]=ch[a];
		psend(p,"=TEL PASS\n=Password Set\n");
		return 3;
	}
	psend(p,"!Invalid command for teleport.\n");
	return 3;
}


extern int weapons_command(struct player *p,char *com) {
	int i,n;
	if (strcmp(com,"HELP")==0) {            /* Lists accepted commands */
		psend(p,"=WEP HELP\n=HELP\n=STATUS\n=SELECT\n=FIRE <ON/OFF>\n");
		return 3;
	}
	if (strcmp(com,"STATUS")==0) {          /* Shows weapon's status */
		psend(p,(sprintf(txt,"=WEP STATUS\n=S%d R%d %s\n",p->weap,p->recharge,
						 (p->flags&FLG_FIRING)?"FIR":""),txt));
		for (i=0;i<MAX_WEAPS;i++) {
			if (p->weap_mask&(1<<i)) {
					if (p->ammo[i]!=-1) {
						psend(p,(sprintf(txt,"=%d %d %s\n",i,p->ammo[i],
										 weap_name[i]),txt));
					} else {
						psend(p,(sprintf(txt,"=%d Inf %s\n",i,weap_name[i]),txt));
					}
			}
		}
		return 3;
	}
	if (strncmp(com,"SELECT",6)==0) {
		n=atoi(&com[6]);
		if ((n<0)||(n>=MAX_WEAPS)) {
			psend(p,"!Invalid weapon ID.\n");
			return 3;
		}
		if (p->weap_mask&(1<<n)) {
			p->weap=n;
			return 3;
		}
		psend(p,"!Invalid weapon ID.\n");
		return 3;
	}
	if (strcmp(com,"FIRE ON")==0) {
		if (!p->body.on) {
			psend(p,"!Not active.\n");
			return 3;
		}
		p->flags|=FLG_FIRING;
		return 3;
	}
	if (strcmp(com,"FIRE OFF")==0) {
		if (!p->body.on) {
			psend(p,"!Not active.\n");
			return 3;
		}
		p->flags&=~FLG_FIRING;
		return 3;
	}
	psend(p,"!Unknown command to weapons system.\n");
	return 3;
}
