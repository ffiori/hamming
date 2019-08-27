CC = gcc
CFLAGS = -w sys/386.c -lm -Wall -O3

byp: bperl-mm-old.o main.o
	$(CC) bperl-mm-old.o main.o $(CFLAGS) -o $@

bperl-mm-old.o: bperl-mm-old.c bperl-mm-old.h main.o
	$(CC) $(CFLAGS) main.o -c bperl-mm-old.c

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

clean:
	rm *.o
