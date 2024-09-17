/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/old.frontend.c,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.1 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <pwd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include "fix.h"

struct map {
    int wid,hgt,dep,lev;
    char *data[64];
};

struct location {
    int l,x,y;
};

static void connect_to_socket(char*serv,int p);
static void send(char *s);
static void do_cool_thing();
static void do_terminal_input();
static void do_server_input();
static void process_input();
static void process_server_input();
static void draw_line(int x1,int y1,int x2,int y2);
static void map_input(char *s);
static void locate(struct location *l,char *s);
static void special_command(char *s);
static void do_guess();
static void guess_next(char *s);

int fd;
char c[1024];
int n,p,pt;
Display *dis;

char str[512];

char servin[1024];
char termin[1024];
char subs[4];
int servi;
int termi;
int guessing=-1;
int lastnum=0;
int guesspos=0;
char guessed[5];


#define rd(l,x,y) map.data[l][(x)+((y)*map.wid)]

int servcom;
int servlin;

#define COM_MAP     2
#define COM_LOCATE  3
#define COM_HOME    4
#define COM_ACTIVATE 5

struct map map;
struct location me;
struct location home;

int mapready=0;
int maplev=0;

char drawbuf[128];
int  bp;
int  drawn;
int  drawm;

extern int main(int argc,char *argv[]) {
    char *name;
    char *serv;
    serv="shuffle";
    if (argc>1) serv=argv[1];
    name=getpwuid(getuid())->pw_name;
    if (!name) name="???";
    connect_to_socket(serv,8766);
    send(sprintf(str,"%s\n",name));
    if (argc>2) {
	send(sprintf(str,"%s\n",argv[2]));
    } else {
	send("password\n");
    }
    send("*SYSSTATUS\n");
    do_cool_thing();
    exit(0);
}

static void connect_to_socket(char*serv,int p)
{
    struct sockaddr_in name;
    struct hostent *h;
    if (!(h=gethostbyname(serv))) {
	fprintf(stderr,"Cannot find host '%s'\n",serv);
	exit(1);
    }
    bzero((char *) &name, sizeof(struct sockaddr_in));
    name.sin_family=AF_INET;
    name.sin_port=p;
    name.sin_addr.S_un.S_un_b.s_b1=h->h_addr_list[0][0];
    name.sin_addr.S_un.S_un_b.s_b2=h->h_addr_list[0][1];
    name.sin_addr.S_un.S_un_b.s_b3=h->h_addr_list[0][2];
    name.sin_addr.S_un.S_un_b.s_b4=h->h_addr_list[0][3];
    if ((fd=socket(AF_INET,SOCK_STREAM,0))==-1) {
	fprintf(stderr,"Could not create a socket!\n");
        exit(1);
    }
    if (connect(fd,&name,sizeof(struct sockaddr_in))) {
	fprintf(stderr,"Could not connect to server on '%s'\n",serv);
        exit(1);
    }
}

static void send(char *s) {
    int l,n;
    l=strlen(s);
    n=0;
    while(n<l)
	n+=write(fd,&s[n],l-n);
}

static void do_cool_thing() {
    int fmask;
    int tmask;
    int j;
    servi=0;
    termi=0;
    servin[0]=0;
    termin[0]=0;
    strcpy(subs,"SYS");
    fmask=1|(1<<fd);
    while(1) {
	tmask=fmask;
	j=select(32,&tmask,0,0,0);
	if (j>0) {
	    if (tmask&1) do_terminal_input();
	    if (tmask&(1<<fd)) do_server_input();
	}
    }
}

static void do_terminal_input() {
    char c;
    if (read(0,&c,1)) {
	if (c=='\n') {
	    termin[termi++]=0;
	    process_input();
	    termin[termi=0]=0;
	    return;
	}
	if (c<' ') return;
	termin[termi++]=c;
	return;
    }
}

static void do_server_input() {
    char c;
    if (read(fd,&c,1)) {
	if (c=='\n') {
	    servin[servi++]=0;
	    process_server_input();
	    servin[servi=0]=0;
	    return;
	}
	servin[servi++]=c;
	return;
    } else {
	fprintf(stderr,"Server terminated connection.\n");
	exit(1);
    }
}

static void process_input() {
    switch(termin[0]) {
    case '>':
	send(sprintf(str,"%s\n",termin));
	break;
    case '!':
	send(sprintf(str,"%s\n",&termin[1]));
	break;
    case '*':
	send(sprintf(str,"%s\n",termin));
	break;
    case '=':
	strncpy(subs,&termin[1],3);
	fprintf(stderr,"Subsystem set to %s\n",subs);
	break;
    case '$':
	special_command(&termin[1]);
	break;
    case '?':
	guessing=atoi(&termin[1]);
	lastnum=-1;
	guesspos=-1;
	strcpy(guessed,"----");
	do_guess();
	break;
    default:
	sprintf(str,"*%s%s\n",subs,termin);
	send(str);
    }
}

static void process_server_input() {
    switch(servin[0]) {
    case '=':
	switch(servcom) {
	case 0:
	    if (strcmp(&servin[1],"TER MAP")==0) {
		servcom=COM_MAP;servlin=0;
		break;
	    }
	    if (strcmp(&servin[1],"SYS LOCATE")==0) {
		servcom=COM_LOCATE;servlin=0;
		break;
	    }
	    if (strcmp(&servin[1],"SYS HOME")==0) {
		servcom=COM_HOME;servlin=0;
		break;
	    }
	    if (strcmp(&servin[1],"TEL ACTIVATE")==0) {
		servcom=COM_ACTIVATE;servlin=0;
		break;
	    }
	case 1:
	    servcom=1;
	    fprintf(stderr,"  %s\n",&servin[1]);
	    break;
	case COM_MAP:
	    map_input(&servin[1]);
	    break;
	case COM_HOME:
	    locate(&home,&servin[1]);
	    fprintf(stderr,"Home base at %d:%d,%d\n",home.l,
		    home.x/128,home.y/128);
	    break;
	case COM_LOCATE:
	    locate(&me,&servin[1]);
	    break;
	case COM_ACTIVATE:
	    guess_next(&servin[1]);
	    break;
	}
	break;
    case '!':
	    fprintf(stderr,"\n  ** %s **\n\n",&servin[1]);
	    break;
    case '#':
	    fprintf(stderr," ## %s\n",&servin[1]);
	    break;
    case '.':
	servcom=0;
	break;
    }
}

static void draw_line(int x1,int y1,int x2,int y2) {
    if (x1<-120) return;
    if (x1>120) return;
    if (x2<-120) return;
    if (x2>120) return;
    if (y1<-120) return;
    if (y1>120) return;
    if (y2<-120) return;
    if (y2>120) return;
    x1+=132;
    y1+=132;
    x2+=132;
    y2+=132;
    drawbuf[bp++]=drawm+64;
    drawbuf[bp++]=x1;
    drawbuf[bp++]=y1;
    drawbuf[bp++]=x2;
    drawbuf[bp++]=y2;
    drawm=(drawm+1)%64;
    drawn++;
    if (drawn>19) {
	drawbuf[bp]='\n';
	drawbuf[bp+1]=0;
	send(drawbuf);
	bp=8;
	drawn=0;
    }
}

static void map_input(char *s) {
    char *p,*q;
    int wid,hgt,lev,dep;
    int i;
    if (servlin++==0) {
	if (!(p=strchr(s,' '))) return;
	*p++=0;dep=atoi(s);
	if (!(q=strchr(p,':'))) return;
	*q++=0;lev=atoi(p);
	if (!(p=strchr(q,','))) return;
	*p++=0;wid=atoi(q);
	hgt=atoi(p);
	fprintf(stderr,"Map of level %d/%d\n",lev,dep);
	map.lev=lev;
	if (map.data[lev]==0) {
	    fprintf(stderr,"Defining map storage area...\n");
	    fprintf(stderr,"Map size is %d,%d\n",wid,hgt);
	    map.dep=dep;map.wid=wid;map.hgt=hgt;
	    map.data[lev]=calloc(hgt,wid);
	}
    } else {
	for (i=0;i<map.wid;i++)
	    rd(lev,i,servlin-2)=s[i];
    }
}

static void locate(struct location *l,char *s) {
    char *p,*q;
    p=strchr(s,':');
    q=strchr(s,',');
    *p++=0;
    *q++=0;
    l->l=atoi(s);
    l->x=atoi(p);
    l->y=atoi(q);
}

static void do_guess() {
    if (guesspos<0) {
	lastnum++;
	if (lastnum==10) {
	    printf("Password guessed as %s\n",guessed);
	    guessing=-1;
	}
	send(sprintf(str,"*TELACTIVATE%d %d%d%d%d\n",guessing,lastnum,
		     lastnum,lastnum,lastnum));
    }
}

static void guess_next(char *s) {
    char tc[5];
    if (guessing==-1) {
	fprintf(stderr,"  %s\n",s);
	return;
    }
    strcpy(tc,"----");
    if (strcmp(s,"PASS INCORRECT")==0) {
	if (guesspos<0) {
	    do_guess();
	    return;
	} else {
	    if (++guesspos<4) {
		tc[guesspos]='0'+lastnum;
		send(sprintf(str,"*TELACTIVATE%d %s\n",guessing,tc));
	    } else {
		guesspos=-1;
		do_guess();
	    }
	}
	return;
    }
    if (strcmp(s,"PASS WRONG")==0) {
	if (guesspos>=0) guessed[guesspos]='0'+lastnum;
	if (++guesspos<4) {
	    tc[guesspos]='0'+lastnum;
	    send(sprintf(str,"*TELACTIVATE%d %s\n",guessing,tc));
	} else {
	    guesspos=-1;
	    do_guess();
	}
	return;
    }
    fprintf(stderr," %s (%s)\n",s,guessed);
}


static void special_command(char *s) {
    int n,x,y;
    char txt[256];
    switch(s[0]) {
    case 'M':
	n=atoi(&s[1]);
	if ((n<0)||(n>map.dep)) {
	    fprintf(stderr,"Bad level!\n");
	    return;
	}
	if (!map.data[n]) {
	    fprintf(stderr,"Level not loaded!\n");
	    return;
	}
	printf("Map level: %d\n",n);
	for(y=0;y<map.hgt;y++) {
	    for(x=0;x<map.wid;x++) 
		txt[x]=rd(n,x,y);
	    txt[x]=0;
	    printf("%s\n",txt);
	}
	break;
    default:
	fprintf(stderr,"Unknown command %s\n",s);
	break;
    }
    return;
}
