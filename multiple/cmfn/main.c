#include	<fcntl.h>
#ifndef  sun
#include	<getopt.h>
#endif
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<unistd.h>

#define		NTIME		1000
#define MAXPATS 112345

char *pats[MAXPATS];

char buf[209715200];
char wbuf[101000];
long tm[NTIME+1];
int nbuf;
extern int hz;
int option = -1;

void startclock();
int stopclock();
void prep(char **pattern, unsigned m, unsigned kval, unsigned gval, unsigned cval);
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
int np;
int npreproc = 1;
int nexec = 1;
int groupsval=1;
int chargval=0;
int main(int argc, char *argv[])
{
	int c, cnt;
	int kval=0;
	int qval=1;
	long *tmp = tm;
	int nmatched, len, totlen;
	long nf, nw;
	char *s, *ss;
    long t;
	extern char *optarg;
	extern int optind;

	while((c = getopt(argc, argv, "k:q:g:c:e:o:")) != -1)
		switch(c)
		{
		case 'k':	kval = atoi(optarg); break;
		case 'q':	qval = atoi(optarg); break;
		case 'g':   groupsval = atoi(optarg); break;
		case 'c':   chargval = atoi(optarg); break;
		case 'e':	nexec = atoi(optarg); break;
		case 'o':	option = atoi(optarg); break;
		default:	goto usage;
		}
		
	np = nexec; //written by Fernando because it makes same number of runs for preprocessing and search.
		
	if((optind >= argc) || (optind+2 < argc)){
usage:
		fprintf(stderr, "usage: %s [-k err] [-e nex] [-g groups of patts] [-c groups of chars] [string] file\n", argv[0]);
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
			*s++ = (c=='\n') ? 0 : c;
		totlen = s - wbuf; //total length of patterns file
		*s = 0;
	}
	tprep = texec = 0;
	nf = nw = 0;
	
	int Plen=0;
	for(s = wbuf; *s!=0; ++s) ++Plen;
	nw = (totlen+1) / (Plen+1); //len+1 in case last pattern finishes in '\0' instead of '\n'
		
	for(s = wbuf; *s;){
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

	    int patnow=0;
	    for(; patnow<nw && patnow<MAXPATS; patnow++) pats[patnow] = ss + patnow*(Plen+1);
		
		//for(patnow=0; patnow < nw; patnow++) printf("%d:_%s_\n",patnow,pats[patnow]);
		
		startclock();
		for(c = 0; c < np; c++)
			//prep(ss, Plen, kval, qval);
			prep(pats, nw, kval, groupsval, chargval); //I use nw to let the algorithm know the number of patterns, given that it doesn't use q-grams. Groups indicates the size of the groups of patterns to analyse together.
		tprep += stopclock();
		
		//startclock();
		nf = exec(buf+1, nbuf, kval, nexec);
		/*
		for(c = 0; c < nexec; c++){
			if((cnt = exec(buf+1, nbuf, kval, nexec))) //I use nexec to let the algorithm know how many runs I want to make.
				nmatched++;
			nf += cnt;
		}
		*/
		//texec += (t = stopclock());
		//if(tmp < &tm[NTIME]) *tmp++ = t;
		
		break;
	}
	long tp=tprep, te=texec;
	npreproc = np;
	if(strlen(argv[0]) > 20) argv[0] = argv[0] + strlen(argv[0]) - 19;
	printf("alg=%-20s np=%-5d match=%-10d p=%.5fs e=%.5fs total=%.5fs\n",
		argv[0], nw, nf/nexec,
		(tp*1.0)/(hz*npreproc),
		(te*1.0)/(hz*nexec),
		(tp*1.0)/(hz*npreproc)+(texec*1.0)/(hz*nexec));
	exit(0);
}
