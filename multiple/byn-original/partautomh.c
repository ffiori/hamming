/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/


#include "partautomh.h"

int searchFPartAutomH (PartAutom *M, register uchar *text, int from, int to, 
		      int *matches)

   { register Tsection *T = M->T.d1;
     register mask *D = M->D.d1;
     register mask *newD = M->newD.d1;
     register mask Din = M->Din;
     register bool *S = M->S;
     register int J = M->J;
     register int active = M->active;
     register int p0 = M->p[0],p2 = M->p[2],p3=M->p[3],p5=M->p[5];
     register mask m1 = M->msk[1],m2 = M->msk[2],m3 = m1|((1 << M->lr) - 1);
     register mask *Glast = M->Glast.d1,*Gany = M->Gany.d1;
     register mask G0 = Gany[0], Gl0 = Glast[0];
     register mask x;
     register int j;
     register mask *aux;
     register uchar cc;
     register int n = from;
     int count = 0;

	/* k=m-1 not solved with this automaton */
     if (M->k == M->m - 1)
	return search1match (S, text, from, to, matches);


        /* if in the initial state, search one of the k+1 first chars */
     if ((active == 0) && !(~D[0] & Gany[0]))
        while (S[text[n]]) n++;

        /* search loop */
     while (true)
        {       /* update matrix */

	  if (active == 0)  /* use first autom as filter */
	     { register mask D0 = D[0];
               while (true)
		 { x = (D0 >> p0) | ((mask*)T)[text[n++]];
                   D0 = ((D0 << 1) | m1) &
                        ((D0 << p3) | m3) &
                        (((x + m1) ^ x) >> 1) &
                        Din;
		   if (~D0 & G0) /* active? */
		      { if (!(D0 & Gl0)) break; /* found */ }
		   else while (S[text[n]]) n++;
		 }
	       D[0] = D0;
	       active = 1;
             }

          cc = text[n++];
          for (j=0; j<=active; j++)
              { x = (D[j] >> p0) | (D[j-1] << p2) | T[j][cc];
                newD[j] = (((D[j] << 1) &
                              ((D[j] << p3) |
                               (D[j+1] >> p5))) | m1)
                          & (((x + m1) ^ x) >> 1)
                          & Din;
                }
          aux = D; D = newD; newD = aux;

                /* check if lower-right state is active */
          if (D[active] & Glast[active])
                /* check if there is an active state */
              while (!(~D[active] & Gany[active]))
                 if (active == 0)  /* search new interesting area */
                    { while (S[text[n]]) n++; break; }
                 else   /* the last column may no longer be active */
                    { if (D[active-1] & Glast[active-1])
                         /* clear old D (now newD). the last row suffices */
                         { newD[active] = Din;
                           active--;
                         }
		      else break;
                    }
         else
             if (active == J)   /* match found */
                {  /* clear last diagonal */
     		  D[J] |= m2;
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
     M->D.d1 = D; M->newD.d1 = newD;
     return count;
   }

int searchXPartAutomH (PartAutom *M, register uchar *text, int from, int to, 
		      int *matches)

   { register Tsection *T = M->T.d1;
     register mask *D = M->D.d1;
     register mask *newD = M->newD.d1;
     register mask Din = M->Din;
     register int J = M->J;
     register int p0 = M->p[0],p2 = M->p[2],p3=M->p[3],p5=M->p[5];
     register mask m1 = M->msk[1],m2 = M->msk[2];
     register mask *Glast = M->Glast.d1,*Gany = M->Gany.d1;  
     register mask x;
     register int j;
     register mask *aux;
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
              { x = (D[j] >> p0) | (D[j-1] << p2) | T[j][cc];
                newD[j] = (((D[j] << 1) &
                              ((D[j] << p3) |
                               (D[j+1] >> p5))) | m1)
                          & (((x + m1) ^ x) >> 1)
                          & Din;
                }
          aux = D; D = newD; newD = aux;

                /* check if lower-right state is active */
          if (!(D[J] & Glast[J]))
                {  /* clear last diagonal */
     		  D[J] |= m2;
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
     M->D.d1 = D; M->newD.d1 = newD;
     return count;
   }


int searchPartAutomH (PartAutom *M, register uchar *text, int from, int to, 
		      int *matches)

   { register Tsection *T = M->T.d1;
     register mask *D = M->D.d1;
     register mask *newD = M->newD.d1;
     register mask Din = M->Din;
     register int J = M->J;
     register int active = M->active;
     register int p0 = M->p[0],p2 = M->p[2],p3=M->p[3],p5=M->p[5];
     register mask m1 = M->msk[1],m2 = M->msk[2],m3 = m1|((1 << M->lr) - 1);
     register mask *Glast = M->Glast.d1,*Gany = M->Gany.d1;  
     register mask Gl0 = Glast[0];
     register mask x;
     register int j;
     register mask *aux;
     register uchar cc;
     register int n = from;
     int count = 0;

        /* k=m-1 not solved with this automaton */
     if (M->k == M->m - 1)
        return search1match (M->S, text, from, to, matches);

        /* search loop */
     while (true)
        {       /* update matrix */

	  if (active == 0)  /* use first autom as filter */
	     { register mask D0 = D[0];
               while (true)
		 { x = (D0 >> p0) | ((mask*)T)[text[n++]];
                   D0 = ((D0 << 1) | m1) &
                        ((D0 << p3) | m3) &
                        (((x + m1) ^ x) >> 1) &
                        Din;
		   if (!(D0 & Gl0)) break; /* found */
		 }
	       D[0] = D0;
	       active = 1;
             }

          cc = text[n++];
          for (j=0; j<=active; j++)
              { x = (D[j] >> p0) | (D[j-1] << p2) | T[j][cc];
                newD[j] = (((D[j] << 1) &
                              ((D[j] << p3) |
                               (D[j+1] >> p5))) | m1)
                          & (((x + m1) ^ x) >> 1)
                          & Din;
                }
          aux = D; D = newD; newD = aux;

                /* check if lower-right state is active */
          if (D[active] & Glast[active])
                /* check if there is an active state */
              while (!(~D[active] & Gany[active]))
                 { if (D[active-1] & Glast[active-1])
                         /* clear old D (now newD). the last row suffices */
                      { newD[active] = Din;
                        active--;
                      }
		   else break;
                 }
          else
             if (active == J)   /* match found */
                {  /* clear last diagonal */
     		  D[J] |= m2;
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
     M->D.d1 = D; M->newD.d1 = newD;
     return count;
   }

int searchVPartAutomH (PartAutom *M, register uchar *text, int from, int to, 
		       int *matches)

   { register Tsection *T = M->T.d1;
     register mask *D = M->D.d1;
     register mask *newD = M->newD.d1;
     register mask Din = M->Din;
     register int J = M->J;
     register int active = M->active;
     register int p0 = M->p[0],p2 = M->p[2],p3=M->p[3],p5=M->p[5];
     register mask m1 = M->msk[1],m2 = M->msk[2],m3 = m1|((1 << M->lr) - 1);
     register mask *Glast = M->Glast.d1,*Gany = M->Gany.d1;
     register mask Gl0 = Glast[0];
     register mask x;
     register int j;
     register mask *aux;
     register uchar cc;
     register int n = from;
     int count = 0;

        /* k=m-1 not solved with this automaton */
     if (M->k == M->m - 1)
        return search1matchV (M->S, text, from, to, matches);

        /* search loop */
     while (n < to)
        {       /* update matrix */

	  if (active == 0)  /* use first autom as filter */
	     { register mask D0 = D[0];
               while (n < to)
		 { x = (D0 >> p0) | ((mask*)T)[text[n++]];
                   D0 = ((D0 << 1) | m1) &
                        ((D0 << p3) | m3) &
                        (((x + m1) ^ x) >> 1) &
                        Din;
		   if (!(D0 & Gl0)) break; /* found */
		 }
	       if (n == to) break;
	       D[0] = D0;
	       active = 1;
             }

          cc = text[n++];
          for (j=0; j<=active; j++)
              { x = (D[j] >> p0) | (D[j-1] << p2) | T[j][cc];
                newD[j] = (((D[j] << 1) &
                              ((D[j] << p3) |
                               (D[j+1] >> p5))) | m1)
                          & (((x + m1) ^ x) >> 1)
                          & Din;
                }
          aux = D; D = newD; newD = aux;

                /* check if lower-right state is active */
          if (D[active] & Glast[active])
                /* check if there is an active state */
              while (!(~D[active] & Gany[active]))
                 { if (D[active-1] & Glast[active-1])
                         /* clear old D (now newD). the last row suffices */
                      { newD[active] = Din;
                        active--;
                      }
		   else break;
                 }
          else
             if (active == J)   /* match found */
                {  /* clear last diagonal */
     		  D[J] |= m2;
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
     M->D.d1 = D; M->newD.d1 = newD;
     return count;
   }

