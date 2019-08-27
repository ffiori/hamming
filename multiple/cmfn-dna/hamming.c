#include	"hamming.h"
#include	"squeeze.h"
#include	"config.h"

int	hamming_min( char * p, int m, char * q, int l )
{
        int	d = 0, md = l;
	int	i, j;

	for( i = 0; i < m - l + 1; i++ )
	{
	        for( d = j = 0; j < l; j++ )
		        if( sget_c( q, j ) != sget_c( p, i + j )) d++;

		if( d < md ) md = d;
	}   

	return md;	
}

int	hamming_k( const unsigned char * t, int b, int e, unsigned char * p, int m, int k, MATCHCODE code )
{
        int	d, i, j, matches = 0;

	for( i = b; i < e; i++ )
	{
	        for( d = j = 0; j < m && d <= k; j++ ) 
		        if( sget_c( t, j + i ) != vmap[ p[ j ]] ) d++;

		if( d <= k ) 
		{
			matches++;
			if( code ) code( i + m - 1, d, p );
		}
	}

	return matches;
}

