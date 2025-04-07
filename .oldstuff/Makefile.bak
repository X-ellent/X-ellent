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
LIBS=		-L/usr/X11R6/lib -lX11 -lm -lc
#DEFINES=	-DTEST -DEBUG -DLIB=$(LIBDIR) -DNON_ANSI_HEADERS
DEFINES=	-DTEST -DEBUG -DLIB=$(LIBDIR) 
OTHER=		-ggdb -Wall -O2 

COMP=		$(CC) $(OTHER) $(DEFINES) $(INCLUDE)

RUNSRC=		init.c main.c mymath.c debug.c xsetup.c draw.c player.c \
		object.c map.c events.c particle.c message.c mines.c shop.c \
		telnet.c terminal.c turret.c bonus.c home.c lift.c addon.c \
		newton.c targetter.c beam.c starburst.c
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
	chgrp hmmm frontend

terminal.temp:	terminal.h
	sed -n -e '1,/**START**/p' terminal.c > terminal.temp
	sed -n -e 's/#define.*OP_\([A-Z][A-Z]*\).*/    if (strcmp(str,"\1")==0) {rom[pc++]=OP_\1;return pc;};/p' terminal.h >> terminal.temp
	sed -n -e '/**END**/,$$p' terminal.c >> terminal.temp
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
	-rm -f $(OBJ)

depend:	$(SRC)
	makedepend $(INCLUDE) $(SRC)

ci:	
	#Please enter log message:
	LOGMSG=`/bin/line`;ci -f -l -m/'$$LOGMSG/' $(SRC) $(HEADERS) \
	Makefile $(DATA)

# DO NOT DELETE THIS LINE -- make depend depends on it.

init.o: /usr/include/math.h /usr/include/features.h /usr/include/sys/cdefs.h
init.o: /usr/include/huge_val.h /usr/include/endian.h /usr/include/bytesex.h
init.o: /usr/include/nan.h /usr/lib/gcc-lib/i486-linux/2.7.2/include/float.h
init.o: /usr/include/values.h /usr/include/ieee754.h /usr/include/ieee854.h
init.o: init.h mymath.h debug.h
main.o: /usr/include/signal.h /usr/include/features.h
main.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
main.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
main.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
main.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
main.o: /usr/include/asm/signal.h /usr/include/unistd.h
main.o: /usr/include/posix_opt.h /usr/include/gnu/types.h
main.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
main.o: /usr/include/confname.h /usr/include/stdio.h /usr/include/libio.h
main.o: /usr/include/_G_config.h /usr/include/stdlib.h /usr/include/errno.h
main.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
main.o: /usr/include/alloca.h fix.h message.h player.h constants.h xbits.h
main.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
main.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
main.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h map.h terminal.h
main.o: damages.h debug.h particle.h init.h telnet.h xsetup.h
main.o: /usr/include/setjmp.h /usr/include/jmp_buf.h shop.h turret.h bonus.h
main.o: draw.h lift.h addon.h
mymath.o: mymath.h /usr/include/math.h /usr/include/features.h
mymath.o: /usr/include/sys/cdefs.h /usr/include/huge_val.h
mymath.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/nan.h
mymath.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/float.h
mymath.o: /usr/include/values.h /usr/include/ieee754.h /usr/include/ieee854.h
debug.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
debug.o: fix.h /usr/include/stdlib.h /usr/include/features.h
debug.o: /usr/include/sys/cdefs.h
debug.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
debug.o: /usr/include/errno.h /usr/include/linux/errno.h
debug.o: /usr/include/asm/errno.h /usr/include/alloca.h
xsetup.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
xsetup.o: /usr/include/string.h /usr/include/features.h
xsetup.o: /usr/include/sys/cdefs.h
xsetup.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
xsetup.o: /usr/include/setjmp.h /usr/include/jmp_buf.h /usr/include/signal.h
xsetup.o: /usr/include/sys/types.h /usr/include/linux/types.h
xsetup.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
xsetup.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
xsetup.o: /usr/include/linux/signal.h /usr/include/asm/signal.h fix.h
xsetup.o: /usr/include/stdlib.h /usr/include/errno.h
xsetup.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
xsetup.o: /usr/include/alloca.h xbits.h /usr/include/X11/Xlib.h
xsetup.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
xsetup.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h fonts.h
xsetup.o: player.h constants.h object.h newton.h map.h terminal.h damages.h
xsetup.o: debug.h telnet.h xsetup.h message.h
draw.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
draw.o: /usr/include/string.h /usr/include/features.h
draw.o: /usr/include/sys/cdefs.h
draw.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
draw.o: /usr/include/setjmp.h /usr/include/jmp_buf.h /usr/include/signal.h
draw.o: /usr/include/sys/types.h /usr/include/linux/types.h
draw.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
draw.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
draw.o: /usr/include/linux/signal.h /usr/include/asm/signal.h fix.h
draw.o: /usr/include/stdlib.h /usr/include/errno.h /usr/include/linux/errno.h
draw.o: /usr/include/asm/errno.h /usr/include/alloca.h player.h constants.h
draw.o: xbits.h /usr/include/X11/Xlib.h /usr/include/X11/X.h
draw.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
draw.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h map.h terminal.h
draw.o: damages.h turret.h mymath.h /usr/include/math.h
draw.o: /usr/include/huge_val.h /usr/include/endian.h /usr/include/bytesex.h
draw.o: /usr/include/nan.h /usr/lib/gcc-lib/i486-linux/2.7.2/include/float.h
draw.o: /usr/include/values.h /usr/include/ieee754.h /usr/include/ieee854.h
draw.o: debug.h particle.h xsetup.h draw.h lift.h beam.h addon.h
player.o: /usr/include/math.h /usr/include/features.h
player.o: /usr/include/sys/cdefs.h /usr/include/huge_val.h
player.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/nan.h
player.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/float.h
player.o: /usr/include/values.h /usr/include/ieee754.h /usr/include/ieee854.h
player.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
player.o: /usr/include/malloc.h
player.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
player.o: /usr/include/string.h /usr/include/setjmp.h /usr/include/jmp_buf.h
player.o: /usr/include/signal.h /usr/include/sys/types.h
player.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
player.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
player.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
player.o: /usr/include/asm/signal.h fix.h /usr/include/stdlib.h
player.o: /usr/include/errno.h /usr/include/linux/errno.h
player.o: /usr/include/asm/errno.h /usr/include/alloca.h constants.h mymath.h
player.o: player.h xbits.h /usr/include/X11/Xlib.h /usr/include/X11/X.h
player.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
player.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h map.h terminal.h
player.o: damages.h debug.h particle.h telnet.h message.h turret.h home.h
player.o: events.h xsetup.h beam.h addon.h
object.o: /usr/include/malloc.h /usr/include/features.h
object.o: /usr/include/sys/cdefs.h
object.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
object.o: /usr/include/math.h /usr/include/huge_val.h /usr/include/endian.h
object.o: /usr/include/bytesex.h /usr/include/nan.h
object.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/float.h
object.o: /usr/include/values.h /usr/include/ieee754.h /usr/include/ieee854.h
object.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
object.o: /usr/include/string.h constants.h object.h newton.h player.h
object.o: xbits.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
object.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
object.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
object.o: /usr/include/sys/bitypes.h /usr/include/X11/X.h
object.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
object.o: /usr/include/X11/Xutil.h fonts.h map.h terminal.h damages.h
object.o: message.h bonus.h mines.h fix.h /usr/include/stdlib.h
object.o: /usr/include/errno.h /usr/include/linux/errno.h
object.o: /usr/include/asm/errno.h /usr/include/alloca.h
map.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
map.o: /usr/include/string.h /usr/include/features.h /usr/include/sys/cdefs.h
map.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
map.o: /usr/include/unistd.h /usr/include/posix_opt.h
map.o: /usr/include/gnu/types.h /usr/include/confname.h
map.o: /usr/include/sys/types.h /usr/include/linux/types.h
map.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
map.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
map.o: /usr/include/stdlib.h /usr/include/errno.h /usr/include/linux/errno.h
map.o: /usr/include/asm/errno.h /usr/include/alloca.h /usr/include/malloc.h
map.o: fix.h map.h player.h constants.h xbits.h /usr/include/X11/Xlib.h
map.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
map.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h fonts.h object.h
map.o: newton.h terminal.h damages.h debug.h home.h lift.h turret.h
map.o: starburst.h message.h
events.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
events.o: /usr/include/X11/keysym.h /usr/include/X11/keysymdef.h player.h
events.o: constants.h xbits.h /usr/include/X11/Xlib.h
events.o: /usr/include/sys/types.h /usr/include/linux/types.h
events.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
events.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
events.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
events.o: /usr/include/X11/Xosdefs.h
events.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
events.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h map.h terminal.h
events.o: damages.h debug.h home.h shop.h mines.h message.h lift.h addon.h
particle.o: /usr/include/malloc.h /usr/include/features.h
particle.o: /usr/include/sys/cdefs.h
particle.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h particle.h
particle.o: player.h constants.h xbits.h /usr/include/X11/Xlib.h
particle.o: /usr/include/sys/types.h /usr/include/linux/types.h
particle.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
particle.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
particle.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
particle.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h fonts.h
particle.o: object.h newton.h map.h terminal.h damages.h turret.h mymath.h
particle.o: /usr/include/math.h /usr/include/huge_val.h /usr/include/endian.h
particle.o: /usr/include/bytesex.h /usr/include/nan.h
particle.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/float.h
particle.o: /usr/include/values.h /usr/include/ieee754.h
particle.o: /usr/include/ieee854.h debug.h
message.o: /usr/include/string.h /usr/include/features.h
message.o: /usr/include/sys/cdefs.h
message.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h message.h
message.o: player.h constants.h xbits.h /usr/include/X11/Xlib.h
message.o: /usr/include/sys/types.h /usr/include/linux/types.h
message.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
message.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
message.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
message.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h fonts.h
message.o: object.h newton.h map.h terminal.h damages.h
mines.o: player.h constants.h xbits.h /usr/include/X11/Xlib.h
mines.o: /usr/include/sys/types.h /usr/include/linux/types.h
mines.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
mines.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
mines.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
mines.o: /usr/include/X11/Xosdefs.h
mines.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
mines.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h map.h terminal.h
mines.o: damages.h mymath.h /usr/include/math.h /usr/include/features.h
mines.o: /usr/include/sys/cdefs.h /usr/include/huge_val.h
mines.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/nan.h
mines.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/float.h
mines.o: /usr/include/values.h /usr/include/ieee754.h /usr/include/ieee854.h
mines.o: bonus.h particle.h
shop.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
shop.o: /usr/include/string.h /usr/include/features.h
shop.o: /usr/include/sys/cdefs.h
shop.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
shop.o: /usr/include/stdlib.h /usr/include/errno.h /usr/include/linux/errno.h
shop.o: /usr/include/asm/errno.h /usr/include/alloca.h fix.h player.h
shop.o: constants.h xbits.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
shop.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
shop.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
shop.o: /usr/include/sys/bitypes.h /usr/include/X11/X.h
shop.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
shop.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h map.h terminal.h
shop.o: damages.h shop.h addon.h
telnet.o: /usr/include/sys/types.h /usr/include/linux/types.h
telnet.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
telnet.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
telnet.o: /usr/include/sys/time.h /usr/include/features.h
telnet.o: /usr/include/sys/cdefs.h /usr/include/linux/time.h
telnet.o: /usr/include/time.h /usr/include/sys/socket.h
telnet.o: /usr/include/linux/socket.h /usr/include/asm/socket.h
telnet.o: /usr/include/asm/sockios.h /usr/include/linux/sockios.h
telnet.o: /usr/include/linux/uio.h /usr/include/netinet/in.h
telnet.o: /usr/include/linux/in.h /usr/include/asm/byteorder.h
telnet.o: /usr/include/sys/ioctl.h /usr/include/linux/ioctl.h
telnet.o: /usr/include/asm/ioctl.h /usr/include/termios.h
telnet.o: /usr/include/linux/termios.h /usr/include/asm/termios.h
telnet.o: /usr/include/asm/termbits.h /usr/include/asm/ioctls.h
telnet.o: /usr/include/sys/socketio.h /usr/include/string.h
telnet.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
telnet.o: /usr/include/errno.h /usr/include/linux/errno.h
telnet.o: /usr/include/asm/errno.h /usr/include/stdio.h /usr/include/libio.h
telnet.o: /usr/include/_G_config.h /usr/include/stdlib.h
telnet.o: /usr/include/alloca.h /usr/include/ctype.h /usr/include/endian.h
telnet.o: /usr/include/bytesex.h constants.h player.h xbits.h
telnet.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
telnet.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
telnet.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h map.h terminal.h
telnet.o: damages.h fix.h
terminal.o: /usr/include/stdio.h /usr/include/libio.h
terminal.o: /usr/include/_G_config.h /usr/include/ctype.h
terminal.o: /usr/include/features.h /usr/include/sys/cdefs.h
terminal.o: /usr/include/endian.h /usr/include/bytesex.h
terminal.o: /usr/include/malloc.h
terminal.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
terminal.o: /usr/include/string.h /usr/include/unistd.h
terminal.o: /usr/include/posix_opt.h /usr/include/gnu/types.h
terminal.o: /usr/include/confname.h /usr/include/sys/types.h
terminal.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
terminal.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
terminal.o: /usr/include/sys/bitypes.h /usr/include/stdlib.h
terminal.o: /usr/include/errno.h /usr/include/linux/errno.h
terminal.o: /usr/include/asm/errno.h /usr/include/alloca.h fix.h player.h
terminal.o: constants.h xbits.h /usr/include/X11/Xlib.h /usr/include/X11/X.h
terminal.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
terminal.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h map.h
terminal.o: terminal.h damages.h home.h draw.h lift.h shop.h debug.h addon.h
terminal.o: message.h
turret.o: /usr/include/math.h /usr/include/features.h
turret.o: /usr/include/sys/cdefs.h /usr/include/huge_val.h
turret.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/nan.h
turret.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/float.h
turret.o: /usr/include/values.h /usr/include/ieee754.h /usr/include/ieee854.h
turret.o: /usr/include/malloc.h
turret.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
turret.o: /usr/include/string.h /usr/include/stdio.h /usr/include/libio.h
turret.o: /usr/include/_G_config.h fix.h /usr/include/stdlib.h
turret.o: /usr/include/errno.h /usr/include/linux/errno.h
turret.o: /usr/include/asm/errno.h /usr/include/alloca.h constants.h turret.h
turret.o: player.h xbits.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
turret.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
turret.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
turret.o: /usr/include/sys/bitypes.h /usr/include/X11/X.h
turret.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
turret.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h map.h terminal.h
turret.o: damages.h mymath.h message.h particle.h
bonus.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
bonus.o: object.h newton.h player.h constants.h xbits.h
bonus.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
bonus.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
bonus.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
bonus.o: /usr/include/sys/bitypes.h /usr/include/X11/X.h
bonus.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
bonus.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
bonus.o: /usr/include/X11/Xutil.h fonts.h map.h terminal.h damages.h bonus.h
bonus.o: message.h particle.h fix.h /usr/include/stdlib.h
bonus.o: /usr/include/features.h /usr/include/sys/cdefs.h
bonus.o: /usr/include/errno.h /usr/include/linux/errno.h
bonus.o: /usr/include/asm/errno.h /usr/include/alloca.h
home.o: /usr/include/string.h /usr/include/features.h
home.o: /usr/include/sys/cdefs.h
home.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
home.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
home.o: /usr/include/malloc.h player.h constants.h xbits.h
home.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
home.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
home.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
home.o: /usr/include/sys/bitypes.h /usr/include/X11/X.h
home.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
home.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h map.h terminal.h
home.o: damages.h shop.h fix.h /usr/include/stdlib.h /usr/include/errno.h
home.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
home.o: /usr/include/alloca.h home.h draw.h xsetup.h /usr/include/setjmp.h
home.o: /usr/include/jmp_buf.h /usr/include/signal.h
home.o: /usr/include/linux/signal.h /usr/include/asm/signal.h message.h
lift.o: /usr/include/malloc.h /usr/include/features.h
lift.o: /usr/include/sys/cdefs.h
lift.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
lift.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
lift.o: /usr/include/string.h /usr/include/ctype.h /usr/include/endian.h
lift.o: /usr/include/bytesex.h lift.h map.h turret.h player.h constants.h
lift.o: xbits.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
lift.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
lift.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
lift.o: /usr/include/sys/bitypes.h /usr/include/X11/X.h
lift.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
lift.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h terminal.h
lift.o: damages.h
addon.o: /usr/include/malloc.h /usr/include/features.h
addon.o: /usr/include/sys/cdefs.h
addon.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
addon.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
addon.o: /usr/include/string.h message.h player.h constants.h xbits.h
addon.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
addon.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
addon.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
addon.o: /usr/include/sys/bitypes.h /usr/include/X11/X.h
addon.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
addon.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h map.h terminal.h
addon.o: damages.h addon.h
newton.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
newton.o: /usr/include/stdlib.h /usr/include/features.h
newton.o: /usr/include/sys/cdefs.h
newton.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
newton.o: /usr/include/errno.h /usr/include/linux/errno.h
newton.o: /usr/include/asm/errno.h /usr/include/alloca.h /usr/include/math.h
newton.o: /usr/include/huge_val.h /usr/include/endian.h
newton.o: /usr/include/bytesex.h /usr/include/nan.h
newton.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/float.h
newton.o: /usr/include/values.h /usr/include/ieee754.h /usr/include/ieee854.h
newton.o: fix.h newton.h mymath.h damages.h player.h constants.h xbits.h
newton.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
newton.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
newton.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
newton.o: /usr/include/sys/bitypes.h /usr/include/X11/X.h
newton.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
newton.o: /usr/include/X11/Xutil.h fonts.h object.h map.h terminal.h turret.h
targetter.o: /usr/include/string.h /usr/include/features.h
targetter.o: /usr/include/sys/cdefs.h
targetter.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
targetter.o: /usr/include/stdio.h /usr/include/libio.h
targetter.o: /usr/include/_G_config.h addon.h constants.h player.h xbits.h
targetter.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
targetter.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
targetter.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
targetter.o: /usr/include/sys/bitypes.h /usr/include/X11/X.h
targetter.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
targetter.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h map.h
targetter.o: terminal.h damages.h targetter.h
beam.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
beam.o: /usr/include/malloc.h /usr/include/features.h
beam.o: /usr/include/sys/cdefs.h
beam.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h beam.h player.h
beam.o: constants.h xbits.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
beam.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
beam.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
beam.o: /usr/include/sys/bitypes.h /usr/include/X11/X.h
beam.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
beam.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h map.h terminal.h
beam.o: damages.h mymath.h /usr/include/math.h /usr/include/huge_val.h
beam.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/nan.h
beam.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/float.h
beam.o: /usr/include/values.h /usr/include/ieee754.h /usr/include/ieee854.h
beam.o: fix.h /usr/include/stdlib.h /usr/include/errno.h
beam.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
beam.o: /usr/include/alloca.h turret.h
starburst.o: starburst.h newton.h beam.h player.h constants.h xbits.h
starburst.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
starburst.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
starburst.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
starburst.o: /usr/include/sys/bitypes.h /usr/include/X11/X.h
starburst.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
starburst.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
starburst.o: /usr/include/X11/Xutil.h fonts.h object.h map.h terminal.h
starburst.o: damages.h mymath.h /usr/include/math.h /usr/include/features.h
starburst.o: /usr/include/sys/cdefs.h /usr/include/huge_val.h
starburst.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/nan.h
starburst.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/float.h
starburst.o: /usr/include/values.h /usr/include/ieee754.h
starburst.o: /usr/include/ieee854.h
client.o: /usr/include/sys/types.h /usr/include/linux/types.h
client.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
client.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
client.o: /usr/include/sys/socket.h /usr/include/features.h
client.o: /usr/include/sys/cdefs.h /usr/include/linux/socket.h
client.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
client.o: /usr/include/linux/sockios.h /usr/include/linux/uio.h
client.o: /usr/include/netinet/in.h /usr/include/linux/in.h
client.o: /usr/include/asm/byteorder.h /usr/include/netdb.h
client.o: /usr/include/paths.h /usr/include/stdio.h /usr/include/libio.h
client.o: /usr/include/_G_config.h /usr/include/pwd.h
client.o: /usr/include/gnu/types.h /usr/include/X11/Xlib.h
client.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
client.o: /usr/include/X11/Xosdefs.h
client.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
client.o: /usr/include/X11/Xutil.h /usr/include/unistd.h
client.o: /usr/include/posix_opt.h /usr/include/confname.h
client.o: /usr/include/string.h /usr/include/stdlib.h /usr/include/errno.h
client.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
client.o: /usr/include/alloca.h /usr/include/sys/time.h
client.o: /usr/include/linux/time.h /usr/include/time.h fix.h
frontend.o: /usr/include/sys/types.h /usr/include/linux/types.h
frontend.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
frontend.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
frontend.o: /usr/include/sys/socket.h /usr/include/features.h
frontend.o: /usr/include/sys/cdefs.h /usr/include/linux/socket.h
frontend.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
frontend.o: /usr/include/linux/sockios.h /usr/include/linux/uio.h
frontend.o: /usr/include/netinet/in.h /usr/include/linux/in.h
frontend.o: /usr/include/asm/byteorder.h /usr/include/netdb.h
frontend.o: /usr/include/paths.h /usr/include/stdio.h /usr/include/libio.h
frontend.o: /usr/include/_G_config.h /usr/include/pwd.h
frontend.o: /usr/include/gnu/types.h /usr/include/X11/Xlib.h
frontend.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
frontend.o: /usr/include/X11/Xosdefs.h
frontend.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
frontend.o: /usr/include/X11/Xutil.h /usr/include/unistd.h
frontend.o: /usr/include/posix_opt.h /usr/include/confname.h
frontend.o: /usr/include/string.h /usr/include/stdlib.h /usr/include/errno.h
frontend.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
frontend.o: /usr/include/alloca.h /usr/include/sys/time.h
frontend.o: /usr/include/linux/time.h /usr/include/time.h
frontend.o: /usr/include/malloc.h /usr/include/math.h /usr/include/huge_val.h
frontend.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/nan.h
frontend.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/float.h
frontend.o: /usr/include/values.h /usr/include/ieee754.h
frontend.o: /usr/include/ieee854.h /usr/include/X11/keysym.h
frontend.o: /usr/include/X11/keysymdef.h fix.h fonts.h
