#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include 	<sys/types.h>
#include 	<sys/stat.h>
#include 	<fcntl.h>
#include	<unistd.h>
#include 	<errno.h>
#include	"memalloc.h"
#include	"fio.h"
#include	"errormsgs.h"

char	* f_read_bytes( char * fn, int * n )
{
	struct stat	buf;
	int		i;
	char		* b;

	stat( fn, &buf );
		
	i = open( fn, O_RDONLY );
		
	if( i == -1 ) 
	{
		char	* msg, * e;

		e = strerror( errno );
		msg = salloc( strlen( e ) + strlen( fn ) + 8 );
		strcat( strcat( strcpy( msg, fn ), ": "), e );
		FATALERRORMSG( msg );
	}

	b = ( char * )salloc( buf.st_size );

	*n = read( i, b, buf.st_size );

/*	printf("Read %d bytes.\n", *n ); */
	
	close( i );

	return b;
}

char	* f_read_rest_bytes( char * fn, int pos, int * n )
{
	struct stat	buf;
	int		i;
	char		* b;

	stat( fn, &buf );
		
	i = open( fn, O_RDONLY );
		
	if( i == -1 ) 
	{
		char	* msg, * e;

		e = strerror( errno );
		msg = salloc( strlen( e ) + strlen( fn ) + 8 );
		strcat( strcat( strcpy( msg, fn ), ": "), e );
		FATALERRORMSG( msg );
	}

	b = ( char * )salloc( buf.st_size - pos );

	lseek( i, pos, SEEK_SET );

	*n = read( i, b, buf.st_size - pos );

/*	printf("Read %d bytes.\n", *n ); */
	
	close( i );

	return b;
}

