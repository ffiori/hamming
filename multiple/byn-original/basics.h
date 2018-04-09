/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/


#ifndef BASICSINCLUDED
#define BASICSINCLUDED

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

typedef int bool;
#define true 1
#define false 0

typedef unsigned long mask;
typedef unsigned char uchar;

#define Sigma 256			/* alphabet size */
#define W (8*sizeof(mask))		/* word length */

uchar Map[Sigma];			/* character mapping */

void *mymalloc(int n);
void myfree(void *p);
void *myrealloc(void *p, int n);

#define malloc(n) mymalloc(n)
#define free(p) myfree(p)
#define realloc(p,n) myrealloc(p,n)

#endif
