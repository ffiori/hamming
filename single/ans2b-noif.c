#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <xmmintrin.h>
#include <immintrin.h>
#include <x86intrin.h>
#define MAXPAT  200
#include        <limits.h>
#include        <memory.h>
#include        <stdlib.h>

#define WORD 64
#ifndef TABTYPE
#define TABTYPE long
#endif
typedef TABTYPE Tab;

#ifndef CHARTYPE
#define CHARTYPE        unsigned char
#endif

#if 0
#define ALPHA 32
#define LOG_ALPHA 5
#define movemask_epi8 _mm256_movemask_epi8
#define cmpeq_epi8 _mm256_cmpeq_epi8
#define loadu_si _mm256_loadu_si256
#define mi __m256i
#else
#define ALPHA 16
#define LOG_ALPHA 4
#define movemask_epi8 _mm_movemask_epi8
#define cmpeq_epi8 _mm_cmpeq_epi8
#define loadu_si _mm_loadu_si128
#define mi __m128i
#endif

#define CHAR_SET (CHAR_MAX-CHAR_MIN+1)

#define LIM 65536

uint8_t d3[LIM];

static struct
{
        int patlen;
        CHARTYPE pat[MAXPAT];
        Tab delta1[CHAR_SET];
} pat;

void pb(int y) {
   int i;
   for(i=0;i<16;i++) printf("%d",
     (0!=(((uint16_t)(1<<15))&(y<<i))));
   printf("\n");}

void pnn(int y) {
   printf("%d\n",y);}

void pn(int y) {
   printf("%d ",y);}

void pc(int y) {
   printf("%c\n",y);}

void prep(CHARTYPE *base, register int m, int k, int q)
{
    int i,j;
    pat.patlen = m;
    for (j = 0; j < MAXPAT; j++) pat.pat[j]=0;
    memcpy(pat.pat, base, m);
    for (i = 0; i < LIM; ++i) d3[i]=0;
    for (i = 0; i < (1<<m); ++i) {
        if ((m-_mm_popcnt_u32(i))<=k) d3[i]=1;
    }
}

int exec(CHARTYPE *y, int n, int k, int q)
{
    int j, matches = 0, m=pat.patlen, nm=n-m;
    uint16_t t;
    mi x_ptr, y_ptr;
    for (j = 0; j < 16; j++) y[n+j]=1;

    x_ptr = loadu_si(( mi *)(pat.pat));

    for (j = 0; j <= nm; j++) {
        y_ptr = loadu_si(( mi *)(y+j));
        t = movemask_epi8( cmpeq_epi8(x_ptr, y_ptr) );
        matches+=d3[t];
    }

    return matches;
}
