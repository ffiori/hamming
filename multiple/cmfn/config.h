#ifndef		_O_CONFIG_H_
#define		_O_CONFIG_H_

#include	<stdlib.h>

// Define COMPRESSED if you use compressed files:

// #define		COMPRESSED

// Define SAVESPACE if you want to save some space as well as preprocessing 
// time for alphabets that are (far) away from exact powers of two. This is 
// allowed only if COMPRESSED is not enabled. It also makes the searching 
// slower by a (small?) constant factor. And what's worse, it is not 
// implemented (yet) for Hamming, LAQ or bit-parallel counters... Only for 
// the basic hierarchy. And, it breaks the optimization algs...

// #define		SAVESPACE

// END OF COMPILATION OPTIONS ------------------------------------------------
// ---------------------------------------------------------------------------

#ifdef COMPRESSED

# ifdef SAVESPACE

#  error Use savespace only without compressed

# endif

#endif

typedef int			( * MATCHCODE )( int i, int score, char * p );

typedef struct config
{
	int		TUPLE;			// # symbols
	int		TAB;			// bits / symbol
	int		TBITS;			// mask for tuple
	int		SBITS;			// mask for symbol
	int		fSIGMA;			// for filtering with mapping
	int		vSIGMA;			// for verification

	size_t		TUPLESPACE;

	int		SPACEMUL;
	int		SPACESHIFT;

	int		ascii;			// using ASCII alphabet?
	int		ignorecase;

	int		ascb;			// first and last (inclusive)
	int		asce;			// possible characters for ASCII

	int		IUBT;			// Using UIB? (for DNA)
	int		IUBP;			// Using UIB? (for DNA)

	unsigned char	* t;			// The text...
	int		n;			// ...and its lenght

	unsigned char	** p;			// The set of patterns
	int		M;			// # patterns
	int		m;			// len of the longest pattern

	int		hamming;		// use hamming distance?

	int		k;			// # errors

       	int		maxdepth;		// for the hierarchy

	int		si;			// superimposition for the 
	                                        // hierarchy leaves

	int		myers;			// ...w/ superimposition & hieararchy
	int		wm;
	int		skipf;			// skip chars (forward matching)?
	int		skipb;			// skip chars (backward)?
	int		laq;			// use LAQ method?
	int		use_bp_cntrs;		// bit parallelism?
	int		opt;			// optimally choice the tuples?
	int		laqh;			// sampling rate
	int		strictblaq;



	int		matches;

	void		*data;			// whatever

} CONFIG;

extern CONFIG	config;

void	config_set( int tuple, int sigma, int ascii, int charg, int ascb, int asce, int ignorecase, int iubt, int iubp );
void	config_data( unsigned char * t, int n, unsigned char ** p, int M );
void	config_opts( int k, int maxdepth, int hamming, int si );
void	config_alg( int skipf, int skipb, int laq, int use_bp_cntrs, int opt, int strictblaq, int laqh, int myers, int wm );
void	config_free( void );

#endif

