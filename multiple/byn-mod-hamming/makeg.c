/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/

/*
 * Construccion de la funcion GoTo.
 */

                                    /* typedef int (*GAutom)[Sigma]; *//* en el char 0 guarda el Out */

int newstate;

void Enter (GAutom G, uchar * pat, int num, int *lrep)
{
    int state = 0, j;

    /*
     * Avanzamos en el automata segun el patron.
     */

    for (j = 0; pat[j] && (G[state][pat[j]] != -1); j++)
        state = G[state][pat[j]];

    /*
     * Insertamos solo si no existia este patron.
     */

    while (pat[j]) {
        G[state][pat[j++]] = ++newstate;
        state = newstate;
    }
    *lrep = G[state][0];
    G[state][0] = num;
}

GAutom MakeG (uchar ** Ps, int jj, int m, int *lrep)
{
    int i, j;
    GAutom G = (GAutom) malloc ((m + 1) * sizeof (*G));

    newstate = 0;               /* here, to allow reuse! */

    for (i = 0; i <= m; i++) {
        G[i][0] = -1;
        for (j = 1; j < Sigma; j++)
            G[i][j] = -1;
    }

    for (i = 0; i < jj; i++)
        Enter (G, Ps[i], i, &lrep[i]);

    /* ommited, since we use it as a trie
       for ( i = 1; i < Sigma; i++ )
       if (G[0][i] == -1) G[0][i] = 0;
     */

    /* character mapping */
    for (i = 0; i <= m; i++) {
        for (j = 1; j < Sigma; j++)
            if (j != Map[j])
                G[i][j] = G[i][Map[j]];
    }

    return G;
}
