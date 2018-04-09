/* (C) 1996 by Kimmo.Fredriksson@cs.Helsinki.FI / No warranty... */



#ifndef	_RND_H_
#define	_RND_H_

#include	<stdlib.h>
#include	<limits.h>

#define		RND_MAX		( RAND_MAX / 2 )

#ifdef	__STRICT_ANSI__
#define	inline
#endif

#if defined __cplusplus

extern "C" {

#endif

static inline int	rndi()
{
	return rand() / 2;  /* this is bad... */
}


static inline float	rndf()
{
	return ( float )rndi() / RND_MAX;
}

unsigned	randomize();

#if defined __cplusplus

}

#endif

#endif	/* _RND_H_ */

