#include	<sys/types.h>
#include	<sys/times.h>
#include	<sys/param.h>

int hz;
double tcmp = 4.91;

static struct tms start;

int startclock()
{
	hz = HZ;
	return times(&start);
}

int stopclock()
{
	struct tms t;

	times(&t);
	return(t.tms_utime - start.tms_utime);
}
