CFLAGS ?= -O3 -Wall -msse4.2 -mavx2
LDFLAGS ?= -lm
ALGS = ef efs efs_long abm abndm asb bsa naive sa tuabndm tusa twsa ans2b ans2 ansb ans ans2b-noif

all: $(ALGS)

%: main.c %.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

twsa: main.c twsa.c
	$(CC) $(CFLAGS) -mpopcnt $^ -o $@ $(LDFLAGS)

efs: main2.c efs.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

efs_long: main2.c efs_long.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

ef: main2.c ef.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	$(RM) $(ALGS)

.PHONY: all clean
