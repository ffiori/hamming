/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/



#ifndef PARTMIXEDINCLUDED
#define PARTMIXEDINCLUDED

#include "basics.h"
#include "partautom.h"

	/* mixed partitioning */

typedef struct sPartMixedV
   { bool large;
     int m,k;           /* m,k for this problem */
     PartAutom dynaut;  /* verif automaton */
     int offset;	/* to beginning of pattern */
     struct sPartMixedV *one,*two;        /* subsearches */
   } PartMixedV;   /* problem */

typedef struct
   { PartAutom *D;    /* to search */
     uint *vfrom,*vto;	/* to know what to verify */
     PartMixedV *V;   /* to verify */
     uint j,r;		/* # pieces, # patterns */
   } PartMixed;

int prepPartMixed (PartMixed *P, uchar **pattern, int r, int m, int k, int j, 
		    int rp, int c);


void initPartMixed (PartMixed *P);

void freePartMixed (PartMixed *P);

int searchPartMixed (PartMixed *P, uchar *text, int from, int to, int *matches);

#endif
