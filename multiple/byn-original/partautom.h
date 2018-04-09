/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/



#ifndef PARTAUTOMINCLUDED
#define PARTAUTOMINCLUDED

#include "basics.h"


	/* Implementation of automaton partition */

typedef int (*Tsearcher) ();
typedef mask Tsection[Sigma];           /* T for a single cell */

typedef struct
   { union
	{ Tsection **d2;			/* T cells (2D) */
	  Tsection *d1;				/* T cells (0,1D) */
	} T;
     union 
	{ mask **d2;				/* D cells (2D) */
	  mask *d1;				/* D cells (1D) */
	  mask d0;				/* D cell  (0D) */
	} D;
     union
	{ mask **d2;				/* new D cells (2D) */
	  mask *d1;				/* new D cells (1D) */
	} newD;
     mask Din;				/* initializer cell */
     bool *S;				/* S table */
     int I,J,lr,lc;			/* last row,col cells */
     int m,k;				/* m and k for this problem */
     int active;			/* last active column */
     int p[6];				/* precomputed values */
     mask msk[3];			/* precomputed masks */
     union
	{ mask *d1;				/* masks to analyze last row */
	  mask d0;				/* mask  to analyze last row */
	} Glast;
     union
	{ mask *d1;				/* masks to analyze all rows */
	  mask d0;				/* mask  to analyze all rows */
	} Gany;
     Tsearcher search;			/* searching procedure */
   } PartAutom;			/* partitioned automaton for a problem */

	/* does the preprocessing to search "pattern" by automaton
	   partitioning. only pattern[0..m-1] is considered.
	   V says if it is to be used for verifications
	   (in that case the automaton does not assume that the
	    pattern follows the text) */

void prepPartAutom (PartAutom *M, uchar *pattern, int m, int k, int c,
		    bool V);

	/* used internally: superimposed automata for pattern[0..np-1] */

void prepSPartAutom (PartAutom *M, uchar **pattern, int np,
                            int k, int c, bool V);

	/* prepares the automaton for a fresh search */

void initPartAutom (PartAutom *M);

	/* frees the automaton */

void freePartAutom (PartAutom *M);

	/* searches with M on text[from..to-1], filling matches at positions>=0.
	*/

#define searchPartAutom(M,text,from,to,matches) \
	(M)->search (M,text,from,to,matches)

	/* estimates the cost per character under this setup
	   (it selects internally the best partition to use)
	*/
double estimatePartAutom (int m, int k, int c, int r);

	/* internals */
int search1match (bool *S, register uchar *text,
                  int from, int to, int *matches);

int search1matchV (bool *S, register uchar *text,
                  int from, int to, int *matches);

#endif
