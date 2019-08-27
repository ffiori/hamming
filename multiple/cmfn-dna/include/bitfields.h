#ifndef	_BITFIELDS_H_
#define	_BITFIELDS_H_

// Concatenates bitvector x, which has b bits, f times.

// bf_copy function is optimed away, and replaced by a constant, if all 
// parameters (x, f, b) are constants, and loop-unrolling is enabled.


#define	BF_COPY( x, f, b )	({ 	int __fields = f;		\
					typeof ( x ) __r = x;		\
					while( --__fields ) 		\
					  __r = ( __r << ( b )) | ( x );\
					__r; 				\
				})

static inline unsigned bf_copy( unsigned x, int f, int b )
{
        while( --f ) x = ( x << b ) | x;

        return x;
}

#endif

