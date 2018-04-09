#ifndef	_ASMU_H_
#define	_ASMU_H_

#include	<stdlib.h>
#include	<unistd.h>

#if defined(__i386__)

#define rep_movsl( dest, src, numwords) \
__asm__ __volatile__ ( \
  "cld\n\t" \
  "rep\n\t" \
  "movsl" \
  : : "S" (src), "D" (dest), "c" (numwords) \
  : "%ecx", "%esi", "%edi" )

#define rep_stosl( dest, value, numwords ) \
__asm__ __volatile__ ( \
  "cld\n\t" \
  "rep\n\t" \
  "stosl" \
  : : "a" (value), "D" (dest), "c" (numwords) \
  : "%ecx", "%edi" )

/*
#define RDTSC( llptr ) ({ \
__asm__ __volatile__ ( \
        ".byte 0x0f; .byte 0x31" \
        : "=A" (llptr) \
        : : "%eax", "%edx"); })

*/


/* 
 * Returns the tick counter (which is incremented every clock cycle).
 * 
 */

#define CPUID	__asm__ __volatile__ ("cpuid\n\t")
#define	RDTSC	({							\
		long long	__stamp;				\
		__asm__ __volatile__ (					\
		"rdtsc\n\t" 						\
		"movl\t %%eax, %0\n\t" 					\
		"movl\t %%edx, %1\n\t" 					\
		: "=m" (__stamp), "=m" (*((( int * )&__stamp) + 1 ))	\
		: : "%eax", "%edx"); 					\
		__stamp; })


/* 
 * Returns the cpu clock frequence (in MHz). The "bug" is that it takes
 * an entire second to execute... Could be (almost) as fast as you want,
 * but the resolution would quickly decrease...
 */

#define	MHz	({	long long t = RDTSC;	\
			usleep( 1000000 ); 	\
			t = RDTSC - t;		\
			t / 1000000.0;		})

#else

#include	<stdlib.h>

#define	rep_movsl( dest, src, numwords ) memcpy( dest, src, ( numwords ) / sizeof( int ))

#define	rep_stosl( dest, value, numwords ) { int i; for( i = 0; i < numwords / sizeof( int ); i++ ) (( int * )( dest ))[ i ] = value; }

#define	RDTSC	0

#endif

#endif

