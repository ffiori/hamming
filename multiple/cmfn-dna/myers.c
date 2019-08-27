#include	"squeeze.h"
#include	"verify.h"
#include	"myers.h"


int	myers_short( PTREE * pt, MATCHCODE code ) 
{
	dpvec 	ph, mh, xv, xh;
	dpvec	eq;
	BMYERS	* v = pt->v.m;
	dpvec	pv = v->vp[ 0 ];
	dpvec	mv = v->vn[ 0 ];
	int	m = pt->m;
	int	mm = m - 1;
	int	score;
	int	matches = 0;
	int	j, k = config.k;

	score = v->score[ 0 ];

	for( j = v->b; j < v->e; j++ )
	{
		eq = v->peq[ sget_c( config.t, j )][ 0 ];
		xv = eq | mv;
		xh = ((( eq & pv ) + pv ) ^ pv ) | eq;

		ph = mv | ~( xh | pv );
		mh = pv & xh;

		score += ( ph >> mm ) & ( dpvec )1;
		score -= ( mh >> mm ) & ( dpvec )1;

		ph <<= 1;
		pv = ( mh << 1 ) | ~( xv | ph );
		mv = ph & xv;

		if( score <= k )
		{
			if( pt->p )
			{
				matches++;
				if( code ) code( j, score, pt->p );
			}
			else
			{
				matches += verify_myers( pt->l, j - pt->l->m - config.k, j + pt->l->m - m + 1, code );
				matches += verify_myers( pt->r, j - pt->r->m - config.k, j + pt->r->m - m + 1, code );
			}
		}
	}

	v->vp[ 0 ] = pv;
	v->vn[ 0 ] = mv;

	v->score[ 0 ] = score;

	return matches;
}

int	myers( PTREE * pt, MATCHCODE code ) 
{ 
	int	k = config.k;
//	int	is, ie, lk = 0;
	int	m = pt->m;
	int 	i, diw, dim, score = 2 * k;
	int	s;
	dpvec 	pc, mc;
	dpvec 	*e;
	dpvec	P, M, U, X, Y;
	dpvec 	Ebit;
	dpvec 	Ebitp;
	int 	seg, rem, r, rd, re, matches = 0;
	BMYERS	* v = pt->v.m;

	seg = DPBLOCKS( m );

	if( seg == 1 ) return myers_short( pt, code );

	rem = m % DPVECBITS;

	if( rem == 0 ) { rem = DPVECBITS; }

	Ebit  = ((( dpvec )1 ) << ( DPVECBITS - 1 ));
	Ebitp = ((( dpvec )1 ) << ( rem - 1 ));

	re = seg - 1;

	diw = DPVECBITS + k;
	dim = rem + k;

	s = rem - 1;

//	printf("seg: %d, rem: %d\n", seg, rem );
//	rd = ( k - 1 ) / DPVECBITS;
//	if( k == 0 ) rd = 0;

	rd = v->rd;

//	lk = ( v->score[ re ] <= k );
//	is = v->b;

	for( i = v->b; i < v->e; i++ )
        { 
		e  = v->peq[ sget_c( config.t, i )];

		pc = mc = ( dpvec )0;
		r = 0;

		while( r <= rd )
		{ 
			U  = *e++;
			P  = v->vp[ r ];
			M  = v->vn[ r ];

			Y  = U | mc;
			X  = ((( Y & P ) + P ) ^ P ) | Y;
			U |= M;

			Y = P;
			P = M | ~( X | Y );
			M = Y & X;
    
			Y = ( P << 1 ) | pc;
			v->vp[ r ] = ( M << 1 ) | mc | ~( U | Y );
			v->vn[ r ] = Y & U;

			score = v->score[ r ];

			if( r == re ) 
			{
				pc = ( P >> s ) & ( dpvec )1;
				mc = ( M >> s ) & ( dpvec )1;
			}
			else
			{				
				pc = P >> ( DPVECBITS - 1 );
				mc = M >> ( DPVECBITS - 1 );
			}

			v->score[ r ] = score + pc - mc; 

			r++;
		}

	//	printf("%d\t", r );

		if( score == k && (( *e & ( dpvec )1 ) | mc ) && r <= re )
		{ 
			v->vp[ r ] = ~( dpvec )0 ^ (( dpvec )1 - mc );
			v->vn[ r ] = pc;
			if( r == re ) 
				v->score[ r ] = score = dim - 1;
			else 
				v->score[ r ] = score = diw - 1;
			rd = r;

		}
		else
		{ 
			score = v->score[ rd ];
			if( rd == re && score > dim ) score = v->score[ --rd ];
			if( rd < re ) while( score > diw ) score = v->score[ --rd ];
		//	while( score > diw ) score = v->score[ --rd ];
                }

		if( rd == re && score <= k )
		{
			if( pt->p )
			{
				matches++;
				if( code ) code( i, score, pt->p );
			}
			else
			{
				matches += verify_myers( pt->l, i - pt->l->m - config.k, i + pt->l->m - m + 1, code );
				matches += verify_myers( pt->r, i - pt->r->m - config.k, i + pt->r->m - m + 1, code );
			}
		}


		/*
//		if( rd == re ) printf("%d\n", score );

		if( rd == re && score <= k )
		{
			if( pt->p )
			{
				matches++;
				if( code ) code( i, score );
			}
			else if( !lk ) 
			{ 
				is = i; 
				lk = 1; 
			}
		}
		else
		{
			if( lk && !pt->p )
			{
				lk = 0;
			//	printf("%d, %d\n", is, i );
				matches += verify_myers( pt->l, is - pt->l->m - config.k, i + pt->l->m - m + 1, code );
				matches += verify_myers( pt->r, is - pt->r->m - config.k, i + pt->r->m - m + 1, code );
			}
		}
		*/
        }

	v->rd = rd;

	return matches;
}




