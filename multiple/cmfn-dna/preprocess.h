//
//	(C) Copyright 2002 by Kimmo Fredriksson.
//
//	The code can be modified and redistributed if this note is left
//	intact.
//
//	Comments, improvements and bug-reports can be emailed to kf@iki.fi
//

#ifndef		_PREPROCESS_H_
#define		_PREPROCESS_H_

#include	<limits.h>
#include	"bped.h"
#include	"wm.h"

#define	VECLEN	32

#if	VECLEN == 32

typedef	unsigned long 		cntr_t;

#elif	VECLEN == 64

typedef	unsigned long long 	cntr_t;

// Obs! Not all the algorithms are adapted to 128bits!
// Obs! If you use 128bits, compile this as c++ program, not c.

#elif	VECLEN == 128

#if !defined __cplusplus
#error ERROR: C++ compiler required!
#endif

#include	"sse2_FxB.H"

typedef	uiFxB	 		cntr_t;

#else

#error Invalid VECLEN

#endif

typedef	unsigned long long  	cntr_laq_t;

#define		CNTR_BITS	( sizeof( cntr_t ) * CHAR_BIT )
#define		CNTR_LAQ_BITS	( sizeof( cntr_laq_t ) * CHAR_BIT )

enum { BASIC, LAQ, BLAQ, MYERS, WUMANBER }; 

typedef	struct	laqc
{
	int		b;	/* number of blocks */
	int		bits;	/* number of bits / block */
	int		h;	/* sampling rate */

	cntr_laq_t	om;
	cntr_laq_t	mm;
	cntr_laq_t	lim;

} LAQC;


typedef union
{
	BMYERS	* m;
	WM	* w;

} VCATOR;

typedef union
{
	char		* s;
	cntr_laq_t	* l;
	char		** sl;
	cntr_t		* bs;
	cntr_t		** bsl;

} TDIST;

typedef struct	ptree
{
	TDIST		t;	/* distance to the best matching tuple */
        unsigned char	* p;	/* ptr to the pattern, if leaf, 0 otherwise */
        int		m;	/* pattern lenght */

	int		d;	/* counter */
	int		i;	/* current pos */

	VCATOR		v;	/* "verificator" */

	void		* a;	/* algorithm spesific data */

        struct ptree	* l;	/* left child */
        struct ptree	* r;	/* right child */


} PTREE;


typedef struct	ptb
{
        cntr_laq_t	* t;	/* distances to the best tuples in the blocks*/
        unsigned char	* p;	/* ptr to the pattern, if leaf, 0 otherwise */
        int		m;	/* pattern lenght */

	int		b;	/* number of blocks */
	int		bits;	/* number of bits / block */
	int		h;	/* sampling rate */

	cntr_laq_t	om;
	cntr_laq_t	mm;
	cntr_laq_t	lim;


	cntr_laq_t	strictD;
	cntr_laq_t	strictH;
	cntr_laq_t	strictHM;

	BMYERS		* v;

        struct ptb	* l;	/* left child */
        struct ptb	* r;	/* right child */

} PTB;

typedef struct	ptbp
{
        TDIST		t;	/* distances to the best matching tuples */
        unsigned char	** p;	/* ptrs to the patterns if leaf, 0 otherwise */
        int		* m;	/* pattern lenghts */
	int		mm;	/* minumum lenght in the set */

	cntr_t		d;
	int		i;

	int		np;	/* # patterns */

	BMYERS		** v;

        struct ptbp	** c;	/* children */

} PTBP;

typedef struct	vecsum
{
	signed char	i[ 1 << 16 ];
	signed char	m[ 1 << 16 ];

} VECSUM;



char 		* build_sp( unsigned char * p, int m );

PTREE		* build_pt( unsigned char ** p, int n, int depth, int a );

void		build_p_pt( PTREE * pt, unsigned char ** p, int n, int depth );

PTBP		* build_ptbp( unsigned char ** p, int n, int a, int b, int depth, int alg );

PTB		* build_pt_b( unsigned char ** p, int n, int k, int depth );

void		free_pt( PTREE * pt, int alg );
void		free_pt_b( PTB * pt );
void		free_ptbp( PTBP * pt, int alg );

unsigned char	** read_patterns( char * fn, int * n );

#endif
