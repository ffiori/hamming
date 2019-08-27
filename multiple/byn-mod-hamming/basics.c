/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/

#include <stdio.h>

void *mymalloc (int n)
{
    void *p;

    if (n == 0)
        return NULL;
    p = (void *) malloc (n);
    if (p == NULL) {
        fprintf (stderr, "Out of memory\n");
        exit (1);
    }
    return p;
}

void myfree (void *p)
{
    if (p != NULL)
        free (p);
}

void *myrealloc (void *p, int n)
{
    if (p == NULL)
        return mymalloc (n);
    if (n == 0) {
        myfree (p);
        return NULL;
    }
    p = (void *) realloc (p, n);
    if (p == NULL) {
        fprintf (stderr, "Out of memory\n");
        exit (1);
    }
    return p;
}
