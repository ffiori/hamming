#include	<fcntl.h>
#ifndef  sun
#include	<getopt.h>
#endif
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<unistd.h>

#define		NTIME		1000

char buf[2097152];
char wbuf[101000];
long tm[NTIME+1];
int nbuf;
extern int hz;
int option = -1;

void startclock();
int stopclock();
void prep(char *pattern, unsigned m, unsigned kval, unsigned qval);
int exec(char *text, unsigned n, unsigned kval, unsigned qval);

int cmp(a1, b1)
	const void *a1, *b1;
{
	const long *a=a1, *b=b1;
	if(*a < *b)
		return(-1);
	else
		return((*a > *b)? 1:0);
}

/* 
 * globals.
 * timing cmfn preprocessing and execution times separately needs these */
long tprep, texec;
int npreproc = 1;
int nexec = 1;
int main(int argc, char *argv[])
{
	int c, cnt;
	int kval=0;
	int qval=1;
	long *tmp = tm;
	int nmatched, len;
	long nf, nw;
	char *s, *ss, *sys = "?";
	int percent = 0;
    long t;
	extern char *optarg;
	extern int optind;

	while((c = getopt(argc, argv, "%k:q:e:p:s:o:")) != -1)
		switch(c)
		{
		case 'k':	kval = atoi(optarg); break;
		case 'q':	qval = atoi(optarg); break;
		case 'p':	npreproc = atoi(optarg); break;
		case 'e':	nexec = atoi(optarg); break;
		case 's':	sys = optarg; break;
		case 'o':	option = atoi(optarg); break;
		case '%':	percent = 1; break;
		default:	goto usage;
		}
	if((optind >= argc) || (optind+2 < argc)){
usage:
		fprintf(stderr, "usage: %s [-k err] [-q wid] [-e nex] [-p npp] [string] file\n", argv[0]);
		exit(1);
	}
	if((c = open(argv[argc-1], 0)) < 0){
		perror(argv[argc-1]);
		exit(1);
	}
	nbuf = read(c, buf+1, sizeof buf-1025);
	close(c);
	if(optind+2 == argc){
		texec = strlen(argv[optind]);
		strcpy(wbuf, argv[optind]);
		wbuf[texec+1] = 0;
	} else {
		s = wbuf;
		while((c = getchar()) != EOF)
			*s++ = (c=='\n')? 0:c;
		*s = 0;
	}
	tprep = texec = 0;
	nf = nw = 0;
	for(s = wbuf; *s;){
		nw++;
		/* convert # to \n so we can do multiword matches */
		ss = s;
		while(*s){
			if(*s == '#')
				*s = '\n';
			s++;
		}
		len = s-ss;
		s++;
		startclock();
		for(c = 0; c < npreproc; c++)
			prep(ss, len, kval, qval);
		tprep += stopclock();
		startclock();
		for(c = 0; c < nexec; c++){
			if(cnt = exec(buf+1, nbuf, kval, qval))
				nmatched++;
			nf += cnt;
		}
		texec += (t = stopclock());
		if(tmp < &tm[NTIME])
			*tmp++ = t;
	}
	printf("alg=%s np=%ld match=%ld p=%.4fs(%ld/%d) e=%.4fs(%ld/%d)\n",
		argv[0], nw, nf/nexec,
		(tprep*1.0)/(hz*npreproc), tprep, npreproc,
		(texec*1.0)/(hz*nexec), texec, nexec);
	exit(0);
}
