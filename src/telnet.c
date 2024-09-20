/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/telnet.c,v $
** $Author: cheesey $
** $Date: 1992/10/15 12:29:25 $
** $Revision: 1.7 $
** $State: Exp $
**
** All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
** anyone wishing to use any of this code may do so, provided that they include
** some acknowledgement of the source (ie me!) 8-)
*/

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#include "constants.h"
#include "player.h"
#include "fix.h"

static int     mysocket,termsocket;
static struct  sockaddr_in myaddr;
struct  sockaddr_in connectaddress;
static char st[256];
static int path;
static int there;
static int fdm[8];
static struct timeval tv;
static int termstate[256];
static char terminput[256][128];

static void do_connect();
static int open_socket(int port);
static void term_connect();
static void term_input(int j);

extern void open_sockets() {
    int i;
    for(i=0;i<8;i++) fdm[i]=0;
    mysocket=open_socket(GAME_SOCK);
    termsocket=open_socket(TERM_SOCK);
}

static int open_socket(int port) {
    int sock;
    int tmp=1;

    bzero((char *) &myaddr, sizeof(struct sockaddr_in));
    myaddr.sin_family=AF_INET;
    myaddr.sin_port=port;
    if ((sock=socket(AF_INET,SOCK_STREAM,0))==-1) {
        (void) perror("Error: can't get socket:");
        exit(1);
    }
    if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char *)&tmp,
		   sizeof(tmp))<0) {
        fprintf(stderr, "DEBUG: setsockopt()\n");
        exit(1);
    }
    if(bind(sock,(struct sockaddr*)&myaddr,
	    sizeof(struct sockaddr_in))==-1) {
        fprintf(stderr, "Unable to bind to port. Is the server already running?\n");
	exit(1);
    }
    if(listen(sock,10)==-1) {
        fprintf(stderr, "DEBUG: listen()\n");
        exit(1);
    }
    (void) ioctl(sock,FIONBIO,&tmp);
    fdm[sock/32]|=(1<<(sock&31));
    return sock;
}

extern void do_login() {
    int rdfiled[8];
    int j;
    tv.tv_usec=10;
    for(j=0;j<8;j++) rdfiled[j]=fdm[j];
    if ((j=select(256,(fd_set*)rdfiled,(fd_set*)0,(fd_set*)0,&tv))<0) {
        fprintf(stderr, "do_login exit\n");
	exit(1);
    }
    if (j>0)
	for (j=0;j<256;j++)
	    if (rdfiled[j/32]&(1<<(j&31))) {
		if (j==mysocket) {
		    do_connect();
		} else {
		    if (j==termsocket) {
			term_connect();
		    } else {
			term_input(j);
		    }
		}
	    }
}

static void do_connect() {
    int cadlen;
    cadlen=sizeof(connectaddress);
    if((path=accept(mysocket,(struct sockaddr*)&connectaddress,&cadlen))<0) {
	perror("Accept failed ");
        return;
    }
    there=-1;
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(connectaddress.sin_addr), client_ip, INET_ADDRSTRLEN);
    setup_player(client_ip);
    close(path);
}

static void term_connect() {
    int cadlen;
    cadlen=sizeof(connectaddress);
    if((path=accept(termsocket,(struct sockaddr*)&connectaddress,&cadlen))<0) {
	perror("Accept failed ");
        return;
    }
    termstate[path]=1;
    terminput[path][0]=0;
    fdm[path/32]|=(1<<(path&31));
}

extern char *tread() {
    int n,r;
    int e;
    int o[8];
    char *cp;
    n=0;e=0;
    if (!there) {
	st[0]=0;
	return st;
    }
    for (n=0;n<8;n++) o[n]=0;
    n=0;
    while(n<256) {
	o[path/32]=(1<<(path&31));
	select(256,o,0,0,0);
	if (o[path/32]&(1<<(path&31))) {
	    r=read(path,&st[n],256-n);
	    if (r==0) {
		st[n]=0;
		n=256;
		there=0;
	    }
	    n+=r;
	}
    }
    for (cp=st;*cp;cp++) if (*cp<' '||*cp>'~') {
	*cp=0;
	return st;
    }
    return st;
}

extern void ctwrite(char *s) {
    write(path,">",1);
    write(path,s,strlen(s));
    write(path,"\n",1);
}

extern char* ctquery(char *s) {
    char *r;
    write(path,"?",1);
    write(path,s,strlen(s));
    write(path,"\n",1);
    r=tread();
    if (!*r) return 0;
    return r;
}

extern char* ctpass() {
    char *r;
    write(path,"P\n",2);
    r=tread();
    if (!*r) return 0;
    return r;
}

static void term_input(int j) {
    int ol,r,i;
    char *nl;
    ol=strlen(terminput[j]);
    r=read(j,&terminput[j][ol],128-ol);
    if (r) {
	while(1) {
	    if (!(nl=strchr(terminput[j],'\n'))) return;
	    *nl=0;
	    for (i=0;terminput[j][i];i++)
		if (terminput[j][i]=='\r') terminput[j][i]=0;
	    termstate[j]=terminal_input(j,termstate[j],terminput[j]);
	    if (termstate[j]) {
		if (nl!=&terminput[j][ol+r]) {
		    strncpy(terminput[j],nl+1,127);
		    ol-=(nl-terminput[j]);
		} else {
		    terminput[j][0]=0;
		    return;
		}
	    } else {
		fdm[j/32]&=~(1<<(j&31));
		termstate[j]=0;
		close(j);
	    }
	}
    } else {
	fdm[j/32]&=~(1<<(j&31));
	terminal_input(j,-1,0);
	termstate[j]=0;
	close(j);
    }
}



