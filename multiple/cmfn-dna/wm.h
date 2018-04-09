#ifndef	_WM_H_
#define	_WM_H_

#include	"kstdlib.h"

typedef unsigned long long  	wmvec;

#define		WMVECBITS	( sizeof( wmvec ) * CHAR_BIT )

typedef	struct wm
{
	wmvec	T[256];
	/*
	int	Q;

	wmvec	initWs;
	wmvec	eMask;
	wmvec	eBit;
	wmvec	sMask;
	wmvec	SF[256], SR[256];
	wmvec	_M, _Min, R[64];
	*/

} WM;

static inline WM	* wm_init()
{
	WM	* wm;

	wm = ( WM * )salloc( sizeof( WM ));

	/*
	wm->Q = 0;
	wm->initWs = ( wmvec )0;
	wm->eMask = ( wmvec )0;
	wm->eBit = ( wmvec )0;
	wm->sMask = ( wmvec )0;
	*/

	return wm;
}



void	wm_pp( WM * wm, unsigned char * p, int m, int k );

void	wm_combine( WM * wm, WM * wm0, WM * wm1 );

//int	wm_search( PTWM * pt, int m, int n, unsigned char *Text, int k );

#endif
