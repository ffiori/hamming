#include	<stdio.h>
#include	<string.h>
#include	"config.h"
#include	"kstdlib.h"

CONFIG	config;


void	config_set( int tuple, int sigma, int ascii, int charg, int ascb, int asce, int ignorecase, int iubt, int iubp )
{
	unsigned long long	bytes;

	config.TUPLE = tuple;
	config.vSIGMA = sigma;
	config.fSIGMA = sigma;
	config.IUBT = iubt;
	config.IUBP = iubp;
	config.ascii = ascii;
	config.ascb = ascb;
	config.asce = asce;
	config.ignorecase = ignorecase;

	if( charg && !config.ascii )
	{
		WARNINGMSG("Character grouping not supported for non-ascii, ignored.");
		charg = 0;
	}

	if( config.ascii )
	{
#		ifdef COMPRESSED
		if( config.ignorecase )
		{
			WARNINGMSG("Case insensitive search not supported for compressd search, ignored.");
			config.ignorecase = 0;
		}
#		endif
		if( iubt || iubp ) WARNINGMSG("IUB codes not supported for ASCII...");
		config.IUBT = config.IUBP = 0;
		config.vSIGMA = asce - ascb + 1;
		if( charg )
		{
			ascb = 0;
			asce = charg - 1;
		}
		config.fSIGMA = asce - ascb + 1;
		if( config.vSIGMA < 2 || config.vSIGMA > 256 )
		{
			FATALERRORMSG("Invalid alphabet, exiting...");
		}
		config.TAB = CLOG2( config.fSIGMA );
	//	printf("TAB = %d\n", config.TAB );
	}
	else if( sigma == 4 )
	{
		if( iubt ) 
		{
			config.vSIGMA = config.fSIGMA = 15;
			config.TAB = 4;
#			ifdef COMPRESSED
			FATALERRORMSG("IUB codes for the text currently not supported for compressed search...");
#			endif
		}
		if( iubp && !iubt ) 
		{
			config.vSIGMA = config.fSIGMA = 4;
			config.TAB = 2;
		}
		if( !iubt && !iubp )
		{
			config.TAB = 2;
		}
	}
	else if( sigma == 20 )
	{
		if( iubt || iubp ) 
			WARNINGMSG("IUB codes not supported for protein...");
		config.IUBT = config.IUBP = 0;
		config.TAB = 5;
	}

	config.TBITS = ( 1 << ( config.TAB * config.TUPLE )) - 1;
	config.SBITS = ( 1 << config.TAB ) - 1;

#	ifdef	SAVESPACE
	config.TUPLESPACE = bytes = pow( config.SIGMA, config.TUPLE );
	config.SPACEMUL = pow( config.SIGMA, config.TUPLE - 1 );
#	else
	config.TUPLESPACE = bytes = ( 1ULL ) << ( config.TAB * config.TUPLE );
	config.SPACESHIFT = config.TAB * ( config.TUPLE - 1 );
#	endif

//	printf("%d, %d, %d, %1.0f\n", config.fSIGMA, config.vSIGMA, config.TAB, ( double )bytes ); //config.TUPLESPACE );

	if( bytes != ( unsigned long long )config.TUPLESPACE ) 
		FATALERRORMSG("Insane amount of memory requested! Check (e.g.) your -t<n> -T<n> ops.");

}


void	config_data( unsigned char * t, int n, unsigned char ** p, int M )
{
	int	i, m = 0, l;

	config.t = t;
	config.n = n;
	config.p = p;
	config.M = M;

	if( p ) for( i = 0; i < M; i++ ) if(( l = strlen( p[ i ] )) > m ) m = l;

	config.m = m;
}

void	config_opts( int k, int maxdepth, int hamming, int si )
{
	config.k = k;
	config.maxdepth = maxdepth;
	config.hamming = hamming;
	config.si = si;


	if( config.si && config.hamming )
	{
		WARNINGMSG("Superimposition not supported for Hamming, ignored.");
		config.si = 0;
	}
}

void	config_alg( int skipf, int skipb, int laq, int use_bp_cntrs, int opt, int strictblaq, int laqh, int myers, int wm )
{
	if( myers && wm )
	{
		FATALERRORMSG("Ivalid algorithm combination, exiting...");
	}

	if(( myers || wm ) && ( skipf || skipb || laq || use_bp_cntrs || opt ))
	{
		FATALERRORMSG("Ivalid algorithm combination, exiting...");
	//	skipf = skipb = laq = use_bp_cntrs = opt = 0;
	}

	config.myers = myers;
	config.wm = wm;
	config.skipf = skipf;
	config.skipb = skipb;
	config.laq = laq;
	config.laqh = laqh;
	config.use_bp_cntrs = use_bp_cntrs;
	config.opt = opt;
	config.strictblaq = strictblaq;


	if( skipf && laq )
	{
		WARNINGMSG("Forward skipping not supported with LAQ.");
		WARNINGMSG("LAQ disabled.");
		config.laq = laq = 0;
	}
	if( use_bp_cntrs && laq && !skipb )
	{
		WARNINGMSG("Cannot use plain LAQ with bit-parallel counters.");
		WARNINGMSG("LAQ disabled.");
		config.laq = laq = 0;
	}
	if( !skipf && !skipb && opt )
	{
		WARNINGMSG("Cannot use q-gram optimizing with linear mode (no skipping).");
		WARNINGMSG("Optimizing disabled.");
		config.opt = opt = 0;
	}
	if( laq && opt && !skipb )
	{
		WARNINGMSG("Cannot use q-gram optimizing with LAQ without backward skipping.\n");
		WARNINGMSG("LAQ disabled.\n");
		config.laq = laq = 0;
	}
	if( laq && config.hamming )
	{
		WARNINGMSG("LAQ with Hamming works but is not really supported.");
	}
	if( strictblaq && ( opt || !( skipb && laq ) || use_bp_cntrs ))
	{
		WARNINGMSG("Strict LAQ can be used only with -Sb -L, strict LAQ disabled.");
		config.strictblaq = strictblaq = 0;
	}

}


void	config_free( void )
{
	int	i;

    if (config.t) free( config.t );


	if (config.p) {
        for( i = 0; i < config.M; i++ )
            if (config.p[i]) free( config.p[ i ] );
        free( config.p );
    }
}
