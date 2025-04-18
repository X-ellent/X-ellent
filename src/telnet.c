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

#include  <sys/types.h>
#include  <sys/time.h>
#include  <sys/socket.h>
#include  <sys/select.h>
#include  <netinet/in.h>
/* #include  <sys/filio.h> */
#include  <sys/ioctl.h>
#include  <string.h>
#include  <errno.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <ctype.h>
#include  <unistd.h>

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
		exit(1);
	}
	if(bind(sock,(struct sockaddr*)&myaddr,
			sizeof(struct sockaddr_in))==-1) {
		exit(1);
	}
	if(listen(sock,10)==-1) {
		exit(1);
	}
	(void) ioctl(sock,FIONBIO,&tmp);
	fdm[sock/32]|=(1<<(sock&31));
	return sock;
}

extern void do_login() {
	fd_set rdfiled;
	int j;
	tv.tv_usec=10;
	FD_ZERO(&rdfiled);
	for(j=0;j<256;j++)
		if (fdm[j/32]&(1<<(j&31)))
			FD_SET(j, &rdfiled);
	if ((j=select(256,&rdfiled,NULL,NULL,&tv))<0) {
		exit(1);
	}
	if (j>0)
		for (j=0;j<256;j++)
			if (FD_ISSET(j, &rdfiled)) {
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
	struct sockaddr_in connectaddress;
	socklen_t cadlen;

	cadlen=sizeof(struct sockaddr_in);
	if((path=accept(mysocket,(struct sockaddr*)&connectaddress,&cadlen))<0) {
		perror("Accept failed ");
		return;
	}
	there=-1;
	setup_player();
	close(path); // Disconnect the client
}

static void term_connect() {
	struct sockaddr_in connectaddress;
	socklen_t cadlen;

	cadlen=sizeof(struct sockaddr_in);
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
	fd_set readfds;
	char *cp;
	if (!there) {
		st[0]=0;
		return st;
	}

	n=0;
	while(n<256) {
		FD_ZERO(&readfds);
		FD_SET(path, &readfds);
		select(path+1, &readfds, NULL, NULL, NULL);
		if (FD_ISSET(path, &readfds)) {
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
	ssize_t bytes_written;
	bytes_written = write(path,">",1);
	if (bytes_written < 0) {
		perror("write error");
	}
	bytes_written = write(path,s,strlen(s));
	if (bytes_written < 0) {
		perror("write error");
	}
	bytes_written = write(path,"\n",1);
	if (bytes_written < 0) {
		perror("write error");
	}
}

extern char* ctquery(char *s) {
	static char response[256];
	ssize_t bytes_written;
	bytes_written = write(path,"?",1);
	if (bytes_written < 0) {
		perror("write error");
	}
	bytes_written = write(path,s,strlen(s));
	if (bytes_written < 0) {
		perror("write error");
	}
	bytes_written = write(path,"\n",1);
	if (bytes_written < 0) {
		perror("write error");
	}
	if (tread(response) < 0) {
		return 0;
	}
	if (!*response) return 0;
	return response;
}

extern char* ctpass() {
	char *r;
	ssize_t bytes_written;
	bytes_written = write(path,"P\n",2);
	if (bytes_written < 0) {
		perror("write error");
	}
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



