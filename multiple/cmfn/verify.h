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

#ifndef		_VERIFY_H_
#define		_VERIFY_H_

#include	"kstdlib.h"
#include	"bped.h"
#include	"hamming.h"
#include	"config.h"
#include	"filter.h"
#include	"myers.h"

static inline int	verify_myers( PTREE * pt, int b, int e, MATCHCODE code )
{
	BMYERS	* v = pt->v.m;

	if( b < v->e )  // continue from previous pos
	{
		pt->v.m->b = pt->v.m->e;
	} 
	else 
	{
		init_v( pt->v.m, pt->p, config.k, pt->m );
		
		pt->v.m->b = b;    // ...or just a new start
	}

	pt->v.m->e = e;

//	printf("%d ---> %d\n", pt->v->b, pt->v->e );

	return myers( pt, code );
}

static inline int	verify( const unsigned char * t, int b, int e, unsigned char * p, BMYERS * v, 
				int m, int k, MATCHCODE code )
{
	TIMELOOPCUTOFF;

	if( config.hamming )
	{
		if( b < v->e )        // continue from previous pos
		{
			v->b = v->e;
		} 
		else 
		{
			v->b = b;       // ...or just a new start
		}

		v->e = e;

		return hamming_k( t, v->b, v->e, p, m, k, code );

	}
	else
	{
		if( v->peq && b < v->e )        // continue from previous pos
		{
			v->b = v->e;
		} 
		else 
		{
			if( !v->peq ) v->peq = prep_beq( p, m ); // first verification

			init_v( v, p, k, m );

			v->b = b;       // ...or just a new start
		}

		v->e = e;

		return bed( t, v, m, k, code );
	}
}

static inline int     verify_pt_bped( PTREE * pt, int n, int i, int m, int k, MATCHCODE code )
{
        int		b, e, w;

	// b = the first character where the match may start
	// e = the last character + 1 where the match may start (Hamming)
	// e = the last character + 1 that may be part of the match (Levenshtein)

//	if( config.skipf ) w = BWIN( m, config.k ); else w = SWIN( m, config.k );

	w = DWIN( m, config.k );

	if( config.skipb )
	{
	//	b = MAX( 0, i - m - k + w ); 
		b = MAX( 0, i ); 
		e = MIN( n, i + m + k + 1 ); 
	}
	else if( config.skipf )
	{
		b = MAX( 0, i - m - k + w );
		e = MIN( n, i + m + k + 1 );
	}
	else 
	{
		b = MAX( 0, i - config.TUPLE + 1 );
		e = MIN( n, i + m + k + 1 );
	}

	return verify_myers( pt, b, e, code );
}

static inline int     verify_cm( const unsigned char * t, int n, int i, unsigned char * p, BMYERS * v, 
				 int m, int k, MATCHCODE code )
{
        int		b, e, w;

	// b = the first character where the match may start
	// e = the last character + 1 where the match may start (Hamming)
	// e = the last character + 1 that may be part of the match (Levenshtein)

//	if( config.skipf ) w = BWIN( m, config.k ); else w = SWIN( m, config.k );

	w = DWIN( m, config.k );

	if( config.skipb )
	{
		if( config.hamming )
		{
		//	b = MAX( 0, i - m + w );
		//	e = MIN( n, i + 1 );
			b = MAX( 0, i ); 
			e = MIN( n, i + m + 1 ); 
		}
		else
		{
		//	b = MAX( 0, i - m - k + w ); 
			b = MAX( 0, i ); 
			e = MIN( n, i + m + k + 1 ); 
		}
	}
	else if( config.skipf )
	{
		if( config.hamming )
		{
			b = MAX( 0, i - m + w );
			e = MIN( n, i + 1 );
		}
		else
		{
			b = MAX( 0, i - m - k + w );
			e = MIN( n, i + m + k + 1 );
		}
	}
	else 
	{
		if( config.hamming )
		{
			b = MAX( 0, i - config.TUPLE + 1 );
			e = MIN( n, i + 1 );
		}
		else
		{
			b = MAX( 0, i - config.TUPLE + 1 );
			e = MIN( n, i + m + k + 1 );
		}
	}

	return verify( t, b, e, p, v, m, k, code );
}

static inline int    verify_laq( const unsigned char * t, int n, int i, unsigned char * p, BMYERS * v, 
				 int m, int r, int k, MATCHCODE code )
{
        int		b, e, h;

	( void )r;

	h = MAX( config.TUPLE, config.laqh );
//	b = MAX( 0, i - 2 * k - config.TUPLE );
	b = MAX( 0, i - 2 * k - h );
	e = MIN( n, i + m + k + 1 );

	return verify( t, b, e, p, v, m, k, code );
}

static int	verify_pt( const unsigned char * t, PTREE * pt, int i, int n, int k,
			   MATCHCODE code )
{
	int	ms = 0;

	if( !pt->p )
	{
		ms += verify_pt( t, pt->l, i, n, k, code );
		ms += verify_pt( t, pt->r, i, n, k, code );
	}
	else
	{
		ms += verify_cm( t, n, i, pt->p, pt->v.m, pt->m, k, code );
	}

	return ms;
}

static int	verify_b_pt( const unsigned char * t, PTB * pt, int i, int n, int k, MATCHCODE code )
{
	int	ms = 0;

	if( !pt->p )
	{
		ms += verify_b_pt( t, pt->l, i, n, k, code );
		ms += verify_b_pt( t, pt->r, i, n, k, code );
	}
	else
	{
		ms += verify_laq( t, n, i, pt->p, pt->v, pt->m, pt->b, k, code );
	}

	return ms;
}

static int	verify_ptbp( const unsigned char * t, PTBP * pt, int i, int n, int k,
			     MATCHCODE code )
{
	int	matches = 0, j;

	if( !pt->p ) for( j = 0; j < pt->np; j++ )
	{
		matches += verify_ptbp( t, pt->c[ j ], i, n, k, code );
	}
	else for( j = 0; j < pt->np; j++ )
	{
		matches += 
			verify_cm( t, n, i, pt->p[ j ], pt->v[ j ], 
				   pt->m[ j ], k, code );
	}

	return matches;
}

#endif


