#ifndef	_MATHF_H_
#define	_MATHF_H_

#include	<math.h>

#define	EPSILON		1e-2

#define	FEQ( a, b )	( ABS(( a ) - ( b )) <= EPSILON ? 1 : 0 )

#define	SQR( x )	(( x ) * ( x ))

#define	ABS( x )	(( x ) < 0 ? -( x ) : ( x ))

#define	ISPOW2( x )	( !(( x ) & (( x ) - 1 )))

#define	IFLOOR( x )	(( int )floor( x ))

#define	CLOG2( x )	ceil( log(( double )( x )) / log( 2.0 ))


#define BSR( x )	({ int  __foo; 				\
			__asm__ ("bsrl\t %1,%0\n\t" 		\
                        : "=r" (__foo) : "r" (x) ); __foo; })

#define ILOG2( x )	ilogb( x )


#define	MATH_MAX_DIV	256

extern	int		math_dividend_div_const[ MATH_MAX_DIV ];

extern	int		math_const_div_divisor[ MATH_MAX_DIV ];

#if defined __cplusplus

extern "C" {

#endif

void	math_build_dividend_div_const( int c );
void	math_build_const_div_divisor( int c );


/*

#ifdef __GNUC__

# ifdef __i386__

#  define ILOG_2( x )	({ int  __foo; 				\
			__asm__ ("bsrl\t %1,%0\n\t" 		\
                        : "=r" (__foo) : "r" (x) ); __foo; })

# else

#  define ILOG_2( x )	({ float __foo = ( float )( x ) + 0.5f; \
			( *(( unsigned * )&__foo ) >> 23 ) - 127; })

# endif

#else

# define ILOG_2( x )	ilogb( x )

//(( int )ceil( LOG_2( x )))

#endif

*/






unsigned long long	math_binomial( unsigned long n, unsigned long k );


#if defined __cplusplus

}

#endif


#endif


