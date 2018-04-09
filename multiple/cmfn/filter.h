//
//	(C) Copyright 2002 by Kimmo Fredriksson.
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

#ifndef		_FILTER_H_
#define		_FILTER_H_

#include	"bped.h"
#include	"config.h"
#include	"kstdlib.h"

/*
#ifdef		HAMMING
#define		BWIN( m, k )	((( m ) + 1 ) / 2 )
#define		SWIN( m, k )	((((( m ) + 1 ) / TUPLE ) - 1 ) * TUPLE )
#else
#define		BWIN( m, k )	((( m ) - ( k ) + 1 ) / 2 )
#define		SWIN( m, k )	((((( m ) - ( k ) + 1 ) / TUPLE ) - 1 ) * TUPLE )
#endif
*/

#define		BLOCKHWIN( m, k )	((( m ) + 1 ) / 2 )
#define		BLOCKLWIN( m, k )	((( m ) - ( k ) + 1 ) / 2 )
#define		BLOCKWIN( m, k )	(( config.hamming ) ? BLOCKHWIN( m, k ) : BLOCKLWIN( m, k ))

#define		SLIDINGHWIN( m, k )	((((( m ) + 1 ) / config.TUPLE ) - 1 ) * config.TUPLE )
#define		SLIDINGLWIN( m, k )	((((( m ) - ( k ) + 1 ) / config.TUPLE ) - 1 ) * config.TUPLE )
#define		SLIDINGWIN( m, k )	(( config.hamming ) ? SLIDINGHWIN( m, k ) : SLIDINGLWIN( m, k ))

#define		BMHWIN( m, k )		( m )
#define		BMLWIN( m, k )		(( m ) - ( k ))
#define		BMWIN( m, k )		(( config.hamming ) ? BMHWIN( m, k ) : BMLWIN( m, k ))

#define		DWIN( m, k )		( config.skipf ? BLOCKWIN( m, k ) : ( config.skipb ? BMWIN( m, k ) : SLIDINGWIN( m, k )))

// Define this if you want timings...

//#define		TIMEIT

#define		CUTOFF	3000.0
#define		CUTLOFF	3000.0

#define		REPETITIONS	1

#ifdef 		TIMEIT

#define		TIMESTART( s )	startclock();

//#define	TIMECUTOFF( rep, tp, ts )
#define		TIMECUTOFF( rep, tp, ts ) { if((( tp + ts ) / ( double )hz ) > CUTOFF ) { rep++; tp /= rep; ts /= rep; break; }}

//#define	TIMELOOPCUTOFF
#define		TIMELOOPCUTOFF	if(( stopclock() / ( double )hz ) > CUTLOFF ) exit( 1 );

#define		TIMESCAN( ts )	ts += stopclock();

#define		TIMEPP( tp )	tp += stopclock();

/*
#define		TIMESCAN()

                                { double t = stopclock() / ( double )hz;	\
                                  printf("%d\t%1.2f\t#scan-T\n", TUPLE, t );	\
	                          printf("%d\t%1.2f\t#scan-k\n", k, t );	\
	                          printf("%d\t%1.2f\t#scan-m\n", m, t );	\
	                          printf("%d\t%1.2f\t#scan-M\n", M, t ); }


#define		TIMEPP()	{ double t = stopclock() / ( double )hz;	\
	                         printf("%d\t%1.2f\t#prep\n", TUPLE, t ); }

*/

#define		TIMETOT( tp, ts ) { double	t; \
                                    t = tp / ( double )hz; \
	                            printf("%d\t%1.2f\t#prep\n", config.TUPLE, t ); \
                                    t = ts / ( double )hz; \
                                    printf("%d\t%1.2f\t#scan-T\n", config.TUPLE, t );	\
	                            printf("%d\t%1.2f\t#scan-k\n", k, t );	\
	                            printf("%d\t%1.2f\t#scan-m\n", m, t );	\
	                            printf("%d\t%1.2f\t#scan-M\n", M, t ); \
                                    t = ( tp + ts ) / ( double )hz; \
                                    printf("%d\t%1.2f\t#tot-T\n", config.TUPLE, t );	\
	                            printf("%d\t%1.2f\t#tot-k\n", k, t );	\
	                            printf("%d\t%1.2f\t#tot-m\n", m, t );	\
	                            printf("%d\t%1.2f\t#tot-M\n", M, t ); \
	                            printf("%d\t%1.2f\t#tot-g\n", groups, t ); \
		                    printf("%d\t%1.2f\t#AVG shift-k\n", config.k, ( double )avg / avgi );}
#else

#define		TIMECUTOFF( rep, tp, ts )
#define		TIMELOOPCUTOFF

#define		TIMESTART( s )	{ VERBOSEMSG( s ); startclock(); }

#define		TIMESCAN( ts )	{ long	 tt = stopclock(); \
	                          double t = tt / ( double )hz;	\
	                          VERBOSEMSGD("Done, matches: %d, time: %1.2f", config.matches, t ); \
                                  ts += tt; }


#define		TIMEPP( tp )	{ long	 tt = stopclock(); \
	                          double t = tt / ( double )hz;	\
                                  VERBOSEMSGD("Done, time: %1.2f", t ); \
                                  tp += tt; }


#define		TIMETOT( tp, ts ) { double	t; \
                                    t = tp / ( double )hz; \
                                    VERBOSEMSGD("Done, total pp time: %1.2f", t ); \
                                    t = ts / ( double )hz; \
                                    VERBOSEMSGD("Total search time: %1.2f", t ); \
                                    t = ( tp + ts ) / ( double )hz; \
                                    VERBOSEMSGD("Total time: %1.2f", t ); }
#endif

#define		GROUPB( i, groups, M, gM, P ) \
	                for( i = 0; i < groups; i++ ) { \
		                gM = M / groups; \
                                if( i == groups - 1 ) gM = M - i * gM;

#define		GROUPE( gM, P )	P += gM; }

#define		REPB( rep, reps ) for( rep = 0; rep < reps; rep++ ) {

#define		REPE( reps, tp, ts ) } tp /= reps; ts /= reps;

//int	myers( PT * pt, MATCHCODE code );

int	cmfn( int groups, MATCHCODE code, int reps );
void cmfn_prep( int groups, MATCHCODE code, int reps );
int cmfn_exec( int groups, MATCHCODE code, int reps );


int	cm( const unsigned char * t, int n, unsigned char * p, int m, int k, MATCHCODE code );

int	cm_pt( const unsigned char * t, int n, unsigned char ** p, int M, int k, int opt, int md, int groups, MATCHCODE code );

int	cm_ptpb( const unsigned char * t, int n, unsigned char ** p, int M, int k, int opt, int md, int groups, MATCHCODE code );

int	lcm_pt( const unsigned char * t, int n, unsigned char ** p, int M, int k, int md, int groups, MATCHCODE code );

int	lcm_b_pt( const unsigned char * t, int n, unsigned char ** p, int M, int k, int md, int groups, MATCHCODE code );

int	lcm_ptpb( const unsigned char * t, int n, unsigned char ** p, int M, int k, int md, int groups, MATCHCODE code );

#endif

