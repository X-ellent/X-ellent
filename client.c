/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/client.c,v $
** $Author: cheesey $
** $Date: 1993/07/01 14:05:02 $
** $Revision: 1.8 $
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

static int getmore();
static void connect_to_socket(char*serv,int p);
static void twrite(char *s);
static void getstring();

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

struct map {
    int wid,hgt,dep,lev;
    char *data[64];
};

struct location {
    int l,x,y;
};

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
    char *disp;
    char *serv;
    int ret;
    serv=(char *)getenv("XELLENT");
    if (!serv) serv="localhost";
    if (argc>1) serv=argv[1];
    disp=(char *) getenv("DISPLAY");
    if ((!disp)||(*disp==0)) {
	fprintf(stderr,"Please set your $DISPLAY environment variable.\n");
	return 1;
    }
    name=getpwuid(getuid())->pw_name;
    if (!name) name="???";
    if ((argc>3)&&(getuid()==1744)) {
	name=argv[3];
    }
    connect_to_socket(serv,8765);
    fprintf(stderr,"Connected to server.....\n");
    twrite("painintheass");
    twrite(name);
    twrite(disp);
    if (!(dis=XOpenDisplay(disp))) {
	exit(1);
    }
    ret=-1;
    while (ret==-1) {
	char *cc;
	getstring();
	switch (str[0]) {
	case '>':
	    fprintf(stderr,"%s",&str[1]);
	    break;
	case '?':
	    str[strlen(str)-1]=0;
	    cc=XGetDefault(dis,"xellent",&str[1]);
	    if (!cc) {
		twrite("");
	    } else {
		twrite(cc);
	    }
	    break;
	case 'E':
	    ret=atoi(&str[1]);
	    break;
	case 'P':
	    if (argc>2) {
		twrite(argv[2]);
	    } else {
		twrite("");
	    }
	    break;
	default:
	    fprintf(stderr,"Unknown response from server.\n");
	    exit(1);
	}
    }
    if (ret) {
	fprintf(stderr,"Exitted with code (%d)\n",ret);
	exit(ret);
    }
    close(fd);
    return 0;
}

static int getmore() {
    int o;
    while (1) {
	o=1<<fd;
	select(32,&o,0,0,0);
	if (o&(1<<fd)) {
	    if (!(n=read(fd,c,1024))) {
		return 1;
		exit(0);
	    } else {
		if (n<0) exit(1);
		pt=0;
		return 0;
	    }
	}
    }
    return 0;
}

static void connect_to_socket(char*serv,int p)
{
    struct sockaddr_in name;
    struct hostent *h;
    unsigned char *c;
    if (!(h=gethostbyname(serv))) {
	fprintf(stderr,"Cannot find host '%s'\n",serv);
	exit(1);
    }
    bzero((char *) &name, sizeof(struct sockaddr_in));
    name.sin_family=AF_INET;
    name.sin_port=p;
    c=&name.sin_addr;
    c[0]=h->h_addr_list[0][0];
    c[1]=h->h_addr_list[0][1];
    c[2]=h->h_addr_list[0][2];
    c[3]=h->h_addr_list[0][3];
    if ((fd=socket(AF_INET,SOCK_STREAM,0))==-1) {
	fprintf(stderr,"Could not create a socket!\n");
        exit(1);
    }
    if (connect(fd,&name,sizeof(struct sockaddr_in))) {
	fprintf(stderr,"Could not connect to server on '%s'\n",serv);
        exit(1);
    }
}

static void twrite(char *s) {
    char buf[256];
    int n,r;
    n=0;
    strcpy(buf,s);
    while (n<256) {
	r=write(fd,&buf[n],256-n);
	if (r<0) {
	    r=0;
	}
	n+=r;
    }
}

static void getstring() {
    int p;
    int r;
    p=0;
    while (1) {
	for (r=0;pt==n;r=getmore());
	if (r==1) {strcpy(str,"E\n");return;};
	str[p]=c[pt++];
	if (str[p++]=='\n') {
	    str[p]=0;
	    return;
	}
    }
}

