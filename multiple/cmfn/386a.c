#ifdef __linux
/* added code for affinity to the CPU with largest number - HSP 2007 */
/* defining _GNU_SOURCE is necessary; -std=gnu89 or -std=gnu99 don't work */
#define 	_GNU_SOURCE
#endif
#include	<sched.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<unistd.h>


static void aff_proc() {
	pid_t pid;
	int i, onln;  /* number of processors online */

	onln = sysconf(_SC_NPROCESSORS_ONLN);
	if(onln > 1) {
		pid = getpid();
#if 0 //def __linux
		cpu_set_t cpu_set;
		CPU_ZERO(&cpu_set);
		CPU_SET(onln-1, &cpu_set);  /* CPU numbering starts from 0 */

		if(sched_setaffinity(pid, sizeof(cpu_set), &cpu_set) != 0) {
			printf("Setting CPU affinity failed.\n");
			abort();
		}

		if(sched_getaffinity(pid, sizeof(cpu_set), &cpu_set) != 0) {
			printf("Getting CPU affinity failed.\n");
			exit(1);
		}
		for(i = onln-2; i >= 0; i--)
		    if(CPU_ISSET(i, &cpu_set)) {
			printf("Failed to unset cpu affinity for cpu %d\n",i);
		    }
		if(!CPU_ISSET(onln-1, &cpu_set)) {
			printf("Failed to set cpu affinity for cpu %d\n",onln-1);
		}
#endif
	}
}





#include	<sys/types.h>
#include	<sys/times.h>
#include	<sys/param.h>
#include	<unistd.h>

#ifdef OLD_STYLE
int hz = HZ;
#else
/* if HZ is not a constant expression, we have to evaluate it on runtime */
/* modified by HSP 2006 */
int hz = -100;
static int hz_set = 0;
#endif
double tcmp = 4.91;

static struct tms start;

void startclock()
{
#ifndef OLD_STYLE
	if(! hz_set) {
#ifdef HZ
		hz = HZ;
#else	/* at least with freeBSD */
		hz = sysconf(_SC_CLK_TCK);
#endif
		hz_set = 1;
		aff_proc();
	};
#endif
	times(&start);
}

int stopclock()
{
	struct tms t;

	times(&t);
	/* we should also keep watch on tms_stime */ 
	return(t.tms_utime - start.tms_utime);
}
