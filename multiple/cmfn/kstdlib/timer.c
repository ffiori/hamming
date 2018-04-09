/*	Kimmo.Fredriksson@cs.Helsinki.FI	*/

#include		<sys/types.h>
#include		<sys/times.h>
/*
#include		<sys/param.h>
*/
#include		"timer.h"

#define RDTSC( llptr ) ({ \
__asm__ __volatile__ ( \
        ".byte 0x0f; .byte 0x31" \
        : "=A" (llptr) \
        : : "eax", "edx"); })


static struct tms	start;
static struct tms	stop;

//static long long 	s = 0L;
//static long long 	e = 0L;

void	startclock() 
{ 
//	RDTSC( s ); return;
	times( &start );
}

long	stopclock()
{
//	RDTSC( e ); return e-s;;
	times( &stop );
	return stop.tms_utime - start.tms_utime;
}

long	get_clock()
{
	struct tms	now;

	times( &now );

	return now.tms_utime;
}
