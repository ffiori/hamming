#include	<stdio.h>
#include	<string.h>
#include	"kstdlib.h"

static int	ed( char * a, char * b )
{
	int	m, n, i, j, d = 0, ** e;

	m = strlen( a );
	n = strlen( b );

	e = ( int ** )allocate_2d( m + 1, n + 1, sizeof( int ));

	for( i = 0; i < m + 1; i++ ) e[ i ][ 0 ] = i;
	for( j = 0; j < n + 1; j++ ) e[ 0 ][ j ] = j;

	for( i = 1; i < m + 1; i++ )
		for( j = 1; j < n + 1; j++ )
	        {
			if( a[ i - 1 ] == b[ j - 1 ] ) 
			{
				d = e[ i - 1 ][ j - 1 ];
			}
			else
			{
				d = MIN( e[ i ][ j - 1 ], e[ i - 1 ][ j ] );
				d = MIN( e[ i - 1 ][ j - 1 ], d );

				++d;
			}

			e[ i ][ j ] = d;
		}

	d = e[ m ][ n ];

	free_2d(( void ** )e );

	return d;
}

/*
static int	hamming( char * p, int m, char * q, int l )
{
        int	d = 0, md = l;
	int	i, j;

	for( i = 0; i < m - l + 1; i++ )
	{
	        for( d = j = 0; j < l; j++ )
		        if( q[ j ] != p[ i + j ] ) d++;

		if( d < md ) md = d;
	}   

	return md;	
}
*/

static int	hamming( char * p, char * q )
{
        int	d = 0;
	int	i, m, n;

	m = strlen( p );
	n = strlen( q );

	m = MIN( m, n );

	for( i = 0; i < m; i++ )
	        if( q[ i ] != p[ i ] ) d++;

	return d;	
}

void	cluster( char ** p, int m, int h )
{
	int	i, j, d, md, mj;
	char	* s;
	double	t;

	printf("Thinking hard (clustering the patterns)... "); fflush( stdout ); startclock();

	for( i = 0; i < m - 1; i++ )
	{
		md = 1 << 30;

		for( j = mj = i + 1; j < m; j++ )
	        {
			if( h )
				d = hamming( p[ i ], p[ j ] );
			else
				d = ed( p[ i ], p[ j ] );

			if( d < md ) 
			{ 
				md = d; 
				mj = j; 
			}
		}

	//	printf("d = %d, j = %d\n", md, mj );
		s = p[ i + 1 ];
		p[ i + 1 ] = p[ mj ];
		p[ mj ] = s;
	}

	t = stopclock() / ( double )hz;
	printf("Done, time: %1.2f\n", t );
}


