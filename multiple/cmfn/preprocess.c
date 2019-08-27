//
//	(C) Copyright 2002 by Kimmo Fredriksson.
//
//	The code can be modified and redistributed if this note is left
//	intact.
//
//	Comments, improvements and bug-reports can be emailed to kf@iki.fi
//

#include	<stdio.h>
#include	<string.h>
#include	<math.h>
#include	"kstdlib.h"
#include	"bitfields.h"
#include	"bped.h"
#include	"squeeze.h"
#include	"preprocess.h"
#include	"config.h"

static inline BMYERS *	init_bped()
{
	BMYERS	* v;

	v = salloc( sizeof( BMYERS ));
	v->peq = 0;
        v->vp = 0;
        v->vn = 0;
        v->score = 0;

	v->b = v->e = 0;

	return v;
}

static inline void free_bped( BMYERS * v )
{
	if( v->peq ) free_2d(( void ** )v->peq );
	if( v->vp ) free( v->vp );
	if( v->vn ) free( v->vn );
	if( v->score ) free( v->score );
	free( v );
}

static inline void	dpveccopy( dpvec * d, dpvec * s, int n )
{
//	int	i;

//	This could be vectorized by icc...

	do *d++ = *s++; while( --n );

//	for( i = 0; i < n; i++ ) d[ i ] = s[ i ];
}


static inline char	sum_bits( dpvec p, dpvec n, int t )
{
	int	c = t, min = t;

	do {	if( p & ( dpvec )1 ) c++;
		if( n & ( dpvec )1 ) c--;

		if( c < min ) min = c;

		p >>= 1;
		n >>= 1;

	} while( p != ( dpvec )0 || n != ( dpvec )0 );

	return min;
}


static void	sum_bits_c( int p, int n, VECSUM * v )
{
	int	cp = 0, cn = 0, min = 0, i;

	i = ( p << 8 ) + n;

	do {	cp += p & 1;
		cn += n & 1;

		if( cp - cn < min ) min = cp - cn;

		p >>= 1;
		n >>= 1;

	} while( p || n );

	v->i[ i ] = cp - cn;
	v->m[ i ] = min;

//	printf("%d %d %d %d\n", cp, cn, cp - cn, min );
}

static void	build_vecsum( VECSUM * V )
{
	int	i, j;

	for( i = 0; i < 256; i++ ) 
		for( j = 0; j < 256; j++ ) sum_bits_c( i, j, V );
}

/*
static inline int	sum_bits_vecsum( dpvec p, dpvec n, int t, VECSUM * v )
{
	int	j;
	int	min = t, s = t;

	do {	j = (( unsigned )( p & 0xff ) << 8 ) + 
		     ( unsigned )( n & 0xff );

		if( s + v->m[ j ] < min ) min = s + v->m[ j ];

		s += v->i[ j ];

		p >>= 8;
		n >>= 8;

	} while( p != ( dpvec )0 || n != ( dpvec )0 );

	return min;
}
*/

static inline int       sum_bits_vecsum( dpvec * pp, dpvec * nn, int b, int t, 
                                         VECSUM * v )
{
        int     	i = 0, j = 0, k;
        int 		min = t, s = t;
        dpvec   	p, n;

        for( i = 0; i < b; i++ ) 
	{
		p = *pp++;
		n = *nn++;

		for( k = 0; k < ( int )sizeof( dpvec ); k++ ) 
                {
			j = (( unsigned )( p & 0xff ) << 8 ) + 
			     ( unsigned )( n & 0xff );

                        if( s + v->m[ j ] < min ) min = s + v->m[ j ];

                        s += v->i[ j ];

                        p >>= 8;
                        n >>= 8;

		} // while( p != ( dpvec )0 || n != ( dpvec )0 );

	}

        return min;
}

static inline void	sum_bit_laq_b( dpvec * p, dpvec * n, int m, int t, char ** tl, int tuple )
{
        int     	i = 0;
        int 		min = t, s = t;
	int		b, w = t, blocks;

	blocks = ( m + t - 1 ) / t;

	b = m - ( blocks - 1 ) * w - 1;

        for( i = 0; i < m; i++ ) 
	{
		s += bv_tstbit((( BITVPTR )p ), i ) - 
		     bv_tstbit((( BITVPTR )n ), i );

		if( s < min ) min = s;

		if( i >= b ) 
		{
			tl[ --blocks ][ tuple ] = min;
			b += w;
		//	printf("%d\t", i );
		}
	}
//	printf("\n%d\n", blocks );
}


static inline void	XXXsum_bit_laq_b( dpvec * p, dpvec * n, int m, int t, char ** tl, int tuple )
{
        int     	i = 0;
        int 		min = t, s = t;
	int		b, w = t, blocks;

	blocks = ( m + t - 1 - config.k ) / t;

	b = m - ( blocks - 1 ) * w - 1 + config.k - w;

        for( i = 0; i < m; i++ ) 
	{
		s += bv_tstbit((( BITVPTR )p ), i ) - 
		     bv_tstbit((( BITVPTR )n ), i );

		if( s < min ) min = s;

		if( i == b ) 
		{
			tl[ --blocks ][ tuple ] = min;
			b += w;
			printf("%d\t", i );
		}
	}
	printf("\n%d\n", blocks );
}


static void	trie_matching_tuples_r( int d, unsigned tuple, int b,
					dpvec ** vp, dpvec ** vn, 
					void * t, dpvec ** eq, VECSUM * V,
					unsigned tm, int m )
{
	int		i;
	unsigned	tt;
	char		* ts = ( char * )t;
	char		** tl = ( char ** )t;

#	ifdef	SAVESPACE
	unsigned	ts = tm;

	tm *= SIGMA;
#	endif

	if( d == TUPLE )
	{
		if( !m )
			ts[ tuple ] = sum_bits_vecsum( vp[ d ], vn[ d ], b, config.TUPLE, V );
		else
			sum_bit_laq_b( vp[ d ], vn[ d ], m, config.TUPLE, tl, tuple );
	}
	else for( i = 0; i < config.fSIGMA; i++ )
	{
		bed_c( i, b, eq, vp[ d ], vn[ d ], vp[ d + 1 ], vn[ d + 1 ] );
#		ifdef	SAVESPACE
		tt = tuple + ( i * ts );
#		else
		tt = tuple | ( i << ( d * config.TAB ));
#		endif
		trie_matching_tuples_r( d + 1, tt, b, vp, vn, t, eq, V, tm, m );
	}
}


static inline int	min_hd_score( int * score, int m )
{
	int	i, d;

	for( i = 1, d = score[ 0 ]; i < m; i++ ) 
		if( score[ i ] < d ) d = score[ i ];

	return d;
}

static inline void	add_hd_score( const unsigned char * p, int m, int c, int * scoreo, int * score )
{
	int	i;

	for( i = 0; i < m; i++ ) 
	{
		score[ i ] = scoreo[ i ];
		if( c != fmap[ p[ i ]] ) score[ i ]++;
	}
}

static void	trie_matching_hd_tuples_r( const unsigned char * p, int m, int d, unsigned tuple, 
					   int ** score, char * t )
{
	int		i;
	unsigned	tt;

	if( d == TUPLE )
	{
		t[ tuple ] = min_hd_score( score[ d ], m - TUPLE + 1 );
	}
	else for( i = 0; i < config.fSIGMA; i++ )
	{
		add_hd_score( p + d, m - TUPLE + 1, i, score[ d ], score[ d + 1 ] );
		tt = tuple | ( i << ( d * config.TAB ));
		trie_matching_hd_tuples_r( p, m, d + 1, tt, score, t );
	}
}


static char *   trie_matching_ed_tuples( const unsigned char * c, int m, VECSUM * V, dpvec ** eq, int alg )
{
        void 		* t = 0;
	int		b, i, j;
	size_t		l;
	unsigned	tuple = 0, sm = 1;
	dpvec		** vp = 0, ** vn = 0;

	b = DPBLOCKS( m );

	vp = ( dpvec ** )allocate_2d( TUPLE + 1, b, sizeof( dpvec ));
	vn = ( dpvec ** )allocate_2d( TUPLE + 1, b, sizeof( dpvec ));

        for( j = 0; j < TUPLE + 1; j++ )
		for( i = 0; i < b; i++ ) 
			vp[ j ][ i ] = vn[ j ][ i ] = ( dpvec )0;

	if( c ) eq = prep_beq( c, m ); 

	l = config.TUPLESPACE; // 1 << ( config.TAB * TUPLE );

	if( alg == BASIC )
	{
		t = salloc( l );

		trie_matching_tuples_r( 0, tuple, b, vp, vn, t, eq, V, sm, 0 );
	}
	else if( alg == BLAQ )
	{
		t = allocate_2d(( m + config.TUPLE - 1 ) / config.TUPLE, l, 1 );

		trie_matching_tuples_r( 0, tuple, b, vp, vn, t, eq, V, sm, m );
	}

	if( c ) free_2d(( void ** )eq );

	free_2d(( void ** )vn );
	free_2d(( void ** )vp );

	return t;
}

static char *   trie_matching_hd_tuples( const unsigned char * c, int m )
{
        char 		* t;
	int		l, i;
	unsigned	tuple = 0;
	int		** score;

	score = ( int ** )allocate_2d( TUPLE + 1, m, sizeof( int ));

	for( i = 0; i < m; i++ ) score[ 0 ][ i ] = 0;

	l = config.TUPLESPACE; // 1 << ( config.TAB * TUPLE );
	t = ( char * )salloc( l );

	trie_matching_hd_tuples_r( c, m, 0, tuple, score, t );

	free_2d(( void ** )score );

	return t;
}

static void *   trie_matching_tuples( const unsigned char * c, int m, VECSUM * V, dpvec ** peq, int alg )
{
	if( config.hamming )
		return trie_matching_hd_tuples( c, m );
	else
		return trie_matching_ed_tuples( c, m, V, peq, alg );
}

static cntr_laq_t	* block_matching_tuples( PTB * pt, int k, VECSUM * V )
{
	int		w, i, j, l;
	char		* t;
	cntr_laq_t	* bt;

	// NOTE: the + 1 in the following comes from the fact that the q-grams
	// are minimized in bit-parallel (in the preprocessing), the search 
	// would not need the extra bit. The -B algorithms do not need it.

	if( config.strictblaq )
		pt->bits = CLOG2( MAX( k + TUPLE + 1, 
					MAX( 2 * TUPLE, 2 * k + 1 ))) + 1;
	else
		pt->bits = CLOG2( k + TUPLE + 1 ) + 1;


	pt->h = config.laqh;

	if( config.laqh && ( pt->h < TUPLE || pt->h > pt->m - TUPLE - k - 1 ))
	{
		WARNINGMSG("Invalid sampling rate, ignored.");
		pt->h = TUPLE;
	}
	else if( !config.laqh ) pt->h = TUPLE;

	pt->h--;

	do {	pt->h++;

	//      pt->b = ( pt->m - k - TUPLE + 1 ) / TUPLE;
		pt->b = ( pt->m - k - pt->h + 1 ) / pt->h;

	} while(( int )CNTR_LAQ_BITS < pt->b * pt->bits );

	w = pt->h + TUPLE - 1 + k;

//	printf("h: %d, q: %d, w: %d, bits: %d, blocks: %d\n", pt->h, TUPLE, w, pt->bits, pt->b );

	if(( config.laqh && ( pt->h != config.laqh )) || 
	   ( config.strictblaq && ( config.TUPLE != pt->h )))
	{
		WARNINGMSG("Cannot use the sampling rate specified, not enough bits!");
		WARNINGMSGD("Using sampling rate %d.", pt->h );
	//	fprintf( stderr, "Block lenght %d, # blocks %d, bits per block %d.\n", 
	//		 w, pt->b, pt->bits );
	}
	if( config.TUPLE != pt->h && config.strictblaq )
	{
	//	FATALERRORMSG("Cannot use strict laq, not enough bits, or invalid sampling rate!");
	}

	if(( int )CNTR_LAQ_BITS < pt->b * pt->bits )
	{
		WARNINGMSGD("Block lenght %d, # blocks %d, bits per block %d.", 
			 w, pt->b, pt->bits );
		FATALERRORMSG("Not enough bits in the counter..." );
	}

	if( pt->b * TUPLE <= k ) 
	{
		WARNINGMSGD("Block lenght %d, # blocks %d, bits per block %d.", 
			 w, pt->b, pt->bits );
		FATALERRORMSG("Not enough blocks, try smaller k or t...");
	}

//	fprintf( stderr, "Block lenght %d, # blocks %d, bits per block %d.\n", w, pt->b, pt->bits );

	l = config.TUPLESPACE; // 1 << ( config.TAB * TUPLE );

	bt = ( cntr_laq_t * )salloc( l * sizeof( cntr_laq_t ));

	for( j = 0; j < l; j++ ) bt[ j ] = ( cntr_laq_t )0;

	for( i = 0; i < pt->b; i++ )
	{
	//	printf("%d, %d\n", pt->m, i * pt->h + w );
		t = trie_matching_tuples( pt->p + i * pt->h, w, V, 0, BASIC );

		for( j = 0; j < l; j++ )
			bt[ j ] |= (( cntr_laq_t )t[ j ] ) << ( pt->bits * i );

		free( t );
	}

	return bt;
}

#if 0
static char	** block_b_matching_tuples( char * p, int m, VECSUM * V )
{
	int		w, i, b, l;
	char		** t;

//	return ( char ** )trie_matching_tuples( p, m, V, 0, BLAQ );

	l = config.TUPLESPACE; 
	b = ( m + config.TUPLE - 1 ) / config.TUPLE;
	t = ( char ** )allocate_2d( b, l, 1 );
	w = m;

	for( i = 0; i < b; i++ )
	{
		t[ i ] = trie_matching_tuples( p, w, V, 0, BASIC );
		w -= config.TUPLE;
	}

	return t;
}
#endif

char	* build_sp( unsigned char * p, int m )
{
	VECSUM	V;

	build_vecsum( &V );

	return trie_matching_tuples( p, m, &V, 0, BASIC );
}	


static inline void	init_block_bits( PTB * pt, int k )
{
	int	i;

	pt->lim = ( cntr_laq_t )( k + 1 ) << (( pt->b - 1 ) * pt->bits );

//	Shifts are module W, you idiot, so this doesnt work on the boundary:
//	pt->mm = (( cntr_laq_t )1 << ( pt->b * pt->bits )) - ( cntr_laq_t )1;

//	and this assumes that cntr_laq_t is unsigned, but that's ok:
	pt->mm = (~( cntr_laq_t )0 ) >> ( CNTR_LAQ_BITS - ( pt->b * pt->bits ));

	for( i = 0, pt->om = ( cntr_laq_t )0; i < pt->b; i++ ) 
		pt->om |= ( cntr_laq_t )1 << ((( i + 1 ) * pt->bits ) - 1 );

	pt->strictH = BF_COPY(( cntr_laq_t )1 << ( pt->bits - 1 ), pt->b, pt->bits );
	pt->strictD = BF_COPY((( cntr_laq_t )1 << ( pt->bits - 1 )) - ( cntr_t )config.k - ( cntr_t )1, pt->b, pt->bits );
	pt->strictHM = ( cntr_laq_t )1 << ( pt->bits * pt->b - 1 );
}

#ifdef	DEBUG

static int	avg_laq_tuple_cntr( cntr_laq_t t, int b )
{
	int		i;
	unsigned	min = TUPLE;

	for( i = 0; i < b; i++ )
	{
		min = MIN( min, t & (( 1 << b ) - 1 ));
	}

	return min;
}

static void	avg_laq_tuple( cntr_laq_t * t, int b, int n, int d )
{
	int	i, tt;
	int	max = 0;
	int	dd[ 16 ];
	double	s = 0.0;

	for( i = 0; i <= TUPLE; i++ ) dd[ i ] = 0;

	for( i = 0; i < n; i++ ) 
	{
		tt = avg_laq_tuple_cntr( t[ i ], b );
		dd[ tt ]++;
		s += tt;
		if( tt > max ) max = tt;
	}

	for( i = 0; i <= TUPLE; i++ ) printf("\t%d: %d\n", i, dd[ i ] );

	printf("(avg.dist, max, depth): (%f, %d, %d)\n", s / n, max, d );


}

#endif


static inline void	min_laq_b_tuples( PTREE * pt, int t )
{
	int		i, j, b;

	b = ( pt->m + config.TUPLE - 1 ) / config.TUPLE;
	pt->t.sl = ( char ** )allocate_2d( b, t, 1 );

	for( i = 0; i < b; i++ ) for( j = 0; j < t; j++ )
	{
		pt->t.sl[ i ][ j ] = MIN( pt->l->t.sl[ i ][ j ], pt->r->t.sl[ i ][ j ] );
	}
}


static inline void	min_laq_tuples( PTB * pt, int t )
{
	int		bits, i;
	cntr_laq_t	J, F, x, y;

	bits = pt->bits;

	for( J = ( cntr_laq_t )0, i = 0; i < pt->b; i++ )
		J = ( J << ( bits )) | (( cntr_laq_t )1 << ( bits - 1 ));

	for( i = 0; i < t; i++ )
	{ 
		x = pt->l->t[ i ];
		y = pt->r->t[ i ];

	//	F = ((( x | J ) - y ) & J ) ^ J;
		F = ((( x | J ) - y ) & J );

		F = F - ( F >> ( bits - 1 ));

	//      pt->t[ i ] = ( x & F ) | ( y & ~F );
	        pt->t[ i ] = ( x & ~F ) | ( y & F );
	}
}

PTB	* build_pt_b_r( unsigned char ** p, int l, int r, int k, VECSUM * V, int d, int md )
{
        PTB		* pt = 0;
	int		t;

        if( r - l < 0 ) return 0;

	pt = ( PTB * )salloc( sizeof( PTB ));

	if( r - l == 0 )
	{
		pt->m = strlen( p[ l ] );
		pt->p = strdup( p[ l ] );
	       	pt->t = block_matching_tuples( pt, k, V );
		pt->l = pt->r = 0;

	//	avg_laq_tuple( pt->t, pt->b, config.TUPLESPACE, d );

		init_block_bits( pt, k );

		pt->v = init_bped();

		return pt;
	}

	pt->p = 0;
	pt->v = 0;

	pt->l = build_pt_b_r( p, l, l + ( r - l ) / 2, k, V, d + 1, md );
	pt->r = build_pt_b_r( p, l + ( r - l ) / 2 + 1, r, k, V, d + 1, md );

	pt->h = MIN( pt->l->h, pt->r->h );
	pt->m = MIN( pt->l->m, pt->r->m );
	pt->b = MIN( pt->l->b, pt->r->b );
	pt->bits = MIN( pt->l->bits, pt->r->bits );

	init_block_bits( pt, k );

	t = config.TUPLESPACE; // 1 << ( config.TAB * TUPLE );

	pt->t = salloc( t * sizeof( cntr_laq_t ));

	min_laq_tuples( pt, t );

	if( d > md )
	{
		free( pt->l->t );
		free( pt->r->t );

		pt->l->t = 0;
		pt->r->t = 0;
	}

	return pt;
}

PTB	* build_pt_b( unsigned char ** p, int n, int k, int depth )
{
 	VECSUM	V;

	build_vecsum( &V );

	return build_pt_b_r( p, 0, n - 1, k, &V, 0, depth );
}


void	free_pt_b( PTB * pt )
{
	if( pt->l ) free_pt_b( pt->l );
	if( pt->r ) free_pt_b( pt->r );
	if( pt->t ) free( pt->t );
	if( pt->p ) free( pt->p );
	if( pt->v ) free_bped( pt->v );
	free( pt );
}

#ifdef DEBUG

static void	avg_tuple( unsigned char * t, int n, int d )
{
	int	i;
	int	max = t[ 0 ];
	int	dd[ 16 ];
	double	s = 0.0;

	for( i = 0; i <= TUPLE; i++ ) dd[ i ] = 0;

	for( i = 0; i < n; i++ ) 
	{
		dd[ t[ i ]]++;
		s += t[ i ];
		if( t[ i ] > max ) max = t[ i ];
	}

	for( i = 0; i <= TUPLE; i++ ) printf("\t%d: %d\n", i, dd[ i ] );

	printf("(avg.dist, max, depth): (%f, %d, %d)\n", s / n, max, d );
}

static void	avg_blaq_tuple( unsigned char ** t, int n, int l, int d )
{
	int	i, j;
	int	max = t[ 0 ][ 0 ];
	int	dd[ 16 ];
	double	s;

	for( j = 0; j < l; j++ ) {
	s = 0.0;

	max = t[ j ][ 0 ];

	for( i = 0; i <= TUPLE; i++ ) dd[ i ] = 0;

	for( i = 0; i < n; i++ ) 
	{
		dd[ t[ j ][ i ]]++;
		s += t[ j ][ i ];
		if( t[ j ][ i ] > max ) max = t[ j ][ i ];
	}

	for( i = 0; i <= TUPLE; i++ ) printf("\t%d: %d\n", i, dd[ i ] );

	printf("(avg.dist, max, depth): (%f, %d, %d)\n", s / n, max, d );
	}
}

#endif

static void	min_pt_tuples( int d, unsigned tuple, PTREE * pt, unsigned tm )
{
	int		i;
	unsigned	tt;

	if( d == TUPLE ) 
		pt->t.s[ tuple ] = MIN( pt->l->t.s[ tuple ], pt->r->t.s[ tuple ] );
	else for( i = 0; i < config.fSIGMA; i++ )
	{
		tt = tuple + ( i * tm );
		min_pt_tuples( d + 1, tt, pt, tm * config.fSIGMA );
	}
}



PTREE	* build_pt_r( unsigned char ** p, int l, int r, VECSUM * V, int d, int md, int a )
{
        PTREE	* pt = 0;
	int	i, t;

        if( r - l < 0 ) return 0;

	pt = ( PTREE * )salloc( sizeof( PTREE ));

	pt->t.s = 0;

	pt->d = 0;
	pt->i = -TUPLE - TUPLE;

	pt->v.m = 0;

	if( r - l == 0 )
	{
		pt->m = strlen( p[ l ] );
		pt->p = strdup( p[ l ] );

		pt->l = pt->r = 0;

		switch( a )
		{
	                case BASIC:
				pt->t.s = trie_matching_tuples( pt->p, pt->m, V, 0, BASIC );
			//	avg_tuple( pt->t, config.TUPLESPACE, d );
				pt->v.m = init_bped();
				break;
	                case BLAQ:
			//	pt->t.sl = block_b_matching_tuples( pt->p, pt->m, V );
				pt->t.sl = trie_matching_tuples( pt->p, pt->m, V, 0, BLAQ );
				pt->v.m = init_bped();
				break;
	                case MYERS:
				pt->v.m = init_bped();
				pt->v.m->peq = prep_beq( pt->p, pt->m );
				init_v( pt->v.m, pt->p, config.k, pt->m );
				break;
	                case WUMANBER:
				pt->v.w = wm_init();
				wm_pp( pt->v.w, pt->p, pt->m, config.k );
				break;
 	                default:
				break;
		}
		/*
		if( a == BASIC )
			avg_tuple( pt->t.s, config.TUPLESPACE, d );
		else
			avg_blaq_tuple( pt->t.sl, config.TUPLESPACE, ( pt->m + config.TUPLE - 1 ) / config.TUPLE, d );

		*/
		return pt;
	}

	pt->p = 0;
	pt->l = build_pt_r( p, l, l + ( r - l ) / 2, V, d + 1, md, a );
	pt->r = build_pt_r( p, l + ( r - l ) / 2 + 1, r, V, d + 1, md, a );
	pt->m = MIN( pt->l->m, pt->r->m );

	switch( a )
	{
	        case BASIC:
			t = config.TUPLESPACE; // 1 << ( config.TAB * TUPLE );

			pt->t.s = salloc( t );

#			ifdef SAVESPACE
			min_pt_tuples( 0, 0, pt, 1 );
#			else
			for( i = 0; i < t; i++ ) 
				pt->t.s[ i ] = MIN( pt->l->t.s[ i ], pt->r->t.s[ i ] );
#			endif

			if( d > md )
			{
				free( pt->l->t.s );
				free( pt->r->t.s );

				pt->l->t.s = 0;
				pt->r->t.s = 0;
			}
			break;
	        case BLAQ:
			min_laq_b_tuples( pt, config.TUPLESPACE );
			if( d > md )
			{
				free_2d( pt->l->t.sl );
				free_2d( pt->r->t.sl );

				pt->l->t.sl = 0;
				pt->r->t.sl = 0;
			}
			break;
	        case MYERS:
			pt->v.m = init_bped();
			pt->v.m->peq = bped_peq_combine( pt->l->v.m->peq, pt->r->v.m->peq, pt->m );
			init_v( pt->v.m, 0, config.k, pt->m );
			break;
	        case WUMANBER:
			pt->v.w = wm_init();
			wm_combine( pt->v.w, pt->l->v.w, pt->r->v.w );
			break;
	        default:
			break;
	}
	/*
	if( 1 || d == 0 )
	{
		if( a == BASIC )
			avg_tuple( pt->t.s, config.TUPLESPACE, d );
		else
			avg_blaq_tuple( pt->t.sl, config.TUPLESPACE, ( pt->m + config.TUPLE - 1 ) / config.TUPLE, d );
	}
	*/

	return pt;
}

PTREE	* build_pt( unsigned char ** p, int n, int depth, int a )
{
	VECSUM	V;

	build_vecsum( &V );

	return build_pt_r( p, 0, n - 1, &V, 0, depth, a );
}

void	build_pt_p_r( PTREE * pt, unsigned char ** p, int l, int r, VECSUM * V, int d, int md )
{
	int	i, t;

        if( r - l < 0 ) return;

	pt->i = -2 * TUPLE;

	if( r - l + 1 <= config.si )
	{
		if( config.si <= 1 )
			pt->t.s = trie_matching_tuples( pt->p, pt->m, V, 0, BASIC );
		else
			pt->t.s = trie_matching_tuples( 0, pt->m, V, pt->v.m->peq, BASIC );
	//	avg_tuple( pt->t, config.TUPLESPACE, d );

		return;
	}

	build_pt_p_r( pt->l, p, l, l + ( r - l ) / 2, V, d + 1, md );
	build_pt_p_r( pt->r, p, l + ( r - l ) / 2 + 1, r, V, d + 1, md );

	t = config.TUPLESPACE; // 1 << ( config.TAB * TUPLE );

	pt->t.s = salloc( t );

#	ifdef SAVESPACE
	min_pt_tuples( 0, 0, pt, 1 );
#	else
	for( i = 0; i < t; i++ ) 
		pt->t.s[ i ] = MIN( pt->l->t.s[ i ], pt->r->t.s[ i ] );

//	avg_tuple( pt->t, t, d );
#	endif

	/*
	if( !config.si && d > md )
	{
		free( pt->l->t );
		free( pt->r->t );

		pt->l->t = 0;
		pt->r->t = 0;
	}
	*/
}

void	build_p_pt( PTREE * pt, unsigned char ** p, int n, int depth )
{
	VECSUM	V;

	build_vecsum( &V );

	build_pt_p_r( pt, p, 0, n - 1, &V, 0, depth );
}

void	free_pt( PTREE * pt, int alg )
{
	if( pt->l ) free_pt( pt->l, alg );
	if( pt->r ) free_pt( pt->r, alg );
	if( alg == BASIC )
	{
		if( pt->t.s ) free( pt->t.s );
	}
	else if( alg == BLAQ )
	{
		if( pt->t.sl ) free_2d( pt->t.sl );
	}
	if( pt->p ) free( pt->p );
	if( pt->v.m ) free_bped( pt->v.m );
	free( pt );
}

#if VECLEN == 128

static inline cntr_t	min_t( cntr_t t, int a, int b )
{
	int	i;
	cntr_t	m;

	if( b != 8 )
	{
		FATALERRORMSG("Invalid number of bits in the counters.");
	}

	m = t;

	for( i = 0; i < a; i++ )
	{
		m = _mm_min_epu8( m, t );
		t = _mm_srli_si128( t, 1 );
//		printf("min=%d\n", ( int )m );
	}

//	printf("min=%d\n", ( int )m );

	return m;
}

#else

static inline cntr_t	min_t( cntr_t t, int a, int b )
{
	int	i;
	cntr_t	bm, m;

	m = bm = (( cntr_t )1 << b ) - ( cntr_t )1;

	for( i = 0; i < a; i++ )
	{
		if(( t & bm ) < ( unsigned )m ) m = t & bm;
		t >>= b;
	}

//	printf("min=%d\n", m );

	return m;
}

#endif

static PTBP	* build_ptbp_r( unsigned char ** p, int l, int r, int a, int b, 
				VECSUM * V, int d, int md, int alg )
{
        PTBP		* pt = 0;
	int		i, j, k, h, x, s;
	cntr_t		q;
	char		* t, ** tl;

        if( r - l < 0 ) return 0;

	pt = ( PTBP * )salloc( sizeof( PTBP ));

	pt->d = ( cntr_t )0;
	pt->i = -2 * TUPLE;

	h = config.TUPLESPACE; // 1 << ( config.TAB * TUPLE );

	if(( j = r - l + 1 ) <= a )
	{
		pt->p = salloc( j * sizeof( char * ));
		pt->m = salloc( j * sizeof( int ));
		pt->np = j;
		pt->c = 0;
		pt->v = salloc( j * sizeof( BMYERS * ));

		for( i = 0; i < j; i++ )
		{
			pt->m[ i ] = strlen( p[ l + i ] );
			pt->p[ i ] = strdup( p[ l + i ] );
		}

		for( pt->mm = pt->m[ 0 ], i = 1; i < j; i++ )
			if( pt->mm > pt->m[ i ] ) pt->mm = pt->m[ i ];

		x = ( pt->mm + config.TUPLE - 1 ) / config.TUPLE;

		if( alg == BASIC )
		{
			pt->t.bs = salloc( h * sizeof( cntr_t ));
			for( k = 0; k < h; k++ ) pt->t.bs[ k ] = ( cntr_t )0;
		}
		else if( alg == BLAQ )
		{
			pt->t.bsl = allocate_2d( x, h, sizeof( cntr_t ));
			for( s = 0; s < x; s++ ) for( k = 0; k < h; k++ ) 
				pt->t.bsl[ s ][ k ] = ( cntr_t )0;
		}

		for( i = 0; i < j; i++ )
		{
			if( alg == BASIC )
			{
				t = trie_matching_tuples( pt->p[ i ], pt->m[ i ], V, 0, BASIC );

				for( k = 0; k < h; k++ ) 
#					if VECLEN == 128
					insertbase( &pt->t.bs[ k ], t[ k ], i );
#					endif
					pt->t.bs[ k ] |= ( cntr_t )t[ k ] << ( b * i );

				free( t );
			}
			else if( alg == BLAQ )
			{
			//	tl = block_b_matching_tuples( pt->p[ i ], pt->m[ i ], V );
				tl = trie_matching_tuples( pt->p[ i ], pt->m[ i ], V, 0, BLAQ );

				for( s = 0; s < x; s++ ) for( k = 0; k < h; k++ ) 
					pt->t.bsl[ s ][ k ] |= ( cntr_t )tl[ s ][ k ] << ( b * i );

				free_2d( tl );
			}
			else
			{
				FATALERRORMSG("Invalid algorithm!");
			}

			pt->v[ i ] = init_bped();
		}

#		if VECLEN == 128
		for( q = 0; i < a; i++ ) insertbase( &q, TUPLE, i );
#		else
		for( q = 0; i < a; i++ ) q |= ( cntr_t )TUPLE << ( b * i );
#		endif

		if( alg == BASIC )
		{
			for( k = 0; k < h; k++ ) pt->t.bs[ k ] |= q;
		}
		else if( alg == BLAQ )
		{
			for( s = 0; s < x; s++ ) for( k = 0; k < h; k++ ) 
				pt->t.bsl[ s ][ k ] |= q;
		}

		return pt;
	}

	pt->p = 0;
	pt->m = 0;

//	j = ( j + a - 1 ) / a;
//	pt->np = MIN( a, j );

	j /= a;
	pt->np = a;

//	printf("j=%d\n", j );

	pt->m = salloc( pt->np * sizeof( int ));
	pt->c = salloc( pt->np * sizeof( PTBP * ));

	for( i = 0; i < pt->np - 1; i++, l += j ) 
		pt->c[ i ] = build_ptbp_r( p, l, l + j - 1, a, b, V, d + 1, md, alg );

	pt->c[ i ] = build_ptbp_r( p, l, r, a, b, V, d + 1, md, alg );

	for( pt->mm = pt->c[ 0 ]->mm, i = 1; i < pt->np; i++ ) 
		if( pt->c[ i ]->mm < pt->mm ) pt->mm = pt->c[ i ]->mm;

	x = ( pt->mm + config.TUPLE - 1 ) / config.TUPLE;

	if( alg == BASIC )
	{
		pt->t.bs = salloc( h * sizeof( cntr_t ));

		for( i = 0; i < h; i++ ) pt->t.bs[ i ] = ( cntr_t )0;

		for( j = 0; j < pt->np; j++ ) for( i = 0; i < h; i++ ) 
		{
#			if VECLEN == 128
			insertbase( &pt->t.bs[ i ], min_t( pt->c[ j ]->t.bs[ i ], a, b ), j );
#			else
			pt->t.bs[ i ] |= min_t( pt->c[ j ]->t.bs[ i ], a, b ) << ( b * j );
#			endif
		}
		if( d > md ) for( j = 0; j < pt->np; j++ )
		{
			free( pt->c[ j ]->t.bs );
			pt->c[ j ]->t.bs = 0;
		}
	}
	else if( alg == BLAQ )
	{
		pt->t.bsl = ( void * )allocate_2d( x, h, sizeof( cntr_t ));

		for( s = 0; s < x; s++ ) for( i = 0; i < h; i++ ) 
			pt->t.bsl[ s ][ i ] = ( cntr_t )0;

		for( j = 0; j < pt->np; j++ ) for( s = 0; s < x; s++ ) for( i = 0; i < h; i++ ) 
			pt->t.bsl[ s ][ i ] |= min_t( pt->c[ j ]->t.bsl[ s ][ i ], a, b ) << ( b * j );

		if( d > md ) for( j = 0; j < pt->np; j++ )
		{
			free_2d( pt->c[ j ]->t.bsl );
			pt->c[ j ]->t.bsl = 0;
		}
	}

	return pt;
}

PTBP	* build_ptbp( unsigned char ** p, int n, int a, int b, int depth, int alg )
{
	VECSUM	V;

	build_vecsum( &V );

	return build_ptbp_r( p, 0, n - 1, a, b, &V, 0, depth, alg );
}

void	free_ptbp( PTBP * pt, int alg )
{
	int	i;

	if( pt->c )
	{
		for( i = 0; i < pt->np; i++ ) free_ptbp( pt->c[ i ], alg );

		free( pt->c );
	}

	if( pt->p )
	{
		for( i = 0; i < pt->np; i++ ) 
		{
			free( pt->p[ i ] );
			free_bped( pt->v[ i ] );
		}

		free( pt->p );
		if( pt->v ) free( pt->v );
	}

	if( alg == BASIC )
	{
		if( pt->t.bs ) free( pt->t.bs );
	}
	else
	{
		if( pt->t.bsl ) free_2d( pt->t.bsl );
	}
	free( pt->m );
	free( pt );
}

unsigned char	** read_patterns( char * fn, int * n )
{
        FILE		* f;
	unsigned char	** p;
	int		i, m, c;

	VERBOSEMSG("Reading patterns... ");

	startclock();

	if( !( f = fopen( fn, "r" ))) 
	{
		perror( fn );
	        FATALERRORMSG("File op failed...");
	}

	p = 0; i = 0; *n = 0;


	do {	do c = fgetc( f ); while( c != '\n' && c != EOF );
 
		i++;

	} while( c != EOF );


	p = salloc( i * sizeof( char * ));

	i = 0;

	fseek( f, 0, SEEK_SET );

	do {    m = 0;

		do {	c = fgetc( f );
		        m++;

		} while( c != '\n' && c != EOF );
 
		fseek( f, -m, SEEK_CUR );

		if( !--m ) break;

		p[ i ] = salloc( m + 1 );
		m = 0;
		while(( c = fgetc( f )) != '\n' ) p[ i ][ m++ ] = c; 
		p[ i ][ m ] = 0;
		i++;

	} while( c != EOF );

	/*
	do {    m = 0;

		do {	c = fgetc( f );
		        m++;

		} while( c != '\n' && c != EOF );
 
		fseek( f, -m, SEEK_CUR );

		if( !--m ) break;

		p = realloc( p, ( i + 1 ) * sizeof( char * ));
		p[ i ] = salloc( m + 1 );
		m = 0;
		while(( c = fgetc( f )) != '\n' ) p[ i ][ m++ ] = c; 
		p[ i ][ m ] = 0;
		i++;

	} while( c != EOF );
	*/
	fclose( f );

	VERBOSEMSGD("Done, read %d patterns, time: %1.2f", i, 
	       stopclock() / ( double )hz );

	*n = i;

//	for( i = 0; i < *n; i++ ) printf("%s, %d\n", p[ i ], strlen( p[ i ] ));

	return p;
}












