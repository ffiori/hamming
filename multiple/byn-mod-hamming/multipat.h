/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/

#ifndef MULTIPATINCLUDED
#define MULTIPATINCLUDED

#include "basics.h"

        /* a fast multipattern search heuristic, based on bmhs.
           the d table is computed as the worst among the
           patterns (length = minimum among patterns). instead
           of matching text against pattern, we search the
           text into a trie of the patterns 
         */

typedef int (*GAutom)[Sigma];

typedef struct
{
    int *d;                     /* d table */
    GAutom G;                   /* the trie of the patterns */
    int minlen;                 /* length of shorter pattern */
    int cant;                   /* nr of patterns */
    int *lrep;                  /* list of other equal subpatterns */
} Multi;

        /* prepare to search patterns Ps[0]..Ps[r-1] */

void prepMulti (Multi * M, uchar ** Ps, int r);

        /* no initialization needed */

void initMulti (Multi * M);

        /* free data */

void freeMulti (Multi * M);

        /* searches M from text, executing code(n,ptr) at each occurrence
           beginning at n.  (the pattern must be in the text)
         */

void searchMulti (Multi * M, uchar * text, int from, int to, int **matches, int *counters);

#endif
