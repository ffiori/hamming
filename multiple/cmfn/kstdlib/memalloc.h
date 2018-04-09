#ifndef		_MEMALLOC_H_
#define		_MEMALLOC_H_

#include	<stdlib.h>


//#ifdef          __ICC

//#define malloc( __bytes )	memalign( 16, __bytes )
//#define malloc			mymemalign

/*
#define malloc( __bytes )	_mm_malloc( __bytes, 16 )
#define free( __p )		_mm_free( __p )
*/

//#else

//#endif

#if defined __cplusplus

extern "C" {

#endif

void	*salloc( size_t );
void	*allocate_2d( int, int, size_t );
void	*allocate_3d( int, int, int, size_t );
void	*reallocate_2d_rows( void *, int, int, size_t );
void	*allocate_1d( int, size_t );
void	copy_3d( void *, void *, int, int, int, size_t );
void	free_3d( void *, int );
void	free_2d( void * );
void	free_1d( void * );

/*
static inline void *	mymemalign( size_t bytes )
{
	void	* m;

	posix_memalign( &m, 16, bytes );

	return m;
}
*/

#if defined __cplusplus

}

#endif

#endif	/*	_MEMALLOC_H_	*/

