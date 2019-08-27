CC = gcc 
CFLAGS = -w mgrep.c sys/386.c magrep.c main.c -lm -Wall -O3 -pipe -Dbitvector=unsigned -finline-functions -ffast-math -fpeephole -funroll-loops -fomit-frame-pointer -DNO_SKIP -DNDEBUG

mm:
	$(CC) $(CFLAGS) -o $@

clean:
	rm *.o
