#ifndef		_IBIT_VECTOR_H_
#define		_IBIT_VECTOR_H_

#include	<limits.h>


// Do this typedef with suitable int type in your source before including 
// this. This is to configure for your exact needs.


#ifndef		bitvector
typedef		unsigned int		bitvector;
#endif

typedef		bitvector *		BITVPTR;

#define		BITS			( CHAR_BIT * sizeof( bitvector ))

#define		setbit( v, i )		v |= ( bitvector )1 << ( i )
#define		clrbit( v, i )		v &= ~(( bitvector )1 << ( i ))
#define		xorbit( v, i )		v ^= ( bitvector )1 << ( i )
#define		tstbit( v, i )		(( v >> ( i )) & ( bitvector )1 )
#define		setvecbits( v )		v = ~( bitvector )0
#define		clrvecbits( v )		v = ( bitvector )0

#define		bv_size( len )		((( len ) + BITS - 1 ) / BITS )
#define		bv_bytes( len )		( bv_size( len ) * sizeof( bitvector ))
#define		bv_hi( i )		(( i ) / BITS )
#define		bv_lo( i )		(( i ) % BITS )

#define		bv_setbit( v, i )	setbit( v[ bv_hi( i ) ], bv_lo( i ))
#define		bv_clrbit( v, i )	clrbit( v[ bv_hi( i ) ], bv_lo( i ))
#define		bv_xorbit( v, i )	xorbit( v[ bv_hi( i ) ], bv_lo( i ))
#define		bv_tstbit( v, i )	tstbit( v[ bv_hi( i ) ], bv_lo( i ))

#define		bv_set( v, len )	memset( v, ~0, ( len ) * sizeof( bitvector ))
#define		bv_clr( v, len )	memset( v,  0, ( len ) * sizeof( bitvector ))

#define		bv_alloc( len )		( BITVPTR )malloc( bv_bytes( len ))
#define		bv_free( v )		free( v )


#define		bv_print_bits( bitz )				\
                { 						\
			int			i;		\
		        unsigned long long 	b = bitz;	\
                                                                \
		        i = sizeof( bitz ) * CHAR_BIT;		\
		        while( i-- ) {				\
			        printf("%d", ( int )b & 1 );	\
			        b >>= 1;			\
			}					\
		        printf("\n");				\
		}

static inline bitvector	bv_getbits( BITVPTR v, int i, int j, int len )
{
	int			h, l;
	bitvector		w;

	h = bv_hi( i );
	l = bv_lo( i );

	if( h == len - 1 ) 
		w = v[ h ] >> l;
	else
		w = ( v[ h ] >> l ) | ( v[ h + 1 ] << ( BITS - l ));

	return w & (( 1 << ( j - i + 1 )) - 1 );
}

static inline int	bv_cmp( BITVPTR v, BITVPTR w, int i )
{
	while( !( --i < 0 )) if( v[ i ] != w[ i ] ) break;
	if( i < 0 ) return 0;
	if( v[ i ] < w[ i ] ) return -1;
	if( v[ i ] > w[ i ] ) return  1;

	return 0; // supress warning
}

static inline void	bv_copy( BITVPTR v, BITVPTR w, int i )
{
	do *v++ = *w++; while( --i );
}

static inline void	bv_or_c( BITVPTR q, BITVPTR v, BITVPTR w, int i )
{
	do *q++ = *v++ | *w++; while( --i );
}

static inline void	bv_or_r_c( BITVPTR q, BITVPTR v, BITVPTR w, 
				   int i, int j )
{
	for( ; i <= j; i++ ) q[ i ] = v[ i ] | w[ i ];
}

static inline void	bv_or( BITVPTR v, BITVPTR w, int i )
{
	do *v++ |= *w++; while( --i );
}

static inline void	bv_and( BITVPTR v, BITVPTR w, int i )
{
	do *v++ &= *w++; while( --i );
}

static inline void	bv_and_c( BITVPTR q, BITVPTR v, BITVPTR w, int i )
{
	do *q++ = *v++ & *w++; while( --i );
}

static inline void	bv_xor_c( BITVPTR q, BITVPTR v, BITVPTR w, int i )
{
	do *q++ = *v++ ^ *w++; while( --i );
}

static inline void	bv_not_c( BITVPTR v, BITVPTR w, int i )
{
	do *v++ = ~*w++; while( --i );
}

static inline void	bv_not( BITVPTR v, int i )
{
	do { *v = ~*v; ++v; } while( --i );
}

static inline void	bv_add( BITVPTR v, BITVPTR w, int j )
{
	int			i;
	unsigned long long	r = 0LL;

	for( i = 0; i < j; i++ )
	{
		r = ( unsigned long long )v[ i ] + 
		    ( unsigned long long )w[ i ] + r;
		v[ i ] = ( bitvector )r;
		r >>= BITS;
	}
}

static inline void	bv_add_c( BITVPTR q, BITVPTR v, BITVPTR w, int j )
{
	int			i;
	unsigned long long	r = 0LL;

	for( i = 0; i < j; i++ )
	{
		r = ( unsigned long long )v[ i ] + 
		    ( unsigned long long )w[ i ] + r;
		q[ i ] = ( bitvector )r;
		r >>= BITS;
	}
}

static inline void	bv_add_r_c( BITVPTR q, BITVPTR v, BITVPTR w, 
				    int i, int j )
{
//	int			k = 0;
	unsigned long long	r = 0LL;

	for( ; i <= j; i++ )
	{
		r = ( unsigned long long )v[ i ] + 
		    ( unsigned long long )w[ i ] + r;
		q[ i ] = ( bitvector )r;
		r >>= BITS;
	}
}

#ifdef __i386__

static inline void	bv_add_asm( BITVPTR v, BITVPTR w, int j )
{
	int		i;

//	__asm__ volatile ("clc");

	__asm__ volatile ("addl	%1,%0" : "=m" (v[0]) : "r" (w[0]) );
	__asm__ volatile ("pushfl");
	for( i = 1; i < j; i++ )
	{
		__asm__ volatile ("popfl");
		__asm__ volatile ("adcl	%1,%0" : "=m" (v[i]) : "r" (w[i]) );
		__asm__ volatile ("pushfl");
//		__asm__ volatile ("addl	%1,%0" : "=m" (v[i]) : "r" (w[i]) );
//		__asm__ volatile ("adcl	$0,%0" : "=m" (v[i+1]) );
	}
	__asm__ volatile ("popfl");

//	__asm__ volatile(    "addl	%1,%0" : "=m" (v[i]) : "r" (w[i]) );
}

#endif

// Assumes that s is at most BITS...

static inline void	bv_shr( BITVPTR v, int s, int i )
{
	while( --i ) 
		{ *v = ( *v >> s ) | (( *( v + 1 )) << ( BITS - s )); ++v; }

	*v >>= s; 
}

// Assumes that s is at most BITS...

static inline void	bv_shl( BITVPTR v, int s, int i )
{
	int		j, bs = BITS - s;

	for( j = i - 1; j > 0; j-- )
		v[ j ] = ( v[ j ] << s ) | (( v[ j - 1 ] ) >> bs );

	v[ j ] <<= s;
}



#ifdef __i386__

// Assumes that s is at most BITS... This is slow, as shld is not pairaple...

static inline void	bv_shl_asm( BITVPTR v, int s, int i )
{
	int		j;

	__asm__("movb	%0,%%cl"  : : "g" (s) : "%cl" ); 
	for( j = i - 1; j > 0; j-- )
	{
		__asm__("shldl	%1,%0" : "=m" (v[j]) : "r" (v[j-1]) : "%cl" );
	}

	v[ j ] <<= s;
}

static inline void	bv_shl_asm_1( BITVPTR v, int i )
{
	int		j;

	for( j = i - 1; j > 0; j-- )
	{
		__asm__("shldl	$1,%1,%0" : "=m" (v[j]) : "r" (v[j-1]) );
	}

	v[ j ] <<= 1;
}

#endif

static inline void	x_bv_shl( BITVPTR v, int s, int i )
{
	bitvector	r = (( 1 << s ) - 1 ) << ( BITS - s );
	int		j;

	for( j = i - 1; j > 0; j-- )
		v[ j ] = ( v[ j ] << s ) | 
			(( v[ j - 1 ] & r ) >> ( BITS - s ));

	v[ j ] <<= s;
}


#endif	/*	_IBIT_VECTOR_H_	*/


