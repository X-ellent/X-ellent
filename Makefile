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
init.o: /usr/include/gnu/stubs.h /usr/include/bits/huge_val.h
init.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
init.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/float.h
init.o: init.h mymath.h debug.h
main.o: /usr/include/signal.h /usr/include/features.h
main.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
main.o: /usr/include/bits/sigset.h /usr/include/bits/types.h
main.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
main.o: /usr/include/bits/signum.h /usr/include/time.h
main.o: /usr/include/bits/siginfo.h /usr/include/bits/sigaction.h
main.o: /usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h
main.o: /usr/include/bits/sigstack.h /usr/include/unistd.h
main.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
main.o: /usr/include/getopt.h /usr/include/stdio.h
main.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h
main.o: /usr/include/libio.h /usr/include/_G_config.h
main.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
main.o: /usr/include/sys/types.h /usr/include/endian.h
main.o: /usr/include/bits/endian.h /usr/include/sys/select.h
main.o: /usr/include/bits/select.h /usr/include/sys/sysmacros.h
main.o: /usr/include/alloca.h fix.h message.h player.h constants.h xbits.h
main.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
main.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
main.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h map.h terminal.h
main.o: damages.h debug.h particle.h init.h telnet.h xsetup.h
main.o: /usr/include/setjmp.h /usr/include/bits/setjmp.h shop.h turret.h
main.o: bonus.h draw.h lift.h addon.h
mymath.o: mymath.h /usr/include/math.h /usr/include/features.h
mymath.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
mymath.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
mymath.o: /usr/include/bits/mathcalls.h
mymath.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/float.h
debug.o: /usr/include/stdio.h /usr/include/features.h
debug.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
debug.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
debug.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h
debug.o: /usr/include/bits/types.h /usr/include/libio.h
debug.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h fix.h
debug.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
debug.o: /usr/include/endian.h /usr/include/bits/endian.h
debug.o: /usr/include/sys/select.h /usr/include/bits/select.h
debug.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
debug.o: /usr/include/alloca.h
xsetup.o: /usr/include/stdio.h /usr/include/features.h
xsetup.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
xsetup.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
xsetup.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h
xsetup.o: /usr/include/bits/types.h /usr/include/libio.h
xsetup.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
xsetup.o: /usr/include/string.h /usr/include/setjmp.h
xsetup.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h fix.h
xsetup.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
xsetup.o: /usr/include/endian.h /usr/include/bits/endian.h
xsetup.o: /usr/include/sys/select.h /usr/include/bits/select.h
xsetup.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h xbits.h
xsetup.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
xsetup.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
xsetup.o: /usr/include/X11/Xutil.h fonts.h player.h constants.h object.h
xsetup.o: newton.h map.h terminal.h damages.h debug.h telnet.h xsetup.h
xsetup.o: message.h
draw.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
draw.o: /usr/include/gnu/stubs.h
draw.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
draw.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h
draw.o: /usr/include/bits/types.h /usr/include/libio.h
draw.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
draw.o: /usr/include/string.h /usr/include/setjmp.h
draw.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h fix.h
draw.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
draw.o: /usr/include/endian.h /usr/include/bits/endian.h
draw.o: /usr/include/sys/select.h /usr/include/bits/select.h
draw.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h player.h
draw.o: constants.h xbits.h /usr/include/X11/Xlib.h /usr/include/X11/X.h
draw.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
draw.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h map.h terminal.h
draw.o: damages.h turret.h mymath.h /usr/include/math.h
draw.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
draw.o: /usr/include/bits/mathcalls.h
draw.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/float.h
draw.o: debug.h particle.h xsetup.h draw.h lift.h beam.h addon.h
player.o: /usr/include/math.h /usr/include/features.h
player.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
player.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
player.o: /usr/include/bits/mathcalls.h
player.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/float.h
player.o: /usr/include/stdio.h
player.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
player.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h
player.o: /usr/include/bits/types.h /usr/include/libio.h
player.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
player.o: /usr/include/malloc.h /usr/include/string.h /usr/include/setjmp.h
player.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h fix.h
player.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
player.o: /usr/include/endian.h /usr/include/bits/endian.h
player.o: /usr/include/sys/select.h /usr/include/bits/select.h
player.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h constants.h
player.o: mymath.h player.h xbits.h /usr/include/X11/Xlib.h
player.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
player.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h fonts.h
player.o: object.h newton.h map.h terminal.h damages.h debug.h particle.h
player.o: telnet.h message.h turret.h home.h events.h xsetup.h beam.h addon.h
object.o: /usr/include/malloc.h
object.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
object.o: /usr/include/math.h /usr/include/features.h
object.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
object.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
object.o: /usr/include/bits/mathcalls.h
object.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/float.h
object.o: /usr/include/stdio.h
object.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h
object.o: /usr/include/bits/types.h /usr/include/libio.h
object.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
object.o: /usr/include/string.h constants.h object.h newton.h player.h
object.o: xbits.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
object.o: /usr/include/time.h /usr/include/endian.h
object.o: /usr/include/bits/endian.h /usr/include/sys/select.h
object.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
object.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h
object.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
object.o: /usr/include/X11/Xutil.h fonts.h map.h terminal.h damages.h
object.o: message.h bonus.h mines.h fix.h /usr/include/stdlib.h
object.o: /usr/include/alloca.h
map.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
map.o: /usr/include/gnu/stubs.h
map.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
map.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h
map.o: /usr/include/bits/types.h /usr/include/libio.h
map.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
map.o: /usr/include/string.h /usr/include/unistd.h
map.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
map.o: /usr/include/getopt.h /usr/include/stdlib.h /usr/include/sys/types.h
map.o: /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h
map.o: /usr/include/sys/select.h /usr/include/bits/select.h
map.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
map.o: /usr/include/alloca.h /usr/include/malloc.h fix.h map.h player.h
map.o: constants.h xbits.h /usr/include/X11/Xlib.h /usr/include/X11/X.h
map.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
map.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h terminal.h
map.o: damages.h debug.h home.h lift.h turret.h starburst.h message.h
events.o: /usr/include/stdio.h /usr/include/features.h
events.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
events.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
events.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h
events.o: /usr/include/bits/types.h /usr/include/libio.h
events.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
events.o: /usr/include/X11/keysym.h /usr/include/X11/keysymdef.h player.h
events.o: constants.h xbits.h /usr/include/X11/Xlib.h
events.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
events.o: /usr/include/bits/endian.h /usr/include/sys/select.h
events.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
events.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h
events.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
events.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h map.h terminal.h
events.o: damages.h debug.h home.h shop.h mines.h message.h lift.h addon.h
particle.o: /usr/include/malloc.h
particle.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
particle.o: particle.h player.h constants.h xbits.h /usr/include/X11/Xlib.h
particle.o: /usr/include/sys/types.h /usr/include/features.h
particle.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
particle.o: /usr/include/bits/types.h /usr/include/time.h
particle.o: /usr/include/endian.h /usr/include/bits/endian.h
particle.o: /usr/include/sys/select.h /usr/include/bits/select.h
particle.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
particle.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
particle.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h fonts.h
particle.o: object.h newton.h map.h terminal.h damages.h turret.h mymath.h
particle.o: /usr/include/math.h /usr/include/bits/huge_val.h
particle.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
particle.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/float.h
particle.o: debug.h
message.o: /usr/include/string.h /usr/include/features.h
message.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
message.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
message.o: message.h player.h constants.h xbits.h /usr/include/X11/Xlib.h
message.o: /usr/include/sys/types.h /usr/include/bits/types.h
message.o: /usr/include/time.h /usr/include/endian.h
message.o: /usr/include/bits/endian.h /usr/include/sys/select.h
message.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
message.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h
message.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
message.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h map.h
message.o: terminal.h damages.h
mines.o: player.h constants.h xbits.h /usr/include/X11/Xlib.h
mines.o: /usr/include/sys/types.h /usr/include/features.h
mines.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
mines.o: /usr/include/bits/types.h
mines.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
mines.o: /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h
mines.o: /usr/include/sys/select.h /usr/include/bits/select.h
mines.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
mines.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
mines.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h fonts.h object.h
mines.o: newton.h map.h terminal.h damages.h mymath.h /usr/include/math.h
mines.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
mines.o: /usr/include/bits/mathcalls.h
mines.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/float.h
mines.o: bonus.h particle.h
shop.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
shop.o: /usr/include/gnu/stubs.h
shop.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
shop.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h
shop.o: /usr/include/bits/types.h /usr/include/libio.h
shop.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
shop.o: /usr/include/string.h /usr/include/stdlib.h /usr/include/sys/types.h
shop.o: /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h
shop.o: /usr/include/sys/select.h /usr/include/bits/select.h
shop.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
shop.o: /usr/include/alloca.h fix.h player.h constants.h xbits.h
shop.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
shop.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
shop.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h map.h terminal.h
shop.o: damages.h shop.h addon.h
telnet.o: /usr/include/sys/types.h /usr/include/features.h
telnet.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
telnet.o: /usr/include/bits/types.h
telnet.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
telnet.o: /usr/include/time.h /usr/include/endian.h
telnet.o: /usr/include/bits/endian.h /usr/include/sys/select.h
telnet.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
telnet.o: /usr/include/sys/sysmacros.h /usr/include/sys/time.h
telnet.o: /usr/include/bits/time.h /usr/include/sys/socket.h
telnet.o: /usr/include/bits/socket.h /usr/include/bits/sockaddr.h
telnet.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
telnet.o: /usr/include/netinet/in.h /usr/include/stdint.h
telnet.o: /usr/include/bits/in.h /usr/include/bits/byteswap.h
telnet.o: /usr/include/sys/ioctl.h /usr/include/bits/ioctls.h
telnet.o: /usr/include/asm/ioctls.h /usr/include/asm/ioctl.h
telnet.o: /usr/include/bits/ioctl-types.h /usr/include/sys/ttydefaults.h
telnet.o: /usr/include/string.h /usr/include/errno.h
telnet.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
telnet.o: /usr/include/asm/errno.h /usr/include/stdio.h
telnet.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h
telnet.o: /usr/include/libio.h /usr/include/_G_config.h
telnet.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
telnet.o: /usr/include/alloca.h /usr/include/ctype.h /usr/include/unistd.h
telnet.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
telnet.o: /usr/include/getopt.h constants.h player.h xbits.h
telnet.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
telnet.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
telnet.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h map.h terminal.h
telnet.o: damages.h fix.h
terminal.o: /usr/include/stdio.h /usr/include/features.h
terminal.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
terminal.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
terminal.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h
terminal.o: /usr/include/bits/types.h /usr/include/libio.h
terminal.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
terminal.o: /usr/include/ctype.h /usr/include/endian.h
terminal.o: /usr/include/bits/endian.h /usr/include/malloc.h
terminal.o: /usr/include/string.h /usr/include/unistd.h
terminal.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
terminal.o: /usr/include/getopt.h /usr/include/stdlib.h
terminal.o: /usr/include/sys/types.h /usr/include/time.h
terminal.o: /usr/include/sys/select.h /usr/include/bits/select.h
terminal.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
terminal.o: /usr/include/alloca.h fix.h player.h constants.h xbits.h
terminal.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
terminal.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
terminal.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h map.h
terminal.o: terminal.h damages.h home.h draw.h lift.h shop.h debug.h addon.h
terminal.o: message.h
turret.o: /usr/include/math.h /usr/include/features.h
turret.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
turret.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
turret.o: /usr/include/bits/mathcalls.h
turret.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/float.h
turret.o: /usr/include/malloc.h
turret.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
turret.o: /usr/include/string.h /usr/include/stdio.h
turret.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h
turret.o: /usr/include/bits/types.h /usr/include/libio.h
turret.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h fix.h
turret.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
turret.o: /usr/include/endian.h /usr/include/bits/endian.h
turret.o: /usr/include/sys/select.h /usr/include/bits/select.h
turret.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
turret.o: /usr/include/alloca.h constants.h turret.h player.h xbits.h
turret.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
turret.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
turret.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h map.h terminal.h
turret.o: damages.h mymath.h message.h particle.h
bonus.o: /usr/include/stdio.h /usr/include/features.h
bonus.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
bonus.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
bonus.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h
bonus.o: /usr/include/bits/types.h /usr/include/libio.h
bonus.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h object.h
bonus.o: newton.h player.h constants.h xbits.h /usr/include/X11/Xlib.h
bonus.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
bonus.o: /usr/include/bits/endian.h /usr/include/sys/select.h
bonus.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
bonus.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h
bonus.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
bonus.o: /usr/include/X11/Xutil.h fonts.h map.h terminal.h damages.h bonus.h
bonus.o: message.h particle.h fix.h /usr/include/stdlib.h
bonus.o: /usr/include/alloca.h
home.o: /usr/include/string.h /usr/include/features.h
home.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
home.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
home.o: /usr/include/stdio.h
home.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h
home.o: /usr/include/bits/types.h /usr/include/libio.h
home.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
home.o: /usr/include/malloc.h player.h constants.h xbits.h
home.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h /usr/include/time.h
home.o: /usr/include/endian.h /usr/include/bits/endian.h
home.o: /usr/include/sys/select.h /usr/include/bits/select.h
home.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
home.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
home.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h fonts.h object.h
home.o: newton.h map.h terminal.h damages.h shop.h fix.h
home.o: /usr/include/stdlib.h /usr/include/alloca.h home.h draw.h xsetup.h
home.o: /usr/include/setjmp.h /usr/include/bits/setjmp.h message.h
lift.o: /usr/include/malloc.h
lift.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
lift.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
lift.o: /usr/include/gnu/stubs.h
lift.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h
lift.o: /usr/include/bits/types.h /usr/include/libio.h
lift.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
lift.o: /usr/include/string.h /usr/include/ctype.h /usr/include/endian.h
lift.o: /usr/include/bits/endian.h /usr/include/stdlib.h
lift.o: /usr/include/sys/types.h /usr/include/time.h
lift.o: /usr/include/sys/select.h /usr/include/bits/select.h
lift.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
lift.o: /usr/include/alloca.h lift.h map.h turret.h player.h constants.h
lift.o: xbits.h /usr/include/X11/Xlib.h /usr/include/X11/X.h
lift.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
lift.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h terminal.h
lift.o: damages.h
addon.o: /usr/include/malloc.h
addon.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
addon.o: /usr/include/stdio.h /usr/include/features.h
addon.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
addon.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h
addon.o: /usr/include/bits/types.h /usr/include/libio.h
addon.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
addon.o: /usr/include/string.h /usr/include/stdlib.h /usr/include/sys/types.h
addon.o: /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h
addon.o: /usr/include/sys/select.h /usr/include/bits/select.h
addon.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
addon.o: /usr/include/alloca.h message.h player.h constants.h xbits.h
addon.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
addon.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
addon.o: /usr/include/X11/Xutil.h fonts.h object.h newton.h map.h terminal.h
addon.o: damages.h addon.h
newton.o: /usr/include/stdio.h /usr/include/features.h
newton.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
newton.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
newton.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h
newton.o: /usr/include/bits/types.h /usr/include/libio.h
newton.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
newton.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
newton.o: /usr/include/endian.h /usr/include/bits/endian.h
newton.o: /usr/include/sys/select.h /usr/include/bits/select.h
newton.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
newton.o: /usr/include/alloca.h /usr/include/math.h
newton.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
newton.o: /usr/include/bits/mathcalls.h
newton.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/float.h
newton.o: fix.h newton.h mymath.h damages.h player.h constants.h xbits.h
newton.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
newton.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
newton.o: /usr/include/X11/Xutil.h fonts.h object.h map.h terminal.h turret.h
targetter.o: /usr/include/string.h /usr/include/features.h
targetter.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
targetter.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
targetter.o: /usr/include/stdio.h
targetter.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h
targetter.o: /usr/include/bits/types.h /usr/include/libio.h
targetter.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h addon.h
targetter.o: constants.h player.h xbits.h /usr/include/X11/Xlib.h
targetter.o: /usr/include/sys/types.h /usr/include/time.h
targetter.o: /usr/include/endian.h /usr/include/bits/endian.h
targetter.o: /usr/include/sys/select.h /usr/include/bits/select.h
targetter.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
targetter.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
targetter.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h fonts.h
targetter.o: object.h newton.h map.h terminal.h damages.h targetter.h
beam.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
beam.o: /usr/include/gnu/stubs.h
beam.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
beam.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h
beam.o: /usr/include/bits/types.h /usr/include/libio.h
beam.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
beam.o: /usr/include/malloc.h beam.h player.h constants.h xbits.h
beam.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h /usr/include/time.h
beam.o: /usr/include/endian.h /usr/include/bits/endian.h
beam.o: /usr/include/sys/select.h /usr/include/bits/select.h
beam.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
beam.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
beam.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h fonts.h object.h
beam.o: newton.h map.h terminal.h damages.h mymath.h /usr/include/math.h
beam.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
beam.o: /usr/include/bits/mathcalls.h
beam.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/float.h fix.h
beam.o: /usr/include/stdlib.h /usr/include/alloca.h turret.h
starburst.o: starburst.h newton.h beam.h player.h constants.h xbits.h
starburst.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
starburst.o: /usr/include/features.h /usr/include/sys/cdefs.h
starburst.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
starburst.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
starburst.o: /usr/include/time.h /usr/include/endian.h
starburst.o: /usr/include/bits/endian.h /usr/include/sys/select.h
starburst.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
starburst.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h
starburst.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
starburst.o: /usr/include/X11/Xutil.h fonts.h object.h map.h terminal.h
starburst.o: damages.h mymath.h /usr/include/math.h
starburst.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
starburst.o: /usr/include/bits/mathcalls.h
starburst.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/float.h
client.o: /usr/include/sys/types.h /usr/include/features.h
client.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
client.o: /usr/include/bits/types.h
client.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
client.o: /usr/include/time.h /usr/include/endian.h
client.o: /usr/include/bits/endian.h /usr/include/sys/select.h
client.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
client.o: /usr/include/sys/sysmacros.h /usr/include/sys/socket.h
client.o: /usr/include/bits/socket.h /usr/include/bits/sockaddr.h
client.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
client.o: /usr/include/netinet/in.h /usr/include/stdint.h
client.o: /usr/include/bits/in.h /usr/include/bits/byteswap.h
client.o: /usr/include/netdb.h /usr/include/rpc/netdb.h /usr/include/stdio.h
client.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h
client.o: /usr/include/libio.h /usr/include/_G_config.h
client.o: /usr/include/bits/stdio_lim.h /usr/include/pwd.h
client.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
client.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
client.o: /usr/include/X11/Xutil.h /usr/include/unistd.h
client.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
client.o: /usr/include/getopt.h /usr/include/string.h /usr/include/stdlib.h
client.o: /usr/include/alloca.h /usr/include/sys/time.h
client.o: /usr/include/bits/time.h fix.h
frontend.o: /usr/include/sys/types.h /usr/include/features.h
frontend.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
frontend.o: /usr/include/bits/types.h
frontend.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h
frontend.o: /usr/include/time.h /usr/include/endian.h
frontend.o: /usr/include/bits/endian.h /usr/include/sys/select.h
frontend.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
frontend.o: /usr/include/sys/sysmacros.h /usr/include/sys/socket.h
frontend.o: /usr/include/bits/socket.h /usr/include/bits/sockaddr.h
frontend.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
frontend.o: /usr/include/netinet/in.h /usr/include/stdint.h
frontend.o: /usr/include/bits/in.h /usr/include/bits/byteswap.h
frontend.o: /usr/include/netdb.h /usr/include/rpc/netdb.h
frontend.o: /usr/include/stdio.h
frontend.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h
frontend.o: /usr/include/libio.h /usr/include/_G_config.h
frontend.o: /usr/include/bits/stdio_lim.h /usr/include/pwd.h
frontend.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
frontend.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
frontend.o: /usr/include/X11/Xutil.h /usr/include/unistd.h
frontend.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
frontend.o: /usr/include/getopt.h /usr/include/string.h /usr/include/stdlib.h
frontend.o: /usr/include/alloca.h /usr/include/sys/time.h
frontend.o: /usr/include/bits/time.h /usr/include/malloc.h
frontend.o: /usr/include/math.h /usr/include/bits/huge_val.h
frontend.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
frontend.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/float.h
frontend.o: /usr/include/X11/keysym.h /usr/include/X11/keysymdef.h fix.h
frontend.o: fonts.h
