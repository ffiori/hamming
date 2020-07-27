#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <xmmintrin.h>
#include <immintrin.h>
#include <x86intrin.h>
#include <limits.h>

#define ASIZE (UCHAR_MAX + 1)
#define PSIZE 256


#include "util.h"

#define ALPHA 16
#define LOG_ALPHA 4
#define movemask_epi8 _mm_movemask_epi8
#define cmpeq_epi8 _mm_cmpeq_epi8
#define loadu_si _mm_loadu_si128
#define mi __m128i

#ifndef CHARTYPE
#define CHARTYPE        unsigned char
#endif
#define MAXPAT  256

static struct
{
        int patlen;
        CHARTYPE pat[MAXPAT];
} pat;

static unsigned char mask[ASIZE*ALPHA];

#include "rank.h"
static int pi[PSIZE];
#define PI(a) (pi[(a)])

static void rank_pi(int m)
{
        CHARTYPE *P=pat.pat;
        int i;

        for (i = 0; i < m; i++) {
                int j = i;
                while (j > 0 && rank[P[pi[j-1]]] < rank
[P[i]]) {
                        pi[j] = pi[j-1];
                        j--;
                }
                pi[j] = i;
        }

#if 0
        for (i = 0; i < m; i++) {
                int id = pi[i];
                printf("pi[%d]=%d, rank[%c]=%d\n", i, i
d, P[id], rank[P[id]]);
        }
#endif
}

void prep(const CHARTYPE *pattern, register int m)
{
	int i, j;
    pat.patlen = m;
    memcpy(pat.pat, pattern, m);

for (j = 0; j < ASIZE; j++)
for (i = 0; i < ALPHA; i++) mask[j*ALPHA+i] = j;

rank_pi(m);

}


int exec(CHARTYPE *y, register int n) {
//int pnaive(const unsigned char *P, int m, 
//const unsigned char *y, int n)

CHARTYPE *P=pat.pat;
int i, j, id, matches = 0, 
x0,x1,x2, y0,y1,y2,m=pat.patlen;
uint32_t k;
mi x_ptr, y_ptr;

y0 = PI(0);
y1 = PI(1);
y2 = PI(2);
x0 = P[PI(0)] << LOG_ALPHA;
x1 = P[PI(1)] << LOG_ALPHA;
x2 = P[PI(2)] << LOG_ALPHA;

for (j = 0; j < n; j += ALPHA) {
  k = ~(uint32_t) 0;

  y_ptr = loadu_si(( mi *)(y + y0));
  x_ptr = loadu_si(( mi *)(mask + x0));
  k &= movemask_epi8( cmpeq_epi8(y_ptr, x_ptr) );


  y_ptr = loadu_si(( mi *)(y + y1));
  x_ptr = loadu_si(( mi *)(mask + x1));
  k &= movemask_epi8( cmpeq_epi8(y_ptr, x_ptr) );

  y_ptr = loadu_si(( mi *)(y + y2));
  x_ptr = loadu_si(( mi *)(mask + x2));
  k &= movemask_epi8( cmpeq_epi8(y_ptr, x_ptr) );


  if (k == 0) goto out;

		for (i = 3; i < m; i++) {
			id = PI(i);
			y_ptr = loadu_si(( mi *)(y + id));
			x_ptr = loadu_si(( mi *)(mask + (P[id] << LOG_ALPHA)));
			k &= movemask_epi8( cmpeq_epi8(y_ptr, x_ptr) );

			if (k == 0) goto out;
		}

		matches += _mm_popcnt_u32(k);
	out: y += ALPHA;
	}

	return matches;
}

//#define INIT_P pnaive_init_p
//#define SEARCH pnaive
//#include "main-pm.c"

