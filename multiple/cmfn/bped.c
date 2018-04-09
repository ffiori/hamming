//
//	(C) Copyright 2002 by Kimmo Fredriksson.
//
//	The code can be freely used for academic/research purposes. Direct
//	or indirect use for commercial advantage is not allowed without
//	written permission from the author.
//
//	The code can be modified and redistributed if this note is left
//	intact.
//
//	Comments, improvements and bug-reports can be emailed to kf@iki.fi
//

#include	<stdio.h>
#include	<stdlib.h>
#include        <ctype.h>
#include	<limits.h>
#include	<string.h>
#include	"squeeze.h"
#include	"kstdlib.h"
#include	"config.h"
#include	"bped.h"

int	* bpmap;

dpvec	** prep_beq( const unsigned char * P, int m )
{ 
	int 	a, p, i, seg, s = config.vSIGMA;
	dpvec 	** peq;

//	s = 256;	// fixme: use fSIGMA and vSIGMA...

	if(( s == 4 ) && config.IUBP ) s += 11;

	seg = DPBLOCKS( m );
	
	peq = ( dpvec ** )allocate_2d( s, seg + 1, sizeof( dpvec ));

	for( a = 0; a < s; a++ ) 
		for( i = 0; i < seg; i++ ) peq[ a ][ i ] = ( dpvec )0;

	for( p = 0; p < m; p++ )
		peq[ bpmap[ P[ p ]]][ p / DPVECBITS ] |= 
				( dpvec )1 << ( p % DPVECBITS );
	/*
	for( a = 0; a < s; a++ ) 
		for( ; p < seg * DPVECBITS; p++ ) 
			peq[ a ][ p / DPVECBITS ] |= 
				( dpvec )1 << ( p % DPVECBITS );
	*/

	if( config.IUBP )
	{
		for( p = 0; p < m; p++ )
		{
			switch( toupper( P[ p ] ))
			{
			        case 'B': set_eq_bitsP( peq, p, dnaC | dnaG | dnaT ); break;
			        case 'D': set_eq_bitsP( peq, p, dnaA | dnaG | dnaT ); break;
			        case 'H': set_eq_bitsP( peq, p, dnaA | dnaC | dnaT ); break;
			        case 'K': set_eq_bitsP( peq, p, dnaG | dnaT ); break;
			        case 'M': set_eq_bitsP( peq, p, dnaA | dnaC ); break;
			        case 'N': set_eq_bitsP( peq, p, dnaA | dnaC | dnaG | dnaT ); break;
			        case 'R': set_eq_bitsP( peq, p, dnaA | dnaG ); break;
			        case 'S': set_eq_bitsP( peq, p, dnaC | dnaG ); break;
			        case 'V': set_eq_bitsP( peq, p, dnaA | dnaC | dnaG ); break;
			        case 'W': set_eq_bitsP( peq, p, dnaA | dnaT ); break;
			        case 'Y': set_eq_bitsP( peq, p, dnaC | dnaT ); break;
			        case 'A':
			        case 'C':
			        case 'G':
			        case 'T': break;
			        default: fprintf( stderr, "Warning: invalid IUB code!\n"); break;
			}
		}
	}
	if( config.IUBT )
	{
		for( p = 0; p < m; p++ ) 
		{
			set_eq_bitsT( peq, p, 'b', dnaC | dnaG | dnaT ); 
			set_eq_bitsT( peq, p, 'd', dnaA | dnaG | dnaT ); 
			set_eq_bitsT( peq, p, 'h', dnaA | dnaC | dnaT ); 
			set_eq_bitsT( peq, p, 'k', dnaG | dnaT ); 
			set_eq_bitsT( peq, p, 'm', dnaA | dnaC ); 
			set_eq_bitsT( peq, p, 'n', dnaA | dnaC | dnaG | dnaT ); 
			set_eq_bitsT( peq, p, 'r', dnaA | dnaG ); 
			set_eq_bitsT( peq, p, 's', dnaC | dnaG ); 
			set_eq_bitsT( peq, p, 'v', dnaA | dnaC | dnaG ); 
			set_eq_bitsT( peq, p, 'w', dnaA | dnaT ); 
			set_eq_bitsT( peq, p, 'y', dnaC | dnaT ); 

		}
	}

	return peq;
}

dpvec	** bped_peq_combine( dpvec ** eq0, dpvec ** eq1, int m )
{
	dpvec	** peq;
	int 	j, a, seg, s = config.vSIGMA;

	if(( s == 4 ) && config.IUBP ) s += 11;

	seg = DPBLOCKS( m );
	
	peq = ( dpvec ** )allocate_2d( s, seg + 1, sizeof( dpvec ));

	for( a = 0; a < s; a++ ) 
		for( j = 0; j < seg; j++ ) 
			peq[ a ][ j ] = eq0[ a ][ j ] | eq1[ a ][ j ];

	return peq;
}


dpvec	** prep_beq_r( const unsigned char ** P, int r )
{
	int	i, j, m, mm;
	dpvec	** rpeq, ** peq;
	int 	a, seg, s = config.vSIGMA;

	if(( s == 4 ) && config.IUBP ) s += 11;

	m = strlen( P[ 0 ] );

	for( i = 1; i < r; i++ ) if(( mm = strlen( P[ i ] )) > m ) m = mm;

	seg = DPBLOCKS( m );
	
	rpeq = ( dpvec ** )allocate_2d( s, seg + 1, sizeof( dpvec ));

	for( a = 0; a < s; a++ ) 
		for( i = 0; i < seg; i++ ) rpeq[ a ][ i ] = ( dpvec )0;

	for( i = 0; i < r; i++ ) 
	{
		mm = strlen( P[ i ] );
		peq = prep_beq( P[ i ], mm );
		seg = DPBLOCKS( mm );

		for( a = 0; a < s; a++ ) 
			for( j = 0; j < seg; j++ ) 
				rpeq[ a ][ j ] |= peq[ a ][ j ];

		free_2d( peq );
	}

	return rpeq;
}

void	init_v( BMYERS * v, unsigned char * p, int k, int m )
{
	int	i, b;

	v->p = p;

	b = DPBLOCKS( m );

	if( !v->vp ) v->vp = salloc( b * sizeof( dpvec ));
	if( !v->vn ) v->vn = salloc( b * sizeof( dpvec ));

	if( !v->score ) v->score = salloc( b * sizeof( int ));

	b = ( k - 1 ) / DPVECBITS;
	if( k == 0 ) b = 0;

	v->rd = b;

	for( i = 0; i <= b; i++ )
	{
		v->vp[ i ] = ~( dpvec )0;
		v->vn[ i ] = ( dpvec )0;

		v->score[ i ] = ( i + 1 ) * DPVECBITS; 
	}

	if( m <= ( int )DPVECBITS ) v->score[ 0 ] = m;
}


int	ed( const unsigned char * t, BMYERS * v, int m, int k, MATCHCODE code )
{
	dpvec 	ph, mh, xv, xh;
	dpvec	sm = ( dpvec )1 << ( m - 1 );
	dpvec	eq;
	dpvec	pv = v->vp[ 0 ];
	dpvec	mv = v->vn[ 0 ];
	int	mm = m - 1;
	int	score;
	int	matches = 0;
	int	j;

	( void )sm;

	score = v->score[ 0 ];

	for( j = v->b; j < v->e; j++ )
	{
		eq = v->peq[ sget_c( t, j )][ 0 ];
		xv = eq | mv;
		xh = ((( eq & pv ) + pv ) ^ pv ) | eq;

		ph = mv | ~( xh | pv );
		mh = pv & xh;

	//	if( ph & sm ) score++; else
	//	if( mh & sm ) score--;

		score += ( ph >> mm ) & ( dpvec )1;
		score -= ( mh >> mm ) & ( dpvec )1;

		ph <<= 1;
		pv = ( mh << 1 ) | ~( xv | ph );
		mv = ph & xv;

		if( score <= k )
		{
			matches++;
			if( code ) code( j, score, v->p );
		}
	}

	v->vp[ 0 ] = pv;
	v->vn[ 0 ] = mv;

	v->score[ 0 ] = score;

	return matches;
}


int	bed( const unsigned char * T, BMYERS * v, int m, int k, MATCHCODE code ) 
{ 
	int 	i, diw, dim, score = k + k, s;
	dpvec 	pc, mc;
	dpvec 	*e;
	dpvec	P, M, U, X, Y;
//	dpvec 	Ebit;
//	dpvec 	Ebitp;
	int 	seg, rem, r, rd, re, matches = 0;

	seg = DPBLOCKS( m );

	if( seg == 1 ) return ed( T, v, m, k, code );

	rem = m % DPVECBITS;

	if( rem == 0 ) rem = DPVECBITS;

//	Ebit  = ((( dpvec )1 ) << ( DPVECBITS - 1 ));
//	Ebitp = ((( dpvec )1 ) << ( rem - 1 ));

	re = seg - 1;

	diw = DPVECBITS + k;
	dim = rem + k;

	s = rem - 1;

//	rd = ( k - 1 ) / DPVECBITS;
//	if( k == 0 ) rd = 0;

	rd = v->rd;

	for( i = v->b; i < v->e; i++ )
        { 
		e  = v->peq[ sget_c( T, i )];

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

		if( score == k && (( *e & ( dpvec )0x1 ) | mc ) && r <= re )
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
			matches++;
			if( code ) code( i, score, v->p );
		}
        }

	v->rd = rd;

	return matches;
}





