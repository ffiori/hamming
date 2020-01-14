CC = gcc 
CFLAGS := $(USERFLAGS) -msse4.2 -mavx2 -w sys/386.c -lm -Wall -O3 -pipe -Dbitvector=unsigned -finline-functions -ffast-math -fpeephole -funroll-loops -fomit-frame-pointer

mbypsb: bperl-mm-multi.o main.o mepsm.o ans2b.o
	$(CC) bperl-mm-multi.o main.o mepsm.o ans2b.o $(CFLAGS) -o $@

bperl-mm-multi.o: bperl-mm-multi.c bperl-mm-multi.h mepsm.o main.o ans2b.o
	$(CC) $(CFLAGS) mepsm.o main.o -c bperl-mm-multi.c

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

mepsm.o:  mepsm.c
	$(CC) $(CFLAGS) -c mepsm.c

ans2b.o: ans2b.c
	$(CC) $(CFLAGS) -c ans2b.c

clean:
	rm *.o
