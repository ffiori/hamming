/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/



#include "cut.h"

void initCut (int k, int r, int jj, Cut *cut)

   { int i;

     cut->j = jj; cut->k = k/jj;

          /* Partition in subpatterns */
     cut->aclen = ((int *) malloc ((1+r*jj)*sizeof(int)))+1;
     cut->len = (int*) malloc (r*jj*sizeof(int));
     cut->ps = (uchar**) malloc (r*jj*sizeof(uchar*));
     cut->aclen[-1] = 0;
     cut->r = 0;
   }

void addCut (uchar *pattern, int m, Cut *cut)

   { int i,r=cut->r,al;
     int mdj,msj,mrj;

     mdj = m/cut->j; msj = (m+cut->j-1)/cut->j; mrj = m-cut->j*mdj;

          /* Partition in subpatterns */
     for (i=0;i<mrj;i++) cut->len[r+i] = msj;
     for (i=mrj;i<cut->j;i++) cut->len[r+i] = mdj;
     al = cut->aclen[r-1];
     cut->aclen[r-1] = 0;
     for (i=0;i<cut->j;i++)
         { cut->aclen[r+i] = cut->aclen[r+i-1]+cut->len[r+i];
           cut->ps[r+i] = (uchar*) malloc (cut->len[r+i]+1);
           strncpy (cut->ps[r+i],pattern+cut->aclen[r+i-1],cut->len[r+i]);
           cut->ps[r+i][cut->len[r+i]] = 0;
         }
     cut->aclen[r-1] = al;
     cut->r += cut->j;
   }

void makeCut (uchar *pattern, int m, int k, int jj, Cut *cut)

   { int i;
     int mdj,msj,mrj;

     cut->j = jj; cut->k = k/jj;
     mdj = m/jj; msj = (m+jj-1)/jj; mrj = m-jj*mdj;

          /* Partition in subpatterns */
     cut->aclen = ((int *) malloc ((1+jj)*sizeof(int)))+1;
     cut->len = (int*) malloc (jj*sizeof(int));
     cut->ps = (uchar**) malloc (jj*sizeof(uchar*));
     for (i=0;i<mrj;i++) cut->len[i] = msj;
     for (i=mrj;i<jj;i++) cut->len[i] = mdj;
     cut->aclen[-1] = 0;
     for (i=0;i<jj;i++)
         { cut->aclen[i] = cut->aclen[i-1]+cut->len[i];
           cut->ps[i] = (uchar*) malloc (cut->len[i]+1);
           strncpy (cut->ps[i],pattern+cut->aclen[i-1],cut->len[i]);
           cut->ps[i][cut->len[i]] = 0;
         }
   }

void freeCut (Cut *cut)

   { int i;
     for (i=0; i<cut->j; i++)
        free (cut->ps[i]);
     free (cut->aclen-1); cut->aclen = NULL;
     free (cut->len); cut->len = NULL;
     free (cut->ps); cut->ps = NULL;
   }


