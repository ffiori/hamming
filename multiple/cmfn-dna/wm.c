
#include	<stdio.h>
#include	"verify.h"
#include	"myers.h"
#include	"wm.h"
#include	"wmp.h"
#include	"preprocess.h"
#include	"kstdlib.h"


void	wm_pp ( WM * wm, unsigned char * p, int m, int k ) 
{ 
	int	i,c;

	( void )k;
   
	if( m >= ( int )WMVECBITS ) FATALERRORMSG("Pattern is too large.");

	for (c = 0; c < 256; c++) wm->T[c] = (wmvec)0;
	for (i = 0; i < m; i++)
		wm->T[p[i]] |= (wmvec)1 << (m-i-1);
}



void	wm_combine( WM * wm, WM * wm0, WM * wm1 )
{
	int	i;

	*wm = *wm1;

	for( i = 0; i < 256; i++ ) 
	{
		wm->T[ i ] = wm0->T[ i ] | wm1->T[ i ];
	}
}



int wm_search( PTREE * pt, PTREE * ptm, int m, int i, int n, unsigned char *Text, int k )
{ 
	register int		j;
	register unsigned char	*pos = Text;    /* pos (here) = text+pos(paper) */
	register unsigned char	*top = Text+n-m+2;
	int 			lpos = -1, matches = 0;
	register int 		tot_part;
	register int 		wind=m-k;  /* size of the search window */
	register wmvec 		B, * T = pt->v.w->T;

	( void )i;

	while (pos<top)
	{ 
		j = wind;
		tot_part=k;
		B=(~((wmvec)0));
		while (1)
		{ 
			B &= T[pos[--j]];
			if (B== (wmvec)0) {
				if (!tot_part--) break;
				B=(~((wmvec)0));
			}
			if (j == 0) { /* verification */
				int npos = pos+j-Text;
				if (npos > lpos) 
				{
					int	s, e;

					s = pos - Text;

					if( pt->p ) 
					{
						e = MIN( s + ptm->m + k, n );
						matches += verify_myers( ptm, s, e, 0 );
					}
					else
					{
						e = MIN( s + ptm->l->m + k, n );
						matches += verify_myers( ptm->l, s, e, 0 );
						e = MIN( s + ptm->r->m + k, n );
						matches += verify_myers( ptm->r, s, e, 0 );
					}

					lpos = npos; 
					break; 
				}
			}
			B<<=1;
		}
		pos += j+1;
	}

	return matches;
}





