PROGRAM	= cmfn-prog
TEST_PROGRAM = cmfn # hume-sunday testbed

VERSION	= 2.0a

PACKET	= $(PROGRAM)$(VERSION)

SRCS = opt.c squeeze.c preprocess.c filter.c cluster.c config.c hamming.c \
		bped.c myers.c wm.c
OBJS = $(SRCS:.c=.o)

PROGDIR	= .

#For Linux:
LIBDIR	= .

CC	= gcc
LINKER	= gcc

OFLAGS	= -pipe -Dbitvector=unsigned -finline-functions -ffast-math -fpeephole -funroll-loops -fomit-frame-pointer -O3
#OFLAGS	= -x c++ -O3 -xW -Dbitvector=unsigned
#OFLAGS	= -O3 -unroll -xW

WFLAGS	= -W #-Wall # compiling gives to many errors... lol
#WFLAGS	= -wd556

#for compilers other than gcc
#MFLAGS	= -Dinline= 

#DFLAGS	= -s -static
DFLAGS	= -s

#for Linux:
CFLAGS	= $(OFLAGS) $(WFLAGS) $(MFLAGS) $(DFLAGS) -I$(LIBDIR)/include 

#-D__NO_MATH_INLINES

LDFLAGS	= -L$(LIBDIR)/lib 

LIBS	= -lm -lkstdlib # -lefence

COMPILE	= $(CC) $(CFLAGS) -c
LINK	= $(LINKER) $(OBJS) $(DFLAGS) $(LDFLAGS) $(LIBS)

#all:	$(PROGRAM) $(TEST_PROGRAM) libs
all:	$(TEST_PROGRAM) libs

$(PROGRAM): $(OBJS)
	@echo Linking $(PROGRAM)
	@$(LINK) -o $(PROGRAM)

main.o: main.c
	@echo Compiling $<
	$(CC) $(CFLAGS) -c $<
cmfn.o: cmfn.c
	@echo Compiling $<
	$(CC) $(CFLAGS) -c $<
$(TEST_PROGRAM): $(filter-out opt.o, $(OBJS)) main.o 386a.o cmfn.o
	$(LINKER) $^ -o $@ $(LDFLAGS) $(LIBS)

depend:
	@makedepend -Y -- $(CFLAGS) -- $(SRCS)

libs:
	@echo Making kstdlib
	@cd kstdlib; make kstdlib

tiny: $(PROGRAM)
	@gzexe $(PROGRAM)
	@rm $(PROGRAM)~


run: $(PROGRAM)
	@$(PROGRAM)

.c.o$<:
	@echo Compiling $*.c
	@$(COMPILE) $*.c

clean:
	@echo Cleaning cmfn
	@rm -f $(OBJS) *.?% core $(TEST_PROGRAM) *.o

#distrib:
#	@tar -czf../$(PACKET).tar.gz -C .. CMFN/

# DO NOT DELETE

opt.o: ./include/kstdlib.h ./include/memalloc.h ./include/timer.h
opt.o: ./include/rnd.h ./include/macros.h ./include/fio.h
opt.o: ./include/ibitvector.h ./include/mathf.h ./include/errormsgs.h bped.h
opt.o: config.h squeeze.h preprocess.h wm.h filter.h cluster.h
squeeze.o: ./include/kstdlib.h ./include/memalloc.h ./include/timer.h
squeeze.o: ./include/rnd.h ./include/macros.h ./include/fio.h
squeeze.o: ./include/ibitvector.h ./include/mathf.h ./include/errormsgs.h
squeeze.o: squeeze.h config.h
preprocess.o: ./include/kstdlib.h ./include/memalloc.h ./include/timer.h
preprocess.o: ./include/rnd.h ./include/macros.h ./include/fio.h
preprocess.o: ./include/ibitvector.h ./include/mathf.h ./include/errormsgs.h
preprocess.o: ./include/bitfields.h bped.h config.h squeeze.h preprocess.h
preprocess.o: wm.h
filter.o: ./include/kstdlib.h ./include/memalloc.h ./include/timer.h
filter.o: ./include/rnd.h ./include/macros.h ./include/fio.h
filter.o: ./include/ibitvector.h ./include/mathf.h ./include/errormsgs.h
filter.o: ./include/bitfields.h bped.h config.h squeeze.h preprocess.h wm.h
filter.o: verify.h hamming.h filter.h myers.h wmp.h
cluster.o: ./include/kstdlib.h ./include/memalloc.h ./include/timer.h
cluster.o: ./include/rnd.h ./include/macros.h ./include/fio.h
cluster.o: ./include/ibitvector.h ./include/mathf.h ./include/errormsgs.h
config.o: config.h ./include/kstdlib.h ./include/memalloc.h ./include/timer.h
config.o: ./include/rnd.h ./include/macros.h ./include/fio.h
config.o: ./include/ibitvector.h ./include/mathf.h ./include/errormsgs.h
hamming.o: hamming.h config.h squeeze.h
bped.o: squeeze.h config.h ./include/kstdlib.h ./include/memalloc.h
bped.o: ./include/timer.h ./include/rnd.h ./include/macros.h ./include/fio.h
bped.o: ./include/ibitvector.h ./include/mathf.h ./include/errormsgs.h bped.h
myers.o: squeeze.h config.h verify.h ./include/kstdlib.h ./include/memalloc.h
myers.o: ./include/timer.h ./include/rnd.h ./include/macros.h ./include/fio.h
myers.o: ./include/ibitvector.h ./include/mathf.h ./include/errormsgs.h
myers.o: bped.h hamming.h filter.h myers.h preprocess.h wm.h
wm.o: verify.h ./include/kstdlib.h ./include/memalloc.h ./include/timer.h
wm.o: ./include/rnd.h ./include/macros.h ./include/fio.h
wm.o: ./include/ibitvector.h ./include/mathf.h ./include/errormsgs.h bped.h
wm.o: config.h hamming.h filter.h myers.h preprocess.h wm.h wmp.h
