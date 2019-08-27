#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#define		NTIME		1000

#define MAXT 6123456
#define MAXP (33*112345)
#define ROOM 65536

#define buf (buf_arr+ROOM)
#define wbuf (wbuf_arr+ROOM)

char buf_arr[MAXT];
char wbuf_arr[MAXP];
//char *buf,*wbuf;

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
    //buf = buf+ROOM;
    //wbuf = wbuf+ROOM;

	int c, cnt;
	int kval=0;
	int qval=1;
	int npreproc = 1;
	int nexec = 1;
	long tp, te, t;
	long *tmp = tm;
	int nmatched, len, totlen;
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
	nbuf = read(c, buf+1, MAXT-ROOM-1025);
	close(c);
	if(optind+2 == argc){
		te = strlen(argv[optind]);
		strcpy(wbuf, argv[optind]);
		wbuf[te+1] = 0;
	} else {
		s = wbuf;
		
		while((c = getchar()) != EOF)
			*s++ = (c=='\n') ? 0 : c;
		totlen = s - wbuf; //total length of patterns file
		*s = 0;
	}
	tp = te = 0;
	nf = nw = 0;
	int Plen=0;
	for(s = wbuf; *s!=0; ++s) ++Plen;
	nw = (totlen+1) / (Plen+1); //len+1 in case last pattern finishes in '\0' instead of '\n'
	for(s = wbuf; *s;){ //executes preprocessing and search for each pattern.
		//nw++;
		/* convert # to \n so we can do multiword matches */
		ss = s;
		while(*s){
			if(*s == '#')
				*s = '\n';
			s++;
		}
		len = s-ss;
		s++;
		
		//preprocess
		startclock();
		for(c = 0; c < npreproc; c++)
			prep(ss, totlen, kval, qval);
			//prep(ss, len, kval, qval); //changed by Fernando
		tp += stopclock();

		//search
		startclock();
		for(c = 0; c < nexec; c++){
			if(cnt = exec(buf+1, nbuf, kval, qval))
				nmatched++;
			nf += cnt;
		}
		te += (t = stopclock());

		if(tmp < &tm[NTIME])
			*tmp++ = t;

		break; //written by Fernando because I want to test multiple pattern matching.
	}
	//printf("alg=%s\tnp=%d\tmatch=%d\tp=%.4fs(%d/%d)\te=%.4fs(%d/%d)\n",
	if(strlen(argv[0]) > 20) argv[0] = argv[0] + strlen(argv[0]) - 19;
	if(nexec==0) nexec=1;
	printf("alg=%-20s np=%-5d match=%-10d p=%.5fs e=%.5fs total=%.5fs\n",
		argv[0], nw, nf/nexec,
		(tp*1.0)/(hz*npreproc),
		(te*1.0)/(hz*nexec),
		(tp*1.0)/(hz*npreproc)+(te*1.0)/(hz*nexec));
	exit(0);
}
