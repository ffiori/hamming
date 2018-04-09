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

#ifndef		_BPED_H_
#define		_BPED_H_

#include	<limits.h>
//#include	"squeeze.h"
#include	"kstdlib.h"
#include	"config.h"


#define		dnaA	1
#define		dnaC	2
#define		dnaG	4
#define		dnaT	8

typedef unsigned long 	dpvec;

#define		DPVECBITS	( sizeof( dpvec ) * CHAR_BIT )
#define		DPHBIT		(( dpvec )1 << ( DPVECBITS - 1 ))

#define		DPBLOCKS( m )	(( m + DPVECBITS - 1 ) / DPVECBITS )
#define		DPMOD( x )	(( x ) & ( DPVECBITS - 1 ))

typedef struct	bmyers	// block-wise computation for long P
{
	int	b;	// start verification at b
	int	e;	// end at e - 1

	int	rd;	// current block

	int	* score;

	char	* p;

	dpvec 	** peq;

	dpvec	* vp;
	dpvec	* vn;

} BMYERS;

extern int	* bpmap;

static inline	void	bp_setmap( int * map )
{
	bpmap = map;
}

static inline void	set_eq_bitsP( dpvec ** peq, int p, int b )
{
	if( b & dnaA ) peq[ bpmap['a']][ p / DPVECBITS ] |= ( dpvec )1 << ( p % DPVECBITS );
	if( b & dnaC ) peq[ bpmap['c']][ p / DPVECBITS ] |= ( dpvec )1 << ( p % DPVECBITS );
	if( b & dnaG ) peq[ bpmap['g']][ p / DPVECBITS ] |= ( dpvec )1 << ( p % DPVECBITS );
	if( b & dnaT ) peq[ bpmap['t']][ p / DPVECBITS ] |= ( dpvec )1 << ( p % DPVECBITS );
}

static inline void	set_eq_bitsT( dpvec ** peq, int p, int c, int b )
{
	c = bpmap[ c ];

	if( b & dnaA ) peq[ c ][ p / DPVECBITS ] |= ( dpvec )1 << ( p % DPVECBITS );
	if( b & dnaC ) peq[ c ][ p / DPVECBITS ] |= ( dpvec )1 << ( p % DPVECBITS );
	if( b & dnaG ) peq[ c ][ p / DPVECBITS ] |= ( dpvec )1 << ( p % DPVECBITS );
	if( b & dnaT ) peq[ c ][ p / DPVECBITS ] |= ( dpvec )1 << ( p % DPVECBITS );
}

dpvec	** bped_peq_combine( dpvec ** eq0, dpvec ** eq1, int m );

dpvec 	** prep_beq( const unsigned char * P, int m );

dpvec	** prep_beq_r( const unsigned char ** P, int r );

static inline dpvec	** prep_beq_c( const unsigned char * p, int m )
{
	return prep_beq( p, m );
}

static inline dpvec	** prep_beq_r_c( const unsigned char ** P, int r )
{
	return prep_beq_r( P, r );
}

static inline void	ed_c( int t, dpvec ** peq, dpvec * vp, dpvec * vn, dpvec * vnp, dpvec * vnn )
{
	dpvec	hn, hp;
	dpvec	x, d;
	dpvec	eq;

	eq = peq[ t ][ 0 ];
	x = eq | *vn;
	d = (( *vp + ( x & *vp )) ^ *vp ) | x;

	hn = *vp & d;
	hp = *vn | ~( *vp | d );

	x = ( hp << 1 ) | ( dpvec )1;
	*vnn = x & d;
	*vnp = ( hn << 1 ) | ~( x | d );
}

static inline void bed_c( int t, int b, dpvec ** peq, dpvec * vp, dpvec * vn, dpvec * vnp, dpvec * vnn )
{ 
	dpvec 	pc, mc;
	dpvec 	*e;
	dpvec	P, M, U = 0, X, Y;
	int 	seg, r, re;

	if( b == 1 ) { ed_c( t, peq, vp, vn, vnp, vnn ); return; }

	seg = b;

	re = seg - 1;

	e  = peq[ t ];

	pc = mc = ( dpvec )0;
	r = 0;

	while( r <= re )
	{ 
		U  = *e++;
		P  = vp[ r ];
		M  = vn[ r ];

		Y  = U | mc;
		X  = ((( Y & P ) + P ) ^ P ) | Y;
		U |= M;

		Y = P;
		P = M | ~( X | Y );
		M = Y & X;
    
		Y = ( P << 1 ) | pc;

		if( r == 0 )  Y |= ( dpvec )1;

		vnp[ r ] = ( M << 1 ) | mc | ~( U | Y );
		vnn[ r ] = Y & U;
    
	//	pc = mc = ( dpvec )0;

	//	if( P & DPHBIT ) pc = ( dpvec )1; else 
	//	if( M & DPHBIT ) mc = ( dpvec )1; 

		pc = P >> ( DPVECBITS - 1 );
		mc = M >> ( DPVECBITS - 1 );

		r++;
	}
}

void	init_v( BMYERS * v, unsigned char * p, int k, int m );

int	ed( const unsigned char * t, BMYERS * v, int m, int k, MATCHCODE code );

int	bed( const unsigned char * T, BMYERS * v, int m, int k, MATCHCODE code ); 


#endif
























































