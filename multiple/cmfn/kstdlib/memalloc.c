#include			<stdlib.h>
#include			<stdio.h>
#include			<string.h>
#include			"memalloc.h"
#include			"errormsgs.h"

void	*salloc( size_t b )
{
	void	*a = malloc( b );

	if( !a ) FATALERRORMSG("Memory allocation failed!");

	return a;
}

void	*allocate_2d( int r, int c, size_t e )
{
	int			i;
	size_t			bi;
	unsigned long long	b;
	void			**a;


	bi = b = ( unsigned long long )r * sizeof( void * );

	if( bi != b ) FATALERRORMSG("Insane amount of memory requested\n");

	a = salloc( bi );

	bi = b = ( unsigned long long )r * c * e;

	if( bi != b ) FATALERRORMSG("Insane amount of memory requested\n");

	a[ 0 ] = salloc( bi );
	for( i = 1; i < r; i++ ) 
		a[ i ] = ( char * )a[ 0 ] + i * c * e; 

	return a;
}


void	*reallocate_2d_rows( void * a, int r, int c, size_t e )
{
	int	i;
	void	** b = a;

	b[ 0 ] = realloc( b[ 0 ], r * c * e );
	b = realloc( b, r * sizeof( void * ));
	for( i = 1; i < r; i++ ) b[ i ] = ( char * )b[ 0 ] + i * c * e;

	return b;
}

void	*allocate_3d( int z, int y, int x, size_t e )
{
	int	i, j;
	void	***a = allocate_2d( z, y, sizeof( void ** ));

	a[ 0 ][ 0 ] = salloc( x * y * z * e );
	for( j = 0; j < z; j++ ) for( i = 0; i < y; i++ ) 
		a[ j ][ i ] = 
			( char * )a[ 0 ][ 0 ] + j * x * y * e + i * x * e;

	return a;
}

void	*allocate_3d_old( int z, int y, int x, size_t e )
{
	int	i;
	void	***a = salloc( x * sizeof( void ** ));

	for( i = 0; i < z; i++ ) a[ i ] = allocate_2d( y, x, e );

	return a;
}

void	copy_3d( void * a, void * b, int x, int y, int z, size_t e )
{
	void	*** aa = a, *** bb = b;
/*	int	i;	*/

	memcpy( **aa, **bb, x * y * z * e );

/*	for( i = 0; i < x; i++ ) memcpy( aa[ i ][ 0 ], bb[ i ][ 0 ], y * z * e ); */
}

void	*allocate_1d( int n, size_t e )
{
	void	*a = salloc( n * e );

	return a;
}

void	free_1d( void *a )
{
	free( a );
}

void	free_2d( void *a )
{
	void	** b = a;

	free( b[ 0 ] );
	free( b );
}

void	free_3d( void *a, int x )
{
	void	*** b = a;
	( void )x;

	free( b[ 0 ][ 0 ] );
	free_2d( b );
}

void	free_3d_old( void *a, int x )
{
	void	*** b = a;
	int	i;

	for( i = 0; i < x; i++ ) free_2d( b[ i ] );

	free( b );
}

