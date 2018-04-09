/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/


#include "partexact.h"
#include "cut.h"

static void build (PartExactB *P, Cut *cut, uchar *pattern, int m, int k,
                   int j, int *i, int *ver, int offset)

   { P->m = m; P->k = k;
     P->large = (j > 1);
     P->offset = m+k+offset;
     prepPartAutom (&P->dynaut,pattern+ver[0],m,k,1,true);
     if (!P->large)  /* simple */
        { P->u.s.l = cut->len[*i];
        }
     else  /* compound */
        { P->u.l.one = (void*)malloc (sizeof(PartExactB));
          P->u.l.two = (void*)malloc (sizeof(PartExactB));
          build (P->u.l.one,cut,pattern,ver[j/2]-ver[0],((j/2)*k)/j,
                 j/2,i,ver,0);
          build (P->u.l.two,cut,pattern,ver[j]-ver[j/2],((j-j/2)*k)/j,
                 j-j/2,i,ver+j/2,ver[j/2]-ver[0]);
        }
   }

void prepPartExact (PartExact *P, uchar **pattern, int r, int m, int k)

   { Cut cut;
     int i,*ver,j;

        /* Cut pattern */
     P->k = k; P->r = r;
     initCut (k,r,k+1,&cut);
     for (j=0;j<r;j++)
         addCut (pattern[j],m,&cut);

        /* prepare for multipattern search */
     prepMulti (&P->M,cut.ps,r*(k+1));
     freeCut (&cut);

        /* Initialize subproblems */
     ver = (int*)malloc ((k+2)*sizeof(int));
     P->A = malloc (r*sizeof(PartExactB));
     for (j=0;j<r;j++)
         { makeCut (pattern[j],m,k,k+1,&cut);
           for (i=0; i<k+1; i++)
               ver[i] = cut.aclen[i-1];
           ver[k+1] = m;
           i = 0;
           build (P->A+j,&cut,pattern[j],m,k,k+1,&i,ver,0);
           freeCut (&cut);
	 }
     free (ver);
   }

static void initPartExactB (PartExactB *P)

   { int i;
     initPartAutom (&P->dynaut);
     if (P->large)
        { initPartExactB (P->u.l.one);
          initPartExactB (P->u.l.two);
        }
   }

void initPartExact (PartExact *P)

   { int j;
     initMulti (&P->M);
     for (j=0;j<P->r;j++)
        initPartExactB(P->A+j);
   }


void freePartExactB (PartExactB *P)

   { if (P->large)
        { freePartExactB (P->u.l.one);
          freePartExactB (P->u.l.two);
          free (P->u.l.one);
          free (P->u.l.two);
        }
     freePartAutom (&P->dynaut);
   }

void freePartExact (PartExact *P)

   { int j;
     for (j=0;j<P->r;j++)
         freePartExactB (P->A+j);
     free (P->A);
     freeMulti (&P->M);
   }


#define BLK 1024

int searchPartExactB (PartExactB *P, register uchar *text,
		     int from, int to, int **matches, int *i, int **pmatches,
		     int *nmatches)

   { register int i1,i2;
     int *m1,*m2;
     register int c1,c2;
     register int o1,o2;
     register int nff,ff,ff1,ff2,tt;
     register int mPk = P->m+P->k;
     int count = 0;
     if (P->large)  /* integrate subsearches */
	{ if (P->u.l.one->large)
	     { m1 = NULL;
	       c1 = searchPartExactB (P->u.l.one,text+from,0,to-from,&m1,
				      i,pmatches,nmatches);
	       m1 = (int*)realloc (m1,c1*sizeof(int));
	     }
	  else
	     { m1 = pmatches[*i];
	       c1 = nmatches[*i];
	       (*i)++;
	     }
	  if (P->u.l.two->large)
	     { m2 = NULL;
	       c2 = searchPartExactB (P->u.l.two,text+from,0,to-from,&m2,
				      i,pmatches,nmatches);
	       m2 = (int*)realloc (m2,c2*sizeof(int));
	     }
	  else
	     { m2 = pmatches[*i];
	       c2 = nmatches[*i];
	       (*i)++;
	     }
	  o1 = P->u.l.one->offset - from; o2 = P->u.l.two->offset - from;
	  i1 = 0; if (i1 < c1) ff1 = m1[i1++]-o1; else ff1 = to+1;
	  i2 = 0; if (i2 < c2) ff2 = m2[i2++]-o2; else ff2 = to+1;
	  if (ff1 < ff2) 
	     { ff = ff1; 
	       if (i1 < c1) ff1 = m1[i1++]-o1; else ff1 = to+1;
	     }
	  else
	     { ff = ff2; 
	       if (i2 < c2) ff2 = m2[i2++]-o2; else ff2 = to+1;
	     }
          while (ff <= to)
	    { tt = ff + mPk; if (tt > to) tt = to;
              while (true)
                 { if (ff1 < ff2) 
                      { nff = ff1;
                        if (i1 < c1) ff1 = m1[i1++]-o1; else ff1 = to+1;
                      }
                   else
                      { nff = ff2;
                        if (i2 < c2) ff2 = m2[i2++]-o2; else ff2 = to+1;
                      }
		   if (nff <= tt) { tt = nff + mPk; if (tt > to) tt = to; }
                   else break;
		 }
              initPartAutom (&P->dynaut);
	      if (matches && ((count+(tt-ff)+BLK-1)/BLK > (count+BLK-1)/BLK))
		 *matches = realloc (*matches,sizeof(int)*((count+(tt-ff)+BLK-1)/BLK)*BLK);
              count += searchPartAutom (&P->dynaut,text,ff,tt, matches ? (*matches)+count : NULL);
	      ff = nff;
            }
          free (m1); free (m2);
	  return count;
	}
   }

int searchPartExact (PartExact *P, uchar *text,
                     int from, int to, int *matches)

   { int i,j;
     int count = 0;
     int **lmatches;
     int *lcount;

     if (P->k == 0) 
	{ searchMulti(&P->M,text,from,to,&matches,&count);
	  return count;
	}

     lmatches = (int**)malloc (P->r*(P->k+1)*sizeof(int*));
     lcount = (int*)malloc (P->r*(P->k+1)*sizeof(int));
     for (i=0;i<P->r*(P->k+1);i++)
	 { lmatches[i] = NULL;
	   lcount[i] = 0;
	 }

     searchMulti (&P->M,text+from,0,to-from,lmatches,lcount);
     for (i=0;i<P->r*(P->k+1);i++)
	 { lmatches[i] = (int*) realloc (lmatches[i],lcount[i]*sizeof(int));
	 }

     count = 0;
     for (j=0; j<P->r; j++)
	 { i = 0;
           count += searchPartExactB (P->A+j, text, from, to, NULL, &i, 
			lmatches + j*(P->k+1), lcount + j*(P->k+1));
	 }

     free (lmatches); free (lcount);
     return count;
   }

