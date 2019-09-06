#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <xmmintrin.h>
#include <immintrin.h>
#include <x86intrin.h>
#define MAXPAT  128
#include        <limits.h>
#include        <memory.h>
#include        <stdlib.h>
#include "ans2b.h"

#define WORD 64
#ifndef TABTYPE
#define TABTYPE long
#endif
typedef TABTYPE Tab;

#ifndef CHARTYPE
#define CHARTYPE        unsigned char
#endif

//~ #if 0
//~ #define movemask_epi8 _mm256_movemask_epi8
//~ #define cmpeq_epi8 _mm256_cmpeq_epi8
//~ #define loadu_si _mm256_loadu_si256
//~ #define mi __m256i
//~ #define m16 32
//~ #else
//~ #define movemask_epi8 _mm_movemask_epi8
//~ #define cmpeq_epi8 _mm_cmpeq_epi8
//~ #define loadu_si _mm_loadu_si128
//~ #define mi __m128i
//~ #define m16 32
//~ #endif

#define CHAR_SET (CHAR_MAX-CHAR_MIN+1)

#define LIM 65536
#define mask (LIM-1)

uint8_t d3[LIM];

typedef struct
{
    int patlen;
    CHARTYPE pat[MAXPAT];
    Tab delta1[CHAR_SET];
} PATTERN;

static PATTERN *pats = NULL;

void pb (int y)
{
    int i;

    for (i = 0; i < 16; i++)
        printf ("%d", (0 != (((uint16_t) (1 << 15)) & (y << i))));
    printf ("\n");
}

void pnn (int y)
{
    printf ("%d\n", y);
}

void pn (int y)
{
    printf ("%d ", y);
}

void pc (int y)
{
    printf ("%c\n", y);
}

int patlen;

void ans_prep (CHARTYPE * base, register int m, int k, int npats)
{
    int i, j;

    patlen = m;

    if (pats != NULL)
        free (pats);
    pats = malloc (sizeof (PATTERN) * npats);

    int patnow;

    if (m <= 16) {
        for (patnow = 0; patnow < npats; patnow++) {
            for (j = 0; j < MAXPAT; j++)
                pats[patnow].pat[j] = 0;
            memcpy (pats[patnow].pat, base + patnow * (m + 1), m);
            //pats[patnow].pat = base+patnow*(m+1);
            for (i = 0; i < LIM; ++i)
                d3[i] = 0;
            for (i = 0; i < (2 << (m - 1)); ++i)
                if ((m - _mm_popcnt_u32 (i)) <= k)
                    d3[i] = 1;
        }
    }
    else {
        for (patnow = 0; patnow < npats; patnow++) {
            for (j = 0; j < MAXPAT; j++)
                pats[patnow].pat[j] = 0;
            memcpy (pats[patnow].pat, base + patnow * (m + 1), m);
            //pats[patnow].pat = base+patnow*(m+1);
            for (i = 0; i < LIM; ++i)
                if ((16 - _mm_popcnt_u32 (i)) <= k)
                    d3[i] = 1;
        }
    }
}

int ans_exec_short (CHARTYPE * y, int n, int k, int patnow)
{
    n--;                        //TODO  careful
    int j, matches = 0, m = patlen, nm = n - m;
    int t;
    __m128i x_ptr, y_ptr;

    x_ptr = _mm_loadu_si128 ((__m128i *) (pats[patnow].pat));

    for (j = 0; j <= nm; j++) {
        y_ptr = _mm_loadu_si128 ((__m128i *) (y + j));
        t = _mm_movemask_epi8 (_mm_cmpeq_epi8 (x_ptr, y_ptr));
        matches+=d3[t];
    }

    return matches;
}

int ans_exec_long (CHARTYPE * y, int n, int k, int patnow)
{
    n--;                        //TODO  careful
    int i, j, id, matches = 0, m = patlen, nm = n - m;
    int t;
    __m256i x_ptr, y_ptr;

    x_ptr = _mm256_loadu_si256 ((__m256i *) (pats[patnow].pat));

    for (j = 0; j <= nm; j++) {
        y_ptr = _mm256_loadu_si256 ((__m256i *) (y + j));
        t = _mm256_movemask_epi8 (_mm256_cmpeq_epi8 (x_ptr, y_ptr));
        //~ if(t<0 || t>256) printf("t %d, n %d, m %d\n",t,n,m);
        if (d3[t & mask]) {
            if ((m - _mm_popcnt_u32 (t)) <= k)
                matches++;
        }
    }

    return matches;
}

/*
fefo@adela:~/Desktop/hamming/multiple/bperl-mm-multi$ ./mbyps -p 1 -e 1 -k 1 ../tests/genome/ecoli.txt < ../tests/genome/ecoli.txt.8.n1000.badpat 
alg=./mbyps              np=1000  match=2080020    p=0.00000s e=0.97000s total=0.97000s
*/
