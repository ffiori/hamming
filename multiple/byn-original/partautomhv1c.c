/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/


#include "partautomhv1c.h"


int searchFPartAutomHV1c (PartAutom *M, register uchar *text,
			   int from, int to, int *matches)

   { register Tsection **T = M->T.d2;
     register mask **D = M->D.d2;
     register mask **newD = M->newD.d2;
     register mask Din = M->Din;
     register bool *S = M->S;
     register int I = M->I,J = M->J;
     register int active = M->active;
     register int p1 = M->p[1];
     register mask m0 = M->msk[0];
     register mask G = M->Glast.d0;      
     register mask x;
     register int i,j;
     register mask **aux;
     register uchar cc;
     register int n = from;
     int count = 0;

        /* k=m-1 not solved with this automaton */
     if (M->k == M->m - 1)
        return search1match (S, text, from, to, matches);

        /* if in the initial state, search one of the k+1 first chars */
     if ((active == 0) && !(~D[0][I] & G))
        while (S[text[n]]) n++;

        /* search loop */
     while (true)
        {       /* update matrix */
          cc = text[n++];
          for (j=0; j<=active; j++)
             for (i=0; i<=I; i++)
                { x = (D[j-1][i] | T[j][i][cc]) &
                      ((newD[j][i-1] >> p1) | m0);
                  newD[j][i] = (((D[j][i] & D[j+1][i]) << 1) |
				   ((D[j][i-1] & D[j+1][i-1]) >> p1))
                               & (((x + 1) ^ x) >> 1)
                               & Din;
                }
          aux = D; D = newD; newD = aux;

                /* check if lower-right state is active */
          if (D[active][I] & G)
                /* check if there is an active state */
              while (true)
                 if (active == 0)  /* search new interesting area */
                    { while (S[text[n]]) n++; break; }
                 else   /* the last column may no longer be active */
                    if (D[active-1][I] & G)
                         /* clear old D (now newD). the last row suffices */
                         { newD[active][I] = Din;
                           active--;
                         }
		    else break;
	  else
             if (active == J)   /* match found */
                {    /* clear last diagonal */
		  for (i=0;i<=I;i++)
		     D[J][i] = Din;
                        /* fill match */
                  if (n > 0) /* otherwise it was just retraining */
                     { if (n <= to) 
                          { if (matches != NULL) matches[count] = n;
                            count++;
                          }
                       else break;
                     }
		}
             else   /* M->active must change */
                  active++;
       }
     M->active = active;
     M->D.d2 = D; M->newD.d2 = newD;
     return count;
   }

int searchXPartAutomHV1c (PartAutom *M, register uchar *text,
			  int from, int to, int *matches)

   { register Tsection **T = M->T.d2;
     register mask **D = M->D.d2;
     register mask **newD = M->newD.d2;
     register mask Din = M->Din;
     register int I = M->I,J = M->J;
     register int p1 = M->p[1];
     register mask m0 = M->msk[0];
     register mask G = M->Glast.d0;      
     register mask x;
     register int i,j;
     register mask **aux;
     register uchar cc;
     register int n = from;
     int count = 0;

        /* k=m-1 not solved with this automaton */
     if (M->k == M->m - 1)
        return search1match (M->S, text, from, to, matches);

        /* search loop */
     while (true)
        {       /* update matrix */
          cc = text[n++];
          for (j=0; j<=J; j++)
             for (i=0; i<=I; i++)
                { x = (D[j-1][i] | T[j][i][cc]) &
                      ((newD[j][i-1] >> p1) | m0);
                  newD[j][i] = (((D[j][i] & D[j+1][i]) << 1) |
				   ((D[j][i-1] & D[j+1][i-1]) >> p1))
                               & (((x + 1) ^ x) >> 1)
                               & Din;
                }
          aux = D; D = newD; newD = aux;

                /* check if lower-right state is active */
          if (!(D[J][I] & G))
                {    /* clear last diagonal */
		  for (i=0;i<=I;i++)
		     D[J][i] = Din;
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
     M->D.d2 = D; M->newD.d2 = newD;
     return count;
   }

int searchPartAutomHV1c (PartAutom *M, register uchar *text,
			  int from, int to, int *matches)

   { register Tsection **T = M->T.d2;
     register mask **D = M->D.d2;
     register mask **newD = M->newD.d2;
     register mask Din = M->Din;
     register int I = M->I,J = M->J;
     register int active = M->active;
     register int p1 = M->p[1];
     register mask m0 = M->msk[0];
     register mask G = M->Glast.d0;      
     register mask x;
     register int i,j;
     register mask **aux;
     register uchar cc;
     register int n = from;
     int count = 0;

        /* k=m-1 not solved with this automaton */
     if (M->k == M->m - 1)
        return search1match (M->S, text, from, to, matches);

        /* search loop */
     while (true)
        {       /* update matrix */
          cc = text[n++];
          for (j=0; j<=active; j++)
             for (i=0; i<=I; i++)
                { x = (D[j-1][i] | T[j][i][cc]) &
                      ((newD[j][i-1] >> p1) | m0);
                  newD[j][i] = (((D[j][i] & D[j+1][i]) << 1) |
				   ((D[j][i-1] & D[j+1][i-1]) >> p1))
                               & (((x + 1) ^ x) >> 1)
                               & Din;
                }
          aux = D; D = newD; newD = aux;

                /* check if lower-right state is active */
          if (D[active][I] & G)
                /* check if there is an active state */
              while (true)
                 if (D[active-1][I] & G)
                       /* clear old D (now newD). the last row suffices */
                    { newD[active][I] = Din;
                      active--;
                    }
		 else break;
	  else
             if (active == J)   /* match found */
                {    /* clear last diagonal */
		  for (i=0;i<=I;i++)
		     D[J][i] = Din;
                        /* fill match */
                  if (n > 0) /* otherwise it was just retraining */
                     { if (n <= to) 
                          { if (matches != NULL) matches[count] = n;
                            count++;
                          }
                       else break;
                     }
		}
             else   /* M->active must change */
                  active++;
       }
     M->active = active;
     M->D.d2 = D; M->newD.d2 = newD;
     return count;
   }

int searchVPartAutomHV1c (PartAutom *M, register uchar *text,
			  int from, int to, int *matches)

   { register Tsection **T = M->T.d2;
     register mask **D = M->D.d2;
     register mask **newD = M->newD.d2;
     register mask Din = M->Din;
     register int I = M->I,J = M->J;
     register int active = M->active;
     register int p1 = M->p[1];
     register mask m0 = M->msk[0];
     register mask G = M->Glast.d0;      
     register mask x;
     register int i,j;
     register mask **aux;
     register uchar cc;
     register int n = from;
     int count = 0;

        /* k=m-1 not solved with this automaton */
     if (M->k == M->m - 1)
        return search1matchV (M->S, text, from, to, matches);

        /* search loop */
     while (n < to)
        {       /* update matrix */
          cc = text[n++];
          for (j=0; j<=active; j++)
             for (i=0; i<=I; i++)
                { x = (D[j-1][i] | T[j][i][cc]) &
                      ((newD[j][i-1] >> p1) | m0);
                  newD[j][i] = (((D[j][i] & D[j+1][i]) << 1) |
				   ((D[j][i-1] & D[j+1][i-1]) >> p1))
                               & (((x + 1) ^ x) >> 1)
                               & Din;
                }
          aux = D; D = newD; newD = aux;

                /* check if lower-right state is active */
          if (D[active][I] & G)
                /* check if there is an active state */
              while (true)
                 if (D[active-1][I] & G)
                       /* clear old D (now newD). the last row suffices */
                    { newD[active][I] = Din;
                      active--;
                    }
		 else break;
	  else
             if (active == J)   /* match found */
                {    /* clear last diagonal */
		  for (i=0;i<=I;i++)
		     D[J][i] = Din;
                        /* fill match */
                  if (n > 0) 
                          { if (matches != NULL) matches[count] = n;
                            count++;
                          }
		}
             else   /* M->active must change */
                  active++;
       }
     M->active = active;
     M->D.d2 = D; M->newD.d2 = newD;
     return count;
   }

