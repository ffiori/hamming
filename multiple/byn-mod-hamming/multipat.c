/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/

#include "multipat.h"
#include "makeg.c"

        /* prepare to search patterns Ps[0]..Ps[r-1] */

void prepMulti (Multi * M, uchar ** Ps, int r)
{
    int c, i, j, totlen;

    M->cant = r;
    M->lrep = (int *) malloc (r * sizeof (int));
    for (i = 0; i < r; i++)
        M->lrep[i] = -1;        /* no similar patterns */

    /* compute the total and minimum length */
    M->minlen = totlen = strlen (Ps[0]);
    for (i = 1; i < r; i++) {
        int l = strlen (Ps[i]);

        totlen += l;
        if (l < M->minlen)
            M->minlen = l;
    }

    /* define the pessimistic d table */
    M->d = (int *) malloc (Sigma * sizeof (int));
    for (c = 0; c < Sigma; c++)
        M->d[c] = M->minlen + 1;
    for (i = 0; i < r; i++) {
        for (j = 0; j < M->minlen; j++)
            if (M->minlen - j < M->d[Ps[i][j]])
                M->d[Ps[i][j]] = M->minlen - j;
    }

    /* compute the trie */
    M->G = MakeG (Ps, r, totlen, M->lrep);

    /* character mapping */
    for (c = 0; c < Sigma; c++)
        if (c != Map[c])
            M->d[c] = M->d[Map[c]];
}

void initMulti (Multi * M)
{
}

        /* free data */

void freeMulti (Multi * M)
{
    free (M->d);
    M->d = NULL;
    free (M->G);
    M->G = NULL;
    free (M->lrep);
    M->lrep = NULL;
}

        /* searches M from text+n, returning where was it found (next position)
           (the pattern must be in the text)
         */

void searchMulti (Multi * M, uchar * text, int from, int to, int **matches, int *count)

{
    register int *d = M->d;
    register GAutom G = M->G;
    register int minlen = M->minlen;
    register int r, state;
    register uchar *tt = text + from;
    register int *lrep = M->lrep;
    int i, pos;

    for (i = 0; i < M->cant; i++)
        count[i] = 0;

    r = 0;
    state = 0;
    while (true) {
        state = G[state][tt[r++]];
        if (state > 0) {
            if (G[state][0] != -1) {    /* found */
                i = G[state][0];
                pos = (tt + r) - (uchar *) text;
                if (pos > 0) {
                    if (pos <= to)
                        while (i != -1) {       /* many equal patterns */
                            if ((count[i] % 1000) == 0)
                                matches[i] = realloc (matches[i], sizeof (int) * (count[i] + 1000));
                            matches[i][count[i]] = pos;
                            count[i]++;
                            i = lrep[i];
                        }
                    else
                        break;
                }
            }
        }
        else {
            tt += d[tt[minlen]];
            r = state = 0;
        }
    }
}
