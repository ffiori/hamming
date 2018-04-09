#ifndef	_FIO_H_
#define	_FIO_H_

#if defined __cplusplus

extern "C" {

#endif

char    * f_read_bytes( char * fn, int * n );
char	* f_read_rest_bytes( char * fn, int pos, int * n );

#if defined __cplusplus

}

#endif

#endif
