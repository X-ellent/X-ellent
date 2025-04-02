# $Source: /dcs/90/cheesey/etc/src/X11/eliteism/RCS/Makefile,v $
# $Author: cheesey $
# $Date: 1992/10/15 12:29:25 $
# $Revision: 1.7 $
# $State: Exp $
#
# All source code Written by Daniel Stephens (cheesey@dcs.warwick.ac.uk)
# anyone wishing to use any of this code may do so, provided that they include
# some acknowledgement of the source (ie me!) 8-)

MAKE=		make
CC=		gcc
SHELL=		/bin/sh

LIBDIR=		\"/home/daniel/etc/src/X11/xellent/\"

INCLUDE=	
LIBS=		-lX11 -lm -lc
#DEFINES=	-DTEST -DEBUG -DLIB=$(LIBDIR) -DNON_ANSI_HEADERS
DEFINES=	-DTEST -DEBUG -DLIB=$(LIBDIR) 
OTHER=		-g -Wall -O2 -fno-common -fcommon

# Clean up dependency tracking
.PHONY: clean all again

COMP=		$(CC) $(OTHER) $(DEFINES) $(INCLUDE)

RUNSRC=		init.c main.c mymath.c debug.c xsetup.c draw.c player.c \
		object.c map.c events.c particle.c message.c mines.c shop.c \
		telnet.c terminal.c turret.c bonus.c home.c lift.c addon.c \
		newton.c targetter.c beam.c starburst.c globals.c
CLISRC=         client.c
FRONTSRC=	frontend.c
SRC=   		$(RUNSRC) $(CLISRC) $(FRONTSRC)

DATA=		player.data price.data term.asm terminal.opcodes testmap

HEADERS=	bonus.h events.h map.h object.h telnet.h xsetup.h constants.h \
		fix.h message.h particle.h terminal.h debug.h home.h mines.h \
		player.h turret.h draw.h init.h mymath.h shop.h xbits.h \
		lift.h

RUNOBJ=		$(RUNSRC:%.c=%.o)
CLIOBJ=		$(CLISRC:%.c=%.o)
FRONTOBJ=	$(FRONTSRC:%.c=%.o)
OBJ=            $(SRC:%.c=%.o)

all:	runme myclient frontend

again:	clean all

runme:	terminal.temp $(RUNOBJ)
	$(COMP) -o runme $(RUNOBJ) $(LIBS)

myclient:	$(CLIOBJ)
	$(COMP) -o myclient $(CLIOBJ) $(LIBS)

frontend:	$(FRONTOBJ)
	$(COMP) -o frontend $(FRONTOBJ) $(LIBS)
	chmod o-rx frontend
	chmod g+x  frontend

terminal.temp:	terminal.h
	sed -n -e '1,/\*\*START\*\*/p' terminal.c > terminal.temp
	sed -n -e 's/#define.*OP_\([A-Z][A-Z]*\).*/	if (strcmp(str,"\1")==0) {rom[pc++]=OP_\1;return pc;};/p' terminal.h >> terminal.temp
	sed -n -e '/\*\*END\*\*/,$$p' terminal.c >> terminal.temp
	cp -f terminal.temp terminal.c

%.o:	%.c
	$(COMP) -c $<

install:	myclient frontend
	-strip myclient
	-strip frontend
	cp -f myclient /dcs/90/cheesey/bin/xellent
	chmod a+x  /dcs/90/cheesey/bin/xellent
	chmod og-r /dcs/90/cheesey/bin/xellent
	chmod g+s  /dcs/90/cheesey/bin/xellent
	cp -f frontend /dcs/90/cheesey/bin/frontend
	
clean:
	-rm -f $(OBJ) runme myclient frontend terminal.temp

# Remove old dependency information
