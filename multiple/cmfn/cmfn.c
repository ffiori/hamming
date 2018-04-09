#ifndef CHARTYPE
#define CHARTYPE unsigned char
#endif

#include <limits.h>
#include <memory.h>
#include "config.h"
#include "squeeze.h"
#include "kstdlib.h"

#define CHARSET (CHAR_MAX-CHAR_MIN+1)

//exactly one of these three values should be !=0
//Careful! Worse results by treating protein as PROT and as ASCII
#define ASCII 1
#define PROT 0
#define DNA 0

/*
 * cmfn parameters (-B -Sb)
 */

/* config_set */
//as recommended in FN paper
#if ASCII
/* ASCII */
    #define tuple 2 // l-gram
    #define sigma 256 /* 4 for DNA, 256 for ascii */
    #define tab 8 /* bits in sigma */
    #define ascii 1
#endif

#if DNA
/* DNA */
    #define tuple 6
    #define sigma 4
    #define tab 2
    #define ascii 0
#endif

#if PROT
    #define tuple 3
    #define sigma 20
    #define tab 5
    #define ascii 0
#endif

//#define charg 64
int charg = 0; //originally 0. Character grouping, not supported for non-ascii alphabets
#define ascb 0 //ascii begin
#define asce 255 //ascii end
#define ignorecase 0
#define iubt 0
#define iubp 0

/* config_data */

/* config_opts */
/*unsigned int k = 0;*/
#define d 32
#define hamming 1 /* only mismatches */
#define si 0 //superimposition

/* config_alg */
int skipf = 0; //originally 0
int skipb = 1; //originally 1
#define laq 0   //only with hamming=0
#define bp 1    //best possible algorithm
//#define optimize 0 //originally 0
int optimize = 0;
#define strictblaq 0 //only with laq
#define laqh 0 //originally set to 0
#define E 0
#define wm 0

/* cmfn */
//#define groups 1
int groups = 1;
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
void prep(CHARTYPE **base, int npats, int k, int groupsval, int chargval)
{
    //static int initialized = 0;
    int i, j;
    int m = strlen(base[0]);

    M = npats;
    groups = groupsval;
    charg = chargval;

    /* configure */
    _emsgs_v = 0; /* verbosity */

    TUPLE = tuple; /* global */
    TBITS = (( 1 << ( tab * TUPLE )) - 1 );
    SBITS = (( 1 << tab ) - 1 );
    config_set(tuple, sigma, ascii, charg, ascb, asce, ignorecase, iubt, iubp);
    config_opts(k, d, hamming, si);
    config_alg(skipf, skipb, laq, bp, optimize, strictblaq, laqh, E, wm);

    patterns = base;

    config_data(NULL, 0, patterns, M);

    //cmfn_prep(groups, code, 1);
}


int exec(CHARTYPE *base, int n, int k, int reps)
{
//puts("exec");
    occ = 0;
    //config_data(base, n, patterns, M);
    //config_data(NULL, 0, patterns, M);
    //cmfn_prep(groups, code, REPETITIONS);
    
    //config_data(base, n, patterns, M);
	config.t = base;
	config.n = n;
    
    get_map(base, n, charg);
    cmfn(groups, code, reps); /* does cmfn preprocesing and searching! */

    //startclock(); /* hack, cmfn does the timing. */
    //npreproc++;
    return occ;
}
