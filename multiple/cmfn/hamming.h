#ifndef	_HAMMING_H_
#define	_HAMMING_H_

#include	"config.h"

/*
typedef	struct hamming
{
	int	b;
	int	e;

} HAMMING;
*/

int	hamming_min( char * p, int m, char * q, int l );
int	hamming_k( const unsigned char * t, int b, int e, unsigned char * p, int m, int k, MATCHCODE code );

#endif
