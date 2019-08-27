/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/

/* 
 *
 * Multi Pattern Matcher 
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include "mgrep.h"
#include <sys/types.h>
#include <sys/mman.h>

/********************************************************
GLOBALS
********************************************************/

GLOBALFLAG GlobalFlag;

/********************************************************
FATAL ERROR FUNCTION
********************************************************/

void FatalError(int errorno, char *mess, ... )
{
    va_list ap;

    va_start(ap,mess);
    vfprintf(stderr,mess,ap);
    va_end(ap);
    exit(errorno);


}

/********************************************************
DETERMINE SIZE OF FILE
********************************************************/

int FileLength(int fd)
{
    int size;

    size = lseek(fd,0,SEEK_END);
    lseek(fd,0,SEEK_SET);

    return size;
}

/********************************************************
Memory Map File
********************************************************/

uint8 *MemoryMapFile(int fd, int filelength, int headroom, int tailroom)
{
    int pagesize = sysconf(_SC_PAGESIZE);
    uint8 *address;

    if( GlobalFlag.Debug )
	printf("pagesize: %d\n",pagesize);

    /* round headroom up to next page multiple */

    headroom += pagesize - 1;
    headroom /= pagesize;
    headroom *= pagesize;    

    address = (uint8 *) valloc( headroom + tailroom + filelength );
    if( GlobalFlag.Debug )
	printf("valloc: %p\n",address);

    address = mmap( address + headroom, filelength, 
		      PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_FIXED, fd, 0);

    if( GlobalFlag.Debug )
	printf("mmap: %p\n",address);

    return address;
}

/********************************************************
MAIN
********************************************************/
int Usage()
{
    printf("usage\n"
           "\tmagrep flags -f pattern_file search_file\n"
           "flags:\n"
           "\td\tdebug\n"
           "\tm\tdo not search\n"
           "\tr\tprint matching pattern at the beginning of each line\n"
           "\tc\treport only number of matching lines\n"
           "\ts\tsilent, no output\n"
           "\tx\toutput statistical information\n"
           "\th\thelp\n");
           
return -1;
}





#define CHARTYPE char

void prep(CHARTYPE * ps, int len, unsigned _k, unsigned _q)
{
    int i;
    uint8 *address = ps;
    
    ProcessPatterns(address,address+len); //[patts...)
}


int exec(CHARTYPE * text, unsigned len, unsigned _k, unsigned _q)
{
    uint8 *address = text;
    
    address[-1] = '\n';
    address[-2] = '\0';
    if( address[len-1] != '\n' )
    {
        //address[len] = '\n';
        //address[len+1] = '\0';
        address[len] = '\0';
    }
    else
    {
        address[len] = '\0';
    }

    if( GlobalFlag.Debug )
        printf("MatchText start\n");

    return MatchText(address, address + len);
}

#if 0

int main(int argc, char *argv[])
{
    int i;
    int fd;
    uint8 *address;
    int len;
    char *PatternFileName;


    PatternFileName = 0;

    while( EOF != (i = getopt(argc,argv,"mdrcsxhvg:f:")) )
    {
	switch(i)
	{
	case 'v':
	    break;
	case '?':
	case 'h':
            return Usage();
	    break;
	case 'r':
	    GlobalFlag.Report = true;
	    break;
	case 'c':
	    GlobalFlag.Silent = true;
	    GlobalFlag.Count = true;
	    break;
	case 's':
	    GlobalFlag.Silent = true;
	    break;
	case 'x':
	    GlobalFlag.Statistics = true;
	    break;
	case 'd':
	    GlobalFlag.Debug = true;
	    break;
        case 'm':
	    GlobalFlag.NoMatching = true;
	    break;
	case 'f':
	    PatternFileName = optarg;
	    break;
        case 'g':
	    GlobalFlag.Seed = atoi(optarg);
	    break;
	}
    }

    /*
     * DO PATTTERN FILE
     */

    if( PatternFileName == 0 )
	FatalError(-1,"No pattern file specified\n");

    fd = open(PatternFileName, O_RDONLY);
    if( fd < 0 )
	FatalError(-1,"cannot open pattern file: %s\n",PatternFileName);

    len = FileLength(fd);
    address = MemoryMapFile(fd,len,65536,65536);
    if( address[len-1] != '\n' )
    {
	address[len] = '\n';
	address[len+1] = '\0';
    }
    else
    {
	address[len] = '\0';
    }

    if( GlobalFlag.Debug )
	printf("ProcessPatterns start\n");
    ProcessPatterns(address,address+len);
    if( GlobalFlag.Debug )
	printf("ProcessPatterns stop\n");
    close(fd);


    /* 
     * DO MATCHING 
     */

    for(argc -= optind, argv += optind; argc > 0; argc--, argv++)
    {
	fd = open(*argv, O_RDONLY);
	if( fd < 0 )
	    FatalError(-1,"cannot open text file: %s\n",*argv);

	len = FileLength(fd);
	address = MemoryMapFile(fd,FileLength(fd),65536,65536);

	address[-1] = '\n';
	address[-2] = '\0';
	if( address[len-1] != '\n' )
	{
	    //address[len] = '\n';
	    //address[len+1] = '\0';
	    address[len] = '\0';
	}
	else
	{
	    address[len] = '\0';
	}

	if( GlobalFlag.Debug )
	    printf("MatchText start\n");

        if( !GlobalFlag.NoMatching )
            MatchText(address, address + len); 

	if( GlobalFlag.Debug )
	    printf("MatchText stop\n");
	close(fd);
    }

    return 0;
}

/********************************************************
  EOF
********************************************************/
#endif






