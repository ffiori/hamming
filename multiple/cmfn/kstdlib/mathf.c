
#include	"mathf.h"



int	math_dividend_div_const[ MATH_MAX_DIV ];

int	math_const_div_divisor[ MATH_MAX_DIV ];


void	math_build_dividend_div_const( int c )
{
	int	i;

	for( i = 0; i < MATH_MAX_DIV; i++ ) 
		math_dividend_div_const[ i ] = i / c;
}


void	math_build_const_div_divisor( int c )
{
	int	i;

	for( i = 1; i < MATH_MAX_DIV; i++ ) 
		math_const_div_divisor[ i ] = c / i;
}


unsigned long long	math_binomial( unsigned long n, unsigned long k )
{
	unsigned long		i;
	unsigned long long	b = 1ULL;

	if( k > n ) return 1ULL;

	for( i = n - k + 1; i <= n; i++ ) b *= i;
	for( i = 2; i <= k; i++ ) b /= i;

	return b;
}

