/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/


#ifndef PARTAUTOMH1CINCLUDED
#define PARTAUTOMH1CINCLUDED


#include "partautom.h"

/* F: using S filter
   V: verification (no filter, does not assume pattern at the end)
   X: does not use active columns nor filter
*/


	/* Implementation of horizontal automaton partition (for lc=1).
	   F uses S table. */

int searchPartAutomH1c (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchFPartAutomH1c (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchVPartAutomH1c (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchXPartAutomH1c (PartAutom *M, uchar *text, int from, int to, int *matches);

#endif
