/*
 * Approximate SBNDM for string matching with k mismatches.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned long word;
#define W (8 * sizeof(word))

static unsigned char *P;
static size_t m;

static word E, B[256];

/* Verify an occurrence allowing k mismatches */
static int verify(unsigned char *S, size_t k)
{
    size_t i, neq = 0;
    for (i = 0; i < m; i++) {
        if (S[i] != P[i] && ++neq > k)
            return 0;
    }
    return 1;
}

void prep(unsigned char *P_, size_t m_, size_t k)
{
    size_t i;
    P = P_; m = m_; (void)k;

    if (m+1 > W) {
        fprintf(stderr, "need m+1=%ld > %d=W bits\n", (long)m+1, (int)W);
        exit(EXIT_FAILURE);
    }

    /* Initialization vector */
    E = ~(word)0 >> (W-m-1);

    /* Incidence matrix */
    memset(B, 0, sizeof(B));
    for (i = 0; i < m; i++)
        B[P[i]] |= (word)1 << i;
}

size_t exec(unsigned char *T, size_t n, size_t k)
{
    word D;
    size_t i, j, e, occ = 0;

    i = 0;
    while (i <= n-m) {
        D = E;
        e = j = 0;
        while (e <= k && j < m) {
            j = j + 1;
            D = (D >> 1) & B[T[i+m-j]];
            if (D == 0) {
                e = e + 1;
                D = E;
            }
        }
        if (j == m && e <= k) {
            if (verify(&T[i], k)) {
                /* match at T[i..i+m-1] */
                occ++;
            }
        }
        i = i + (m-j+1);
    }

    return occ;
}
