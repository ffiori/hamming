
#ifndef		_MACROS_H_
#define		_MACROS_H_

#define	MIN( a, b )		(( a ) < ( b ) ? ( a ) : ( b ))
#define	MAX( a, b )		(( a ) > ( b ) ? ( a ) : ( b ))

#define	SWAP( yesiknow, a, b )	{ yesiknow c = a; a = b; b = c; }

#define	CAST(idiot_t, idiot_o)	(*((idiot_t *)&idiot_o))

#endif	/*	_MACROS_H_	*/

