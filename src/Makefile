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

LIBDIR=         \"../data\" # TODO

INCLUDE=	
LIBS=		-L/usr/X11R6/lib -lX11 -lXau -lm -lc -lpthread
DEFINES=	-DLIB=$(LIBDIR) -DEBUG
CFLAGS=		-ggdb -Wall -O0 -fno-omit-frame-pointer -fstack-protector-all -Wno-incompatible-pointer-types

COMP=		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDE)

RUNSRC=		init.c main.c mymath.c debug.c xsetup.c draw.c player.c \
		object.c map.c events.c particle.c message.c mines.c shop.c \
		telnet.c terminal.c turret.c bonus.c home.c lift.c addon.c \
		newton.c targetter.c beam.c starburst.c
CLISRC=         client.c
FRONTSRC=	frontend.c
SRC=   		$(RUNSRC) $(CLISRC) $(FRONTSRC)

RUNOBJ=		$(RUNSRC:%.c=%.o)
CLIOBJ=		$(CLISRC:%.c=%.o)
FRONTOBJ=	$(FRONTSRC:%.c=%.o)
OBJ=            $(SRC:%.c=%.o)

all:	runme myclient frontend

again:	clean all

#runme:	terminal.temp $(RUNOBJ)
runme:	$(RUNOBJ)
	$(COMP) -o runme $(RUNOBJ) $(LIBS)

myclient:	$(CLIOBJ)
	$(COMP) -o myclient $(CLIOBJ) $(LIBS)

frontend:	$(FRONTOBJ)
	$(COMP) -o frontend $(FRONTOBJ) $(LIBS)
	chmod o-rx frontend
	chmod g+x  frontend

terminal.temp:	terminal.h
	sed -n -e '1,/\*\*START\*\*/p' terminal.c > terminal.temp
	sed -n -e 's/#define.*OP_\([A-Z][A-Z]*\).*/    if (strcmp(str,"\1")==0) {rom[pc++]=OP_\1;return pc;};/p' terminal.h >> terminal.temp
	sed -n -e '/\*\*END\*\*/,$$p' terminal.c >> terminal.temp
	mv -f terminal.temp terminal.c

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
	-rm -f $(OBJ)

depend:	$(SRC)
	makedepend $(INCLUDE) $(SRC)

# DO NOT DELETE THIS LINE -- make depend depends on it.

init.o: init.h mymath.h debug.h
main.o: fix.h message.h player.h constants.h xbits.h fonts.h object.h
main.o: newton.h map.h terminal.h damages.h debug.h particle.h init.h
main.o: telnet.h xsetup.h shop.h turret.h bonus.h draw.h lift.h addon.h
mymath.o: mymath.h
debug.o: fix.h
xsetup.o: fix.h xbits.h fonts.h player.h constants.h object.h newton.h map.h
xsetup.o: terminal.h damages.h debug.h telnet.h xsetup.h message.h
draw.o: fix.h player.h constants.h xbits.h fonts.h object.h newton.h map.h
draw.o: terminal.h damages.h turret.h mymath.h debug.h particle.h xsetup.h
draw.o: draw.h lift.h beam.h addon.h
player.o: fix.h constants.h mymath.h player.h xbits.h fonts.h object.h
player.o: newton.h map.h terminal.h damages.h debug.h particle.h telnet.h
player.o: message.h turret.h home.h events.h xsetup.h beam.h addon.h
object.o: constants.h object.h newton.h player.h xbits.h fonts.h map.h
object.o: terminal.h damages.h message.h bonus.h mines.h fix.h
map.o: fix.h map.h player.h constants.h xbits.h fonts.h object.h newton.h
map.o: terminal.h damages.h debug.h home.h lift.h turret.h starburst.h
map.o: message.h
events.o: player.h constants.h xbits.h fonts.h object.h newton.h map.h
events.o: terminal.h damages.h debug.h home.h shop.h mines.h message.h lift.h
events.o: addon.h
particle.o: particle.h player.h constants.h xbits.h fonts.h object.h newton.h
particle.o: map.h terminal.h damages.h turret.h mymath.h debug.h
message.o: message.h player.h constants.h xbits.h fonts.h object.h newton.h
message.o: map.h terminal.h damages.h
mines.o: player.h constants.h xbits.h fonts.h object.h newton.h map.h
mines.o: terminal.h damages.h mymath.h bonus.h particle.h
shop.o: fix.h player.h constants.h xbits.h fonts.h object.h newton.h map.h
shop.o: terminal.h damages.h shop.h addon.h
telnet.o: constants.h player.h xbits.h fonts.h object.h newton.h map.h
telnet.o: terminal.h damages.h fix.h
terminal.o: fix.h player.h constants.h xbits.h fonts.h object.h newton.h
terminal.o: map.h terminal.h damages.h home.h draw.h lift.h shop.h debug.h
terminal.o: addon.h message.h
turret.o: fix.h constants.h turret.h player.h xbits.h fonts.h object.h
turret.o: newton.h map.h terminal.h damages.h mymath.h message.h particle.h
bonus.o: object.h newton.h player.h constants.h xbits.h fonts.h map.h
bonus.o: terminal.h damages.h bonus.h message.h particle.h fix.h
home.o: player.h constants.h xbits.h fonts.h object.h newton.h map.h
home.o: terminal.h damages.h shop.h fix.h home.h draw.h xsetup.h message.h
lift.o: lift.h map.h turret.h player.h constants.h xbits.h fonts.h object.h
lift.o: newton.h terminal.h damages.h
addon.o: message.h player.h constants.h xbits.h fonts.h object.h newton.h
addon.o: map.h terminal.h damages.h addon.h
newton.o: fix.h newton.h mymath.h damages.h player.h constants.h xbits.h
newton.o: fonts.h object.h map.h terminal.h turret.h
targetter.o: addon.h constants.h player.h xbits.h fonts.h object.h newton.h
targetter.o: map.h terminal.h damages.h targetter.h
beam.o: beam.h player.h constants.h xbits.h fonts.h object.h newton.h map.h
beam.o: terminal.h damages.h mymath.h fix.h turret.h
starburst.o: starburst.h newton.h beam.h player.h constants.h xbits.h fonts.h
starburst.o: object.h map.h terminal.h damages.h mymath.h
client.o: fix.h
frontend.o: fix.h fonts.h
