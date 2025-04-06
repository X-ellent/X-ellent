/*
** $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/argh.c,v $
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
#include <malloc.h>
#include <math.h>

#include "fix.h"
#include "fonts.h"
#define FRONTFONT "-b&h-lucidatypewriter-bold-r-normal-sans-10-100-75-75-m-60-iso8859-1"

#define MAPWID 450
#define MAPHGT 450
#define MINMAPWID 150
#define MINMAPHGT 150
#define FWINWID 450
#define FWINHGT 600

struct map {
	int wid,hgt,dep,lev;
	int sqr;
	char *data[64];
	struct mypixmap *tinymap[64];
};

int displev=-1;
int mapa,mapb,mapc;
char myname[32];
char newname[32];

struct location {
	int l,x,y;
};

struct teleport {
	struct location loc;
	int num;
	int dest;
	char pass[6];
	struct teleport *next;
};

struct lift {
	struct location loc;
	int num;
	int targ;
	char pass[8];
	struct lift *next;
};

int levs[6];
int change,mapx,mapy;
int tarx,tary;
int spotx,spoty;
int myrot;
struct location me;
struct location targ;
struct location spot;
struct location home;
struct teleport *firsttel;
struct lift *firstlift;

#define PI M_PI

double sintable[720];
double *sn;
double *cs;

#define ABS(x) (((x)>=0)?(x):-(x))

char otxt[256];
int dashrot;

Display *disp;
int screen;
Window win;
GC gc;
GC red;
GC black;
GC whiteb;
GC green;
GC grey;
GC dgrey;
GC white;
GC blue;
GC yellow;
GC stripea;
GC stripeb;
GC ice;
XFontStruct *font;
XFontStruct *tfont;
XFontStruct *bfont;
XFontStruct *lfont;
int fh,fw,fo;
int bh,bw,bo;
int th,tw,to;
static XColor xc,ex;

struct map map;

struct mypixmap {
	Pixmap pix;
	int wid,hgt,sqr,xo,yo;
};

struct mypixmap back;
struct mypixmap unknown;

int interminal;
int inhome;
int inshop;

int fd;

char str[512];

char servin[1024];
char termin[1024];
int servi;
int termi;

char currentcommand[64];
int  commandline;

/*
  char dr>awbuf[128];
  int  bp;
  int  drawn;
  int  drawm;
*/

char guessed[32];
char preguess[128];
char postguess[128];
char guesscom[128];
char guessing[32];
int guessnum=-1;
int guesspos=0;
int guesslen=4;
int guesscount=0;
int guesstyp;
int guessval;
int guessvalb;
char *guesspass;

#define GUESS_TELDEST 1
#define GUESS_LIFMOVE 2
#define GUESS_TELACTI 3

extern int main(int argc,char *argv[]);
extern void data_input(char *s);
extern void get_location(struct location *l,char *s);
extern struct teleport *find_teleport(int n);
extern void read_teleport(char *s);
extern void read_map(char *s,int l);
extern void init_display(char *d);
extern void alloc_pixmap(struct mypixmap *p,int w,int h);
extern void Setup_color(char *dname,char *dcol);
extern void print_text(char *s);
extern void draw_map(int n);
extern void build_sintable();
extern void load_map();
extern void save_map();
extern void scribble_map(int n,struct mypixmap *p);

static void connect_to_socket(char*serv,int p);
static void tsend(char *s);
static void do_cool_thing();
static void do_terminal_input();
static void do_server_input();
/*static void draw_line(int x1,int y1,int x2,int y2);*/
static void process_input();
static void process_server_input();

#define rd(l,x,y) map.data[l][(x)+((y)*map.wid)]
#define scc(a,b,c) Setup_color(a,b);XSetForeground(disp,c,xc.pixel);

extern int main(int argc,char *argv[]) {
	char *name;
	char *disp;
	char *serv;
	char *home;
	int i;
	serv="shuffle";
	if (argc>1) serv=argv[1];
	name=getpwuid(getuid())->pw_name;
	if (!name) name="???";
	build_sintable();
	home=getenv("HOME");
	if (home) chdir(home);
	init_display(disp=getenv("DISPLAY"));
	for (i=0;i<64;i++) map.tinymap[i]=&unknown;
	load_map();
	for (i=0;i<6;i++) {
		levs[i]=i;
		change=-1;
	}
	connect_to_socket(serv,8766);
	tsend(sprintf(str,"%s\n",name));
	if (argc>2) {
		tsend(sprintf(str,"%s\n",argv[2]));
	} else {
		fprintf(stderr,"I require a password\n");
		exit(1);
	}
	tsend("*SYSSTATUS\n");
	tsend("*SYSLOCATE ON\n");
	tsend("*SYSHOME\n");
	tsend("*TERTELEPORTS\n");
	tsend("*TERLIFTS\n");
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

static void tsend(char *s) {
	int l,n;
	l=strlen(s);
	n=0;
	while(n<l)
		n+=write(fd,&s[n],l-n);
}

static void do_cool_thing() {
	int fmask;
	int tmask;
	struct timeval tv;
	int j;
	servi=0;
	termi=0;
	servin[0]=0;
	termin[0]=0;
	fmask=1|(1<<fd);
	while(1) {
		tv.tv_usec=500000;
		tv.tv_sec=0;
		tmask=fmask;
		j=select(32,&tmask,0,0,&tv);
		if (j>0) {
			if (tmask&1) do_terminal_input();
			if (tmask&(1<<fd)) do_server_input();
		} else {
			draw_map(me.l);
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
		tsend(drawbuf);
		bp=8;
		drawn=0;
	}
}
*/

static void process_input() {
	switch(termin[0]) {
	case 'n':
		strcpy(myname,&termin[1]);
		strcpy(newname,&termin[1]);
		break;
	case 'N':
		strcpy(newname,&termin[1]);
		break;
	case '?':
		special_command(&termin[1]);
		break;
	default:
		tsend(sprintf(str,"%s\n",termin));
	}
}

static void process_server_input() {
	switch(servin[0]) {
	case '!':
		sprintf(otxt,"  %s",servin);print_text(otxt);
		break;
	case '.':
		commandline=-1;
		if (strcmp(currentcommand,"TER MAP")==0) {
			update_map(me.l);
			change=-1;
			save_map();
		}
		if (strcmp(currentcommand,"TER TELEPORTS")==0) {
			change=-1;
		}
		currentcommand[0]=0;
		break;
	case '=':
		if (commandline==-1) {
			strcpy(currentcommand,&servin[1]);
		} else {
			if (strcmp(currentcommand,"TER TELEPORTS")==0) {
				read_teleport(&servin[1]);break;}
			if (strcmp(currentcommand,"TER LIFTS")==0) {
				read_lift(&servin[1]);break;}
			if (strcmp(currentcommand,"TER MAP")==0) {
				read_map(&servin[1],commandline++);break;}
			if (strcmp(currentcommand,"SYS HOME")==0) {
				get_location(&home,&servin[1]);break;}
			if (strcmp(currentcommand,guesscom)==0) {
				process_guess(&servin[1]);break;}
			sprintf(otxt,"%s %3d %s",currentcommand,commandline,servin);
			print_text(otxt);
		}
		commandline++;
		break;
	case '#':
		data_input(&servin[1]);
		break;
	case 'K':
		if (servin[1]=='F') {
			int n;
			n=atoi(&servin[2]);
			if (n==11) {
				tsend("*SYSSPIN10\n");
				break;
			}
			if (n==12) {
				tsend("*SYSSPIN100\n");
				break;
			}
			tsend(sprintf(str,"*WEPSELECT%d\n",atoi(&servin[2])-1));
			break;
		}
		if ((servin[1]=='P')&&(servin[2]=='F')) {
			struct teleport *t;
			switch(servin[3]) {
			case '1':
				for(levs[1]=(levs[1]+1)&63;!
				map.data[levs[1]];
					levs[1]=(levs[1]+1)&63);
			case '2':
				change=-1;
				break;
			case '3':
				for(t=firsttel;t;t=t->next)
					tsend(sprintf(str,"*TELACTIVATE%d %s\n",t->num,t->pass));
				break;
			default:
				sprintf(otxt,"Keypress %s",&servin[1]);
				print_text(otxt);
				break;
			}
			break;
		}
		sprintf(otxt,"Keypress %s",&servin[1]);
		print_text(otxt);
		break;
	default:
		sprintf(otxt,"Unknown prefix %s",servin);
		print_text(otxt);
		break;
	}
}

extern void data_input(char *s) {
	char *ss,*sss;
	char tempname[32];
	int n;
	int lm,ln;
	double x,y;
	if (strncmp(s,"TARGET ",7)==0) {
		ss=strchr(&s[7],' ');
		if (!ss) return;
		*ss=0;
		ss++;
		get_location(&targ,ss);
		return;
	}
	if (strncmp(s,"LOCATE ",7)==0) {
		fprintf(stderr, s);
		ss=strchr(&s[7],' ');
		if (!ss) return;
		*ss=0;
		ss++;
		myrot=atoi(&s[7]);
		get_location(&me,ss);
		draw_map(me.l);
		if ((lm=strlen(myname))!=(ln=strlen(newname))) {
			if (lm>ln) {
				n=random()%lm;
				strcpy(tempname,myname);
				strcpy(&myname[n],&tempname[n+1]);
			} else {
				if (lm) {
					n=random()%lm;
					strcpy(tempname,myname);
					strcpy(&myname[n+1],&tempname[n]);
					myname[n]=newname[n];
				} else {
					myname[1]=0;
					myname[0]=newname[random()%ln];
				}
			}
			tsend(sprintf(str,"*SYSNAME%s\n",myname));
		} else {
			if (strcmp(myname,newname)) {
				for (n=random()&31;myname[n]==newname[n];n=random()&31);
				myname[n]=newname[n];
				tsend(sprintf(str,"*SYSNAME%s\n",myname));
			}
		}
		return;
	}
	if (strncmp(s,"LASER TARGET ",13)==0) {
		ss=strchr(&s[13],' ');
		if (!ss) return;
		*ss=0;
		ss++;
		sss=strchr(ss,' ');
		if (!sss) return;
		*sss=0;
		sss++;
		get_location(&me,ss);
		n=atoi(sss);
		x=me.x+n*128*sn[myrot]/100;
		y=me.y-n*128*cs[myrot]/100;
		spot.x=x;
		spot.y=y;
		spot.l=me.l;
/*	draw_map(me.l);*/
		if (map.data[me.l])
			if (rd(me.l,(int)x/128,(int)y/128)=='O') return;
		tsend("*WEPSELECT8\n");
		tsend("*SYSNAMEA Damn Good Shot\n");
		strcpy(myname,newname);
		return;
	}
	if (strcmp(s,"TERM ENTER")==0) {
		tsend("*TERMAP\n");
		tsend("*TERTELEPORTS\n");
		tsend("*TERLIFTS\n");
		interminal=-1;
		return;
	}
	if (strcmp(s,"TERM EXIT")==0) {
		interminal=0;
		return;
	}
	sprintf(otxt,"  Exception -> %s",s);print_text(otxt);
}

extern void get_location(struct location *l,char *s) {
	char *ss,*sss;
	ss=strchr(s,':');
	sss=strchr(s,',');
	if ((!ss)||(!sss)) return;
	*ss=0;
	*sss=0;
	l->l=atoi(s);
	l->x=atoi(ss+1);
	l->y=atoi(sss+1);
	*ss=':';
	*sss=',';
}

extern struct teleport *find_teleport(int n) {
	struct teleport *t;
	for (t=firsttel;t;t=t->next)
		if (t->num==n) return t;
	return 0;
}

extern struct lift *find_lift(int n) {
	struct lift *l;
	for (l=firstlift;l;l=l->next)
		if (l->num==n) return l;
	return 0;
}

extern struct teleport *locate_teleport(int l,int x,int y) {
	struct teleport *t;
	for (t=firsttel;t;t=t->next)
		if ((t->loc.l==l)&&((t->loc.x/128)==x)&&((t->loc.y/128)==y)) return t;
	return 0;
}

extern struct lift *locate_lift(int x,int y) {
	struct lift *l;
	for (l=firstlift;l;l=l->next)
		if (((l->loc.x/128)==x)&&((l->loc.y/128)==y)) return l;
	return 0;
}

extern void read_teleport(char *s) {
	char *ds,*loc;
	int num;
	struct teleport *t;
	if (!(ds=strchr(s,' '))) return;
	if (!(loc=strchr(ds+1,' '))) return;
	*ds=0;
	*loc=0;
	ds++;
	loc++;
	num=atoi(s);
	if (!(t=find_teleport(num))) {
		t=(struct teleport *) calloc(1,sizeof(struct teleport));
		t->num=num;
		t->next=firsttel;
		firsttel=t;
		strcpy(t->pass,"----");
		get_location(&t->loc,loc);
		sprintf(otxt,"Initialising storage for teleport %d",num);
		print_text(otxt);
	}
	t->dest=atoi(ds);
}

extern void read_lift(char *s) {
	char *tl,*loc;
	int num;
	struct lift *l;
	if (!(loc=strchr(s,' '))) return;
	if (!(tl=strchr(loc,' '))) return;
	*tl=0;
	*loc=0;
	tl++;
	loc++;
	num=atoi(s);
	if (!(l=find_lift(num))) {
		l=(struct lift *) calloc(1,sizeof(struct lift));
		l->num=num;
		l->next=firstlift;
		firstlift=l;
		strcpy(l->pass,"------");
		get_location(&l->loc,loc);
		sprintf(otxt,"Initialising storage for lift %d",num);
		print_text(otxt);
	}
	l->targ=atoi(tl);
}


extern void read_map(char *s,int l) {
	char *lev,*wid,*hgt;
	int x,sx,sy;
	if (l==0) {
		lev=strchr(s,' ');*lev=0;lev++;
		wid=strchr(lev,':');*wid=0;wid++;
		hgt=strchr(wid,',');*hgt=0;hgt++;
		map.dep=atoi(s);
		map.lev=atoi(lev);
		map.wid=atoi(wid);
		map.hgt=atoi(hgt);
		sx=MAPWID/map.wid;
		sy=MAPWID/map.wid;
		map.sqr=(sx<sy)?sx:sy;
		if (!map.data[map.lev]) {
			sprintf(otxt,"Initialising storage space for level %d",map.lev);
			print_text(otxt);
			map.data[map.lev]=calloc(map.hgt,map.wid);
		}
	} else {
		for (x=0;x<map.wid;x++)
			rd(map.lev,x,l-1)=s[x];
	}
}

extern void init_display(char *d) {
	unsigned long vm;
	int fail;
	if (!(disp=XOpenDisplay(d))) {
		fprintf(stderr,"Could not open display, sorry");
		return;
	}
	screen=DefaultScreen(disp);
	gc=DefaultGC(disp,screen);
	vm=GCForeground|GCBackground|GCFont|GCLineStyle|GCFillStyle;
	win=XCreateSimpleWindow(disp,RootWindow(disp,screen),0,0,
								FWINWID,FWINHGT,0,WhitePixel(disp,screen),
								BlackPixel(disp,screen));
	if (!(tfont=XLoadQueryFont(disp,FRONTFONT))) fail=-1;
	if (!(bfont=XLoadQueryFont(disp,HUGEFONT))) fail=-1;
	if (!(font=XLoadQueryFont(disp,GAMEFONT))) fail=-1;
	if (!(lfont=XLoadQueryFont(disp,TERMFONT))) fail=-1;
	fw=font->max_bounds.width;
	fo=font->max_bounds.ascent;
	fh=fo+font->max_bounds.descent;
	bw=bfont->max_bounds.width;
	bo=bfont->max_bounds.ascent;
	bh=bfont->max_bounds.descent+bo/2;
	tw=lfont->max_bounds.width;
	to=lfont->max_bounds.ascent;
	th=to+lfont->max_bounds.descent;
	XCopyGC(disp,gc,vm,black=XCreateGC(disp,win,0,0));
	XCopyGC(disp,gc,vm,white=XCreateGC(disp,win,0,0));
	XCopyGC(disp,gc,vm,whiteb=XCreateGC(disp,win,0,0));
	XCopyGC(disp,gc,vm,ice=XCreateGC(disp,win,0,0));
	XCopyGC(disp,gc,vm,red=XCreateGC(disp,win,0,0));
	XCopyGC(disp,gc,vm,green=XCreateGC(disp,win,0,0));
	XCopyGC(disp,gc,vm,grey=XCreateGC(disp,win,0,0));
	XCopyGC(disp,gc,vm,dgrey=XCreateGC(disp,win,0,0));
	XCopyGC(disp,gc,vm,blue=XCreateGC(disp,win,0,0));
	XCopyGC(disp,gc,vm,yellow=XCreateGC(disp,win,0,0));
	XCopyGC(disp,gc,vm,stripeb=XCreateGC(disp,win,0,0));
	XCopyGC(disp,gc,vm,stripea=XCreateGC(disp,win,0,0));
	scc("white","white",white);
	scc("whiteb","grey45",whiteb);
	scc("ice","white",ice);
	scc("red","red",red);
	XSetForeground(disp,stripea,xc.pixel);
	scc("green","seagreen",green);
	XSetForeground(disp,stripeb,xc.pixel);
	scc("blue","DodgerBlue",blue);
	scc("yellow","yellow",yellow);
	scc("black","black",black);
	XSetBackground(disp,red,xc.pixel);
	scc("grey","grey50",grey);
	scc("dgrey","grey75",dgrey);
	XSetLineAttributes(disp,stripea,1,LineOnOffDash,0,0);
	XSetLineAttributes(disp,stripeb,1,LineOnOffDash,0,0);
	alloc_pixmap(&back,MAPWID,MAPHGT);
	alloc_pixmap(&unknown,MINMAPWID,MINMAPHGT);
	XSelectInput(disp,win,ButtonPressMask);
	XStoreName(disp,win,"X-ellent Frontend");
	XSetFont(disp,red,font->fid);
	XSetFont(disp,ice,tfont->fid);
	XMapRaised(disp,win);
	XFlush(disp);
	XClearWindow(disp,win);
	XDrawRectangle(disp,back.pix,blue,0,0,back.wid-1,back.hgt-1);
	XFillRectangle(disp,unknown.pix,red,0,0,unknown.wid-1,unknown.hgt-1);
}

extern void alloc_pixmap(struct mypixmap *p,int w,int h) {
	p->pix=XCreatePixmap(disp,win,w,h,DefaultDepth(disp,screen));
	p->wid=w;
	p->hgt=h;
	XFillRectangle(disp,p->pix,black,0,0,p->wid,p->hgt);
	XDrawRectangle(disp,p->pix,blue,0,0,p->wid-1,p->hgt-1);
}

extern void Setup_color(char *dname,char *dcol) {
	char *cc;
	cc=XGetDefault(disp,"xellentf",dname);
	if (cc) {
		if (XAllocNamedColor(disp,DefaultColormap(disp,screen),cc,&ex,&xc))
			return;
		fprintf(stderr,"Cannot find color %s\n",cc);
	}
	XAllocNamedColor(disp,DefaultColormap(disp,screen),dcol,&ex,&xc);
}

extern void print_text(char *s) {
	printf("%s\n",s);
}

extern void draw_map(int n) {
	int xo,yo;
	int xc,yc;
	struct teleport *t,*td;
	struct mypixmap *m;
	XEvent xev;
	int w,i,j;
	w=map.sqr;
	if (displev!=n) {
		scribble_map(n,&back);
		change=-1;
		displev=n;
		if (n>0) {
			levs[0]=n-1;
			change=-1;
		}
		if (n<(map.dep-1)) {
			levs[2]=n+1;
			change=-1;
		}
	}
	if (levs[1]==displev) {
		for(levs[1]=(levs[1]+1)&63;!
			map.data[levs[1]];
			levs[1]=(levs[1]+1)&63);
		change=-1;
	}
	if (levs[1]==levs[0]) {
		change=-1;
		for(levs[1]=(levs[1]+1)&63;!
			map.data[levs[1]];
			levs[1]=(levs[1]+1)&63);
	}
	if (levs[1]==levs[2]) {
		change=-1;
		for(levs[1]=(levs[1]+1)&63;!
			map.data[levs[1]];
			levs[1]=(levs[1]+1)&63);
	}
	xo=(back.wid-(w*map.wid))/2;
	yo=(back.hgt-(w*map.hgt))/2;
	if (change) {
		XCopyArea(disp,back.pix,win,gc,0,0,back.wid,back.hgt,0,0);
	} else {
		XCopyArea(disp,back.pix,win,gc,mapx-20,mapy-20,40,40,mapx-20,mapy-20);
		XCopyArea(disp,back.pix,win,gc,tarx-20,tary-20,40,40,tarx-20,tary-20);
		XCopyArea(disp,back.pix,win,gc,spotx-20,spoty-20,40,40,
				  spotx-20,spoty-20);
	}
	sprintf(str,"%d:%d,%d          ",me.l,me.x/128,me.y/128);
	XDrawImageString(disp,win,red,4,fh,str,strlen(str));
	if (change) {
		XCopyArea(disp,map.tinymap[levs[0]]->pix,win,gc,0,0,
				  map.tinymap[levs[0]]->wid,
				  map.tinymap[levs[0]]->hgt,0,MAPHGT);
		XCopyArea(disp,map.tinymap[levs[1]]->pix,win,gc,0,0,
				  map.tinymap[levs[1]]->wid,
				  map.tinymap[levs[1]]->hgt,150,MAPHGT);
		XCopyArea(disp,map.tinymap[levs[2]]->pix,win,gc,0,0,
				  map.tinymap[levs[2]]->wid,
				  map.tinymap[levs[2]]->hgt,300,MAPHGT);
	}
	if (home.l==me.l) {
		XDrawRectangle(disp,win,black,home.x*w/128-7+xo,home.y*w/128-7+yo,
					   15,15);
		XDrawRectangle(disp,win,yellow,home.x*w/128+xo-8,home.y*w/128+yo-8,
					   17,17);
		XDrawRectangle(disp,win,black,home.x*w/128-9+xo,home.y*w/128-9+yo,
					   19,19);
	}
	XDrawRectangle(disp,win,black,me.x*w/128-1+xo,me.y*w/128-1+yo,3,3);
	XDrawRectangle(disp,win,yellow,me.x*w/128+xo-2,me.y*w/128+yo-2,5,5);
	XDrawRectangle(disp,win,black,me.x*w/128-3+xo,me.y*w/128-3+yo,7,7);

	XDrawRectangle(disp,win,black,targ.x*w/128-1+xo,targ.y*w/128-1+yo,3,3);
	XDrawRectangle(disp,win,red,targ.x*w/128+xo-2,targ.y*w/128+yo-2,5,5);
	XDrawRectangle(disp,win,black,targ.x*w/128-3+xo,targ.y*w/128-3+yo,7,7);
	tarx=targ.x*w/128+xo;
	tary=targ.y*w/128+yo;

	spotx=spot.x*w/128+xo;
	spoty=spot.y*w/128+yo;
	XDrawLine(disp,win,yellow,spotx-10,spoty-10,spotx+10,spoty+10);
	XDrawLine(disp,win,yellow,spotx-10,spoty+10,spotx+10,spoty-10);

	XDrawLine(disp,win,red,mapx=me.x*w/128+xo,mapy=me.y*w/128+yo,
			  me.x*w/128+xo+16*sn[myrot],me.y*w/128+yo-16*cs[myrot]);
	XDrawLine(disp,win,red,mapx=me.x*w/128+xo,mapy=me.y*w/128+yo,
			  me.x*w/128+xo+16*sn[myrot],me.y*w/128+yo-16*cs[myrot]);
	dashrot-=5;
	dashrot&=63;
	XSetDashes(disp,stripea,(dashrot&63),"\040\040",2);
	XSetDashes(disp,stripeb,(dashrot+32)&63,"\040\040",2);
	for (t=firsttel;t;t=t->next)
		if (t->loc.l==me.l)
			if (td=find_teleport(t->dest)) {
				if (t->loc.l==td->loc.l) {
					XDrawLine(disp,win,stripea,t->loc.x*w/128+xo,
							  t->loc.y*w/128+yo,
							  td->loc.x*w/128+xo,td->loc.y*w/128+yo);
					XDrawLine(disp,win,stripeb,t->loc.x*w/128+xo,
							  t->loc.y*w/128+yo,
							  td->loc.x*w/128+xo,td->loc.y*w/128+yo);
				} else {
					for (i=0;i<3;i++)
						if (levs[i]==td->loc.l) {
							m=map.tinymap[levs[i]];
							XDrawLine(disp,win,stripea,
									  t->loc.x*w/128+xo,t->loc.y*w/128+yo,
									  td->loc.x*m->sqr/128+m->xo+(150*(i%3)),
									  td->loc.y*m->sqr/128+m->yo+MAPHGT+
									  (150*((int)i/3)));
							XDrawLine(disp,win,stripeb,
									  t->loc.x*w/128+xo,t->loc.y*w/128+yo,
									  td->loc.x*m->sqr/128+m->xo+(150*(i%3)),
									  td->loc.y*m->sqr/128+m->yo+MAPHGT+
									  (150*((int)i/3)));
						}
				}
			}
	if (change) change=0;
	XFlush(disp);
	while (XPending(disp)) {
		int x,y;
		XNextEvent(disp,&xev);
		switch(xev.xany.type) {
		case ButtonPress:
/*	    fprintf(stderr,"Button %d at %d,%d\n",xev.xbutton.button,
					xev.xbutton.x,xev.xbutton.y);*/
			x=xev.xbutton.x;
			y=xev.xbutton.y;
			if ((x<MAPWID)&&(y<MAPHGT)) {
				struct lift *l;
				struct teleport *t;
				x=(x-xo)/w;
				y=(y-yo)/w;
/*		fprintf(stderr,"Map coordinates... %d:%d,%d\n",displev,x,y);*/
				if (map.data[displev])
					switch(rd(displev,x,y)) {
					case 'l':
					case 'L':
						l=locate_lift(x,y);
						if (l) {
							switch(xev.xbutton.button) {
							case 1:
								summon_lift(l->num,me.l);
								fprintf(stderr,"Summoning Lift\n");
								break;
							case 2:
								summon_lift(l->num,0);
								fprintf(stderr,"Raising Lift\n");
								break;
							case 3:
								summon_lift(l->num,map.dep-1);
								fprintf(stderr,"Lowering Lift\n");
								break;
							}
						} else {
							fprintf(stderr,"Unknown Lift\n");
						}
						break;
					case 'X':
						t=locate_teleport(displev,x,y);
						if (t) {
							fprintf(stderr,"Activating teleport...\n");
							activate_teleport(t);
						} else {
							fprintf(stderr,"Unknown teleport...\n");
						}
						break;
					default:
						break;
					}
			}
			break;
		default:
			break;
		}
	}
}

extern void build_sintable()
{
	int i;
	for (i=1;i<90;i++) {
		sintable[i]=sin(((double) i)/180.0*PI);
		sintable[180-i]=sintable[i];
	}
	sintable[0]=0;
	sintable[90]=1;
	for (i=0;i<180;i++)
		sintable[i+180]=-sintable[i];
	for (i=0;i<360;i++)
		sintable[i+360]=sintable[i];
	sn=sintable;
	cs=&sintable[90];
}

extern void load_map() {
	FILE *mf;
	char *cc,*oc;
	int n,x,y;
	int sx,sy;
	char inp[1024];
	if (!(mf=fopen(".xellent.map","rb"))) {
		fprintf(stderr,"Cannot open .xellent.map\n");
		return;
	}
	*inp=0;fgets(inp,1023,mf);cc=inp;
	oc=cc;cc=strchr(cc,' ');*cc++=0;map.wid=atoi(oc);
	oc=cc;cc=strchr(cc,' ');*cc++=0;map.hgt=atoi(oc);
	oc=cc;cc=strchr(cc,' ');*cc++=0;map.dep=atoi(oc);
	while(*inp=0,fgets(inp,1023,mf),cc=inp,(*inp)&&(strcmp(inp,"END\n"))) {
		n=atoi(inp);
		sx=MAPWID/map.wid;
		sy=MAPWID/map.wid;
		map.sqr=(sx<sy)?sx:sy;
		if (!map.data[n])
			map.data[n]=calloc(map.hgt,map.wid);
		for (y=0;y<map.hgt;y++) {
			*inp=0;fgets(inp,1023,mf);cc=inp;
			for (x=0;x<map.wid;x++)
				rd(n,x,y)=inp[x];
		}
	}
	fclose(mf);
	for (n=0;n<64;n++)
		if (map.data[n]) update_map(n);
}

extern void save_map() {
	FILE *mf;
	int n,x,y;
	char inp[1024];
	if (!(mf=fopen(".xellent.map","w"))) {
		fprintf(stderr,"Cannot open .xellent.map\n");
		return;
	}
	fprintf(mf,"%d %d %d \n",map.wid,map.hgt,map.dep);
	for (n=0;n<map.dep;n++)
		if (map.data[n]) {
			fprintf(mf,"%d \n",n);
			for(y=0;y<map.hgt;y++) {
				for(x=0;x<map.wid;x++)
					inp[x]=rd(n,x,y);
				inp[x]=0;
				fprintf(mf,"%s \n",inp);
			}
		}
	fprintf(mf,"END\n");
	fclose(mf);
}

extern void scribble_map(int n,struct mypixmap *p) {
	int x,y;
	int sx,sy;
	int cx,cy;
	int w,xo,yo;
	char c1,c2,c3,cc;
	int lw,hw;
	if ((n<0)||(n>=map.dep)) {
		XFillRectangle(disp,p->pix,black,0,0,p->wid,p->hgt);
		return;
	}
	sx=p->wid/map.wid;
	sy=p->hgt/map.hgt;
	w=(sx<sy)?sx:sy;
	p->sqr=w;
	xo=(p->wid-(w*map.wid))/2;
	yo=(p->hgt-(w*map.hgt))/2;
	p->xo=xo;
	p->yo=yo;
	if (!map.data[n]) {
		XFillRectangle(disp,p->pix,black,0,0,p->wid,p->hgt);
		XDrawRectangle(disp,p->pix,blue,xo,yo,map.wid*w-1,map.hgt*w-1);
		return;
	}
	lw=w/4;
	hw=w-lw-1;
	XFillRectangle(disp,p->pix,black,0,0,p->wid,p->hgt);
	XDrawRectangle(disp,p->pix,blue,xo,yo,map.wid*w-1,map.hgt*w-1);
	for(y=1;y<map.hgt;y++)
		for(x=1;x<map.wid;x++) {
			cx=x*w+xo;
			cy=y*w+yo;
			cc=rd(n,x,y);
			if (cc!=' ') XFillRectangle(disp,p->pix,dgrey,cx,cy,w,w);
			switch (cc) {
			case ' ':
				if (n>=map.dep) break;
				if (!map.data[n+1]) break;
				if (rd(n+1,x,y)!=' ') {
					XFillRectangle(disp,p->pix,grey,cx,cy,w,w);
					if ((rd(n+1,x,y)=='l')||(rd(n+1,x,y)=='L'))
						XFillRectangle(disp,p->pix,red,cx+lw,cy+lw,
									   hw-lw,hw-lw);
				}
				break;
			case 'H':
				XDrawRectangle(disp,p->pix,blue,cx+lw,cy+lw,hw-lw,hw-lw);
				break;
			case 'L':
			case 'l':
				XDrawRectangle(disp,p->pix,red,cx+lw,cy+lw,hw-lw,hw-lw);
				break;
			case 'X':
				XDrawRectangle(disp,p->pix,green,cx+lw,cy+lw,hw-lw,hw-lw);
				XDrawArc(disp,p->pix,green,cx+lw,cy+lw,hw-lw,hw-lw,0,360*64);
				break;
			case 'Z':
				XDrawArc(disp,p->pix,blue,cx+lw,cy+lw,hw-lw,hw-lw,0,360*64);
				break;
			case 'C':
				XDrawArc(disp,p->pix,red,cx+lw,cy+lw,hw-lw,hw-lw,0,360*64);
				break;
			case 'O':
				XDrawArc(disp,p->pix,white,cx+lw,cy+lw,hw-lw,hw-lw,0,360*64);
				break;
			case 'F':
				XFillRectangle(disp,p->pix,red,cx+lw,cy+lw,hw-lw,hw-lw);
				XDrawRectangle(disp,p->pix,white,cx+lw,cy+lw,hw-lw,hw-lw);
				break;
			case 'T':
				XFillRectangle(disp,p->pix,green,cx+lw,cy+lw,hw-lw,hw-lw);
				XDrawRectangle(disp,p->pix,white,cx+lw,cy+lw,hw-lw,hw-lw);
				break;
			case 'S':
				XFillRectangle(disp,p->pix,blue,cx+lw,cy+lw,hw-lw,hw-lw);
				XDrawRectangle(disp,p->pix,white,cx+lw,cy+lw,hw-lw,hw-lw);
				break;
			case '+':
				XFillRectangle(disp,p->pix,ice,cx,cy,w,w);
				break;
			case '-':
				XDrawRectangle(disp,p->pix,ice,cx+lw,cy+lw,hw-lw,hw-lw);
				break;
			default:
			}
		}
	for(y=1;y<map.hgt;y++)
		for(x=1;x<map.wid;x++) {
			cx=x*w+xo;
			cy=y*w+yo;
			c1=(rd(n,x-1,y)==' ')?' ':'*';
			c2=(rd(n,x,y-1)==' ')?' ':'*';
			c3=(rd(n,x,y)==' ')?' ':'*';
			if (c1!=c3)
				if (c1==' ') {
					XDrawLine(disp,p->pix,white,cx,cy,cx,cy+w);
				} else {
					XDrawLine(disp,p->pix,whiteb,cx,cy,cx,cy+w);
				}
			if (c2!=c3)
				if (c2==' ') {
					XDrawLine(disp,p->pix,white,cx,cy,cx+w,cy);
				} else {
					XDrawLine(disp,p->pix,whiteb,cx,cy,cx+w,cy);
				}
		}
	return;
}

extern void update_map(int n) {
	if (map.tinymap[n]==&unknown) {
		map.tinymap[n]=(struct mypixmap *)calloc(1,sizeof(struct mypixmap));
		alloc_pixmap(map.tinymap[n],MINMAPWID,MINMAPHGT);
	}
	scribble_map(n,map.tinymap[n]);
}

extern void special_command(char *s) {
	int n;
	int d;
	char *ss;
	unsigned char tstr[64];
	struct teleport *t,*tt;
	if (strcmp(s,"VIS")==0) {
		tsend("=VISCLEAR\n=VISMODE1\n");
		strcpy(tstr,"*VISDRAWA....B....C....D....\n");
		str[9]=132;str[10]=132-6;str[11]=132;str[12]=132-20;
		str[14]=132;str[15]=132-26;str[16]=132;str[17]=132-35;
		str[19]=132-10;str[20]=132-23;str[21]=132-3;str[22]=132-23;
		STR[24]=132+10;STR[25]=132-23;STR[26]=132+3;STR[27]=132-23;

	}(    IF (STRNCMP(S,"TLEV",4)==0) {
		N=ATOI(&S[4]);
		FOR (T=FIRSTTEL;T;T=T->NEXT)
			IF (T->LOC.L==N) {
				TT=FIND_TELEPORT(T->DEST);
				IF (TT) {
					SPRINTF(OTXT,
							"TELEPORT %D %D:%D,%D DESTINATION %D %D:%D,%D",
							T->NUM,T->LOC.L,(INT)T->LOC.X/128,
							(INT)T->LOC.Y/128,TT->NUM,TT->LOC.L,
							(INT)TT->LOC.X/128,(INT)TT->LOC.Y/128);
				} ELSE {
					SPRINTF(OTXT,
							"TELEPORT %D %D:%D,%D DESTINATION %D",
							T->NUM,T->LOC.L,(INT)T->LOC.X/128,
							(INT)T->LOC.Y/128,T->DEST);
				}
				PRINT_TEXT(OTXT);
			}
		RETURN 3;
	}
	IF (STRNCMP(S,"TDEST",5)==0) {
		N=ATOI(&S[5]);
		FOR (T=FIRSTTEL;T;T=T->NEXT) {
			TT=FIND_TELEPORT(T->DEST);
			IF ((TT)&&(TT->LOC.L==N)) {
				SPRINTF(OTXT,
						"TELEPORT %D %D:%D,%D DESTINATION %D %D:%D,%D",
						T->NUM,T->LOC.L,(INT)T->LOC.X/128,
						(INT)T->LOC.Y/128,TT->NUM,TT->LOC.L,
						(INT)TT->LOC.X/128,(INT)TT->LOC.Y/128);
				PRINT_TEXT(OTXT);
			}
		}
		RETURN 3;
	}
	IF (STRNCMP(S,"TLIST",5)==0) {
		FOR (T=FIRSTTEL;T;T=T->NEXT)
			FPRINTF(STDERR,"TELEPORT %D  %D:%D,%D  -> %D   <%S>\N",T->NUM,
					T->LOC.L,(INT)T->LOC.X/128,(INT)T->LOC.Y/128,T->DEST,
					T->PASS);
		RETURN;
	}
	IF (STRNCMP(S,"TSET",4)==0) {
		IF (*GUESSCOM) {
			FPRINTF(STDERR,"GUESSER BUSY...\N");
			RETURN;
		}
		SS=STRCHR(&S[4],' ');
		IF (!SS) {
			FPRINTF(STDERR,"NO DESTINATION!...\N");
			RETURN;
		}
		*SS++=0;
		N=ATOI(&S[4]);
		T=FIND_TELEPORT(N);
		IF (!T) {
			FPRINTF(STDERR,"UNKNOWN TELEPORT...\N");
			RETURN;
		}
		GUESSVAL=N;
		D=ATOI(SS);	tt=find_teleport(d);
		if (!tt) {
			fprintf(stderr,"Unknown destination teleport...\n");
			return;
		}
		strcpy(guesscom,"TEL SET");
		sprintf(preguess,"*TELSET%d ",n);
		sprintf(postguess," %d",d);
		guesslen=4;
		for(n=0;n<guesslen;n++) guessed[n]='-';
		guessing[guesslen]=0;
		guessed[guesslen]=0;
		if (*t->pass) {
			strcpy(guessing,t->pass);
			guessnum=-1;
			guesspos=guesslen-1;
		} else {
			for(n=0;n<guesslen;n++) guessing[n]='0';
			guessnum=0;
			guesspos=-1;
		}
		guesscount=0;
		guesstyp=GUESS_TELDEST;
		guessvalb=d;
		guesspass=t->pass;
		tsend(sprintf(str,"%s%s%s\n",preguess,guessing,postguess));
		return 3;
	}
	if (strncmp(s,"LGET",4)==0) {
		n=atoi(&s[4]);
		summon_lift(n);
		return;
	}
	fprintf(stderr,"Unknown special command %s\n",s);
}

extern void process_guess(char *s) {
	int n;
	struct teleport *t;
	struct lift *l;
	if (strcmp(s,"PASS WRONG")==0) {
		if (guesspos==-1) {
			guesspos=0;
			for(n=1;n<guesslen;n++) guessing[n]='-';
			guessing[guesspos]='0'+guessnum;
			tsend(sprintf(str,"%s%s%s\n",preguess,guessing,postguess));
			return;
		} else {
			guessed[guesspos]='0'+guessnum;
			if ((++guesscount)==guesslen) {
				guessnum=-1;
				strcpy(guessing,guessed);
				tsend(sprintf(str,"%s%s%s\n",preguess,guessed,postguess));
				return;
			}
			guesspos++;
			if (guesspos==guesslen) {
				guesspos=-1;
				guessnum++;
				if (guessnum==10) {
					guessnum=-1;
					strcpy(guessing,guessed);
					return;
				}
				for(n=0;n<guesslen;n++) guessing[n]='0'+guessnum;
				tsend(sprintf(str,"%s%s%s\n",preguess,guessing,postguess));
				return;
			} else {
				for(n=0;n<guesslen;n++) guessing[n]='-';
				guessing[guesspos]='0'+guessnum;
				tsend(sprintf(str,"%s%s%s\n",preguess,guessing,postguess));
				return;
			}
		}
		return;
	}
	if (strcmp(s,"PASS INCORRECT")==0) {
		if (guesspos!=-1) {
			guesspos++;
			if (guesspos==guesslen) {
				guesspos=-1;
			} else {
				for(n=0;n<guesslen;n++) guessing[n]='-';
				guessing[guesspos]='0'+guessnum;
				tsend(sprintf(str,"%s%s%s\n",preguess,guessing,postguess));
				return;
			}
		}
		guessnum++;
		if (guessnum==10) {
			guessnum=-1;
			strcpy(guessing,guessed);
			tsend(sprintf(str,"%s%s%s\n",preguess,guessed,postguess));
			return;
		}
		for(n=0;n<guesslen;n++) guessing[n]='0'+guessnum;
		tsend(sprintf(str,"%s%s%s\n",preguess,guessing,postguess));
		return;
	}
	fprintf(stderr,"%s <%s> %s\n",guesscom,guessing,s);
	if (guesspass)
		strcpy(guesspass,guessing);
	switch(guesstyp) {
	case GUESS_TELDEST:
		t=find_teleport(guessval);
		t->dest=guessvalb;
		change=-1;
		break;
	case GUESS_LIFMOVE:
		l=find_lift(guessval);
		l->targ=guessvalb;
		change=-1;
		break;
	case GUESS_TELACTI:
		break;
	default:
		break;
	}
	guesstyp=0;
	guesscom[0]=0;
}

extern void summon_lift(int n,int lev) {
	struct lift *l;
	if (*guesscom) {
		fprintf(stderr,"Guesser busy...\n");
		return;
	}
	l=find_lift(n);
	if (!l) {
		fprintf(stderr,"Unknown lift...\n");
		return;
	}
	guessval=n;
	strcpy(guesscom,"LIF MOVE");
	sprintf(preguess,"*LIFMOVE%d ",n);
	sprintf(postguess," %d",lev);
	guesslen=6;
	for(n=0;n<guesslen;n++) guessed[n]='-';
	guessing[guesslen]=0;
	guessed[guesslen]=0;
	if (*l->pass) {
		strcpy(guessing,l->pass);
		guessnum=-1;
		guesspos=guesslen-1;
	} else {
		for(n=0;n<guesslen;n++) guessing[n]='0';
		guessnum=0;
		guesspos=-1;
	}
	guesscount=0;
	guesstyp=GUESS_LIFMOVE;
	guessvalb=me.l;
	guesspass=l->pass;
	tsend(sprintf(str,"%s%s%s\n",preguess,guessing,postguess));
}

extern void activate_teleport(struct teleport *t) {
	int n;
	if (*guesscom) {
		fprintf(stderr,"Guesser busy...\n");
		return;
	}
	if (!t) {
		fprintf(stderr,"Unknown teleport...\n");
		return;
	}
	strcpy(guesscom,"TEL ACTIVATE");
	sprintf(preguess,"*TELACTIVATE%d ",t->num);
	sprintf(postguess," ");
	guesslen=4;
	for(n=0;n<guesslen;n++) guessed[n]='-';
	guessing[guesslen]=0;
	guessed[guesslen]=0;
	if (*t->pass) {
		strcpy(guessing,t->pass);
		guessnum=-1;
		guesspos=guesslen-1;
	} else {
		for(n=0;n<guesslen;n++) guessing[n]='0';
		guessnum=0;
		guesspos=-1;
	}
	guesscount=0;
	guesstyp=GUESS_TELACTI;
	guesspass=t->pass;
	tsend(sprintf(str,"%s%s%s\n",preguess,guessing,postguess));
	return 3;
}

