/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/

#ifndef BMHSINCLUDED
#define BMHSINCLUDED

#include "basics.h"

                /* boyer-moore-horspool-sunday search */

typedef struct
{
    int *d;                     /* the d table of bmhs */
    uchar *pat;                 /* pattern */
    int m;                      /* pattern length */
} Bmhs;

        /* prepares to search for pattern[0..m-1] */

void prepBmhs (Bmhs * B, uchar * pattern, int m);

        /* no init needed */

void initBmhs (Bmhs * B);

        /* frees data */

void freeBmhs (Bmhs * B);

        /* searches in text filling matches */

int searchBmhs (Bmhs * B, register uchar * text, int from, int to, int *matches);

#endif
