/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/

#include "partmixed.h"
#include "cut.h"

static void buildV (PartMixedV * P, Cut * cut, uchar * pattern, int m, int k, int j, int *ver, int offset, int c)
{
    P->large = (j > 1);
    P->m = m;
    P->k = k;
    P->offset = m + k + offset;
    prepPartAutom (&P->dynaut, pattern + ver[0], m, k, 1, true);
    if (P->large) {             /* compound */
        P->one = (void *) malloc (sizeof (PartMixedV));
        P->two = (void *) malloc (sizeof (PartMixedV));
        buildV (P->one, cut, pattern, ver[j / 2] - ver[0], ((j / 2) * k) / j, j / 2, ver, 0, c);
        buildV (P->two, cut, pattern, ver[j] - ver[j / 2], ((j - j / 2) * k) / j, j - j / 2, ver + j / 2, ver[j / 2] - ver[0], c);
    }
}

int prepPartMixed (PartMixed * P, uchar ** pattern, int r, int m, int k, int jj, int rp, int c)
{
    int rr, d, j, p, i, ksj, jjj = jj;
    Cut cut;
    int *ver;

    /* Cut pattern */
    initCut (k, r, jj, &cut);
    for (rr = 0; rr < r; rr++)
        addCut (pattern[rr], m, &cut);

    ksj = k / jj;

    /* Adjust the superposition */

    jj *= r;
    if (rp == 0)
        rp = 1;
    if (rp > jj)
        rp = jj;
    j = (jj + rp - 1) / rp;
    rp = jj / j;
    d = jj - j * rp;

    /* Initialize subproblems */

    P->D = malloc (j * sizeof (PartAutom));
    P->vfrom = malloc (r * sizeof (PartAutom));
    P->vto = malloc (r * sizeof (PartAutom));
    for (rr = 0; rr < r; rr++) {
        P->vfrom[rr] = j;
        P->vto[rr] = 0;
    }
    p = 0;
    for (i = 0; i < j; i++) {
        prepSPartAutom (P->D + i, cut.ps + p, rp + (i < d), ksj, c, false);
        rr = p / jjj;
        p += rp + (i < d);
        while (rr < (p - 1 + jjj - 1) / jjj) {
            if (P->vfrom[rr] > i)
                P->vfrom[rr] = i;
            if (P->vto[rr] < i)
                P->vto[rr] = i;
            rr++;
        }
    }
    freeCut (&cut);
    jj = jjj;
    P->j = j;
    P->r = r;

    /* Prepare verification */

    ver = (int *) malloc ((jj + 1) * sizeof (int));
    P->V = malloc (r * sizeof (PartMixedV));
    for (j = 0; j < r; j++) {
        makeCut (pattern[j], m, k, jj, &cut);
        for (i = 0; i < jj; i++)
            ver[i] = cut.aclen[i - 1];
        ver[jj] = m;
        buildV (P->V + j, &cut, pattern[j], m, k, jj, ver, 0, c);
        freeCut (&cut);
    }
    free (ver);
    return rp + (d > 0);
}

static void initPartMixedV (PartMixedV * P)
{
    initPartAutom (&P->dynaut);
    if (P->large) {
        initPartMixedV (P->one);
        initPartMixedV (P->two);
    }
}

void initPartMixed (PartMixed * P)
{
    int i;

    for (i = 0; i < P->j; i++)
        initPartAutom (P->D + i);
    for (i = 0; i < P->r; i++)
        initPartMixedV (P->V + i);
}

static void freePartMixedV (PartMixedV * P)
{
    if (P->large) {
        freePartMixedV (P->one);
        freePartMixedV (P->two);
        free (P->one);
        free (P->two);
    }
    freePartAutom (&P->dynaut);
}

void freePartMixed (PartMixed * P)
{
    int i;

    for (i = 0; i < P->j; i++)
        freePartAutom (P->D + i);
    for (i = 0; i < P->r; i++)
        freePartMixedV (P->V + i);
    free (P->D);
    free (P->V);
    free (P->vfrom);
    free (P->vto);
}

int searchPartMixedV (PartMixedV * P, register uchar * text, int from, int to, int *matches, int *lmatches, int lcount)
{
    register int i1, i2;
    register int *m1, *m2;
    register int c1, c2;
    register int o1, o2;
    register int nff, ff, ff1, ff2, tt;
    register int mPk = P->m + P->k;
    int count = 0;

    if (P->large) {             /* integrate subsearches */
        m1 = (int *) malloc ((to - from) * sizeof (int));
        c1 = searchPartMixedV (P->one, text + from, 0, to - from, m1, lmatches, lcount);
        m1 = (int *) realloc (m1, c1 * sizeof (int));
        m2 = (int *) malloc ((to - from) * sizeof (int));
        c2 = searchPartMixedV (P->two, text + from, 0, to - from, m2, lmatches, lcount);
        m2 = (int *) realloc (m2, c2 * sizeof (int));
        o1 = mPk - from;
        o2 = mPk - from;
        i1 = 0;
        if (i1 < c1)
            ff1 = m1[i1++] - o1;
        else
            ff1 = to + 1;
        i2 = 0;
        if (i2 < c2)
            ff2 = m2[i2++] - o2;
        else
            ff2 = to + 1;
        if (ff1 < ff2) {
            ff = ff1;
            if (i1 < c1)
                ff1 = m1[i1++] - o1;
            else
                ff1 = to + 1;
        }
        else {
            ff = ff2;
            if (i2 < c2)
                ff2 = m2[i2++] - o2;
            else
                ff2 = to + 1;
        }
        while (ff <= to) {
            tt = ff + 2 * mPk;
            if (tt > to)
                tt = to;
            while (true) {
                if (ff1 < ff2) {
                    nff = ff1;
                    if (i1 < c1)
                        ff1 = m1[i1++] - o1;
                    else
                        ff1 = to + 1;
                }
                else {
                    nff = ff2;
                    if (i2 < c2)
                        ff2 = m2[i2++] - o2;
                    else
                        ff2 = to + 1;
                }
                if (nff <= tt) {
                    tt = nff + 2 * mPk;
                    if (tt > to)
                        tt = to;
                }
                else
                    break;
            }
            initPartAutom (&P->dynaut);
            count += searchPartAutom (&P->dynaut, text, ff, tt, matches == NULL ? NULL : matches + count);
            ff = nff;
        }
        free (m1);
        free (m2);
        return count;
    }
    else {                      /* simple search or superimposed + verification */
        m1 = lmatches;
        c1 = lcount;
        i1 = 0;
        o1 = mPk + from;
        o2 = mPk - from;
        while (i1 < c1) {
            ff = m1[i1] - o2;
            tt = m1[i1] + o1;
            while (++i1 < c1) {
                if (m1[i1] - o2 <= tt)
                    tt = m1[i1] + o1;
                else
                    break;
            }
            if (tt > to)
                tt = to;
            initPartAutom (&P->dynaut);
            count += searchPartAutom (&P->dynaut, text, ff, tt, matches == NULL ? NULL : matches + count);
        }
        return count;
    }
}

int searchPartMixed (PartMixed * P, uchar * text, int from, int to, int *matches)
{
    int i, j;
    int count = 0;
    int **lmatches, *matches1;
    int *lcount, count1, *pos;

    lmatches = (int **) malloc (P->j * sizeof (int *));
    lcount = (int *) malloc (P->j * sizeof (int));
    for (i = 0; i < P->j; i++) {
        lmatches[i] = (int *) malloc ((to - from) * sizeof (int));
        lcount[i] = searchPartAutom (P->D + i, text, from, to, lmatches[i]);
        lmatches[i] = realloc (lmatches[i], lcount[i] * sizeof (int));
    }

    count = 0;
    matches1 = (int *) malloc ((to - from) * sizeof (int));
    pos = (int *) malloc (P->j * sizeof (int));
    for (j = 0; j < P->r; j++) {
        int r;
        int **lmatches1 = lmatches + P->vfrom[j];
        int *lcount1 = lcount + P->vfrom[j];
        int top = P->vto[j] - P->vfrom[j] + 1;
        int min, rmin;

        count1 = 0;
        for (r = 0; r < P->j; r++)
            pos[r] = 0;
        while (1) {
            min = to + 1;
            for (r = 0; r < top; r++) {
                if ((pos[r] < lcount1[r]) && (lmatches1[r][pos[r]] < min)) {
                    min = lmatches1[r][pos[r]];
                    rmin = r;
                }
            }
            if (min > to)
                break;
            if ((count1 == 0) || (min != matches1[count1 - 1]))
                matches1[count1++] = lmatches1[rmin][pos[rmin]];
            pos[rmin]++;
        }
        count += searchPartMixedV (P->V + j, text, from, to, matches == NULL ? NULL : matches + count, matches1, count1);
    }

    for (i = 0; i < P->j; i++)
        free (lmatches[i]);
    free (lmatches);
    free (lcount);
    free (matches1);
    free (pos);
    return count;
}
