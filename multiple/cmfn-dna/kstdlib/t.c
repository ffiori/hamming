#include	"timer.h"

int	foo[ 1 << 24 ];

int	main()
{
	int	i;

	printf("%d\n", sysconf( _SC_CLK_TCK ));

	startclock();
	for( i = 0; i < 1 << 24; i++ ) foo[ i ] = i;
	printf("%d\n", stopclock());

	startclock();
	for( i = 1 << 24 - 1; i >= 0; i-- ) foo[ i ] = i;
	printf("%d\n", stopclock());

	return 0;
}
