#include	<stdio.h>
#define		NTIME		1000

char buf[105001025]; /* 1 001 025 */
char   wbuf[640200];   /* 101 000 */
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
	int npreproc = 1;
	int nexec = 1;
	long tp, te, t;
	long *tmp = tm;
	int nmatched, len;
	long nf, nw;
	long nstep, tstep;
	double var, mean;
	char *s, *ss, *sys = "?";
	int percent = 0;
	extern char *optarg;
	extern int optind;

	while((c = getopt(argc, argv, "%e:p:s:o:")) != -1)
		switch(c)
		{
		case 'p':	npreproc = atoi(optarg); break;
		case 'e':	nexec = atoi(optarg); break;
		case 's':	sys = optarg; break;
		case 'o':	option = atoi(optarg); break;
		case '%':	percent = 1; break;
		default:	goto usage;
		}
	if((optind >= argc) || (optind+2 < argc)){
usage:
		fprintf(stderr, "usage: %s [-e nexec] [-p npreproc] [string] file\n", argv[0]);
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
	var = 0;
#ifdef	STATS
	for(c = 0; c < NSTEP; c++)
		stats.step[c] = 0;
	stats.jump = 0;
	stats.cmp = 0;
	stats.slow = 0;
	stats.extra = 0;
	nmatched = 0;
#endif
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
			prep(ss, len);
		tp += stopclock();
		startclock();
		for(c = 0; c < nexec; c++){
			if(cnt = exec(buf+1, nbuf))
				nmatched++;
			nf += cnt;
		}
		te += (t = stopclock());
		if(tmp < &tm[NTIME])
			*tmp++ = t;
		var += t*1.0*t;
	}
#ifdef	STATS
	for(nstep = tstep = 0, c = 0; c < NSTEP; c++)
		if(stats.step[c]){
/*			printf("n(%d)=%d\n", c, stats.step[c]);/**/
			nstep += stats.step[c];
			tstep += stats.step[c]*c;
		}
	printf("sys=%s alg=%s match=%d ninput=%d step=%.2f cmp+jump=%.1f(%.1f%%)(%.1f+%.1f) slow=%.1f nmatched=%d extra=%.2f\n", sys, argv[0], nf, nbuf,
		tstep/(nstep*(float)nexec),
		1.*(stats.cmp+stats.jump)/nw, (stats.cmp+stats.jump)*100./(nbuf*nw),
		1.*stats.cmp/nw, 1.0*stats.jump/nw, 1.0*stats.slow/nw, nmatched,
		1.0*stats.extra/nw);
#else
	printf("sys=%s alg=%s nw=%d match=%d p=%.4fs(%d/%d) e=%.4fs(%d/%d)\n",
		sys, argv[0], nw, nf/nexec,
		(tp*1.0)/(hz*npreproc), tp, npreproc,
		(te*1.0)/(hz*nexec), te, nexec);
	if(percent){
		mean = te/nw;
		printf("%% %.8e", (var/nw-mean*mean)/(hz*hz*nexec));
		qsort(tm, nw, sizeof(tm[0]), cmp);
		tm[nw] = tm[nw-1];
		for(c = 0; c <= 20; c++)
			printf(" %.4e", (1.0*tm[(c*nw)/20])/(hz*nexec));
		printf("\n");
	}
#endif
	exit(0);
}
