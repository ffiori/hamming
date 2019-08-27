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

#if 1
#define movemask_epi8 _mm256_movemask_epi8
#define cmpeq_epi8 _mm256_cmpeq_epi8
#define loadu_si _mm256_loadu_si256
#define mi __m256i
#define m16 32

#else

#define movemask_epi8 _mm_movemask_epi8
#define cmpeq_epi8 _mm_cmpeq_epi8
#define loadu_si _mm_loadu_si128
#define mi __m128i
#define m16 32

#endif

#define CHAR_SET (CHAR_MAX-CHAR_MIN+1)

#define LIM 65536

uint16_t d3[LIM];

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



void ans_prep(CHARTYPE *base, register int m, int k, int q)
{
  int i,j;
  pat.patlen = m;
  
  if(m<=16){
        for (j = 0; j < MAXPAT; j++) pat.pat[j]=0;
        memcpy(pat.pat, base, m);
        for (i = 0; i < LIM; ++i) d3[i]=0;
        for (i = 0; i < (2<<(m-1)); ++i) if ((m-_mm_popcnt_u32(i))<=k) d3[i]=1;
  }
  else{
      for (j = 0; j < MAXPAT; j++) pat.pat[j]=0;
      memcpy(pat.pat, base, m);
      for (i = 0; i < LIM; ++i) if ((16-_mm_popcnt_u32(i))<=k) d3[i]=1;
      //memcpy(pat.pat, base, m); //why twice?
  }
}

#define MAXTEXT 1024
//~ CHARTYPE text[MAXTEXT];

int ans_exec(CHARTYPE *y, int n, int k, int q)
{n--;//TODO  careful
	int i, j, id, matches = 0, m=pat.patlen, nm=n-m;
	uint32_t t,mask;
	mi x_ptr, y_ptr;

    //~ for(i=0; i<n; i++) text[i]=y[i];

    mask = (1<<16)-1;
	//~ for (j = 0; j < m16; j++) text[n+j]=1;
    //~ y=text;

	x_ptr = loadu_si(( mi *)(pat.pat));

	for (j = 0; j <= nm; j++) {
		y_ptr = loadu_si(( mi *)(y+j));
		t = movemask_epi8( cmpeq_epi8(x_ptr, y_ptr) );

		if (d3[t&mask]){
		    if(m<=16) matches++;
            else if((m-_mm_popcnt_u32(t))<=k) matches++;
        }
	}

	return matches;
}
