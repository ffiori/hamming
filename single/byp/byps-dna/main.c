#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define		NTIME		1000

//char buf[2001000];
char buf[8*1123456]; //~8MB
char wbuf[33*112345]; //~10^5 patterns of length <=32
long tm[NTIME+1];
int nbuf;
extern int hz;
int option = -1;

cmp(a, b)
	long *a, *b;
{
	if(*a < *b)
		return(-1);
	else
		return((*a > *b)? 1:0);
}

main(argc, argv)
	char **argv;
{
	int c, cnt;
	int kval=0;
	int qval=1;
	int npreproc = 1;
	int nexec = 1;
	long tp, te, t;
	long *tmp = tm;
	int nmatched, len;
	long nf, nw;
	char *s, *ss, *sys = "?";
	int percent = 0;
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
		te = strlen(argv[optind]);
		strcpy(wbuf, argv[optind]);
		wbuf[te+1] = 0;
	} else {
		s = wbuf;
		while((c = getchar()) != EOF)
			*s++ = (c=='\n')? 0:c;
		*s = 0;
	}
	tp = te = 0;
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
		for(c = 0; c < npreproc; c++) {
                    prep(ss, len, kval, qval);
                }
		tp += stopclock();
		startclock();
		for(c = 0; c < nexec; c++){
                    int i;
                    for(i=0; i<len; ++i) buf[1+nbuf+i] = 1;
                    if(cnt = exec(buf+1, nbuf, kval, qval))
                        nmatched++;
                    nf += cnt;
		}
		te += (t = stopclock());               
		if(tmp < &tm[NTIME])
                    *tmp++ = t;                
	}
	printf("alg=%-15s np=%-5d match=%-10d p=%.5fs e=%.5fs total=%.5fs\n",
		argv[0], nw, nf/nexec,
		(tp*1.0)/(hz*npreproc),
		(te*1.0)/(hz*nexec),
		(tp*1.0)/(hz*npreproc)+(te*1.0)/(hz*nexec));
	exit(0);
}
