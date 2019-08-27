/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/


#ifndef AUTOMINCLUDED
#define AUTOMINCLUDED

#include "basics.h"
#include "partautom.h"

int searchAutom (PartAutom *A, uchar *text, int from, int to, int *matches);
int searchFAutom (PartAutom *A, uchar *text, int from, int to, int *matches);
int searchVAutom (PartAutom *A, uchar *text, int from, int to, int *matches);

#endif
