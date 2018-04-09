//
//	(C) Copyright 2002, 2003 by Kimmo Fredriksson.
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
#include	<string.h>
#include	"kstdlib.h"
#include	"bitfields.h"
#include	"bped.h"
#include	"squeeze.h"
#include	"preprocess.h"
#include	"verify.h"
#include	"filter.h"
#include	"config.h"
#include	"myers.h"
#include	"wm.h"
#include	"wmp.h"

extern long tprep, texec; // time variables, defined in main.c

typedef struct bpmsks
{
	cntr_t	D, H, J, K;
	int	A, B;

} BPMSKS;

// buffer for the optimizing alg (only), not nice, but fast...

#define		MAX_PAT_LEN	( 1 << 12 )

static int	MM[ MAX_PAT_LEN + 20 ];

static cntr_t	MMBP[ MAX_PAT_LEN + 20 ];
static cntr_t	MMBPO[ MAX_PAT_LEN + 20 ];


int	cm( const unsigned char * t, int n, unsigned char * p, int m, int k, MATCHCODE code )
{
	int         	i, j, d, w, sw, M = 1;
	char *		mt;
	int             matches = 0;
//	int		candidates = 0;
	BMYERS		v;
	long		tp = 0L, ts = 0L;

	( void )M;

	TIMESTART("Preprocessing... "); 

	bp_setmap( fmap );

	mt = build_sp( p, m );

	TIMEPP( tp );

	w = BLOCKWIN( m, k );

	v.peq = 0;
	v.vn = v.vp = 0;
	v.score = 0;
	v.b = v.e = 0;

	TIMESTART("Searching... ");

	bp_setmap( vmap );

	for( i = 0; i < n - m; i += sw )
        {
	        j = w - TUPLE;
		d = 0;

		do {    d += mt[ sget_t( config.t, i + j ) ];
	                j -= TUPLE;

		} while(( d <= config.k ) && ( j >= 0 ));

		if( j < 0 ) sw = 1; else sw = j + TUPLE + 1;

		if( d <= k )
		{
			matches += verify_cm( t, n, i, p, &v, m, k, code );
		}

		/*

		do {    d += mt[ sget_t( t, i + j ) ];
		        j += TUPLE;

		} while(( d <= k ) && ( j < w - TUPLE + 1 )); 

		if( d <= k )
		{ 
		        candidates++;
			matches += verify_cm( t, n, i, p, &v, m, k, code );
		}
		*/
	}

	TIMESCAN( ts );

	return matches;
}

static int	match_pt( PTREE * pt, int i, int depth, MATCHCODE code );

static inline int	match_pt_verify( PTREE * pt, int i, int depth, MATCHCODE code )
{
	int	sl = 1, sr = 1;

//	if( d > config.k ) return 0;

	if( pt->p || ( depth >= config.maxdepth ))
	{
		if( config.si )
			config.matches += verify_pt_bped( pt, config.n, i, pt->m, config.k, code );
		else
			config.matches += verify_pt( config.t, pt, i, config.n, config.k, code );

		return 1;
	}

	if( !pt->p ) 
        {
		if( !pt->l->t.s && !pt->l->t.sl ) 
			config.matches += verify_pt_bped( pt->l, config.n, i, pt->m, config.k, code );
		else
			sl = match_pt( pt->l, i, depth + 1, code );

		if( !pt->r->t.s && !pt->r->t.sl )
			config.matches += verify_pt_bped( pt->r, config.n, i, pt->m, config.k, code );
		else
			sr = match_pt( pt->r, i, depth + 1, code );

		return MIN( sl, sr );
	}

	return 1;

#if 0       

	if( pt->p || ( !pt->t && !config.si ))
	{
		matches += verify_pt( config.t, pt, i, config.n, config.k, code );

		return matches;
	}

	if( !pt->p && ( !config.si || ( pt->l->t && pt->r->t ))) 
        {
	        matches += match_pt( pt->l, i, depth + 1, code );
		matches += match_pt( pt->r, i, depth + 1, code );

		return matches;
	}

	if( !pt->p && config.si && !pt->t )
	{
	//	printf("%d\n", i );
		return verify_myers( pt, i - 3*TUPLE, i + pt->m + TUPLE + config.k + 1, code );
	}

	return matches;
	
#endif
}

static inline int	match_pt_opt( PTREE * pt, int i, int we, int depth, MATCHCODE code )
{
        int		j, tt;
	char		* t = pt->t.s;
	int		*M = MM; //config.data;

	for( j = -TUPLE; j < 0; j++ ) M[ j + TUPLE ] = 0;

	tt = sget_t( config.t, j + i );

	do {	M[ j + TUPLE ] = MAX( t[ tt ] + M[ j - TUPLE + TUPLE ], 
				      M[ j - 1 + TUPLE ] );

	        j++;

#	      	ifdef COMPRESSED
		tt = sget_t( config.t, j + i );
#		else
		tt = sget_tn( tt, config.t, j + i );
#		endif

	} while( M[ j + TUPLE - 1 ] <= config.k && ( j < we ));

	if( M[ j + TUPLE - 1 ] > config.k ) return 1;

	return match_pt_verify( pt, i, depth, code );
}

static inline int	match_pt_opt_b( PTREE * pt, int i, int we, int depth, MATCHCODE code )
{
        int		j, tt, shift;
	char		* t = pt->t.s;
	int		*M = MM; //config.data;

	for( j = we + TUPLE; j > we; j-- ) M[ j ] = 0;

	tt = sget_t( config.t, j + i );

	do {	M[ j ] = MAX( t[ tt ] + M[ j + TUPLE ], M[ j + 1 ] );

	        j--;

#	      	ifdef COMPRESSED
		tt = sget_t( config.t, j + i );
#		else
		tt = sget_tp( tt, config.t, j + i );
#		endif

	} while(( M[ j + 1 ] <= config.k ) && ( j >= 0 ));

	if( j < 0 ) shift = 1; else shift = j + 2; // j + 1 ???

	if( M[ j + 1 ] > config.k ) return shift;

	return match_pt_verify( pt, i, depth, code );
}

static inline int	match_pt_opt_laq_b( PTREE * pt, int i, int we, int depth, MATCHCODE code )
{
        int		j, tt, shift, l = 0, h = 0;
	char		** t = pt->t.sl;
	int		*M = MM; //config.data;

	for( j = we + TUPLE; j > we; j-- ) M[ j ] = 0;

	tt = sget_t( config.t, j + i );

	do {	M[ j ] = MAX( t[ l ][ tt ] + M[ j + TUPLE ], M[ j + 1 ] );

	        j--;

		if( ++h == TUPLE ) { h = 0; ++l; }

#	      	ifdef COMPRESSED
		tt = sget_t( config.t, j + i );
#		else
		tt = sget_tp( tt, config.t, j + i );
#		endif

	} while(( M[ j + 1 ] <= config.k ) && ( j >= 0 ));

	if( j < 0 ) shift = 1; else shift = j + 2; // j + 1 ???

	if( M[ j + 1 ] > config.k ) return shift;

	return match_pt_verify( pt, i, depth, code );
}

static inline int	match_pt_b( PTREE * pt, int i, int j, int depth, MATCHCODE code )
{
        int		d = 0, shift;

	do {    d += pt->t.s[ sget_t( config.t, i + j ) ];
	        j -= TUPLE;

	} while(( d <= config.k ) && ( j >= 0 ));

	/*
	if( j < 0 ) shift = 1; else shift = j + TUPLE + 1; 
	if( d > config.k ) return shift;
	return match_pt_verify( pt, i, depth, code );
	*/

	if( d <= config.k ) return match_pt_verify( pt, i, depth, code );

	if( j < 0 ) shift = 1; else shift = j + TUPLE + 1; 

	return shift;
} 

static inline int	match_pt_laq_b( PTREE * pt, int i, int j, int depth, MATCHCODE code )
{
        int		d = 0, l = 0, shift;

//	printf("%d, %d\n", ( pt->m + config.TUPLE - 1 ) / config.TUPLE, j / config.TUPLE );

	do {    d += pt->t.sl[ l++ ][ sget_t( config.t, i + j ) ];
	        j -= TUPLE;

	} while(( d <= config.k ) && ( j >= 0 ));

	if( d <= config.k ) return match_pt_verify( pt, i, depth, code );

	if( j < 0 ) shift = 1; else shift = j + TUPLE + 1; 

//	printf("%d\t", l );

	return shift;

	/*
	if( j < 0 ) shift = 1; else shift = j + TUPLE + 1; 
	if( d > config.k ) return shift;
	return match_pt_verify( pt, i, depth, code );
	*/
} 

static int	match_pt_lbs_verify( PTB * pt, int i, int j, int shift, int depth, MATCHCODE code );

static inline int	match_pt_laq_b_strict( PTB * pt, int i, int j, int depth, MATCHCODE code )
{
	int 		l = 0, h, sj = j, shift;
        cntr_laq_t	d;
	cntr_laq_t	H, HM;

	d = pt->strictD; H = pt->strictH; HM = pt->strictHM;

	do {    d = ( d & H ) | (( d & ~H ) + ( pt->t[ sget_t( config.t, i + j ) ] << ( l++ * pt->bits )));
		j -= pt->h;

	} while((( d & HM ) != HM ) && ( l < pt->b ));

//	if( l < pt->b ) 
//		d = ( d & H ) | (( d & ~H ) + ( pt->t[ sget_t( config.t, i + j ) ] << ( l++ * pt->bits )));

       	if(( d & HM ) != HM )
	{
		shift = pt->h; 
		shift = match_pt_lbs_verify( pt, i, sj, shift, depth, code );
	}
	else
	{
		d = ((( d & H ) ^ H ) & pt->mm );
		h = math_dividend_div_const[ ILOG2( d & ~HM )];
		shift = ( pt->b - h - 1 ) * pt->h;
	}

	return shift;
} 


static int	match_pt_lbs_verify( PTB * pt, int i, int j, int shift, int depth, MATCHCODE code )
{
	int	lshift, rshift;

	if( pt->p ) 
	{
		config.matches += verify_b_pt( config.t, pt, i, config.n, config.k, code );
	}
	else
	{
		lshift = match_pt_laq_b_strict( pt->l, i, j, depth + 1, code );
		rshift = match_pt_laq_b_strict( pt->r, i, j, depth + 1, code );
		
		shift = MIN( lshift, rshift );
	}

	return shift;
}



static inline int	match_pt_f( PTREE * pt, int i, int we, int depth, MATCHCODE code )
{
        int		j, d;
	char		* t = pt->t.s;

	j = d = 0;

	do {    d += t[ sget_t( config.t, i + j ) ];
	        j += TUPLE;

	} while(( d <= config.k ) && ( j < we ));

	if( d > config.k ) return 1;

	return match_pt_verify( pt, i, depth, code );
} 


static inline int	match_pt_l( PTREE * pt, int i, int we, int depth, MATCHCODE code )
{
        int		j, d;
	char		* t = pt->t.s;

	if( i > pt->i + TUPLE )
	{
		j = d = 0;

		do {    d += t[ sget_t( config.t, i + j ) ];
	                j += TUPLE;

		} while(( j < we ));

	}
	else
	{
		d = pt->d - t[ sget_t( config.t, pt->i ) ]
		          + t[ sget_t( config.t, i + we - 1 ) ];
	}

	pt->i = i;
	pt->d = d;

	if( d > config.k ) return 1;

	return match_pt_verify( pt, i, depth, code );
} 


static int	match_pt( PTREE * pt, int i, int depth, MATCHCODE code )
{
        int		m, w, we;

	TIMELOOPCUTOFF;

	m = pt->m;

//	if( config.skipf ) w = BWIN( m, config.k ); else w = SWIN( m, config.k );
//	we = w - TUPLE + 1;

	w = DWIN( m, config.k );
	if( config.skipb ) we = w - TUPLE; else we = w - TUPLE + 1;

	if( config.opt )
	{
		if( config.skipb && !config.laq )
			return match_pt_opt_b( pt, i, we, depth, code );
		else if( config.skipb && config.laq )
			return match_pt_opt_laq_b( pt, i, we, depth, code );
		else
			return match_pt_opt( pt, i, we, depth, code );
	}
	else if( config.laq && config.skipb )
	{
		return match_pt_laq_b( pt, i, we, depth, code );
	} 
	else if( config.skipb )
	{
		return match_pt_b( pt, i, we, depth, code );
	} 
	else if( config.skipf )
	{
		return match_pt_f( pt, i, we, depth, code );
	} 
	else
	{
		return match_pt_f( pt, i, we, depth, code );
	} 
}

#if VECLEN != 128

#define	FOR_CNTRS_NOVER_K( j, pt, d, msks ) \
		for( j = 0; j < pt->np; j++, d >>= 1 ) \
			if( !(( d >>= msks->B - 1 + config.opt ) & ( cntr_t )1 )) 

#else

#define	FOR_CNTRS_NOVER_K( j, pt, d, msks ) \
		for( j = 0; j < pt->np; j++, d = _mm_srli_si128( d, 1 )) \
                        if((( int )d ) < config.k + 1 )

/*
		for( j = 0; j < pt->np; j++ ) \
                        if((( base_t * )&d )[ j ] <= config.k )
*/

#endif

static int	match_ptbp( PTBP * pt, BPMSKS * msks, int i, int depth, MATCHCODE code );

static int	match_ptbp_verify( PTBP * pt, BPMSKS * msks, cntr_t d, int i, int depth, MATCHCODE code )
{
	int	j, shift = 1 << 30, tshift;

//	if(( d & msks->H ) == msks->H ) return 0;
    //printf("ix: %d\n", i);

	if( depth >= config.maxdepth )
	{
		config.matches += verify_ptbp( config.t, pt, i, config.n, config.k, code );

		shift = 1;
	}
	else if( pt->p )
	{
		FOR_CNTRS_NOVER_K( j, pt, d, msks )
			config.matches += verify_cm( config.t, config.n, i, pt->p[ j ], pt->v[ j ], pt->m[ j ], config.k, code );

		shift = 1;
	}
	else if( !pt->p )
	{ 
		FOR_CNTRS_NOVER_K( j, pt, d, msks )
		{
			tshift = match_ptbp( pt->c[ j ], msks, i, depth + 1, code );
		//	printf("%d\t", j );
			/*
			if( pt->c[ j ]->p ) 
				shift = 1; 
			else 
				shift = MIN( shift, tshift );
			*/
			shift = MIN( shift, tshift );
		}
	}

	return shift;
}

#if VECLEN != 128 

static inline int	match_ptbp_opt( PTBP * pt, BPMSKS * msks, int i, int we,
					int depth, MATCHCODE code )
{
	cntr_t			d, o, x, y, f;
	cntr_t			* M = MMBP;
        int			j, tt;
	cntr_t			* t = pt->t.bs;

	for( j = -TUPLE; j < 0; j++ ) M[ j + TUPLE ] = msks->D;
		
	tt = sget_t( config.t, j + i );

	do {	x = M[ j - TUPLE + TUPLE ];
	        o = x & msks->H;
		x = ( x & ~msks->H ) + t[ tt ];
		x |= o;
		y = M[ j - 1 + TUPLE ];
		f = (( x | msks->J ) - y ) & msks->J;
		f = f - ( f >> msks->B );
		d = M[ j + TUPLE ] = ( x & f ) | ( y & ~f );

		j++;

#		ifdef COMPRESSED
		tt = sget_t( config.t, j + i );
#		else
		tt = sget_tn( tt, config.t, j + i );
#		endif

	} while((( d & msks->H ) != msks->H ) && ( j < we ));

	if(( d & msks->H ) == msks->H ) return 1;

	return match_ptbp_verify( pt, msks, d, i, depth, code );
}

#else

static inline int	match_ptbp_opt( PTBP * pt, BPMSKS * msks, int i, int we,
					int depth, MATCHCODE code )
{
	FATALERRORMSG("-B -Sf -O not implemented for SSE2.");
}

#endif

#if VECLEN != 128 

static inline int	match_ptbp_opt_b( PTBP * pt, BPMSKS * msks, int i, int we,
					int depth, MATCHCODE code )
{
	cntr_t			o, x, y, f;
	cntr_t			* M = MMBP, * dO = MMBPO;
        int			j, tt, shift, oshift = 1, lshift;
	cntr_t			* t = pt->t.bs;
	BPMSKS			m = *msks;

	for( j = we + TUPLE; j > we; j-- ) M[ j ] = dO[ j ] = m.D;

	tt = sget_t( config.t, j + i );

	do {	x = M[ j + TUPLE ];
	        o = x & m.H;
		x = ( x & ~m.H ) + t[ tt ];
		x |= o;
		y = M[ j + 1 ];
		f = (( x | m.J ) - y ) & m.J;
		f -= f >> m.B;
		M[ j ] = ( x & f ) | ( y & ~f );

	        if(( dO[ j ] & msks->H ) != ( M[ j ] & msks->H )) oshift = j + 1, dO[ j ] = M[ j ];

		j--;

#		ifdef COMPRESSED
		tt = sget_t( config.t, j + i );
#		else
		tt = sget_tp( tt, config.t, j + i );
#		endif

	} while((( M[ j + 1 ] & m.H ) != m.H ) && ( j >= 0 ));


	if(( M[ j + 1 ] & m.H ) != m.H ) 
	{
		lshift = match_ptbp_verify( pt, msks, M[ j + 1 ], i, depth, code );

		return MIN( oshift, lshift );
	}
	else
	{
		if( j < 0 ) shift = 1; else shift = j + 2; // j + 1 ??? 
		return shift;
	}

	/*

	if( j < 0 ) shift = 1; else shift = j + 1; 

	if(( M[ j + 1 ] & m.H ) == m.H ) return shift;

	return match_ptbp_verify( pt, msks, M[ j + 1 ], i, depth, code );
	*/
}

#else

static inline int	match_ptbp_opt_b( PTBP * pt, BPMSKS * msks, int i, int we,
					int depth, MATCHCODE code )
{
	FATALERRORMSG("-B -Sf -O not implemented for SSE2.");
}

#endif


static inline int	match_ptbp_opt_laq_b( PTBP * pt, BPMSKS * msks, int i, int we,
					int depth, MATCHCODE code )
{
	cntr_t			o, x, y, f;
	cntr_t			* M = MMBP, * dO = MMBPO;
        int			j, tt, shift, oshift = 1, lshift, h = 0, l = 0;
	cntr_t			** t = pt->t.bsl;
	BPMSKS			m = *msks;

	for( j = we + TUPLE; j > we; j-- ) M[ j ] = dO[ j ] = m.D;

	tt = sget_t( config.t, j + i );

	do {	x = M[ j + TUPLE ];
	        o = x & m.H;
		x = ( x & ~m.H ) + t[ l ][ tt ];
		x |= o;
		y = M[ j + 1 ];
		f = (( x | m.J ) - y ) & m.J;
		f -= f >> m.B;
		M[ j ] = ( x & f ) | ( y & ~f );

	        if(( dO[ j ] & msks->H ) != ( M[ j ] & msks->H )) oshift = j + 1, dO[ j ] = M[ j ];

		j--;

		if( ++h == TUPLE ) { h = 0; ++l; }

#		ifdef COMPRESSED
		tt = sget_t( config.t, j + i );
#		else
		tt = sget_tp( tt, config.t, j + i );
#		endif

	} while((( M[ j + 1 ] & m.H ) != m.H ) && ( j >= 0 ));


	if(( M[ j + 1 ] & m.H ) != m.H ) 
	{
		lshift = match_ptbp_verify( pt, msks, M[ j + 1 ], i, depth, code );

		return MIN( oshift, lshift );
	}
	else
	{
		if( j < 0 ) shift = 1; else shift = j + 2; // j + 1 ??? 
		return shift;
	}
}

static inline int	match_ptbp_f( PTBP * pt, BPMSKS * msks, int i, int we,
					int depth, MATCHCODE code )
{
	cntr_t			d;
        int			j;
	cntr_t			* t = pt->t.bs;

	j = 0, d = msks->D;

	do {    d  = ( d & msks->H ) | (( d & ~msks->H ) + t[ sget_t( config.t, i + j )] );

	} while((( d & msks->H ) != msks->H ) && (( j += TUPLE ) < we ));

	if(( d & msks->H ) == msks->H ) return 1;

	return match_ptbp_verify( pt, msks, d, i, depth, code );
}

static inline int	match_ptbp_l( PTBP * pt, BPMSKS * msks, int i, int we,
					int depth, MATCHCODE code )
{
	cntr_t			d;
        int			j;
	cntr_t			* t = pt->t.bs;

	if( i > pt->i + TUPLE )
	{
		j = 0, d = msks->D;

		do {    d  += t[ sget_t( config.t, i + j )];

	//	} while(( j += TUPLE ) < we );
		} while((( d & msks->H ) != msks->H ) && (( j += TUPLE ) < we ));

		if( j >= we ) 
		{
			pt->d = d;
			pt->i = i;
		}
	}
	else
	{
		d = pt->d = pt->d - t[ sget_t( config.t, pt->i )] 
		                  + t[ sget_t( config.t, i + we - 1 )];

		pt->i = i;
	}

	if(( d & msks->H ) == msks->H ) return 1;

	return match_ptbp_verify( pt, msks, d, i, depth, code );
}

static inline int	match_ptbp_laq_b( PTBP * pt, BPMSKS * msks, int i, int j,
					  int depth, MATCHCODE code )
{
	cntr_t			d, dO;
	cntr_t			** t = pt->t.bsl;
	int			l = 0, shift, oshift = 1, lshift;

	d = dO = msks->D;

	do {    d  = ( d & msks->H ) | (( d & ~msks->H ) + t[ l++ ][ sget_t( config.t, i + j )] );
	        if(( d & msks->H ) != ( dO & msks->H )) oshift = j + 1, dO = d;
	        j -= TUPLE;

	} while((( d & msks->H ) != msks->H ) && j >= 0 );

	if(( d & msks->H ) != msks->H ) 
	{
		lshift = match_ptbp_verify( pt, msks, d, i, depth, code );

		return MIN( oshift, lshift );
	}
	else
	{
		if( j < 0 ) shift = 1; else shift = j + TUPLE + 1; 
		return shift;
	}
	/*
	if( j < 0 ) shift = 1; else shift = j + TUPLE + 1; 

	if(( d & msks->H ) == msks->H ) return shift;

	return match_ptbp_verify( pt, msks, d, i, depth, code );
	*/
}

#if VECLEN != 128

static inline int	match_ptbp_b( PTBP * pt, BPMSKS * msks, int i, int j,
				      int depth, MATCHCODE code )
{
	cntr_t			d, dO;
	cntr_t			* t = pt->t.bs;
	int			shift, oshift = 1, lshift;

	d = dO = msks->D;

	do {    d  = ( d & msks->H ) | (( d & ~msks->H ) + t[ sget_t( config.t, i + j )] ); // sget lolsz
	        if(( d & msks->H ) != ( dO & msks->H )) oshift = j + 1, dO = d;
	        j -= TUPLE;

	} while((( d & msks->H ) != msks->H ) && j >= 0 );

	if(( d & msks->H ) != msks->H ) 
	{
		lshift = match_ptbp_verify( pt, msks, d, i, depth, code );

		return MIN( oshift, lshift );
	}
	else
	{
		if( j < 0 ) shift = 1; else shift = j + TUPLE + 1; 
		return shift;
	}

//	if(( d & msks->H ) != ( cntr_t )0 ) return shift; else return lshift;
}

#else

static inline int	match_ptbp_b( PTBP * pt, BPMSKS * msks, int i, int j,
				      int depth, MATCHCODE code )
{
	cntr_t			d, dO, k = config.k + 1;
	cntr_t			* t = pt->t.bs;
	int			shift, oshift = 1, lshift;

	d = dO = 0;

	do {    d  = d + t[ sget_t( config.t, i + j )];
	        if(( d < k ) != ( dO < k )) oshift = j + 1, dO = d;
	        j -= TUPLE;

	} while(( d < k ) && j >= 0 );

	if( d < k )
	{
		lshift = match_ptbp_verify( pt, msks, d, i, depth, code );

		return MIN( oshift, lshift );
	}
	else
	{
		if( j < 0 ) shift = 1; else shift = j + TUPLE + 1; 
		return shift;
	}

//	if(( d & msks->H ) != ( cntr_t )0 ) return shift; else return lshift;
}

#endif

static int	match_ptbp( PTBP * pt, BPMSKS * msks, int i, int depth, MATCHCODE code )
{
        int			m, w, we;

	TIMELOOPCUTOFF;

	m = pt->mm;

//	if( config.skipf ) w = BWIN( m, k ); else w = SWIN( m, k );
//	we = w - TUPLE + 1;

	w = DWIN( m, config.k );
	if( config.skipb ) we = w - TUPLE; else we = w - TUPLE + 1;

	if( config.opt )
	{
		if( config.skipb && !config.laq )
			return match_ptbp_opt_b( pt, msks, i, we, depth, code );
		else if( config.skipb && config.laq )
			return match_ptbp_opt_laq_b( pt, msks, i, we, depth, code );
		else
			return match_ptbp_opt( pt, msks, i, we, depth, code );
	}
	else if( config.laq && config.skipb )
	{
		return match_ptbp_laq_b( pt, msks, i, we, depth, code );
	} 
	else if( config.skipb )
	{
		return match_ptbp_b( pt, msks, i, we, depth, code );
	} 
	else if( config.skipf )
	{
		return match_ptbp_f( pt, msks, i, we, depth, code );
	} 
	else
	{
	//	return match_ptbp_l( pt, msks, i, we, depth, code );
		return match_ptbp_f( pt, msks, i, we, depth, code );
	} 

//	return match_ptbp_verify( pt, msks, d, i, depth, code );
}

int	match_b_pt( PTB * pt, int i, int depth, MATCHCODE code )
{
	int             j, h, matches = 0;
	cntr_laq_t	d, o, nom;

	if( i < 0 ) return 0;

	nom = ~pt->om & pt->mm;
	o = ( cntr_laq_t )0;
	d = ( cntr_laq_t )0;

	for( j = i, h = 0; j < config.n - TUPLE && h < pt->b; h++, j += pt->h )
	{
		d = ( d << pt->bits ) + pt->t[ sget_t( config.t, j ) ];
		o = ( o << pt->bits ) | ( d & pt->om );
		d &= nom;
	}

	if((( d + o ) & pt->mm ) >= pt->lim ) return 0;

	if( pt->p || ( depth >= config.maxdepth ))
	{
	//	matches += verify_b_pt( config.t, pt, j - TUPLE, config.n, config.k, code );
		matches += verify_b_pt( config.t, pt, i, config.n, config.k, code );

		return matches;
	}

	if( !pt->p ) 
        {
		matches += match_b_pt( pt->l, i, depth + 1, code );
	        matches += match_b_pt( pt->r, i, depth + 1, code );
	}

	return matches;
}



int	lcm_b_pt( const unsigned char * t, int n, unsigned char ** p, int M, int k, int md, 
		  int groups, MATCHCODE code )
{
	int         	i, m = 0, gi, gM, so, avg = 0, avgi = 1;
	PTB *		pt;
	cntr_laq_t	d, nom, o;
	long		tp = 0L, ts = 0L;

	config.matches = 0;

	GROUPB( gi, groups, M, gM, p );

	TIMESTART("Preprocessing... "); 

	bp_setmap( fmap );

	pt = build_pt_b( p, gM, k, md );

	m = pt->m;

	TIMEPP( tp );

	TIMESTART("Searching... "); 

	bp_setmap( vmap );

        nom = ~pt->om & pt->mm;
//      pt->lim = ( cntr_laq_t )( k + 1 ) << (( pt->b - 1 ) * pt->bits );
	o = ( cntr_laq_t )0;
	d = ( cntr_laq_t )0;
	o = pt->om;

	so = pt->h * ( pt->b - 1 );

	for( i = 0; i < n - pt->m; i += pt->h )
	{
		d = ( d << pt->bits ) + pt->t[ sget_t( t, i ) ];
		o = ( o << pt->bits ) | ( d & pt->om );
		d &= nom;

	//	printf("%d\n", (( d + o ) & pt->mm ) >> (( pt->b - 1 ) * pt->bits ));

		if((( d + o ) & pt->mm ) < pt->lim ) 
		{
			if( pt->p ) 
				config.matches += verify_b_pt( t, pt, i - so, n, k, code );
	    		else
			        config.matches += match_b_pt( pt->l, i - so, 1, code )
					        + match_b_pt( pt->r, i - so, 1, code );

		}
	}

	TIMESCAN( ts );

	free_pt_b( pt );

	GROUPE( gM, p );

	TIMETOT( tp, ts );

	return config.matches;
}




int	cmfn( int groups, MATCHCODE code, int reps )
{
    startclock();
	
	int         	i, j, w, we, m = 0, mm, gi, gM;
	int             n = config.n, dd;
	int		avg = 0, avgi = 1, rep;
        cntr_t		d;
	PTREE *		pt = 0;
	PTBP *		ptbp = 0;
	PTB *		ptb = 0;
	PTREE *		ptm = 0;
	PTREE *		ptwm = 0;
	long		tp = 0L, ts = 0L;
	BPMSKS		msks;
	int		M = config.M, k = config.k, md = config.maxdepth;
	unsigned char	** p = config.p, * t = config.t;

    tprep += stopclock()*reps;

	( void )avg, ( void )avgi;

	config.matches = 0;

	REPB( rep, reps );

	M = config.M;
	t = config.t;
	p = config.p;

	if( config.laq && !config.skipb )
	{
		WARNINGMSG("LAQ not supported here...");
		return 0;
	}

	GROUPB( gi, groups, M, gM, p );

	//TIMESTART("Preprocessing... "); 
    startclock();

	if( config.wm )
	{	
	//	m = strlen( *p );
	//	ppWM( *p, m, config.k );
		bp_setmap( vmap );

		ptm = build_pt( p, gM, md, MYERS );

		ptm->v.m->b = 0;
		ptm->v.m->e = n;

		ptwm = build_pt( p, gM, md, WUMANBER );
	} 
	else if( config.myers )
	{
		bp_setmap( vmap );

		ptm = build_pt( p, gM, md, MYERS );

		ptm->v.m->b = 0;
		ptm->v.m->e = n;
	}
	else if( config.use_bp_cntrs )
	{
		bp_setmap( fmap );

		for( i = m = 0; i < gM; i++ ) 
			if( m < ( mm = strlen(( char * )p[ i ] ))) m = mm;


		if( config.skipf || config.skipb )
			msks.B = CLOG2( MAX( k + TUPLE + 1, MAX( 2 * TUPLE, 2 * k + 1 )));
		else
			msks.B = CLOG2( m - k ) + 1;

#		if VECLEN == 128
		msks.B = 8;
#		endif

		msks.A = CNTR_BITS / ( msks.B + config.opt );

		DBGMSG("A=%d, B=%d\n", msks.A, msks.B );

		if( config.laq && config.skipb )
			ptbp = build_ptbp( p, gM, msks.A, msks.B + config.opt, md, BLAQ );
        else
			ptbp = build_ptbp( p, gM, msks.A, msks.B + config.opt, md, BASIC );

		m = ptbp->mm;

		/*
		for( msks.H = msks.D = msks.J = msks.K = ( cntr_t )0, i = 0; i < msks.A; i++ )
		{
		//	msks.H = ( msks.H << ( msks.B + config.opt )) | (( cntr_t )1 << ( msks.B - 1 ));
		//	msks.D = ( msks.D << ( msks.B + config.opt )) | ((( cntr_t )1 << ( msks.B - 1 )) - ( cntr_t )k - ( cntr_t )1 );
		//	msks.J = ( msks.J << ( msks.B + config.opt )) | (( cntr_t )1 << ( msks.B - 1 + config.opt ));
		//	msks.K = ( msks.K << msks.B ) | ( cntr_t )k;
		}
		*/

		msks.H = BF_COPY(( cntr_t )1 << ( msks.B - 1 ), msks.A, msks.B + config.opt );
		msks.D = BF_COPY((( cntr_t )1 << ( msks.B - 1 )) - ( cntr_t )k - ( cntr_t )1, msks.A, msks.B + config.opt );
		msks.J = BF_COPY(( cntr_t )1 << ( msks.B - 1 + config.opt ), msks.A, msks.B + config.opt );
		msks.K = BF_COPY(( cntr_t )k, msks.A, msks.B );
		msks.K |= msks.H;
	}
	else if( config.laq && !config.strictblaq && config.skipb )
	{
		bp_setmap( fmap );
		pt = build_pt( p, gM, md, BLAQ );
		m = pt->m;
	}
	else if( config.laq && config.strictblaq && config.skipb )
	{
		bp_setmap( fmap );
		ptb = build_pt_b( p, gM, k, md );
		m = ptb->m;
		math_build_dividend_div_const( ptb->bits );
	}
	else
	{
		if( config.si )
		{
			bp_setmap( vmap );
			pt = build_pt( p, gM, md, MYERS );
			bp_setmap( fmap );
			build_p_pt( pt, p, gM, md );
		}
		else
		{
			bp_setmap( fmap );
			pt = build_pt( p, gM, md, BASIC );
		}

		m = pt->m;
	}

//	if( config.skipf ) w = BWIN( m, k ); else w = SWIN( m, k );

	if( !config.myers && !config.wm )
	{
		w = DWIN( m, k );

		if( w < TUPLE ) 
		{
			FATALERRORMSG("Cannot set up sliding window: use smaller tuple, or smaller k. Exiting...");
		}
	} else w = 0;

	//TIMEPP( tp );
    tprep += stopclock();




//////////////////// search
	startclock();
	config.matches = 0;

	//TIMESTART("Searching... "); 
    //startclock();

	bp_setmap( vmap );

	if( config.wm )
	{
		VERBOSEMSG("Using -W");

	//	matches += wm( m, config.n, config.t, config.k );
		config.matches += wm_search( ptwm, ptm, ptwm->m, 0, config.n, config.t, config.k );
	} 
	else if( config.myers )
	{
		VERBOSEMSG("Using -M");

		config.matches += myers( ptm, code );

	}
	else if( config.skipf && config.opt && !config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -Sf -O");

		we = w - TUPLE + 1;

	//	config.data = salloc(( config.m + TUPLE ) * sizeof( int ));

		for( i = 0; i < n - m; i += w )
		{
			match_pt_opt( pt, i, we, 0, code );

	//		avg += w;
	//		avgi++;

		}
		VERBOSEMSGD("OPT F AVG=%1.2f", ( double )avg / avgi );
	} 
	else if( config.laq && !config.strictblaq && config.skipb && !config.use_bp_cntrs && !config.opt )
	{
		VERBOSEMSG("Using -L -Sb");

		we = w - TUPLE - 0;

		for( i = 0; i < n - m; i += w )
		{
			w = match_pt_laq_b( pt, i, we, 0, code );

			avg += w;
			avgi++;
		}
		VERBOSEMSGD("B AVG=%1.2f", ( double )avg / avgi );
	}
	else if( config.laq && !config.strictblaq && config.skipb && !config.use_bp_cntrs && config.opt )
	{
		VERBOSEMSG("Using -L -Sb -O");

		we = w - TUPLE - 0;

		for( i = 0; i < n - m; i += w )
		{
			w = match_pt_opt_laq_b( pt, i, we, 0, code );

			avg += w;
			avgi++;
		}
		VERBOSEMSGD("B AVG=%1.2f", ( double )avg / avgi );
	}
	else if( config.laq && config.strictblaq && config.skipb && !config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -L -F -Sb");

		we = ptb->b * ptb->h - TUPLE - 1 + config.k;

		for( i = 0; i < n - m; i += w )
		{
			w = match_pt_laq_b_strict( ptb, i, we, 0, code );

			avg += w;
			avgi++;
		}
		VERBOSEMSGD("B AVG=%1.2f", ( double )avg / avgi );
	}
	else if( config.skipb && config.opt && !config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -Sb -O");

		we = w - TUPLE;

	//	config.data = salloc(( config.m + TUPLE ) * sizeof( int ));

		for( i = 0; i < n - m; i += w )
		{
			w = match_pt_opt_b( pt, i, we, 0, code );

			avg += w;
			avgi++;

		}
		VERBOSEMSGD("OPT B AVG=%1.2f", ( double )avg / avgi );
	}
	else if( config.skipf && !config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -Sf");

		we = w - TUPLE + 1;

		for( i = 0; i < n - m; i += w )
		{
			match_pt_f( pt, i, we, 0, code );

		//	avg += w;
		//	avgi++;

		}
		VERBOSEMSGD("AVG=%1.2f", ( double )avg / avgi );
	}
	else if( config.skipb && !config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -Sb");

		we = w - TUPLE - 0;

		for( i = 0; i < n - m; i += w )
		{
			w = match_pt_b( pt, i, we, 0, code );

			avg += w;
			avgi++;
		}
		VERBOSEMSGD("B AVG=%1.2f", ( double )avg / avgi );
	}
	else if( !config.skipf && !config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -");

		for( i = dd = 0; i < w - TUPLE; i += TUPLE )
			dd += pt->t.s[ sget_t( t, i ) ];

		for( ; i < n - m; i += TUPLE )
		{
			dd += pt->t.s[ sget_t( t, i ) ];
			if( dd <= k ) 
			{
				if( pt->p ) 
					config.matches += verify_pt( t, pt, i - w + TUPLE, n, k, code );
				else
				{
					match_pt( pt->l, i - w + TUPLE, 1, code );
					match_pt( pt->r, i - w + TUPLE, 1, code );
				}
			}

			dd -= pt->t.s[ sget_t( t, i - w + TUPLE ) ];
		}
	}
	else if( config.skipf && config.opt && config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -Sf -O -B");

		we = w - TUPLE + 1;

		for( i = 0; i < n - m; i += w )
		{
			match_ptbp_opt( ptbp, &msks, i, we, 0, code );
		}
	}
	else if( config.skipb && config.opt && !config.laq && config.use_bp_cntrs ) //entra acá TODO
	{
		VERBOSEMSG("Using -Sb -O -B");

		we = w - TUPLE;

		for( i = 0; i < n - m; i += w )
		{
			w = match_ptbp_opt_b( ptbp, &msks, i, we, 0, code );

			avg += w;
			avgi++;
		}
		VERBOSEMSGD("B OPT BP AVG=%1.2f", ( double )avg / avgi );
	}
	else if( config.skipb && config.opt && config.laq && config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -Sb -O -B -L");

		we = w - TUPLE;

		for( i = 0; i < n - m; i += w )
		{
			w = match_ptbp_opt_laq_b( ptbp, &msks, i, we, 0, code );

			avg += w;
			avgi++;
		}
		VERBOSEMSGD("B OPT BP AVG=%1.2f", ( double )avg / avgi );
	}
	else if( config.skipf && config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -Sf -B");

		we = w - TUPLE + 1;

		for( i = 0; i < n - m; i += w )
		{
			match_ptbp_f( ptbp, &msks, i, we, 0, code );
		}
	}
	else if( config.laq && config.skipb && config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -L -Sb -B");

		we = w - TUPLE;

		for( i = 0; i < n - m; i += w )
		{
			w = match_ptbp_laq_b( ptbp, &msks, i, we, 0, code );

			avg += w;
			avgi++;
		}
		VERBOSEMSGD("B BP AVG=%1.2f", ( double )avg / avgi );
	}
	else if( config.skipb && config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -Sb -B");

		we = w - TUPLE;

		for( i = 0; i < n - m; i += w )
		{
			w = match_ptbp_b( ptbp, &msks, i, we, 0, code );

			avg += w;
			avgi++;
		}
		VERBOSEMSGD("B BP AVG=%1.2f", ( double )avg / avgi );
	}
	else if( !config.skipf && !config.skipb && config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -B");

		for( i = 0, d = ( cntr_t )0; i < w - TUPLE; i += TUPLE )
			d += ptbp->t.bs[ sget_t( t, i ) ];

		for( ; i < n - m; i += TUPLE )
		{
			d += ptbp->t.bs[ sget_t( t, i ) ];
			if((( msks.K - d ) & msks.H ) != ( cntr_t )0 ) 
		        {
				cntr_t	dd = d;

				if( ptbp->p )
				{
					FOR_CNTRS_NOVER_K( j, ptbp, dd, ( &msks ))
						config.matches += verify_cm( t, n, i - w + TUPLE, ptbp->p[ j ], ptbp->v[ j ], ptbp->m[ j ], k, code );
				}
				else
				{
					FOR_CNTRS_NOVER_K( j, ptbp, dd, ( &msks ))
						match_ptbp( ptbp->c[ j ], &msks, i - w + TUPLE, 1, code );
				}
			}

			d -= ptbp->t.bs[ sget_t( t, i - w + TUPLE ) ];
		}
	}
	else
	{
		FATALERRORMSG("Invalid algorithm...");
	}

	//TIMESCAN( ts );
    //texec += stopclock();
	
	if( pt && config.laq ) free_pt( pt, BLAQ ); else
	if( pt && !config.laq ) free_pt( pt, BASIC ); else
	if( ptbp && config.laq ) free_ptbp( ptbp, BLAQ ); else
	if( ptbp && !config.laq ) free_ptbp( ptbp, BASIC ); else
	if( ptm ) free_pt( ptm, MYERS ); else
	if( ptwm ) free_pt( ptm, WUMANBER ); else
	if( ptb ) free_pt_b( ptb );
	
	texec += stopclock();
	
	/*
	if( config.use_bp_cntrs && config.laq ) free_ptbp( ptbp, BLAQ );
	else if( config.use_bp_cntrs && !config.laq ) free_ptbp( ptbp, BASIC );
	else if( config.myers ) free_pt( ptm ); 
	else if( !config.wm ) free_pt( pt ); else free_pt( ptwm );
	*/

	GROUPE( gM, p );

	TIMECUTOFF( rep, tp, ts );

	REPE( rep, tp, ts );

	TIMETOT( tp, ts );

	return config.matches;
}














int         	i, j, w, we, m = 0, mm, gi, gM;
int             n, dd;
int		avg = 0, avgi = 1, rep;
cntr_t		d;
PTREE *		pt = 0;
PTBP *		ptbp = 0;
PTB *		ptb = 0;
PTREE *		ptm = 0;
PTREE *		ptwm = 0;
long		tp = 0L, ts = 0L;
BPMSKS		msks;
int		M, k, md;
unsigned char	** p, * t;
//( void )avg, ( void )avgi;

int ereps=1, preps=1;

void cmfn_prep( int groups, MATCHCODE code, int reps )
{
    ( void )avg, ( void )avgi;
    /*
int pcount;
for(pcount=0; pcount<preps; pcount++){
*/
	if( pt && config.laq ) free_pt( pt, BLAQ ); else
	if( pt && !config.laq ) free_pt( pt, BASIC ); else
	if( ptbp && config.laq ) free_ptbp( ptbp, BLAQ ); else
	if( ptbp && !config.laq ) free_ptbp( ptbp, BASIC ); else
	if( ptm ) free_pt( ptm, MYERS ); else
	if( ptwm ) free_pt( ptm, WUMANBER ); else
	if( ptb ) free_pt_b( ptb );

    n = config.n;
    M = config.M; k = config.k; md = config.maxdepth;
    p = config.p; t = config.t;



	//REPB( rep, reps );

	M = config.M;
	t = config.t;
	p = config.p;

	if( config.laq && !config.skipb )
	{
		WARNINGMSG("LAQ not supported here...");
		return;
	}

	GROUPB( gi, groups, M, gM, p );

	//TIMESTART("Preprocessing... "); 
    //startclock();

	if( config.wm )
	{	
	//	m = strlen( *p );
	//	ppWM( *p, m, config.k );
		bp_setmap( vmap );

		ptm = build_pt( p, gM, md, MYERS );

		ptm->v.m->b = 0;
		ptm->v.m->e = n;

		ptwm = build_pt( p, gM, md, WUMANBER );
	} 
	else if( config.myers )
	{
		bp_setmap( vmap );

		ptm = build_pt( p, gM, md, MYERS );

		ptm->v.m->b = 0;
		ptm->v.m->e = n;
	}
	else if( config.use_bp_cntrs ) //entra aca
	{
		bp_setmap( fmap );

		for( i = m = 0; i < gM; i++ ) 
			if( m < ( mm = strlen(( char * )p[ i ] ))) m = mm;


		if( config.skipf || config.skipb )
			msks.B = CLOG2( MAX( k + TUPLE + 1, MAX( 2 * TUPLE, 2 * k + 1 )));
		else
			msks.B = CLOG2( m - k ) + 1;

#		if VECLEN == 128
		msks.B = 8;
#		endif

		msks.A = CNTR_BITS / ( msks.B + config.opt );

		DBGMSG("A=%d, B=%d\n", msks.A, msks.B );

		if( config.laq && config.skipb )
			ptbp = build_ptbp( p, gM, msks.A, msks.B + config.opt, md, BLAQ );
        else
			ptbp = build_ptbp( p, gM, msks.A, msks.B + config.opt, md, BASIC );

		m = ptbp->mm; //TODO acá se rompe

		/*
		for( msks.H = msks.D = msks.J = msks.K = ( cntr_t )0, i = 0; i < msks.A; i++ )
		{
		//	msks.H = ( msks.H << ( msks.B + config.opt )) | (( cntr_t )1 << ( msks.B - 1 ));
		//	msks.D = ( msks.D << ( msks.B + config.opt )) | ((( cntr_t )1 << ( msks.B - 1 )) - ( cntr_t )k - ( cntr_t )1 );
		//	msks.J = ( msks.J << ( msks.B + config.opt )) | (( cntr_t )1 << ( msks.B - 1 + config.opt ));
		//	msks.K = ( msks.K << msks.B ) | ( cntr_t )k;
		}
		*/

		msks.H = BF_COPY(( cntr_t )1 << ( msks.B - 1 ), msks.A, msks.B + config.opt );
		msks.D = BF_COPY((( cntr_t )1 << ( msks.B - 1 )) - ( cntr_t )k - ( cntr_t )1, msks.A, msks.B + config.opt );
		msks.J = BF_COPY(( cntr_t )1 << ( msks.B - 1 + config.opt ), msks.A, msks.B + config.opt );
		msks.K = BF_COPY(( cntr_t )k, msks.A, msks.B );
		msks.K |= msks.H;
	}
	else if( config.laq && !config.strictblaq && config.skipb )
	{
		bp_setmap( fmap );
		pt = build_pt( p, gM, md, BLAQ );
		m = pt->m;
	}
	else if( config.laq && config.strictblaq && config.skipb )
	{
		bp_setmap( fmap );
		ptb = build_pt_b( p, gM, k, md );
		m = ptb->m;
		math_build_dividend_div_const( ptb->bits );
	}
	else
	{
		if( config.si )
		{
			bp_setmap( vmap );
			pt = build_pt( p, gM, md, MYERS );
			bp_setmap( fmap );
			build_p_pt( pt, p, gM, md );
		}
		else
		{
			bp_setmap( fmap );
			pt = build_pt( p, gM, md, BASIC );
		}

		m = pt->m;
	}

//	if( config.skipf ) w = BWIN( m, k ); else w = SWIN( m, k );

	if( !config.myers && !config.wm )
	{
		w = DWIN( m, k );

		if( w < TUPLE ) 
		{
			FATALERRORMSG("Cannot set up sliding window: use smaller tuple, or smaller k. Exiting...");
		}
	} else w = 0;

	//TIMEPP( tp );
    //tprep += stopclock();
    
    int counter;
    for(counter=0; counter<ereps; counter++)
        cmfn_exec(groups, code, REPETITIONS);
    
    GROUPE( gM, p );
    return;
}



int cmfn_exec( int groups, MATCHCODE code, int reps )
{
	config.matches = 0;

/*
    n = config.n;
    M = config.M; k = config.k; md = config.maxdepth;
    p = config.p; t = config.t;
*/
	//REPB( rep, reps );

	M = config.M;
	t = config.t;
	p = config.p;


	//GROUPB( gi, groups, M, gM, p );
	//TIMESTART("Searching... "); 

	bp_setmap( vmap );

	if( config.wm )
	{
		VERBOSEMSG("Using -W");

	//	matches += wm( m, config.n, config.t, config.k );
		config.matches += wm_search( ptwm, ptm, ptwm->m, 0, config.n, config.t, config.k );
	} 
	else if( config.myers )
	{
		VERBOSEMSG("Using -M");

		config.matches += myers( ptm, code );

	}
	else if( config.skipf && config.opt && !config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -Sf -O");

		we = w - TUPLE + 1;

	//	config.data = salloc(( config.m + TUPLE ) * sizeof( int ));

		for( i = 0; i < n - m; i += w )
		{
			match_pt_opt( pt, i, we, 0, code );

	//		avg += w;
	//		avgi++;

		}
		VERBOSEMSGD("OPT F AVG=%1.2f", ( double )avg / avgi );
	} 
	else if( config.laq && !config.strictblaq && config.skipb && !config.use_bp_cntrs && !config.opt )
	{
		VERBOSEMSG("Using -L -Sb");

		we = w - TUPLE - 0;

		for( i = 0; i < n - m; i += w )
		{
			w = match_pt_laq_b( pt, i, we, 0, code );

			avg += w;
			avgi++;
		}
		VERBOSEMSGD("B AVG=%1.2f", ( double )avg / avgi );
	}
	else if( config.laq && !config.strictblaq && config.skipb && !config.use_bp_cntrs && config.opt )
	{
		VERBOSEMSG("Using -L -Sb -O");

		we = w - TUPLE - 0;

		for( i = 0; i < n - m; i += w )
		{
			w = match_pt_opt_laq_b( pt, i, we, 0, code );

			avg += w;
			avgi++;
		}
		VERBOSEMSGD("B AVG=%1.2f", ( double )avg / avgi );
	}
	else if( config.laq && config.strictblaq && config.skipb && !config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -L -F -Sb");

		we = ptb->b * ptb->h - TUPLE - 1 + config.k;

		for( i = 0; i < n - m; i += w )
		{
			w = match_pt_laq_b_strict( ptb, i, we, 0, code );

			avg += w;
			avgi++;
		}
		VERBOSEMSGD("B AVG=%1.2f", ( double )avg / avgi );
	}
	else if( config.skipb && config.opt && !config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -Sb -O");

		we = w - TUPLE;

	//	config.data = salloc(( config.m + TUPLE ) * sizeof( int ));

		for( i = 0; i < n - m; i += w )
		{
			w = match_pt_opt_b( pt, i, we, 0, code );

			avg += w;
			avgi++;

		}
		VERBOSEMSGD("OPT B AVG=%1.2f", ( double )avg / avgi );
	}
	else if( config.skipf && !config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -Sf");

		we = w - TUPLE + 1;

		for( i = 0; i < n - m; i += w )
		{
			match_pt_f( pt, i, we, 0, code );

		//	avg += w;
		//	avgi++;

		}
		VERBOSEMSGD("AVG=%1.2f", ( double )avg / avgi );
	}
	else if( config.skipb && !config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -Sb");

		we = w - TUPLE - 0;

		for( i = 0; i < n - m; i += w )
		{
			w = match_pt_b( pt, i, we, 0, code );

			avg += w;
			avgi++;
		}
		VERBOSEMSGD("B AVG=%1.2f", ( double )avg / avgi );
	}
	else if( !config.skipf && !config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -");

		for( i = dd = 0; i < w - TUPLE; i += TUPLE )
			dd += pt->t.s[ sget_t( t, i ) ];

		for( ; i < n - m; i += TUPLE )
		{
			dd += pt->t.s[ sget_t( t, i ) ];
			if( dd <= k ) 
			{
				if( pt->p ) 
					config.matches += verify_pt( t, pt, i - w + TUPLE, n, k, code );
				else
				{
					match_pt( pt->l, i - w + TUPLE, 1, code );
					match_pt( pt->r, i - w + TUPLE, 1, code );
				}
			}

			dd -= pt->t.s[ sget_t( t, i - w + TUPLE ) ];
		}
	}
	else if( config.skipf && config.opt && config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -Sf -O -B");

		we = w - TUPLE + 1;

		for( i = 0; i < n - m; i += w )
		{
			match_ptbp_opt( ptbp, &msks, i, we, 0, code );
		}
	}
	else if( config.skipb && config.opt && !config.laq && config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -Sb -O -B");

		we = w - TUPLE;

		for( i = 0; i < n - m; i += w )
		{
			w = match_ptbp_opt_b( ptbp, &msks, i, we, 0, code );

			avg += w;
			avgi++;
		}
		VERBOSEMSGD("B OPT BP AVG=%1.2f", ( double )avg / avgi );
	}
	else if( config.skipb && config.opt && config.laq && config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -Sb -O -B -L");

		we = w - TUPLE;

		for( i = 0; i < n - m; i += w )
		{
			w = match_ptbp_opt_laq_b( ptbp, &msks, i, we, 0, code );

			avg += w;
			avgi++;
		}
		VERBOSEMSGD("B OPT BP AVG=%1.2f", ( double )avg / avgi );
	}
	else if( config.skipf && config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -Sf -B");

		we = w - TUPLE + 1;

		for( i = 0; i < n - m; i += w )
		{
			match_ptbp_f( ptbp, &msks, i, we, 0, code );
		}
	}
	else if( config.laq && config.skipb && config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -L -Sb -B");

		we = w - TUPLE;

		for( i = 0; i < n - m; i += w )
		{
			w = match_ptbp_laq_b( ptbp, &msks, i, we, 0, code );

			avg += w;
			avgi++;
		}
		VERBOSEMSGD("B BP AVG=%1.2f", ( double )avg / avgi );
	}
	else if( config.skipb && config.use_bp_cntrs ) //TODO entra acá
	{
		VERBOSEMSG("Using -Sb -B");

		we = w - TUPLE;

		for( i = 0; i < n - m; i += w )
		{
			w = match_ptbp_b( ptbp, &msks, i, we, 0, code );

			avg += w;
			avgi++;
		}
		VERBOSEMSGD("B BP AVG=%1.2f", ( double )avg / avgi );
	}
	else if( !config.skipf && !config.skipb && config.use_bp_cntrs )
	{
		VERBOSEMSG("Using -B");

		for( i = 0, d = ( cntr_t )0; i < w - TUPLE; i += TUPLE )
			d += ptbp->t.bs[ sget_t( t, i ) ];

		for( ; i < n - m; i += TUPLE )
		{
			d += ptbp->t.bs[ sget_t( t, i ) ];
			if((( msks.K - d ) & msks.H ) != ( cntr_t )0 ) 
		        {
				cntr_t	dd = d;

				if( ptbp->p )
				{
					FOR_CNTRS_NOVER_K( j, ptbp, dd, ( &msks ))
						config.matches += verify_cm( t, n, i - w + TUPLE, ptbp->p[ j ], ptbp->v[ j ], ptbp->m[ j ], k, code );
				}
				else
				{
					FOR_CNTRS_NOVER_K( j, ptbp, dd, ( &msks ))
						match_ptbp( ptbp->c[ j ], &msks, i - w + TUPLE, 1, code );
				}
			}

			d -= ptbp->t.bs[ sget_t( t, i - w + TUPLE ) ];
		}
	}
	else
	{
		FATALERRORMSG("Invalid algorithm...");
	}
	
	//GROUPE( gM, p );
	
	return config.matches;
}


































#if 0

static inline int	XXmatch_pt_laq_b_strict( PTB * pt, int i, int j, int depth, MATCHCODE code )
{
//	int 		l = 0, ls = -pt->bits, le = (pt->b-1) * pt->bits, h, matches = 0, v, sj = j;
	int 		l = 0, h, v, sj = j, shift; //, lshift, rshift;
        cntr_laq_t	d = ( cntr_laq_t )0, o;
//	cntr_laq_t	lim = pt->lim, ll = ( cntr_laq_t )0;
//	cntr_laq_t	nom = ~pt->om & pt->mm;

//	msks.H = BF_COPY(( cntr_t )1 << ( msks.B - 1 ), msks.A, msks.B + config.opt );
//	msks.D = BF_COPY((( cntr_t )1 << ( msks.B - 1 )) - ( cntr_t )k - ( cntr_t )1, msks.A, msks.B + config.opt );

	do {    d += pt->t[ sget_t( config.t, i + j ) ] << ( l++ * pt->bits );

	//	d += pt->t[ sget_t( config.t, i + j ) ] << ( ls += pt->bits );
	//	o = d & pt->om;
	//	d &= ~o;

		d &= ~( o = d & pt->om );
	        j -= TUPLE;

		/*
		if((( d >> ( l* pt->bits )) & pt->mm ) >= lim ) 
		{
			ll++;
			lim >>= pt->bits;
			if( ll > 1 ) break;
		}
		*/

//	} while( l < pt->b );
	} while(((( d | o )) < pt->lim ) && ( l < pt->b ));
//	} while(((( d | o ) & pt->mm ) < pt->lim ) && ( l < pt->b ));
//	} while(((( d | o ) & pt->mm ) < pt->lim ) && ( ls < le ));
//	} while(((( d | o ) & pt->mm ) < pt->lim ) && ( j >= 0 ));

//	printf("%d, %d, %d\n", l, pt->b, j );

//	if( l > pt->b ) printf("Timbeeer!\n");

	d |= o;

	v = ( d & pt->mm ) < pt->lim;

	for( shift = TUPLE, h = 1, d <<= pt->bits; h < pt->b; h++ )
	{
		if(( d & pt->mm ) >= pt->lim ) 
			shift += TUPLE; 
		else 
		{
			break;
		}

		d <<= pt->bits;
	}

//	printf("%d/%d\t", l, h - 1 );


/*
	if( depth == 0 ) for( h = 0; h < pt->bits; h++ )
	{
		if(( d & pt->mm ) >= pt->lim ) 
			*sw = h * TUPLE; 
		else 
		{
			printf("%d\t", h - 1 );
			break;
		}

		if( *sw == 0 ) *sw = TUPLE;

		d <<= pt->bits;
	}
*/
	if( v ) shift = match_pt_lbs_verify( pt, i, sj, shift, depth, code );

	/*
	{
		if( pt->p ) 
		{
			config.matches += verify_b_pt( config.t, pt, i, config.n, config.k, code );
		}
		else
		{
			lshift = match_pt_laq_b_strict( pt->l, i, sj, depth + 1, code );
			rshift = match_pt_laq_b_strict( pt->r, i, sj, depth + 1, code );

			shift = MIN( lshift, rshift );
		}
	}
	*/

	return shift;
} 


#endif
