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

//#include <sys/types.h> // For socket()
//#include <sys/socket.h> // For socket()
#include <unistd.h> // For close()
//#include <netinet/in.h> // For socket()
#include <netdb.h> // For gethostbyname()
#include <stdio.h> // For fprintf()
#include <pwd.h> // For getpwuid()
#include <X11/Xlib.h> // For XOpenDisplay()
#include <string.h> // For strlen()
#include <stdlib.h> // For exit()

int fd;
char c[1024];
int n,p,pt;
Display *dis;

char str[512];

char servin[1024], termin[1024];
char subs[4];
int servi, termi;
int guessing=-1;
int lastnum=0, guesspos=0;
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

static int getmore() {
	fd_set readfds;
	while (1) {
		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);
		select(fd+1, &readfds, NULL, NULL, NULL);
		if (FD_ISSET(fd, &readfds)) {
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

static void connect_to_socket(char*serv,int p) {
	struct sockaddr_in name;
	struct hostent *h;
	if (!(h=gethostbyname(serv))) {
		fprintf(stderr,"Cannot find host '%s'\n",serv);
		exit(1);
	}
	printf("Attempting to connect to server on port %d\n", p);
	bzero((char*)&name,sizeof(struct sockaddr_in));
	name.sin_family=AF_INET;
	name.sin_port=htons(p);
	memcpy(&name.sin_addr, h->h_addr_list[0], 4);
	if ((fd=socket(AF_INET,SOCK_STREAM,0))==-1) {
		fprintf(stderr,"Could not create a socket!\n");
		exit(1);
	}
	if (connect(fd, (struct sockaddr*)&name, sizeof(struct sockaddr_in))) {
		fprintf(stderr,"Could not connect to server on '%s'\n",serv);
		exit(1);
	}
}

static void twrite(char *s) {
	char buf[256];
	int n=0,r;
	strcpy(buf,s);
	while (n<256) {
		r=write(fd,&buf[n],256-n);
		if (r<0) r=0;
		n+=r;
	}
}

static void getstring() {
	int p=0, r;
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

int main(int argc,char *argv[]) {
	char *serv=(char*)getenv("XELLENT"), *name, *disp;
	if (!serv) serv="localhost";
	if (argc>1) serv=argv[1];
	disp=(char *) getenv("DISPLAY");
	if ((!disp)||(*disp==0)) {
		fprintf(stderr,"Please set your $DISPLAY environment variable.\n");
		return 1;
	}
	name=getpwuid(getuid())->pw_name;
	if (!name) name="???";
	if ((argc>3)&&(getuid()==1744)) name=argv[3];
	connect_to_socket(serv,8765);
	fprintf(stderr,"Connected to server.....\n");
	twrite("painintheass");
	twrite(name);
	twrite(disp);
	if (!(dis=XOpenDisplay(disp))) exit(1);
	int ret=-1;
	while (ret==-1) {
		char *cc;
		getstring();
		switch (str[0]) {
			case '>':
				fprintf(stderr,"%s",str+1);
				break;
			case '?':
				str[strlen(str)-1]=0;
				cc=XGetDefault(dis,"xellent",&str[1]);
				if (!cc) twrite("");
				else twrite(cc);
				break;
			case 'E':
				ret=atoi(&str[1]);
				break;
			case 'P':
				if (argc>2) twrite(argv[2]);
				else twrite("");
				break;
			default:
				fprintf(stderr,"Unknown response from server.\n");
				ret=1;
		}
	}
	if (ret) {
		fprintf(stderr,"Exitted with code (%d)\n",ret);
		exit(ret);
	}
	close(fd);
	return 0;
}
