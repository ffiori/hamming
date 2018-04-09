/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/


#ifndef PARTAUTOMHV1CINCLUDED
#define PARTAUTOMHV1CINCLUDED


#include "partautom.h"

/* F: using S filter
   V: verification (no filter, does not assume pattern at the end)
   X: does not use active columns nor filter
*/


	/* Implementation of generalized automaton partition for lc=1.
	   F uses S table */

int searchPartAutomHV1c (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchFPartAutomHV1c (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchVPartAutomHV1c (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchXPartAutomHV1c (PartAutom *M, uchar *text, int from, int to, int *matches);

#endif
