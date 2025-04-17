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
CC=			gcc
SHELL=		/bin/sh

# Define our suffixes
.SUFFIXES: .c .o

LIBDIR=		../data
INCLUDE=

IS_CYGWIN := $(shell uname -s | grep -i cygwin)

# Set LIBS based on platform
ifeq ($(IS_CYGWIN),)
	# Linux/Unix systems
	LIBS=	-lX11 -lm -lc -lpthread
else
	LIBS=	-static -lm -lc -lpthread -l:libcygwin.a -l:libX11.dll.a
endif

# Base CFLAGS with all necessary flags
CFLAGS=     -g -Wall -O2 -MMD
#CFLAGS=	-g -Wall -O2 -fno-common -fstack-protector-all -Wextra -MMD
#CFLAGS=	-g -Wall -O0 -fno-omit-frame-pointer -fstack-protector-all -Wextra -Wconversion -MMD
DEFINES=	-DLIB=\"$(LIBDIR)\" -DEBUG -DSHOW_THIRD_LEVEL

COMP=		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDE)

RUNSRC=		main.c mymath.c debug.c xsetup.c draw.c player.c \
			object.c map.c events.c particle.c message.c mines.c shop.c \
			telnet.c terminal.c turret.c bonus.c home.c lift.c addon.c \
			newton.c targetter.c beam.c starburst.c globals.c
CLISRC=		client.c
FRONTSRC=	frontend.c
SRC=		$(RUNSRC) $(CLISRC) $(FRONTSRC)

DATA=		player.data price.data term.asm terminal.opcodes testmap

HEADERS=	bonus.h events.h map.h object.h telnet.h xsetup.h constants.h \
			fix.h message.h particle.h terminal.h debug.h home.h mines.h \
			player.h turret.h draw.h init.h mymath.h shop.h xbits.h \
			lift.h

RUNOBJ=		$(RUNSRC:%.c=%.o)
CLIOBJ=		$(CLISRC:%.c=%.o)
FRONTOBJ=	$(FRONTSRC:%.c=%.o)
OBJ=		$(SRC:%.c=%.o)

# Clean up dependency tracking
.PHONY: clean all again

all:	runme myclient frontend

again:	clean all

runme:	$(RUNOBJ)
	$(COMP) -o runme $(RUNOBJ) $(LIBS)

myclient:	$(CLIOBJ)
	$(COMP) -o xellent $(CLIOBJ) $(LIBS)

frontend:	$(FRONTOBJ)
	$(COMP) -o frontend $(FRONTOBJ) $(LIBS)
	chmod o-rx frontend
	chmod g+x  frontend

# Suffix rules for compilation
.c.o:
	$(COMP) -c $< -o $@

install:	myclient frontend
	-strip xellent
	-strip frontend
	cp -f xellent /dcs/90/cheesey/bin/
	chmod a+x  /dcs/90/cheesey/bin/xellent
	chmod og-r /dcs/90/cheesey/bin/xellent
	chmod g+s  /dcs/90/cheesey/bin/xellent
	cp -f frontend /dcs/90/cheesey/bin/frontend

clean:
	-rm -f $(OBJ) runme myclient frontend *.d

# Include auto-generated dependencies
-include $(OBJ:.o=.d)
