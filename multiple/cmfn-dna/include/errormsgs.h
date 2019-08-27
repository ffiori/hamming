#ifndef	_ERRORMSGS_H_
#define	_ERRORMSGS_H_

#include	<stdio.h>


extern int	_emsgs_w;
extern int	_emsgs_e;
extern int	_emsgs_f;
extern int	_emsgs_d;
extern int	_emsgs_v;


static char			_macros_h_message_[ 1024 ];

#define BMSG( x,... )		(sprintf( _macros_h_message_, x, __VA_ARGS__ ), _macros_h_message_ )

#define	STDERRMSG( s )		fprintf( stderr, "\n\t%s\n\n", s )

#define	STDERRMSGV( s )		fprintf( stderr, "%s\n", s )

#define	LOCMSG( s )		fprintf( stderr, "\n%s: FILE %s: FUNCTION %s: LINE %d: ", s, __FILE__, __FUNCTION__, __LINE__ )

#define	WARNINGMSG( s )		({ if( _emsgs_w ) { LOCMSG("WARNING"); STDERRMSG( s ); }})

#define	WARNINGMSGD( s,... )	{ if( _emsgs_w ) { LOCMSG("WARNING"); STDERRMSG( BMSG( s, __VA_ARGS__ )); }}

#define	ERRORMSG( s )		{ if( _emsgs_e ) { LOCMSG("ERROR"); STDERRMSG( s ); }}

#define	FATALERRORMSG( s )	{ if( _emsgs_f ) ERRORMSG( s ); exit( 1 ); }

#define	DBGMSG( s,... )		{ if( _emsgs_d ) STDERRMSG( BMSG( s, __VA_ARGS__ )); }

#define	DBGLMSG( s,... )	{ if( _emsgs_d ) { LOCMSG("DEBUG"); STDERRMSG( BMSG( s, __VA_ARGS__ )); }}

#define	VERBOSEMSG( s )		{ if( _emsgs_v ) STDERRMSGV( s ); }

#define	VERBOSEMSGD( s,... )	{ if( _emsgs_v ) STDERRMSGV( BMSG( s, __VA_ARGS__ )); }

#endif

