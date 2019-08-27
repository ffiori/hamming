/* (C) 1996 by Kimmo.Fredriksson@cs.Helsinki.FI / No warranty... */

#include	<stdlib.h>
#include	<sys/time.h>

#include	"rnd.h"

/*

unsigned	randomize()
{
	static int		rndseed = 1;
	struct timeval		t0;
	__timezone_ptr_t	t1 = 0;

	gettimeofday( &t0, t1 );
	rndseed = ( int )t0.tv_usec;
	srand( rndseed );

	return rndseed;
}

*/

unsigned	randomize()
{
	static int	rndseed = 1;
	struct timeval	t0;
	struct timezone	t1;

	gettimeofday( &t0, &t1 );
	rndseed = ( int )t0.tv_usec;
	srand( rndseed );

	return rndseed;
}

