/*
 * Approximate BNDM for string matching with k mismatches.
 * Mismatch counting method based on Shift-Add.
 */

#include <stdio.h>
#include <stdlib.h>

typedef unsigned long word;
#define W (8 * sizeof(word))

static unsigned char *P;
static size_t m;

static unsigned int L;
static word Hbit, Lmask, Hmask, IV, Vmask, lim;
static word B[256];

/* ceil(log2(x)) */
static unsigned int clog2(size_t x)
{
    unsigned int i;
    for (i = 0; ((size_t)1 << i) < x; i++);
    return i;
}

void prep(unsigned char *P_, size_t m_, size_t k)
{
    int c;
    size_t i;
    P = P_; m = m_;

    L = clog2(k+1) + 1;
    if (m*L > W) {
        fprintf(stderr, "need m*L=%ld > %d=W bits\n", (long)(m*L), (int)W);
        exit(EXIT_FAILURE);
    }

    /* Initialize constants */
    Hbit = (word)1 << (L-1);
    Lmask = Hmask = IV = 0;
    for (i = 0; i < m; i++) {
        Lmask = (Lmask << L) | 1;
        Hmask = (Hmask << L) | Hbit;
        IV = (IV << L) | (Hbit - (k+1));
    }
    Vmask = Hmask | (Hmask - Lmask);
    lim = Hmask ^ (Hmask >> L);

    /* Incidence matrix */
    for (c = 0; c < 256; c++)
        B[c] = Lmask;
    for (i = 0; i < m; i++)
        B[P[m-1 - i]] ^= (word)1 << i*L;
}

size_t exec(unsigned char *T, size_t n, size_t k)
{
    word D, O;
    size_t j, pos, last, occ = 0; (void)k;

    T--; /* T = T[1..n] */
    pos = 0;
    while (pos <= n-m) {
        D = IV;
        O = 0;
        j = last = m;
        while (~(D | O) & Hmask) {
            D = D + B[T[pos+j]];
            j = j - 1;
            if (((D | O) & lim) == 0) {
                if (j > 0) {
                    last = j;
                } else {
                    /* match at T[pos+1..pos+m] */
                    occ++;
                }
            }
            D = (D << L) | Hbit;
            O = (O << L) | (D & Hmask);
            D = D & ~Hmask;
        }
        pos = pos + last;
    }

    return occ;
}
