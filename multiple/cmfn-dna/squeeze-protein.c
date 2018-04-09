#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

int	main( int argc, char ** argv )
{
	unsigned char	* t;
	unsigned short	c;
	FILE		* f;
	int		k, n = 0, i, j, C;

//	ACDEFGHIKLMNPQRSTVWY

	f = fopen( argv[ 1 ], "r" );

	fseek( f, 0, SEEK_END );
	n = ftell( f );
	fseek( f, 0, SEEK_SET );

	t = ( char * )malloc( n );
	for( i = 0; i < n; i++ ) t[ i ] = fgetc( f );
	fclose( f );

	for( c = 0, i = k = 0; i < n; i++ )
	{
		C = toupper( t[ i ] );
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
			printf("%c", c & 0xff );
			k -= 8;
			c >>= 8;
		}
	}

	while( k > 0 )
	{
		printf("%c", c & 0xff );
		k -= 8;
		c >>= 8;
	}

	return 0;
}
