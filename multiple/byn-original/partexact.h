/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/



#ifndef PARTEXACTINCLUDED
#define PARTEXACTINCLUDED

#include "basics.h"
#include "cut.h"
#include "multipat.h"
#include "partautom.h"

	/* pattern partitioning into exact match */

typedef struct sPartExactB
   { bool large;
     int m,k;           /* m,k for this problem */
     PartAutom dynaut;  /* verif automaton */
     int offset;        /* to beginning of pattern */
     union
        { struct /* large */
            { struct sPartExactB *one,*two;        /* subsearches */
            } l;
          struct /* small */
            { int l;                    /* length of fst pattern */
            } s;
        } u;
   } PartExactB;   /* problem */

typedef struct
   { Multi M;                   /* multipattern search */
     int k;			/* nr of errors */
     int r;			/* nr of patterns */
     PartExactB *A;		/* hierarchical verification */
   } PartExact;         /* partitioning into exact search */

void prepPartExact (PartExact *P, uchar **pattern, int r, int m, int k);

void initPartExact (PartExact *P);

void freePartExact (PartExact *P);

int searchPartExact (PartExact *P, uchar *text, int from, int to, int *matches);

#endif
