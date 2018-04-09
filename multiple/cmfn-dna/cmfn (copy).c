#ifndef CHARTYPE
#define CHARTYPE unsigned char
#endif

#include <limits.h>
#include <memory.h>
#include "config.h"
#include "squeeze.h"
#include "kstdlib.h"

#define CHARSET (CHAR_MAX-CHAR_MIN+1)

/*
 * cmfn parameters (-B -Sb)
 */

/* config_set */
#if 1
/* ASCII */
#define tuple 2
#define sigma 256 /* 4 for DNA, 256 for ascii */
#define tab 8 /* bits in sigma */
#define ascii 1
#else
/* DNA */
#define tuple 6
#define sigma 4
#define tab 2
#define ascii 0
#endif

#define charg 0
#define ascb 0
#define asce 255
#define ignorecase 0
#define iubt 0
#define iubp 0

/* config_data */

/* config_opts */
/*unsigned int k = 0;*/
#define d 32
#define hamming 1 /* only mismatches */
#define si 0

/* config_alg */
#define skipf 0
#define skipb 1
#define laq 0
#define bp 1
#define optimize 0
#define strictblaq 0
#define laqh 0
#define E 0
#define wm 0

/* cmfn */
#define groups 1
#define code output /* or output */
#define REPETITIONS 1

static unsigned char **patterns;
static int M;
static int occ;

MATCHCODE output( int i, int score, char * p )
{
    /*printf("Match at %d, distance %d (%s)!\n", i, score, p );*/
    occ++;
    return 0;
}

extern int npreproc;
void prepold(CHARTYPE *base, int m, int k, int q)
{
    static int initialized = 0;
    int i, j;

    /* prep may be called multiple times */
    if (initialized) {
        for (i = 0; i < M; i++)
            free(patterns[i]);
        free(patterns);
    } else initialized = 1;

    /* configure */
    _emsgs_v = 0; /* verbosity */

    TUPLE = tuple; /* global */
    TBITS = (( 1 << ( tab * TUPLE )) - 1 );
    SBITS = (( 1 << tab ) - 1 );
    config_set(tuple, sigma, ascii, charg, ascb, asce, ignorecase, iubt, iubp);
    config_opts(k, d, hamming, si);
    config_alg(skipf, skipb, laq, bp, optimize, strictblaq, laqh, E, wm);

    /* m rotations */
    M = 1; /* M = m for circular search, M = 1 single search. */
    patterns = (unsigned char **)malloc(sizeof(unsigned char *) * M);
    if (!patterns) abort();
    for (i = 0; i < M; i++) {
        patterns[i] = (unsigned char *)malloc(sizeof(unsigned char) * m+ + 1);
        if (!patterns[i]) abort();
        for (j = 0; j < m; j++) {
            patterns[i][j] = base[(i+j) % m];
        }
        patterns[i][j] = 0;
    }
puts("prep");
    /* Execute once... */
    npreproc = 0;
}

void prep(CHARTYPE *base, int m, int k, int q)
{
    static int initialized = 0;
    int i, j;

    /* prep may be called multiple times */
    if (initialized) {
        for (i = 0; i < M; i++)
            free(patterns[i]);
        free(patterns);
    } else initialized = 1;

    /* configure */
    _emsgs_v = 0; /* verbosity */

    TUPLE = tuple; /* global */
    TBITS = (( 1 << ( tab * TUPLE )) - 1 );
    SBITS = (( 1 << tab ) - 1 );
    config_set(tuple, sigma, ascii, charg, ascb, asce, ignorecase, iubt, iubp);
    config_opts(k, d, hamming, si);
    config_alg(skipf, skipb, laq, bp, optimize, strictblaq, laqh, E, wm);

    /* m rotations */
    M = 1; /* M = m for circular search, M = 1 single search. */
    patterns = (unsigned char **)malloc(sizeof(unsigned char *) * M);
    if (!patterns) abort();
    for (i = 0; i < M; i++) {
        patterns[i] = (unsigned char *)malloc(sizeof(unsigned char) * m+ + 1);
        if (!patterns[i]) abort();
        for (j = 0; j < m; j++) {
            patterns[i][j] = base[(i+j) % m];
        }
        patterns[i][j] = 0;
    }
puts("prep");
    /* Execute once... */
    npreproc = 0;
}


int exec(CHARTYPE *base, int n, int k, int q)
{
puts("exec");
    occ = 0;
    config_data(base, n, patterns, M);
    get_map(base, n, charg);
    cmfn(groups, code, REPETITIONS); /* does cmfn preprocesing and searching! */

    startclock(); /* hack, cmfn does the timing. */
    npreproc++;
    return occ;
}
