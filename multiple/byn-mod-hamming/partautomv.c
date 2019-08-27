/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/

#include "partautomv.h"

int searchFPartAutomV (PartAutom * M, register uchar * text, int from, int to, int *matches)
{
    register Tsection *T = M->T.d1;
    register mask *D = M->D.d1;
    register mask Din = M->Din;
    register bool *S = M->S;
    register int I = M->I;
    register int p0 = M->p[0], p1 = M->p[1], p3 = M->p[3];
    register mask m0 = M->msk[0], m1 = M->msk[1], m2 = M->msk[2];
    register mask Glast = M->Glast.d0, Gany = M->Gany.d0;
    register mask x;
    register mask pD, aD, pnD;
    register int i;
    register uchar cc;
    register int n = from;
    int count = 0;

    /* k=m-1 not solved with this automaton */
    if (M->k == M->m - 1)
        return search1match (S, text, from, to, matches);

    /* if in the initial state, search one of the k+1 first chars */
    if (!(~D[I] & Gany))
        while (S[text[n]])
            n++;

    /* search loop */
    while (true) {              /* update matrix */
        cc = text[n++];
        pD = pnD = Din;
        for (i = 0; i <= I; i++) {
            aD = D[i];
            x = ((aD >> p0) | T[i][cc]) & ((pnD >> p1) | m0);
            pnD = D[i] = ((aD << 1) | ((pD >> p1) & m1))
                & ((aD << p3) | ((pD << 2) & m1) | m2)
                & (((x + m1) ^ x) >> 1)
                & Din;
            pD = aD;
        }

        /* check if lower-right state is active */
        if (pnD & Glast)
            /* check if there is an active state */
        {
            if (!(~D[I] & Gany))
                while (S[text[n]])
                    n++;
        }
        else {                  /* match found */
            /* clear last diagonal */
            for (i = 0; i <= I; i++)
                D[i] |= m2;
            /* fill match */
            if (n > 0) {        /* otherwise it was just retraining */
                if (n <= to) {
                    if (matches != NULL)
                        matches[count] = n;
                    count++;
                }
                else
                    break;
            }
        }
    }
    return count;
}

int searchPartAutomV (PartAutom * M, register uchar * text, int from, int to, int *matches)
{
    register Tsection *T = M->T.d1;
    register mask *D = M->D.d1;
    register mask Din = M->Din;
    register int I = M->I;
    register int p0 = M->p[0], p1 = M->p[1], p3 = M->p[3];
    register mask m0 = M->msk[0], m1 = M->msk[1], m2 = M->msk[2];
    register mask Glast = M->Glast.d0;
    register mask x;
    register mask pD, aD, pnD;
    register int i;
    register uchar cc;
    register int n = from;
    int count = 0;

    /* k=m-1 not solved with this automaton */
    if (M->k == M->m - 1)
        return search1match (M->S, text, from, to, matches);

    /* search loop */
    while (true) {              /* update matrix */
        cc = text[n++];
        pD = pnD = Din;
        for (i = 0; i <= I; i++) {
            aD = D[i];
            x = ((aD >> p0) | T[i][cc]) & ((pnD >> p1) | m0);
            pnD = D[i] = ((aD << 1) | ((pD >> p1) & m1))
                & ((aD << p3) | ((pD << 2) & m1) | m2)
                & (((x + m1) ^ x) >> 1)
                & Din;
            pD = aD;
        }

        /* check if lower-right state is active */
        if (!(pnD & Glast))
            /* match found */
        {                       /* clear last diagonal */
            for (i = 0; i <= I; i++)
                D[i] |= m2;
            /* fill match */
            if (n > 0) {        /* otherwise it was just retraining */
                if (n <= to) {
                    if (matches != NULL)
                        matches[count] = n;
                    count++;
                }
                else
                    break;
            }
        }
    }
    return count;
}

int searchVPartAutomV (PartAutom * M, register uchar * text, int from, int to, int *matches)
{
    register Tsection *T = M->T.d1;
    register mask *D = M->D.d1;
    register mask Din = M->Din;
    register int I = M->I;
    register int p0 = M->p[0], p1 = M->p[1], p3 = M->p[3];
    register mask m0 = M->msk[0], m1 = M->msk[1], m2 = M->msk[2];
    register mask Glast = M->Glast.d0;
    register mask x;
    register mask pD, aD, pnD;
    register int i;
    register uchar cc;
    register int n = from;
    int count = 0;

    /* k=m-1 not solved with this automaton */
    if (M->k == M->m - 1)
        return search1matchV (M->S, text, from, to, matches);

    /* search loop */
    while (n < to) {            /* update matrix */
        cc = text[n++];
        pD = pnD = Din;
        for (i = 0; i <= I; i++) {
            aD = D[i];
            x = ((aD >> p0) | T[i][cc]) & ((pnD >> p1) | m0);
            pnD = D[i] = ((aD << 1) | ((pD >> p1) & m1))
                & ((aD << p3) | ((pD << 2) & m1) | m2)
                & (((x + m1) ^ x) >> 1)
                & Din;
            pD = aD;
        }

        /* check if lower-right state is active */
        if (!(pnD & Glast))
            /* match found */
        {                       /* clear last diagonal */
            for (i = 0; i <= I; i++)
                D[i] |= m2;
            /* fill match */
            if (n > 0) {
                if (matches != NULL)
                    matches[count] = n;
                count++;
            }
        }
    }
    return count;
}
