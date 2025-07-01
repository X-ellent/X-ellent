/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/small.frontend.c,v $
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
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <malloc.h>

static void connect_to_socket(char*serv,int p);
static void tsend(char *s);
static void do_cool_thing();
static void do_terminal_input();
static void do_server_input();
static void process_input();
static void process_server_input();
static void data_input(char *s);

int fd;

char str[512];

char servin[1024];
char termin[1024];
int servi;
int termi;

char currentcommand[64];
int  commandline;

int main(int argc,char *argv[]) {
	char *name;
	char *serv;
	serv="shuffle";
	if (argc>1) serv=argv[1];
	name=getpwuid(getuid())->pw_name;
	if (!name) name="???";
	connect_to_socket(serv,8766);
	tsend((sprintf(str,"%s\n",name),str));
	if (argc>2) tsend(sprintf(str,"%s\n",argv[2]));
	else {
		fprintf(stderr,"I require a password\n");
		exit(1);
	}
	tsend("*SYSSTATUS\n");
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
	bzero((char*) &name, sizeof(struct sockaddr_in));
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
	if (connect(fd,(struct sockaddr *)&name,sizeof(struct sockaddr_in))) {
		fprintf(stderr,"Could not connect to server on '%s'\n",serv);
		exit(1);
	}
}

static void tsend(char *s) {
	int l,n=0;
	l=strlen(s);
	while(n<l) n+=write(fd,&s[n],l-n);
}

static void do_cool_thing() {
	fd_set fmask, tmask;
	int j;
	servi=termi=servin[0]=termin[0]=0;
	fmask=1|(1<<fd);
	while(1) {
		tmask=fmask;
		j=select(32,&tmask,0,0,0);
		if (j>0) {
			if (FD_ISSET(0, &tmask)) do_terminal_input();
			if (FD_ISSET(fd, &tmask)) do_server_input();
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


/*
static void draw_line(int x1,int y1,int x2,int y2) {
	if (x1<-120) return;
	if (x1>120) return;
	if (x2<-120) return;
	if (x2>120) return;
	if (y1<-120) return;
	if (y1>120) return;
	if (y2<-120) return;
	if (y2>120) return;
	x1+=132;y1+=132;x2+=132;y2+=132;
	drawbuf[bp++]=drawm+64;
	drawbuf[bp++]=x1;
	drawbuf[bp++]=y1;
	drawbuf[bp++]=x2;
	drawbuf[bp++]=y2;
	drawm=(drawm+1)%64;
	if (++drawn>19) {
		drawbuf[bp]='\n';
		drawbuf[bp+1]=0;
		tsend(drawbuf);
		bp=8;
		drawn=0;
	}
}
*/

static void process_input() {
	switch(termin[0]) {
	default:
		tsend(sprintf(str,"%s\n",termin));
	}
}

static void process_server_input() {
	switch(servin[0]) {
	case '!':
		fprintf(stderr,"  %s\n",servin);
		break;
	case '.':
		commandline=-1;
		currentcommand[0]=0;
		break;
	case '=':
		if (commandline==-1) {
			strcpy(currentcommand,&servin[1]);
		} else {
			printf("%s %3d %s\n",currentcommand,commandline,servin);
		}
		commandline++;
		break;
	case '#':
		data_input(&servin[1]);
		break;
	default:
		fprintf(stderr,"Unknown prefix %s\n",servin);
		break;
	}
}

void data_input(char *s) {
	fprintf(stderr,"  Exception -> %s\n",s);
}

