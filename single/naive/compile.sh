gcc  -O3 -msse4.2 -w naive16.c 386.c main.c -o naive16.t -lm
gcc  -O2 -march=native -msse4.2 -mavx2 -w naive32.c 386.c main.c -o naive32.t -lm
