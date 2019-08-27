/*
 * Approximate Boyer-Moore for string matching with k mismatches.
 */

#include <stdio.h>
#include <stdlib.h>

typedef unsigned long word;
#define W (8 * sizeof(word))

static unsigned char *P;
static size_t m;

static size_t D[1024][256] = {{0}};

void prep(unsigned char *P_, size_t m_, size_t k)
{
    int c;
    size_t i, s;
    P = P_; m = m_;

    if (m > 1024) {
        fprintf(stderr, "m=%ld exceeds the limit M=1024\n", (long)m);
        exit(EXIT_FAILURE);
    }

    P--; /* P = P[1..m] */
    for (i = m-k; i <= m; i++) {
        for (c = 0; c < 256; c++) {
            for (s = 1; s < i; s++) {
                if (P[i-s] == c)
                    break;
            }
            D[i][c] = s;
        }
    }
}

size_t exec(unsigned char *T, size_t n, size_t k)
{
    size_t i, j, d, h, neq, occ = 0;

    T--; /* T = T[1..n] */
    j = m;
    while (j <= n) {
        neq = 0;
        d = m-k;
        for (i = m, h = j; i > 0 && neq <= k; i--, h--) {
            if (i >= m-k && D[i][T[h]] < d)
                d = D[i][T[h]];
            if (T[h] != P[i])
                neq++;
        }
        if (neq <= k) {
            /* match at T[j-m+1..j] */
            occ++;
        }
        j = j + d;
    }

    return occ;
}
