/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/terminal.h,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.8 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#ifndef My_TERMINAL_H
#define My_TERMINAL_H

#include "constants.h"

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

#define ROMBASE 1024

struct login {
    struct player *p;
    int x;              /* Cursor address on screen */
    int psp;            /* Position in pc stack */
    int nsp;            /* Position in num stack */
    int state;          /* Set if performing operation of some sort */
    int tmp;            /* Internal register for operations */
    int tmpb;           /* Internal register for operations */
    int pc[20];         /* My pc stack */
    int num[64];        /* My num stack */
    int a,b;            /* Number registers */
    int status;         /* Status register */
    char ram[512];      /* Terminal's ram */
};

struct label {
    char name[16];
    int adr;
    struct label *next;
};


struct label *firstlabel;
int startpc;

struct login tty[TERM_NUMBER];
char rom[TERM_ROMSIZE];

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

extern void init_term(struct player *p);
extern void init_all_term();
extern void run_program(struct player *p);
extern void term_option(struct player *p,int n);
extern int terminal_input(int chan,int state,char *s);
extern void terminal_disconnect(int n);
extern void tsend(int n,char *s);
extern void psend(struct player *p,char *s);
extern int terminal_operand(struct player *p,char *s);
extern int terminal_command(struct player *p,char *sys,char *com);
extern int system_command(struct player *p,char *com);
extern int login_command(struct player *p,char *com);

#endif
