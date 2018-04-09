/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/

#ifndef CUTINCLUDED
#define CUTINCLUDED

#include "basics.h"

        /* cuts a pattern into subpatterns */

typedef struct
{
    int *aclen;                 /* accum lengths of Ps[0..i] */
    int *len;                   /* length of Ps[i] */
    uchar **ps;                 /* subpatterns */
    int j;                      /* number of subpatterns */
    int k;                      /* k/j */
    int r;                      /* # of patterns already added */
} Cut;

void makeCut (uchar * pattern, int m, int k, int jj, Cut * cut);

void freeCut (Cut * cut);

void initCut (int k, int r, int jj, Cut * cut);

void addCut (uchar * pattern, int m, Cut * cut);

#endif
