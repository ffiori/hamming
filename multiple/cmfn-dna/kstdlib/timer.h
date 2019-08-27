/*	Kimmo.Fredriksson@cs.Helsinki.FI	*/

#ifndef		_TIMER_H_
#define		_TIMER_H_

#include	<unistd.h>

#define		hz		sysconf( _SC_CLK_TCK )

#if defined __cplusplus

extern "C" {

#endif

void		startclock(); 
long		stopclock();
long		get_clock();

#if defined __cplusplus

}

#endif

#endif	/*	_TIMER_H_	*/

