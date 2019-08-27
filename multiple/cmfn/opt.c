//
//	(C) Copyright 2003 by Kimmo Fredriksson.
//
//
//	The code can be freely used for academic/research purposes. Direct
//	or indirect use for commercial advantage is not allowed without
//	written permission from the author.
//
//	The code can be modified and redistributed if this note is left
//	intact.
//
//	Comments, improvements and bug-reports can be emailed to kf@iki.fi
//


#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>
#include	<math.h>
#include	<unistd.h>
#include	"kstdlib.h"
#include	"bped.h"
#include	"squeeze.h"
#include	"preprocess.h"
#include	"filter.h"
#include	"cluster.h"
#include	"config.h"

//#include	"asmu.h"

long tprep = 0, texec = 0;
MATCHCODE	code = 0;

MATCHCODE	output( int i, int score, char * p )
{
	printf("Match at %d, distance %d (%s)!\n", i, score, p );

	return 0;
}

void	usage( char * prog )
{
	fprintf( stderr, 
		 "\nSearches pattern(s) from a given (possibly packed) file.\n\n");
	fprintf( stderr, 
		 "Usage: %s [options], where the options are:\n", prog );

	fprintf( stderr, 
		 "\n\tAlphabet options:\n\n");

	fprintf( stderr, 
		 "\t-D\tuse DNA alphabet (4)\n");
	fprintf( stderr, 
		 "\t-P\tuse Protein alphabet (20)\n");
	fprintf( stderr, 
		 "\t-A\tuse ASCII alphabet (256)\n");
	fprintf( stderr, 
		 "\t-Nf[,l]\tuse ASCII alphabet, but only codes from f to l\n");
	fprintf( stderr,
		 "\t-T n\tmap the (ASCII) characters into n groups (n < alphabet size)\n");
	fprintf( stderr,
		 "\t-C\tbe case insensitive (for non-compressed ASCII)\n");
	fprintf( stderr, 
		 "\t-up\tuse IUB degeneracy codes for patterns\n");
	fprintf( stderr, 
		 "\t-ut\tuse IUB degeneracy codes for text (for non-compressed DNA only)\n");

	fprintf( stderr, 
		 "\n\tAlgorithm options:\n\n");

	fprintf( stderr, 
		 "\t\t(no option) basic linear time filter\n");
	fprintf( stderr, 
		 "\t-Sf\tskip characters (fixed skipping, forward matching)\n");
	fprintf( stderr, 
		 "\t-Sb\tskip characters (dynamic skipping, backward matching)\n");
	fprintf( stderr, 
		 "\t-O\toptimal q-gram selection\n");
	fprintf( stderr, 
		 "\t-B\tuse bit-parallel counters\n");
	fprintf( stderr, 
		 "\t-L\tuse LAQ\n");
	fprintf( stderr, 
		 "\t-F\tuse strict matching tables for -L -Sb\n");
	fprintf( stderr, 
		 "\t-M\tuse plain Myers with superimposition and hierarchical verif.\n");
	fprintf( stderr, 
		 "\t-W\tas -M, but use ABNDM (Wu & Manber based suffix automaton).\n");

	fprintf( stderr,
		 "\n\t\tSome of the algorithm options can be combined, the\n");
	fprintf( stderr, "\t\tpossibilities are:\n\n");
//	fprintf( stderr,
//		 "\t-Sf -O, -Sb -O, -Sb -L, -B -Sf, -B -Sb, -B -Sf -O, -B -Sb -O, -B -Sb -L\n\n");
	fprintf( stderr,
		 "\t\t-Sf -O, -Sb -O, -Sb -L, -Sb -O -L, -Sb -L -F,\n");
	fprintf( stderr,
		 "\t\t-B -Sf, -B -Sb, -B -Sf -O, -B -Sb -O, -B -Sb -L, -B -Sb -O -L\n\n");
	fprintf( stderr,
		 "\t\tAll other combinations are illegal. The options\n");
	fprintf( stderr,
		 "\t\t-O or -F can't be applied by themselves.\n");

	fprintf( stderr, 
		 "\n\tSearch options:\n\n");

	fprintf( stderr, 
		 "\t-k n\tallow at most n edit operations / mismatches\n");
	fprintf( stderr, 
		 "\t-t n\tuse q-grams of length n\n");
	fprintf( stderr, 
		 "\t-h n\tuse sampling rate h for (plain) -L, h >= q\n");
	fprintf( stderr, 
		 "\t-s\tallow only substitutions (Hamming distance)\n");
	fprintf( stderr, 
		 "\t-p pat\tuse pat as the pattern (don't use this, use -r always)\n");
	fprintf( stderr, 
		 "\t-r file\tread the pattern(s) from file\n");
	fprintf( stderr, 
		 "\t-f file\tthe (possibly packed) file to search\n");
	fprintf( stderr, 
		 "\t-o\toutput the matches\n");

	fprintf( stderr,
		 "\n\tAdvanced / experimental options:\n\n");

	fprintf( stderr, 
		 "\t-c\tcluster the patterns before preprocessing\n");
	fprintf( stderr, 
		 "\t-g\tpartition the patterns into small groups, searched separately\n");
	fprintf( stderr, 
		 "\t-G n\tpartition the patterns into n groups, searched separately\n");
	fprintf( stderr, 
		 "\t-d n\tuse at most n-order filtering (hierarchy cut-off).\n");
	fprintf( stderr, 
		 "\t-I n\tsuperimpose at most n patterns before preprocessing (don't!).\n");


	fprintf( stderr, 
		 "\n\tMiscellaneous options:\n\n");

	fprintf( stderr, 
		 "\t-v\tbe verbose\n");
	fprintf( stderr, 
		 "\t-?\tshow this message an exit\n\n");

	fprintf( stderr,
		 "\tSome option combinations might be broken, e.g. -Tx -C might not work.\n\n");
}

static void	read_num_pair( char * arg, int * a, int * b )
{
	int	x = 0, y = 0, i = 0;

	if( !arg ) return;
	if( !isdigit( arg[ 0 ] ))
	{
		WARNINGMSGD("Invalid argument: %s\n", arg );
		FATALERRORMSG("Invalid argument");
	}

	do {	x = x * 10 + arg[ i ] - '0';
		i++;

	} while( isdigit( arg[ i ] ));

	*a = x;

	if( isspace( arg[ i ] )) return;
	if( arg[ i ] == '\0') return;

	if( arg[ i ] != ',')
	{
		WARNINGMSGD("Invalid argument: %s\n", arg + i );
		FATALERRORMSG("Invalid argument");
	}

	i++;

	if( !isdigit( arg[ i ] ))
	{
		WARNINGMSGD("Invalid argument: %s\n", arg + i );
		FATALERRORMSG("Invalid argument");
	}

	do {	y = y * 10 + arg[ i ] - '0';
		i++;

	} while( isdigit( arg[ i ] ));

	*b = y;
}

#if 0

void	Myers( unsigned char * t, int n, unsigned char * p, int m )
{
	BMYERS	v;
	long	tp = 0L, ts = 0L;
	int	k = config.k, M = 1, groups = 1;

	( void )groups, ( void )M;

	TIMESTART("Preprocessing... "); 

	v.peq = 0;
        v.vp = 0;
        v.vn = 0;
        v.score = 0;

	init_v( &v, p, k, m );

	v.b = 0;
	v.e = n;

	v.peq = prep_beq( p, m );

	TIMEPP( tp );

	TIMESTART("Searching... "); 

	bed( t, &v, m, k, code );

	TIMESCAN( ts );

	TIMETOT( tp, ts );
}

#endif

void	print_pats_rnd( char ** p, int r )
{
	int	i, j;
	char	*t;

	for( i = 0; i < r; i++ )
	{
		j = rand() % ( r - i );
		printf("%s\n", p[ i + j ] );
		t = p[ i + j ];
		p[ i + j ] = p[ i ];
		p[ i ] = t;
	}
}

int	main( int argc, char ** argv )
{
        unsigned char   * t = 0, * p = 0, ** P = 0;
	char		* tf = 0, * pf = 0;
	int		n = 0, m, M, k = 0, d = 32;
	int		opt, clusterit = 0, groupem = 0, groups = 1;
	int		i, s = 0, hamming = 0;
	int		skipf = 0, skipb = 0, laq = 0, bp = 0, optimize = 0;
	int		iubt = 0, iubp = 0, laqh = 0, strictblaq = 0;
	int		ascb = 0, asce = 255, ascii = 0;
	int		ignorecase = 0, tab, sigma = 256;
	int		E = 0, wm = 0, charg = 0, si = 0;

        tab = 0;

	_emsgs_v = 0;

	while(( opt = getopt( argc, argv, "WT:I:MCN:S:LBOFDPAk:t:p:r:f:d:G:ovcgsu:h:?")) != -1 )
	{
		switch( opt )
		{
 		        case 'T' :
				charg = atoi( optarg );
				if( charg < 2 || charg > 255 )
				{
					WARNINGMSG("Invalid character groupping, ignored.");
					charg = 0;
				}
				break;
		        case 'h':
				laqh = atoi( optarg );
				break;
		        case 'I' : 
				si = atoi( optarg );
				break;
		        case 'W' : 
				wm = 1;
				break;
		        case 'M' : 
				E = 1;
				break;
		        case 'C' :
				ignorecase = 1;
				break;
		        case 'N' :
				read_num_pair( optarg, &ascb, &asce );
				ascii = 1;
				tab   = 8;
				sigma = 1 << tab;
				break;
		        case 'S' :
				if( !optarg ) skipf = skipb = 0; else
				if( toupper( optarg[ 0 ] ) == 'F') skipf = 1; else
				if( toupper( optarg[ 0 ] ) == 'B') skipb = 1; else
					WARNINGMSG("Invalid argument for -S, ignored");
				break;
		        case 'L' :
				laq = 1;
				break;
		        case 'B' :
				bp = 1;
				break;
		        case 'O' :
				optimize = 1;
				break;
		        case 'F' :
				strictblaq = 1;
				break;
		        case 'u' :
				if( !optarg ) iubt = iubp = 0;
				if( toupper( optarg[ 0 ] ) == 'T') iubt = 1;
				if( toupper( optarg[ 0 ] ) == 'P') iubp = 1;
				break;
		        case 'k' : 
				k = atoi( optarg );
				break;
		        case 't' : 
				TUPLE = atoi( optarg );
				break;
		        case 'p' : 
				p = strdup( optarg );
				break;
		        case 'r' : 
				pf = strdup( optarg );
				break;
		        case 'f' :
				tf = strdup( optarg );
				break;
		        case 'd' :
				d = atoi( optarg );
				break;
		        case 'o' :
				code = ( MATCHCODE )output;
				break;
		        case 'v' :
				_emsgs_v = 1;
				break;
		        case 's' :
				hamming = 1;
				break;
		        case 'c' :
				clusterit = 1;
				break;
		        case 'G' :
				groups = atoi( optarg );
				break;
		        case 'g' :
				groupem = 1;
				break;
		        case '?' :
				usage( argv[ 0 ] );
				exit( 0 );
				break;
		        case 'D' :
				ascii = 0;
				tab   = 2;
				sigma = 1 << tab;
				break;
		        case 'P' :
				ascii = 0;
				tab   = 5;
				sigma = 20;
				break;
		        case 'A' :
				ascii = 1;
				ascb = 0;
				asce = 255;
				tab   = 8;
				sigma = 1 << tab;
				break;
		        default:  
				fprintf( stderr, "Ignored...\n");
				break;
		}
	}

	if(( !p && !pf ) || !tf || ( TUPLE < 1 ) || !tab ) 
	{ 
		usage( argv[ 0 ] ); 
		return 1; 
	}

	TBITS = (( 1 << ( tab * TUPLE )) - 1 );
	SBITS = (( 1 << tab ) - 1 );

	VERBOSEMSG("Reading file... ");

	startclock();

	t = f_read_bytes( tf, &n );

	VERBOSEMSGD("Done, read %d bytes, time: %1.2f", n, 
	       stopclock() / ( double )hz );

#	ifdef COMPRESSED
	if( sigma == 4 ) n *= 4;
	if( sigma == 20 ) n = ( n * 8 ) / 5;
#	endif

	if( p ) { M = 0; m = strlen( p ); } else m = 0;

	if( pf ) 
	{
		P = read_patterns( pf, &M );

		if( clusterit ) cluster(( char ** )P, M, E );

	//	print_pats_rnd( P, M );

		if( groupem ) 
		{
			for( i = 0; i < M; i++ ) s += strlen( P[ i ] );
		//	groups = binomial( TUPLE, k ) * s / pow( sigma, TUPLE ) + 1;
			groups = s / pow( sigma, TUPLE ) + 1;

			if( groups > M ) groups = M;

			VERBOSEMSGD("The number of groups is %d", groups );
		}
	}

	printf("config_set(%d, %d, %d, %d, %d, %d, %d, %d, %d);\n",
                        TUPLE, sigma, ascii, charg, ascb, asce, ignorecase, iubt, iubp);
	config_set( TUPLE, sigma, ascii, charg, ascb, asce, ignorecase, iubt, iubp );
        printf("config_data(t, %d, P, %d);\n", n, M);
	config_data( t, n, P, M );
        printf("config_opts(%d, %d, %d, %d);\n", k, d, hamming, si);
	config_opts( k, d, hamming, si );
        printf("config_alg(%d, %d, %d, %d, %d, %d, %d, %d, %d);\n",
                skipf, skipb, laq, bp, optimize, strictblaq, laqh, E, wm);
	config_alg( skipf, skipb, laq, bp, optimize, strictblaq, laqh, E, wm );

	tab = config.TAB;
	TBITS = config.TBITS;

	get_map( t, n, charg );

//	if( ascii && charg ) get_group_map( t, n, charg );

//	if( E ) { Myers( t, n, p, m ); return 0; }
//	else 
	if( p ) cm(( char * )t, n, p, m, k, code ); 
	else if( config.laq && !config.skipb ) lcm_b_pt(( char * )t, n, P, M, k, d, groups, code ); 
	else {
        cmfn( groups, code, REPETITIONS );
    }

	config_free();

	if( tf ) free( tf );
	if( pf ) free( pf );
	if( p ) free( p );

	return 0;
}


