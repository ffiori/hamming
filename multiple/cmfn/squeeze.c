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
#include	<string.h>
#include        <ctype.h>
#include	"kstdlib.h"
#include	"squeeze.h"
#include	"config.h"

int	TUPLE = 6;				// # symbols
//int	TAB   = 2;				// bits / symbol
int	TBITS = (( 1 << ( 2 * 6 )) - 1 );	// mask for tuple
int	SBITS = (( 1 << 2 ) - 1 );		// mask for symbol
//int	SIGMA = 1 << 2;

// int	SPI   = ( sizeof( int ) * CHAR_BIT ) / 2;


typedef struct ii
{
	int	n;
	int	i;

} INDEX;

int	fmap[ 256 ];	// character map for filtering...
int	vmap[ 256 ];	// ...and for verification

/*
int     get_c( unsigned char * p, int i )
{
	char    * P = "ACDEFGHIKLMNPQRSTVWY";
        int       C, j;

	C = toupper( p[ i ] );

	if( SIGMA == 4 )
	{
		if( C == 'A') return 0;
		if( C == 'C') return 1;
		if( C == 'G') return 2;
		if( C == 'T') return 3;

		return -1;
	}
	else if( SIGMA == 20 ) 
	{
		for( j = 0; j < 20; j++ ) if( P[ j ] == p[ i ] ) return i;

		return -1;
	}
	else
	{
		return ( int )p[ i ];
	}

	return -1;
}

int     get_asc( unsigned char p )
{
	char    * P = "ACDEFGHIKLMNPQRSTVWY";

	if( SIGMA == 4 )
	{
		if( p == 0 ) return 'a';
		if( p == 1 ) return 'c';
		if( p == 2 ) return 'g';
		if( p == 3 ) return 't';
	}
	else if( SIGMA == 20 )
		return P[ p ];
	else	
		return p;

	return p;
}

int     put_c( unsigned char p )
{
	putchar( get_asc( p ));
	
	return p;
}

void	print_c( const unsigned char * c, int m )
{
        int	i;

	for( i = 0; i < m; i++ ) put_c( sget_c( c, i ));
	printf("\n");
}


int *	cmap( char * s, int n )
{
	int	i, j;
	int	* map;

	map = salloc( 256 * sizeof( int ));

	for( i = 0; i < 256; i++ ) map[ i ] = -1;

	for( i = 0; i < n; i++ ) map[( unsigned )s[ i ]] = 1;

	for( i = j = 0; i < 256; i++ ) if( map[ i ] == 1 ) map[ i ] = j++;

	return map;
}

*/

int	int_cmp( const void * a, const void * b )
{
	int	x = (( struct ii * )a )->n, y = (( struct ii * )b )->n;

	if( x > y ) return -1;
	if( x < y ) return  1;

	return 0;
}

static void	get_group_map( unsigned char * t, int n, int g )
{
	int		i, m = 0;
	struct	ii	ii[ 256 ];

	for( i = 0; i < 256; i++ ) 
	{
		fmap[ i ] = ii[ i ].n = 0;
		ii[ i ].i = i;
	}

	if( config.ignorecase ) 
		for( i = 0; i < n; i++ ) ii[ toupper( t[ i ] )].n++;
	else
		for( i = 0; i < n; i++ ) ii[ t[ i ]].n++;

	qsort( ii, 256, sizeof( struct ii ), int_cmp );

	for( i = 0; i < 256; i++ ) 
	{
		fmap[ ii[ i ].i ] = i % g;
		m += ii[ i ].n;
	//	if( ii[ i ].n )
	//		printf("Character: %d (%c)\tcount: %d\t--> group %d\n", 
	//		       ii[ i ].i, ( isprint( ii[ i ].i ) ? ii[ i ].i : ' ' ), ii[ i ].n, i % g );
	}

//	printf("Total count %d, file size %d\n", m, n );
}

void	get_map( unsigned char * t, int n, int g )
{
	int			i, j;
	const unsigned char	* prot		= "acdefghiklmnpqrstvwy";
//	unsigned char		* iubprot	= "acdefghiklmnpqrstvwybzx";
//	unsigned char		* dna  		= "acgt";
	const unsigned char	* iubdna	= "acgtbdhkmnrsvwy";

	for( i = 0; i < 256; i++ ) vmap[ i ] = 0;

//	if(( config.SIGMA == 256 ) && config.ascii )
	//	for( i = j = 0; i < config.SIGMA; i++, j++ ) 
	//		vmap[ i ] = j;
	if( config.ascii )
		for( j = 0, i = config.ascb; i <= config.asce; i++, j++ ) 
		{
			vmap[ i ] = j;
			if( config.ignorecase ) vmap[ toupper( i )] = j;
		}
	else if(( config.vSIGMA == 20 ) && !config.ascii )
		for( i = j = 0; i < config.vSIGMA; i++, j++ ) 
			vmap[ prot[ i ]] = vmap[ toupper( prot[ i ] )] = j;
	else if((( config.vSIGMA == 4 ) || ( config.vSIGMA == 15 )) && !config.ascii )
		for( i = j = 0; i < 15; i++, j++ ) 
			vmap[ iubdna[ i ]] = vmap[ toupper( iubdna[ i ] )] = j;

	/*
	else if(( SIGMA == 4 ) && !config.IUBT )
		for( i = j = 0; i < SIGMA; i++, j++ ) 
			vmap[ dna[ i ]] = vmap[ toupper( dna[ i ] )] = j;
	else if(( SIGMA == 4 || SIGMA == 15 ) && ( config.IUBT || config.IUBP ))
		for( i = j = 0; i < 15; i++, j++ ) 
			vmap[ iubdna[ i ]] = vmap[ toupper( iubdna[ i ] )] = j;
	*/
	else
	{
		FATALERRORMSG("Invalid alphabet! Exiting...");
	}

	if( g ) 
		get_group_map( t, n, g );
	else
		for( i = 0; i < 256; i++ ) fmap[ i ] = vmap[ i ];
}

/*
char * squeeze( unsigned char * p, int m )
{
        char    c, * P;
	int	C, k = 0, h = 0, i, j;

	if( SIGMA == 4 )
	{
		P = ( char * )salloc(( m + 3 ) / 4 + 8 );

		for( i = 0; i < ( m + 3 ) / 4; i++ )
		{
			for( c = 0, j = 3; j >= 0; j-- )
			{
				if( i * 4 + j >= m ) continue; 
				C = toupper( p[ i * 4 + j ] );
				if( C == 'A') c = ( c << 2 ) | 0; else
				if( C == 'C') c = ( c << 2 ) | 1; else
			        if( C == 'G') c = ( c << 2 ) | 2; else
				if( C == 'T') c = ( c << 2 ) | 3; else
					printf("Invalid char.\n");
			}

			P[ k++ ] = c;
		}

		return P;
	}

	if( SIGMA == 20 )
	{
		unsigned short	c;

		P = ( char * )salloc( m );

		for( c = 0, i = k = 0; i < m; i++ )
		{
			C = toupper( p[ i ] );
			if( C == 'A') c |=  0 << k; else
			if( C == 'C') c |=  1 << k; else
			if( C == 'D') c |=  2 << k; else
			if( C == 'E') c |=  3 << k; else
			if( C == 'F') c |=  4 << k; else
			if( C == 'G') c |=  5 << k; else
			if( C == 'H') c |=  6 << k; else
			if( C == 'I') c |=  7 << k; else
			if( C == 'K') c |=  8 << k; else
			if( C == 'L') c |=  9 << k; else
			if( C == 'M') c |= 10 << k; else
			if( C == 'N') c |= 11 << k; else
			if( C == 'P') c |= 12 << k; else
			if( C == 'Q') c |= 13 << k; else
			if( C == 'R') c |= 14 << k; else
			if( C == 'S') c |= 15 << k; else
			if( C == 'T') c |= 16 << k; else
			if( C == 'V') c |= 17 << k; else
			if( C == 'W') c |= 18 << k; else
			if( C == 'Y') c |= 19 << k; else
				printf("Invalid char.\n");
			k += 5;

			if( k >= 8 )
			{
				P[ h++ ] = c & 0xff;
				k -= 8;
				c >>= 8;
			}
		}

		while( k > 0 )
		{
			P[ h++ ] = c & 0xff;
			k -= 8;
			c >>= 8;
		}

	//	print_c( P, m );

		return P;
	}

	return strdup( p );
}

*/
