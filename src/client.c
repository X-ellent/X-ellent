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
#include <X11/Xauth.h>
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

void generate_random_cookie(char *cookie, int length) {
    const char charset[] = "0123456789ABCDEF";
    for (int i = 0; i < length; i++) {
        cookie[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    cookie[length] = '\0';
}

char* setup_xauthority(const char *display) {
    char *xauth_file = getenv("XAUTHORITY");
    if (!xauth_file) {
        xauth_file = malloc(strlen(getenv("HOME")) + 14);
        sprintf(xauth_file, "%s/.Xauthority", getenv("HOME"));
        setenv("XAUTHORITY", xauth_file, 1);
    }

    FILE *fp = fopen(xauth_file, "r+");
    if (!fp) {
        fp = fopen(xauth_file, "w+");
    }
    if (!fp) {
        fprintf(stderr, "Failed to open Xauthority file\n");
        return NULL;
    }

    Xauth auth_entry;
    auth_entry.family = FamilyLocal;
    auth_entry.address = (char*)display;
    auth_entry.address_length = strlen(display);
    auth_entry.number = "0";
    auth_entry.name = "MIT-MAGIC-COOKIE-1";
    auth_entry.name_length = strlen(auth_entry.name);

    char cookie[33];
    generate_random_cookie(cookie, 32);
    auth_entry.data = cookie;
    auth_entry.data_length = 32;

    if (XauWriteAuth(fp, &auth_entry) == 0) {
        fprintf(stderr, "Failed to write Xauthority entry\n");
        fclose(fp);
        return NULL;
    }

    fclose(fp);
    return strdup(cookie);
}

extern int main(int argc,char *argv[]) {
    char *name;
    char *disp;
    char *serv;
    int ret;
    serv=(char *)getenv("XELLENT");
    if (!serv) serv="127.0.0.1";
    if (argc>1) serv=argv[1];
    disp = getenv("DISPLAY");
    if (!disp || *disp == '\0') {
        disp = "localhost:0";
	fprintf(stderr, "DISPLAY was not set. Defaulting to %s\n", disp);
        fflush(stderr);
        setenv("DISPLAY", disp, 1);
    }
    char *xauth_cookie = setup_xauthority(disp);
    if (!xauth_cookie) {
        fprintf(stderr, "Failed to set up Xauthority\n");
        fflush(stderr);
        return 1;
    }
    if (argc>3) name=argv[3];
    else name=getpwuid(getuid())->pw_name;
    if (!name) name="???";
    connect_to_socket(serv,8765);
    fprintf(stderr,"Connected to server.....\n");
    twrite("painintheass");
    twrite(name);
    twrite(disp);
    twrite(xauth_cookie); // TODO - losing backwards compatibility?
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
    fd_set readfds;
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
	select(fd + 1, &readfds, NULL, NULL, NULL);
	if (FD_ISSET(fd, &readfds)) {
	    if (!(n = read(fd,c,1024))) { // TODO: Validate input
		return 1;
		exit(0);
	    } else {
		if (n < 0) exit(1);
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
    memcpy(&name.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
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
    int n,r;
    n=0;
    strncpy(buf, s, sizeof(buf));
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

