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

#ifndef		_SQUEEZE_H_
#define		_SQUEEZE_H_

#include	<limits.h>
#include	<math.h>
#include	"config.h"

#define		INT_BIT		( sizeof( int ) * CHAR_BIT )

extern int	TUPLE;	// # symbols
//extern int	TAB;	// bits / symbol
extern int	TBITS;	// mask for tuple
extern int	SBITS;	// mask for symbol
//extern int	SIGMA;

extern int	fmap[ 256 ];
extern int	vmap[ 256 ];

//extern int	SPI   = ( sizeof( int ) * CHAR_BIT ) / 2;

#if defined(COMPRESSED)

static inline int	sget_c_asc( unsigned int * p, unsigned int i )
{
        return ( p[ i >> 2 ] >> (( i & 0x3 ) << 3 )) & 0xff;
}

static inline int	sget_c_dna( unsigned int * p, unsigned int i )
{
        return ( p[ i >> 4 ] >> (( i & 0xf ) << 1 )) & 3;
}

static inline int	sget_c( const unsigned char * p, unsigned int i )
{
	return ( *(( unsigned long long * )(( int * )p + ( i * config.TAB ) / INT_BIT )) >> 
		 (( i * config.TAB ) % INT_BIT )) & SBITS;
}

static inline int	sget_t_asc( unsigned int * t, unsigned int i )
{
	return ( *(( unsigned long long * )( t + ( i >> 2 ))) >> 
		 (( i & 0x3 ) << 3 )) & TBITS;
}

static inline int	sget_t_dna( unsigned int * t, unsigned int i )
{
	return ( *(( unsigned long long * )( t + ( i >> 4 ))) >> 
		 (( i & 0xf ) << 1 )) & TBITS;
}

static inline int	sget_t( const unsigned char * t, unsigned int i )
{
	return ( *(( unsigned long long * )(( int * )t + ( i * config.TAB ) / INT_BIT )) >> 
		 (( i * config.TAB ) % INT_BIT )) & TBITS;
}

#elif defined(SAVESPACE)

static inline int	sget_c( const unsigned char * p, unsigned int i )
{
	return vmap[ p[ i ]];
}

static inline int	sget_t( const unsigned char * t, unsigned int i )
{
	int			j = TUPLE, k = 0;
	const unsigned char	* p = t + i;

	while( --j >= 0 ) k = ( k * config.SIGMA ) + fmap[ p[ j ]];

	return k;
}

static inline int	sget_tn( unsigned int o, const unsigned char * t, unsigned int i )
{
	return ( o - fmap[ t[ i - 1 ]] ) / config.SIGMA + fmap[ t[ TUPLE + i - 1 ]] * config.SPACEMUL;
}

static inline int	sget_tp( unsigned int o, const unsigned char * t, unsigned int i )
{
	return (( o * config.SIGMA ) + fmap[ t[ i ]] ) % config.TUPLESPACE;
}

#else

static inline int	sget_c( const unsigned char * p, unsigned int i )
{
	return vmap[ p[ i ]];
}

static inline int	sget_t( const unsigned char * t, unsigned int i )
{
	int			j = TUPLE, k = 0;
	const unsigned char	* p = t + i;

	while( --j >= 0 ) k = ( k << config.TAB ) + fmap[ p[ j ]];

	return k;
}

static inline int	sget_tn( unsigned int o, const unsigned char * t, unsigned int i )
{
	return ( o >> config.TAB ) + ( fmap[ t[ TUPLE + i - 1 ]] << config.SPACESHIFT );
}

static inline int	sget_tp( unsigned int o, const unsigned char * t, unsigned int i )
{
	return (( o << config.TAB ) + fmap[ t[ i ]] ) & TBITS;
}


#endif

//void	get_group_map( unsigned char * t, int n, int g );

//void	get_map( void );

void	get_map( unsigned char * t, int n, int g );

// int     get_c( unsigned char * p, int i );

int     get_asc( unsigned char p );

int     put_c( unsigned char p );

void	print_c( const unsigned char * c, int m );

int *	cmap( char * s, int n );

char *	squeeze( unsigned char * p, int m );


#endif
