/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/

#include "partautomh1c.h"

int searchFPartAutomH1c (PartAutom * M, register uchar * text, int from, int to, int *matches)
{
    register Tsection *T = M->T.d1;
    register mask *D = M->D.d1;
    register mask *newD = M->newD.d1;
    register mask Din = M->Din;
    register bool *S = M->S;
    register int J = M->J;
    register int active = M->active;
    register mask G = M->Glast.d0;
    register mask x;
    register int j;
    register mask *aux;
    register uchar cc;
    register int n = from;
    int count = 0;

    /* k=m-1 not solved with this automaton */
    if (M->k == M->m - 1)
        return search1match (S, text, from, to, matches);

    /* if in the initial state, search one of the k+1 first chars */
    if ((active == 0) && !(~D[0] & G))
        while (S[text[n]])
            n++;

    /* search loop */
    while (true) {              /* update matrix */
        cc = text[n++];
        for (j = 0; j <= active; j++) {
            x = D[j - 1] | T[j][cc];
            newD[j] = (((D[j] & D[j + 1]) << 1) | 1)
                & (((x + 1) ^ x) >> 1)
                & Din;
        }
        aux = D;
        D = newD;
        newD = aux;

        /* check if lower-right state is active */
        if (D[active] & G)
            /* check if there is an active state */
            while (true)
                if (active == 0) {      /* search new interesting area */
                    while (S[text[n]])
                        n++;
                    break;
                }
                else /* the last column may no longer be active */ if (D[active - 1] & G)
                    /* clear old D (now newD). the last row suffices */
                {
                    newD[active] = Din;
                    active--;
                }
                else
                    break;
        else if (active == J) { /* match found *//* clear last diagonal */
            D[J] = Din;
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
        else                    /* M->active must change */
            active++;
    }
    M->active = active;
    M->D.d1 = D;
    M->newD.d1 = newD;
    return count;
}

int searchXPartAutomH1c (PartAutom * M, register uchar * text, int from, int to, int *matches)
{
    register Tsection *T = M->T.d1;
    register mask *D = M->D.d1;
    register mask *newD = M->newD.d1;
    register mask Din = M->Din;
    register int J = M->J;
    register mask G = M->Glast.d0;
    register mask x;
    register int j;
    register mask *aux;
    register uchar cc;
    register int n = from;
    int count = 0;

    /* k=m-1 not solved with this automaton */
    if (M->k == M->m - 1)
        return search1match (M->S, text, from, to, matches);

    /* search loop */
    while (true) {              /* update matrix */
        cc = text[n++];
        for (j = 0; j <= J; j++) {
            x = D[j - 1] | T[j][cc];
            newD[j] = (((D[j] & D[j + 1]) << 1) | 1)
                & (((x + 1) ^ x) >> 1)
                & Din;
        }
        aux = D;
        D = newD;
        newD = aux;

        /* check if lower-right state is active */
        if (!(D[J] & G)) {      /* clear last diagonal */
            D[J] = Din;
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
    M->D.d1 = D;
    M->newD.d1 = newD;
    return count;
}

int searchPartAutomH1c (PartAutom * M, register uchar * text, int from, int to, int *matches)
{
    register Tsection *T = M->T.d1;
    register mask *D = M->D.d1;
    register mask *newD = M->newD.d1;
    register mask Din = M->Din;
    register int J = M->J;
    register int active = M->active;
    register mask G = M->Glast.d0;
    register mask x;
    register int j;
    register mask *aux;
    register uchar cc;
    register int n = from;
    int count = 0;

    /* k=m-1 not solved with this automaton */
    if (M->k == M->m - 1)
        return search1match (M->S, text, from, to, matches);

    /* search loop */
    while (true) {              /* update matrix */

        if (active == 0) {      /* use first autom as filter */
            register mask D0 = D[0];

            while (true) {
                x = ((mask *) T)[text[n++]];
                D0 = ((D0 << 1) | 1) & (((x + 1) ^ x) >> 1) & Din;
                if (!(D0 & G))
                    break;      /* found */
            }
            D[0] = D0;
            active = 1;
        }

        cc = text[n++];
        for (j = 0; j <= active; j++) {
            x = D[j - 1] | T[j][cc];
            newD[j] = (((D[j] & D[j + 1]) << 1) | 1)
                & (((x + 1) ^ x) >> 1)
                & Din;
        }
        aux = D;
        D = newD;
        newD = aux;

        /* check if lower-right state is active */
        if (D[active] & G)
            /* check if there is an active state */
            while (true)
                if (D[active - 1] & G)
                    /* clear old D (now newD). the last row suffices */
                {
                    newD[active] = Din;
                    active--;
                }
                else
                    break;
        else if (active == J) { /* match found *//* clear last diagonal */
            D[J] = Din;
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
        else                    /* M->active must change */
            active++;
    }
    M->active = active;
    M->D.d1 = D;
    M->newD.d1 = newD;
    return count;
}

int searchVPartAutomH1c (PartAutom * M, register uchar * text, int from, int to, int *matches)
{
    register Tsection *T = M->T.d1;
    register mask *D = M->D.d1;
    register mask *newD = M->newD.d1;
    register mask Din = M->Din;
    register int J = M->J;
    register int active = M->active;
    register mask G = M->Glast.d0;
    register mask x;
    register int j;
    register mask *aux;
    register uchar cc;
    register int n = from;
    int count = 0;

    /* k=m-1 not solved with this automaton */
    if (M->k == M->m - 1)
        return search1matchV (M->S, text, from, to, matches);

    /* search loop */
    while (n < to) {            /* update matrix */

        if (active == 0) {      /* use first autom as filter */
            register mask D0 = D[0];

            while (true) {
                x = ((mask *) T)[text[n++]];
                D0 = ((D0 << 1) | 1) & (((x + 1) ^ x) >> 1) & Din;
                if (!(D0 & G))
                    break;      /* found */
            }
            D[0] = D0;
            active = 1;
        }

        cc = text[n++];
        for (j = 0; j <= active; j++) {
            x = D[j - 1] | T[j][cc];
            newD[j] = (((D[j] & D[j + 1]) << 1) | 1)
                & (((x + 1) ^ x) >> 1)
                & Din;
        }
        aux = D;
        D = newD;
        newD = aux;

        /* check if lower-right state is active */
        if (D[active] & G)
            /* check if there is an active state */
            while (true)
                if (D[active - 1] & G)
                    /* clear old D (now newD). the last row suffices */
                {
                    newD[active] = Din;
                    active--;
                }
                else
                    break;
        else if (active == J) { /* match found *//* clear last diagonal */
            D[J] = Din;
            /* fill match */
            if (n > 0) {
                if (matches != NULL)
                    matches[count] = n;
                count++;
            }
        }
        else                    /* M->active must change */
            active++;
    }
    M->active = active;
    M->D.d1 = D;
    M->newD.d1 = newD;
    return count;
}
