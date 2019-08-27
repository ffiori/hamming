/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/


#include "partautom.h"
#include <math.h>

int searchAutom (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchFAutom (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchVAutom (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchPartAutomHV (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchFPartAutomHV (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchVPartAutomHV (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchXPartAutomHV (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchPartAutomHV1c (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchFPartAutomHV1c (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchVPartAutomHV1c (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchXPartAutomHV1c (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchPartAutomH (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchFPartAutomH (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchVPartAutomH (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchXPartAutomH (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchPartAutomV (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchFPartAutomV (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchVPartAutomV (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchPartAutomH1c (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchFPartAutomH1c (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchVPartAutomH1c (PartAutom *M, uchar *text, int from, int to, int *matches);
int searchXPartAutomH1c (PartAutom *M, uchar *text, int from, int to, int *matches);

	/* to force a decision externally */

bool pautomForceHoriz = false;
bool pautomForceVert = false;
bool dontuseSForce = false;
bool useSForce = false;
bool pautomForceActive = false;
bool pautomForceNoActive = false;


static void buildTsection (uchar **pattern, int np, int offs, 
			   int rows, int cols,
		           int Rows, int Cols, 
		           Tsection T, mask Tin)

	/* builds a T table for a cell of rows x cols (plus the
	   overflow row). the cell has space for Rows x Cols. the useful 
	   characters start at pattern[0..np-1] + offs.
	   Only rows+cols chars are needed from the patterns, so the relevant
	   rows+cols bits fit in a word (since (Rows+1)Cols <= w). 
	   Tin is a base mask, for not existing characters = (01^rows)^cols.
	   the block is aligned to the right of the word. 
	   the columns are aligned to the left of the block.
	   each row is aligned to the bottom (i.e. right) of its column.
	*/

   { int i,j,c,r;
     mask t[Sigma],u;

	/* we first set up a mask t with the r+c needed bits */

     for (c=0; c<Sigma; c++) t[c] = ~0;
     for (r=0;r<np;r++)
        for (i=0; i<rows+cols-1; i++) t[pattern[r][offs+i]] &= ~ (1 << i);

	/* now, build T as a sequence of length-r overlapping
	   segments of t */

     u = (1 << rows) - 1;   /* to mask the segments, u = 1^r */

     for (c=0; c<Sigma; c++)
         if (~t[c])	/* the character appears in the pattern, process it */
            { T[c] = 0;	   /* to ensure 0's in the unused left area */
              for (j=0; j<Cols; j++)
                  { T[c] <<= (Rows+1);
		    if (j < cols)
		       { T[c] |= t[c] & u;
                         t[c] >>= 1;
		       }
		    else T[c] |= u;
                  }
            }
         else T[c] = Tin; /* the char is not present, fast process */

	/* make T equal for equally mapped characters */
     for (c=0; c<Sigma; c++) if (c != Map[c]) T[c] = T[Map[c]];
   }

static void buildTmask (uchar **pattern, int np, PartAutom *M)

	/* builds the T table for pattern "pattern" and problem M.
	   the matrices are stored columnwise because the search is
	   that way, to have more reference locality 
	*/

   { int i,j;
     mask Tin;

	/* allocate cells */
     
     if (M->I && M->J)
        { M->T.d2 = (Tsection**) malloc ((M->J+1) * sizeof(Tsection*));
          for (j=0; j<=M->J; j++) 
	      M->T.d2[j] = (Tsection*) malloc ((M->I+1) * sizeof(Tsection));
	}
     else M->T.d1 = (Tsection*) malloc ((M->J+M->I+1) * sizeof(Tsection));

	/* build an empty mask for T */
     M->Din = (1 << M->lr) - 1;
     for (i=1; i<M->lc; i++) M->Din |= M->Din << (M->lr+1);

	/* fill cells */
     for (j=0; j<=M->J; j++)
         for (i=0; i<=M->I; i++)
	   { buildTsection (pattern,np,i*(M->lr)+j*(M->lc),
			    i<M->I ? M->lr : (M->k+1) - M->I*(M->lr),
			    j<M->J ? M->lc : (M->m-M->k) - M->J*(M->lc),
			    M->lr,M->lc, 
			    (M->I && M->J) ? M->T.d2[j][i] : M->T.d1[i+j],
			    M->Din);
	   }
   }


static void buildD (PartAutom *M)

	/* builds the D matrix for problem M (current and new D) */

   { int i,j,r,del;

	/* allocate space and put useful part surrounded by a top row and
	   a left and right column (to avoid special cases) */
	/* fill borders with appropriate values:
		D[-1,j] = 1s
		D[i,-1] = 0s
		D[i,J]  = 1s */

     if (M->I && M->J)
        { M->D.d2 = ((mask**)malloc ((M->J+3)*sizeof(mask*))) + 1;
          M->newD.d2 = ((mask**)malloc ((M->J+3)*sizeof(mask*))) + 1;
          for (j=-1; j<=M->J+1; j++)
	      { M->D.d2[j] = ((mask*)malloc ((M->I+2)*sizeof(mask))) + 1;
	        M->newD.d2[j] = ((mask*)malloc ((M->I+2)*sizeof(mask))) + 1;
	      }
          for (j=0; j<=M->J+1; j++) M->D.d2[j][-1] = M->newD.d2[j][-1] = M->Din;
          for (i=0; i<=M->I; i++)
	      { M->D.d2[-1][i] = M->newD.d2[-1][i] = 0;
	        M->D.d2[M->J+1][i] = M->newD.d2[M->J+1][i] = M->Din;
	      }
	}
     else if (M->I)
	       { M->D.d1 = ((mask*) malloc ((M->I+2)*sizeof(mask))) + 1;
	       }
     else if (M->J)
	       { M->D.d1 = ((mask*) malloc ((M->J+3)*sizeof(mask))) + 1;
	         M->newD.d1 = ((mask*) malloc ((M->J+3)*sizeof(mask))) + 1;
	         M->D.d1[-1] = M->newD.d1[-1] = 0;
	         M->D.d1[M->J+1] = M->newD.d1[M->J+1] = M->Din;
	       }

        /* precompute some measures */
     M->p[0] = M->lr+1;
     M->p[1] = M->lr-1;
     M->p[2] = (M->lr+1)*(M->lc-1);
     M->p[3] = M->lr+2;
     M->p[4] = (M->lr+1)*(M->lc-1)+M->lr-1;
     M->p[5] = (M->lr+1)*(M->lc-1)-1;

	/* precompute masks */
     M->msk[0] = (1 << (M->lr+1)) - 2;  /* (1^lr 0)^lc */
     M->msk[1] = 1;                  /* (0^lr 1)^lc */
     del = M->lc*(M->J+1)-(M->m-M->k); /* desf in diagos of last column */
     M->msk[2] = (1 << M->lr) - 1;      /* (01^lr) at last column */
     M->msk[2] <<= (M->lr+1)*del;
     for (j=1;j<M->lc;j++)
         { M->msk[0] |= M->msk[0] << (M->lr+1);
           M->msk[1] |= M->msk[1] << (M->lr+1);
         }
     if (M->J && (M->lc > 1))
        { M->Glast.d1 = 1+(mask*)malloc ((M->J+2)*sizeof(mask));
          M->Gany.d1 = 1+(mask*)malloc ((M->J+2)*sizeof(mask));
          for (j=0;j<=M->J;j++)
	     { r = j < M->J ? 0 : del;
	       M->Glast.d1[j] = 1 << ((M->k - M->lr*M->I) + (M->lr+1)*r);
	       M->Gany.d1[j] = M->Glast.d1[j];
	       while (++r < M->lc) M->Gany.d1[j] |= M->Gany.d1[j] << (M->lr+1);
	     }
	}
     else
	{ r = del;
	  M->Glast.d0 = 1 << ((M->k - M->lr*M->I) + (M->lr+1)*r);
	  M->Gany.d0 = M->Glast.d0;
	  while (++r < M->lc) M->Gany.d0 |= M->Gany.d0 << (M->lr+1);
	}
   }

static double estimate (int m, int k, int c, int r, bool active, bool vert)

   { double alpha = k/(double)m;
     double actcols;
     int I,lr,lc,J;
     double cost;
     int actdiag;

     if ((m-k)*(k+2) <= W) return 102;

     actcols = 0.9*k/(1-1.09*sqrt(r/(double)c)) - k + 1;
     if (actcols < 1) actcols = 1;
     if (actcols > m-k) actcols = m-k;

     I = (k+W-1)/(W-1);
     lr = (k+I)/I;
     lc = W/(lr+1); if (lc > m-k) lc = m-k;
     J = (m-k+lc-1)/lc;
     actdiag = ceil(actcols/lc);

     if (vert)
	{ lr = W/(m-k) - 1;
	  I = (k+lr)/lr;
	  cost = 166 + 136 * I;
	}
     else
	{ if (I > 1)
             if (J > 1)
                  cost = 12 + (active ? 166 : 0) + 
			      227 * I * (active ? actdiag : J);
             else ;
          else
             if (lc > 1)
                { if (active)
		     { 	/* compute prob of activating last diag of fst
			   automaton (m-k = lc). this one costs 102 
			   as the simple one */
		       double prob = pow ((1.09*1.09*r*(lc+k)*(lc+k))/(c*lc*lc),
					  lc);
		       cost = 102 + prob * (68 + 125 * actdiag);
		     }
		  else cost = 101 + 125 * J;
		}
             else cost = 113 + (active ? 20 : 0) +
			     83 * (active ? actdiag : J);
	}
     return cost;
   }

double estimatePartAutom (int m, int k, int c, int np)

   { double acost,nacost,vcost,cost;
     bool active;

     if ((m-k)*(k+2) <= W) return 102;

     acost = estimate (m,k,c,np,true,false);
     nacost = estimate (m,k,c,np,false,false);
     if (2*(m-k) <= W) vcost = estimate (m,k,c,np,false,true);

     if (pautomForceActive) active = true;
     else if (pautomForceNoActive) active = false;
     else active = (acost < nacost);

     cost = active ? acost : nacost;

        /* we may prefer a vertical partition  */
     if ((2*(m-k) <= W) &&
         (pautomForceVert || (!pautomForceHoriz && (vcost < cost))))
        return vcost;

     return cost;
   }

	/* does the preprocessing to search "pattern" by automaton
	   partitioning. only pattern[0..m-1] si considered, and k
	   is the number of errors */ 

void prepSPartAutom (PartAutom *M, uchar **pattern, int np,
			    int k, int c, bool V)

   { int ch,i,m,r;
     double acost,nacost,vcost,cost;
     bool active;
     bool useS;

              /* m */
     m = strlen(pattern[0]);
     for (i=1;i<np;i++)
        { int l = strlen(pattern[i]);
          if (m>l) m=l;
        }


        /* set initial values */
     M->m = m; M->k = k;

        /* I = celing((k+1)/(w-1))
           lr = ceiling ((k+1)/I)
           lc = floor (w/(lr+1))
           J = ceiling ((m-k)/lc) */
	/* we store I-1,J-1 for convenience */

     acost = estimate (m,k,c,np,true,false);
     nacost = estimate (m,k,c,np,false,false);
     if (2*(m-k) <= W) vcost = estimate (m,k,c,np,false,true); else vcost = 0;

     if (pautomForceActive) active = true;
     else if (pautomForceNoActive) active = false;
     else active = (acost < nacost);

     cost = active ? acost : nacost;

	/* we may prefer a vertical partition  */
     if ((2*(m-k) <= W) && 
	 (pautomForceVert || (!pautomForceHoriz && (vcost < cost))))
	{    /* vertical partition preferred */
	  int lr = W/(m-k) - 1;
          int I = (k+lr)/lr;
	  M->J = 0; M->I = I-1;
	  M->lc = m-k; M->lr = lr;
	  cost = vcost;
        }
     else   /* horizontal partition preferred */
        { M->I = (k+W-1)/(W-1) - 1;
          M->lr = (k+M->I+1)/(M->I+1);
          M->lc = W/(M->lr+1); if (M->lc > m-k) M->lc = m-k;
          M->J = (m-k+M->lc-1)/ M->lc - 1;
	}
     

	/* compute T, D and some values */
     buildTmask (pattern,np,M);
     buildD (M);

        /* compute the S table */
     M->S = (bool*) malloc (Sigma*sizeof(bool));
     for (ch=0;ch<Sigma;ch++) M->S[ch] = true;
     for (r=0;r<np;r++)
	 for (i=0;i<=k;i++) M->S[pattern[r][i]] = false;
     for (ch=0;ch<Sigma;ch++) if (ch != Map[ch]) M->S[ch] = M->S[Map[ch]];

	/* determine the use of S */
     if (useSForce) useS = true;
     else if (dontuseSForce) useS = false;
     else useS = (k/(double)m < log(c)/(3*log(m)));

	/* assign proper searching procedure */
     if (M->J)
	if (M->I)
	     if (M->lc > 1)
	          M->search = NULL; /* never happens, searchPartAutomHV */
	     else M->search = 
		     V ? searchVPartAutomHV1c : 
		    (!active ? searchXPartAutomHV1c :
		     (useS ? searchFPartAutomHV1c : searchPartAutomHV1c));
	else if (M->lc > 1)
                  M->search = 
		     V ? searchVPartAutomH : 
		    (!active ? searchXPartAutomH :
		     (useS ? searchFPartAutomH : searchPartAutomH));
             else M->search = 
		     V ? searchVPartAutomH1c : 
		    (!active ? searchXPartAutomH1c :
		     (useS ? searchFPartAutomH1c : searchPartAutomH1c));
     else 
	if (M->I)
	     M->search = 
		     V ? searchVPartAutomV : 
		     (useS ? searchFPartAutomV : searchFPartAutomV);
	else M->search = 
			V ? searchVAutom : 
			(dontuseSForce ? searchAutom : searchFAutom);

   }

void prepPartAutom (PartAutom *M, uchar *pattern, int m, int k, int c, bool V)

   { char ch = pattern[m];
     pattern[m] = 0;
     prepSPartAutom (M,&pattern,1,k,c,V);
     pattern[m] = ch;
   }


	/* prepares the automaton for a fresh search */

void initPartAutom (PartAutom *M)

   { int i,j;
    	
	/* put inactive states */
     if (M->I)
	if (M->J)
           for (j=0;j<=M->J;j++)
              for (i=0; i<=M->I; i++)
	          M->D.d2[j][i] = M->newD.d2[j][i] = M->Din;
	else for (i=0; i<=M->I; i++)
		  M->D.d1[i] = M->Din;
     else if (M->J)
	       for (j=0;j<=M->J;j++)
		  M->D.d1[j] = M->newD.d1[j] = M->Din;
	  else M->D.d0 = M->Din;

	/* state that only the first column is active */
     M->active = 0;
   }

        /* frees the automaton */

void freePartAutom (PartAutom *M)

   { int j; 
     if (M->I && M->J)
        { for (j=-1; j<=M->J+1; j++)
	    { free (M->D.d2[j]-1);
	      free (M->newD.d2[j]-1);
	    }
	  for (j=0; j<=M->J; j++)
	      free (M->T.d2[j]);
          free (M->D.d2-1); M->D.d2 = NULL;
          free (M->newD.d2-1); M->newD.d2 = NULL;
	  free (M->T.d2); M->T.d2 = NULL;
	}
     else if (M->I)
	{ free (M->D.d1-1); M->D.d1 = NULL;
          free (M->T.d1); M->T.d1 = NULL;
	}
     else if (M->J)
	{ free (M->D.d1-1); M->D.d1 = NULL;
          free (M->newD.d1-1); M->newD.d1 = NULL;
          free (M->T.d1); M->T.d1 = NULL;
	}
     else
	{ free (M->T.d1); M->T.d1 = NULL;
	}
     if (M->J && (M->lc > 1))
        { free (M->Glast.d1-1); M->Glast.d1 = NULL;
          free (M->Gany.d1-1); M->Gany.d1 = NULL;
	}
     free (M->S); M->S = NULL;
   }

int search1match (bool *S, register uchar *text,
                  int from, int to, int *matches)

   { register int n = from;
     register int count = 0;

     if (n < 0) n = 0;
     while (true)
       { while (S[text[n++]]);
         if (n > to) break;
         if (matches != NULL) matches[count] = n;
	 count++;
       }
     return count;
   }

int search1matchV (bool *S, register uchar *text,
                  int from, int to, int *matches)

   { register int n = from;
     register int count = 0;

     if (n < 0) n = 0;
     while (true)
       { while ((n < to) && S[text[n]]) n++;
         if (n == to) break;
         if (matches != NULL) matches[count] = ++n;
	 count++;
       }
     return count;
   }

