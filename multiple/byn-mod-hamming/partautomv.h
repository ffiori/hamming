/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/

#ifndef PARTAUTOMVINCLUDED
#define PARTAUTOMVINCLUDED

#include "partautom.h"

        /* Implementation of vertical automaton partition.
           F uses S table */

int searchPartAutomV (PartAutom * M, uchar * text, int from, int to, int *matches);
int searchFPartAutomV (PartAutom * M, uchar * text, int from, int to, int *matches);
int searchVPartAutomV (PartAutom * M, uchar * text, int from, int to, int *matches);

#endif
