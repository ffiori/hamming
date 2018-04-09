/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/

#include "bmhs.h"

        /* prepares to search for pattern[0..m-1] */

void prepBmhs (Bmhs * B, uchar * pattern, int m)
{
    int j, c;

    B->d = (int *) malloc (Sigma * sizeof (int));
    for (c = 0; c < Sigma; c++)
        B->d[c] = m + 1;
    for (j = 0; j < m; j++)
        B->d[pattern[j]] = m - j;
    for (c = 0; c < Sigma; c++)
        if (c != Map[c])
            B->d[c] = B->d[Map[c]];
    B->pat = pattern;
    B->m = m;
}

void initBmhs (Bmhs * B)
{
}

        /* frees data */

void freeBmhs (Bmhs * B)
{
    free (B->d);
    B->d = NULL;
}

        /* searches in text+n */

int searchBmhs (Bmhs * B, register uchar * text, int from, int to, int *matches)
{
    register char *pat = B->pat;
    register int m = B->m;
    register int *d = B->d;
    register uchar *tt = text + from;
    register int l;
    int n;
    int count = 0;

    while (true) {
        for (l = 0; Map[tt[l]] == pat[l]; l++);
        if (l == m) {           /* found */
            n = tt - text + m;
            if (n > to)
                break;
            if (matches != NULL)
                matches[count] = n;
            count++;
        }
        tt += d[tt[m]];
    }
    return count;
}
