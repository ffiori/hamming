/*
 * Two-way Shift-Add for string matching with k mismatches.
 */

#include <stdio.h>
#include <stdlib.h>

typedef unsigned long word;
#define W (8 * sizeof(word))

static unsigned char *P;
static size_t m;

static unsigned int L;
static word Hbit, Lmask, Hmask, IV, Vmask;
static word B[256];

#if 1
# define popcount __builtin_popcountl
#else
int popcount(word x)
{
    int c =0;
    while (x) {
        x &= (x-1);
        c++;
    }
    return c;
}
#endif

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

    L = k ? clog2(k+1) + 1 : 2;
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

    /* Incidence matrix */
    for (c = 0; c < 256; c++)
        B[c] = Lmask;
    for (i = 0; i < m; i++)
        B[P[i]] ^= (word)1 << i*L;
}

size_t exec(unsigned char *T, size_t n, size_t k)
{
    word D;
    size_t i, j, occ = 0;

    /* TwSA */
    for (i = m-1; i <= n-m; i = i + m) {
        D = IV + B[T[i]];
        for (j = 1; j < m && ~D & Hmask; j++) {
            D = D + ((~D >> (L-1)) & (B[T[i-j]] << j*L))
                  + ((~D >> (L-1)) & (B[T[i+j]] >> j*L));
        }
        if (~D & Hmask) {
            /* non-zero bits in (~D & Hmask) mark matches */
            occ = occ + popcount(~D & Hmask);
        }
    }

    /* Apply TuSA for the last alignments */
    D = ~(word)0;
    for (i = i - (m-1); i < n; i++) {
        D = ((D << L) | (Hbit - (k+1))) + (B[T[i]] & ~(D << 1));
        if (!(D & ((word)1 << (m*L-1)))) {
            /* match at T[i-m+1..i] */
            occ++;
        }
    }

    return occ;
}
