/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/

void *A[10000];
int S[10000];
int N = 0;

void ERROR (void)
{
    printf ("heap error!!\n");
    fflush (stdout);
    exit (1);
}

void *dumpHeap ()
{
    int i;
    int tot;

    for (i = 0; i < N; i++) {
        printf ("%x: %i\n", A[i], S[i]);
        tot += S[i];
    }
    printf ("Total: %i\n", tot);
}

void *Malloc (int n)
{
    if (n == 0)
        return NULL;
    A[N] = (void *) malloc (n);
    S[N++] = n;
    if (N == 10000) {
        printf ("heap excedio capacidad\n");
        ERROR ();
    }
    if (A[N - 1] == NULL)
        ERROR ();
    return A[N - 1];
}

void Free (void *p)
{
    int i;

    if (p == NULL)
        return;
    for (i = 0; i < N; i++)
        if (A[i] == p) {
            free (p);
            A[i] = A[--N];
            return;
        }
    ERROR ();
}

void *Realloc (void *p, int n)
{
    int i;

    if (p == NULL)
        return Malloc (n);
    if (n == 0) {
        Free (p);
        return NULL;
    }
    for (i = 0; i < N; i++)
        if (A[i] == p) {
            A[i] = (void *) realloc (p, n);
            S[i] = n;
            if (A[i] == NULL)
                ERROR ();
            return A[i];
        }
    ERROR ();
}

void *Access (void *p, int n)
{
    int i;

    for (i = 0; i < N; i++)
        if ((A[i] <= p) && (S[i] + ((int) A[i]) >= n + ((int) p))) {
            return;
        }
    ERROR ();
}
