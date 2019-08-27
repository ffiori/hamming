/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/


#include "autom.h"

	/* searches from text with automaton A. */

int searchFAutom (PartAutom *A, register uchar *text, 
		  int from, int to, int *matches)

   { register mask x;
     register mask D = A->D.d0;
     register mask Din = A->Din;
     register mask m2 = A->msk[2];
     register mask m1 = A->msk[1];
     register mask m3 = m1 | m2;
     register mask G = A->Glast.d0;
     register mask Gact = A->Gany.d0;
     register int p0 = A->p[0];
     register int p3 = A->p[3];
     register mask *T = A->T.d1[0];
     register bool *S = A->S;
     register int n = from;
     int count = 0;

        /* k=m-1 not solved with this automaton */
     if (A->k == A->m - 1)
        return search1match (S, text, from, to, matches);

     if (!(~D & Gact)) while (S[text[n]]) n++;

        /* search loop */
     while (true)
         { x = (D >> p0) | T[text[n++]];
           D = ((D << 1) | m1) &
                ((D << p3) | m3) &
                (((x + m1) ^ x) >> 1) &
                Din;
           if (~D & Gact) /* active? */
              { if (!(D & G))  /* found */
	           {    /* clear the last diagonal */
     	             D |= m2;
		        /* invoke code */
                     if (n > 0) /* otherwise it was just retraining */
                        { if (n <= to) 
			     { if (matches != NULL) matches[count] = n;
			       count++;
			     }
                          else break;
                        }
			/* may be inactive now (optional optimization) */
		     if (!(~D & Gact)) while (S[text[n]]) n++;
	           }
	      }
           else while (S[text[n]]) n++;
         }
     A->D.d0 = D;
     return count;
   }

int searchAutom (PartAutom *A, register uchar *text, 
		  int from, int to, int *matches)

   { register mask x;
     register mask *T = A->T.d1[0];
     register mask D = A->D.d0;
     register mask Din = A->Din;
     register mask m2 = A->msk[2];
     register mask m1 = A->msk[1];
     register mask m3 = m1 | m2;
     register mask G = A->Glast.d0;
     register int p0 = A->p[0];
     register int p3 = A->p[3];
     register int n = from;
     int count = 0;

        /* k=m-1 not solved with this automaton */
     if (A->k == A->m - 1)
        return search1match (A->S, text, from, to, matches);

        /* search loop */
     while (true)
         { x = (D >> p0) | T[text[n++]];
           D = ((D << 1) | m1) &
                ((D << p3) | m3) &
                (((x + m1) ^ x) >> 1) &
                Din;
           if (!(D & G))  /* found */
	      {    /* clear the last diagonal */
     	        D |= m2;
	           /* fill match */
                if (n > 0) /* otherwise it was just retraining */
                   { if (n <= to) 
			{ if (matches != NULL) matches[count] = n;
			  count++;
			}
                     else break;
                   }
	      }
         }
     A->D.d0 = D;
     return count;
   }

int searchVAutom (PartAutom *A, register uchar *text, 
		  int from, int to, int *matches)

   { register mask x;
     register mask *T = A->T.d1[0];
     register mask D = A->D.d0;
     register mask Din = A->Din;
     register mask m2 = A->msk[2];
     register mask m1 = A->msk[1];
     register mask m3 = m1 | m2;
     register mask G = A->Glast.d0;
     register int p0 = A->p[0];
     register int p3 = A->p[3];
     register int n = from;
     int count = 0;

        /* k=m-1 not solved with this automaton */
     if (A->k == A->m - 1)
        return search1matchV (A->S, text, from, to, matches);

        /* search loop */
     while (n < to)
         { x = (D >> p0) | T[text[n++]];
           D = ((D << 1) | m1) &
                ((D << p3) | m3) &
                (((x + m1) ^ x) >> 1) &
                Din;
           if (!(D & G))  /* found */
	      {    /* clear the last diagonal */
     	        D |= m2;
	           /* fill match */
                if (n > 0) 
		   { if (matches != NULL) matches[count] = n;
		     count++;
		   }
	      }
         }
     A->D.d0 = D;
     return count;
   }
