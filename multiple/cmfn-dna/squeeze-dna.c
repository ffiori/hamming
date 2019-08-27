#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

int	main( int argc, char ** argv )
{
	unsigned char	* t, c;
	FILE		* f;
	int		n = 0, i, j, C;

	f = fopen( argv[ 1 ], "r" );

	fseek( f, 0, SEEK_END );
	n = ftell( f );
	fseek( f, 0, SEEK_SET );

	t = ( char * )malloc( n );
	for( i = 0; i < n; i++ ) t[ i ] = fgetc( f );
	fclose( f );

	for( i = 0; i < n / 4; i++ )
	{
		for( c = 0, j = 3; j >= 0; j-- )
		{
			C = toupper( t[ i * 4 + j ] );
			if( C == 'A') c = ( c << 2 ) | 0; else
			if( C == 'C') c = ( c << 2 ) | 1; else
			if( C == 'G') c = ( c << 2 ) | 2; else
			if( C == 'T') c = ( c << 2 ) | 3; else
				fprintf( stderr, "Invalid char.\n");
		}
		printf("%c", c );
	}

	return 0;
}
