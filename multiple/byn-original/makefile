CC	= gcc 

FLAGS	= sys/386.c -pipe -Dbitvector=unsigned -finline-functions -ffast-math -fpeephole -funroll-loops -fomit-frame-pointer -O3

exactp: main.o lsearch
	$(CC) $(FLAGS) -o byn main.o lsearch -lm

main.o: main.c search.c
	$(CC) $(FLAGS) -c main.c

#~ search.o: search.c
#~ 	$(CC) $(FLAGS) -c search.c 

all: lsearch
	echo ok

lsearch: bmhs.o autom.o partautom.o multipat.o cut.o partexact.o partmixed.o partautom.o partautomh.o partautomv.o partautomh1c.o partautomhv1c.o basics.o 
	ar rcv lsearch bmhs.o autom.o partautom.o multipat.o cut.o partexact.o partautom.o partautomh.o partautomv.o partautomh1c.o partautomhv1c.o partmixed.o basics.o
	ranlib lsearch

basics.o: basics.h basics.c
	$(CC) $(FLAGS) -c basics.c

autom.o: autom.c autom.h basics.h
	$(CC) $(FLAGS) -c autom.c

partmixed.o: partmixed.c partmixed.h basics.h
	$(CC) $(FLAGS) -c partmixed.c

partautom.o: partautom.c partautom.h basics.h 
	$(CC) $(FLAGS) -c partautom.c

partautomh.o: partautomh.c partautomh.h partautom.h basics.h
	$(CC) $(FLAGS) -c partautomh.c

partautomhv1c.o: partautomhv1c.c partautomhv1c.h partautom.h basics.h
	$(CC) $(FLAGS) -c partautomhv1c.c

partautomh1c.o: partautomh1c.c partautomh1c.h partautom.h basics.h
	$(CC) $(FLAGS) -c partautomh1c.c

partautomv.o: partautomv.c partautomv.h partautom.h basics.h
	$(CC) $(FLAGS) -c partautomv.c

partexact.o: partexact.c partexact.h basics.h
	$(CC) $(FLAGS) -c partexact.c

multipat.o: multipat.c multipat.h basics.h makeg.c
	$(CC) $(FLAGS) -c multipat.c

bmhs.o: bmhs.c bmhs.h basics.h
	$(CC) $(FLAGS) -c bmhs.c

cut.o: cut.c cut.h basics.h
	$(CC) $(FLAGS) -c cut.c

clean:
	rm lsearch *.o
