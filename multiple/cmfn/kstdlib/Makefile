# Makefile for libcool
# (C) 1998 Kimmo.Fredriksson@cs.Helsinki.FI

LIBRARY	= kstdlib

VERSION	= 0.1a

PACKET	= $(LIBRARY)$(VERSION)

SRCS	=	memalloc.c	\
		timer.c		\
		rnd.c		\
		errormsgs.c	\
		mathf.c		\
		fio.c

OBJS	=	memalloc.o	\
		timer.o		\
		rnd.o		\
		errormsgs.o	\
		mathf.o		\
		fio.o

SRCDIR	= .

LIBDIR	= ..

CC	= gcc
AR	= ar

OFLAGS	= -O3 -finline-functions -ffast-math -funroll-loops -fomit-frame-pointer -fpeephole

CFLAGS	= $(OFLAGS) $(WFLAGS) $(MFLAGS) $(DFLAGS) -fPIC -I$(LIBDIR)/include

COMPILE	= $(CC) $(CFLAGS) -c
BUILD	= $(AR) -r lib$(LIBRARY).a $(OBJS)

$(LIBRARY): $(OBJS)
	@echo Building $(LIBRARY)
	@$(BUILD)
	@cp lib$(LIBRARY).a $(LIBDIR)/lib/
	@cp *.h $(LIBDIR)/include/

depend:
	makedepend -Y -- $(CFLAGS) -- $(SRCS)

.c.o$<:
	@echo Compiling $*.c
	@$(COMPILE) $*.c

clean:
	@echo Cleaning $(LIBRARY)
	@rm -f $(OBJS) *.?% core lib$(LIBRARY).a

# DO NOT DELETE

memalloc.o: memalloc.h errormsgs.h
timer.o: timer.h
rnd.o: rnd.h
mathf.o: mathf.h
fio.o: memalloc.h fio.h errormsgs.h
