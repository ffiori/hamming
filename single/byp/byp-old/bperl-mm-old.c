/* BPERL (Navarro)
 * byn/bperl/search.c
 * The original idea is from Baeza-Yates and Perleberg.
 *
 * It is a particular case of pattern partitioning, where j = k+1, and hence
 * the search is made with 0 errors. We use a boyer-moore-horspool-sunday 
 * algorithm extended tu multipattern searching.
 */




	/* Codigo a agregar para contar los matches */

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "bperl-mm-old.h"

#define safe_free(x) { if((x)!=NULL) free(x); }
#define MAXPATTERN 200
#define CHARTYPE unsigned char

static CHARTYPE pattern[MAXPATTERN];
static unsigned int m;

int GZL_SIGMA =256;

#ifndef ENGLISH

#define GZL_chartoint(c) (((c) <= '9') ? ((c)-'/') : \
			 (((c) <= 'Z') ? ((c)+11-'@') :  \
			 (((c) <= 'z') ? ((c)+38-'a') : \
				         ((c)+64-161))))
#define GZL_inttochar(i) (((i) < 11) ? ((i)+'/') : \
			 (((i) < 38) ? ((i)-11+'@') :  \
			 (((i) < 64) ? ((i)-38+'a') : \
			               ((i)-64+161))))
#else

#define GZL_chartoint(c) ((c)=='\n') ? 0 : (((c)=='_') ? 1 : (((c) <= '9') ? ((c)-'0'+2) : ((c)-'a'+11)))
#define GZL_inttochar(i) ((i)==0) ? '\n' : (((i)==1) ? '_' : (((i) <= 11) ? ((i)-2+'0') : ((i)-11+'a')))

#endif

int nmatch;

#define MATCH(i) \
  { nmatch++; \
    /* printf ("Match at %i\n",i); */  \
  }

#define END_MATCH \
    return (nmatch);
//  printf ("Total: %i\n",COUNT);



#define fatal_error(msg) \
   { printf (msg); \
     exit(1); \
   }

#define MAIN(progname,invoc) \
main (int argc, char **argv) \
 \
   { struct stat sdata; \
     FILE *f; \
     int n,m,k; \
     char *pat,*text; \
 \
     if (argc < 5) \
        fatal_error ("Usage: " progname " <pat> <k> <file> <sigma>\n"); \
     pat = argv[1]; \
     m = strlen(pat); \
     k = atoi(argv[2]); \
     GZL_SIGMA = atoi(argv[4]); \
     if ((k < 0) || (k >= m))  \
	fatal_error ("Debe ser 0 <= k < m\n"); \
     if (stat(argv[3],&sdata) != 0) \
        fatal_error ("No pude stat archivo\n"); \
     n = sdata.st_size; \
     text = (char*) malloc (n+m+1); \
     if (text == NULL) \
        fatal_error ("No pude alocar memoria\n"); \
     f = fopen (argv[3],"r"); \
     if (fread (text,n,1,f) != 1) \
        fatal_error ("No pude leer file\n"); \
     fclose(f); \
     invoc (m,n,pat,text,k); \
     END_MATCH; \
   } 


/*bmhs.c*/
	/* prepares to search for pattern[0..m-1] */

void prepBmhs (Bmhs *B, uchar *pattern, int m, int k)

   { int j,c; 
     B->d = (int*)malloc (Sigma*sizeof(int));
     for(c=0; c<Sigma; c++) B->d[c] = m+1;
     for(j=0; j<m; j++) B->d[pattern[j]] = m-j;
     B->pat = pattern;
     B->m = m;
   }

void initBmhs (Bmhs *B)

   {
   }

	/* frees data */

void freeBmhs (Bmhs *B)

   { free (B->d); B->d = NULL;
   }
     
	/* searches in text+n */

void searchBmhs (Bmhs *B, register uchar *text, register int n, 
		 Tcode code, void *ptr)

   { bool ret;
     runBmhs ((*B),text,n,ret = code (n,ptr),ret);
   }


/*partexact.c*/
void prepPartExact (PartExact *P, uchar *pattern, int m, int k)

   {
     freePartExact (P);
	/* cut into subpatterns */
     makeCut (pattern,m,k,k+1,&P->cut);

	/* prepare for multipattern search */
     prepMulti (&P->M,P->cut.ps,k+1);
     
	/* prepare for dynamic programming */
     prepVer (&P->dyn,pattern,m,k);

	/* precompute some values */
     P->back = m-P->cut.len[k];
     P->forth = m+1;
   }

void initPartExact (PartExact *P)

   { initMulti (&P->M);
   }

int searchPartExact (PartExact *P, register uchar *text, register int n, 
		     Tcode code, void *ptr)

   { bool ret;
     runPartExact ((*P),text,n,ret = code (n,ptr),ret);
   }

void freePartExact (PartExact *P)

   { freeVer (&P->dyn);
     freeMulti (&P->M);
     freeCut (&P->cut);
   }


/*makeg.c*/

/*
 * Construccion de la funcion GoTo.
 */

/* typedef int (*GAutom)[Sigma]; */ /* en el char 0 guarda el Out */

int newstate;

void	Enter( GAutom G, uchar *pat, int num )
   {
	int	state = 0,
		j;
	/*
	 * Avanzamos en el automata segun el patron.
	 */

	for ( j = 0; pat[j] && (G[state][pat[j]] != -1); j++ )
	     state = G[state][pat[j]];

	/*
	 * Insertamos solo si no existia este patron.
	 */

	while (pat[j])
	   {
	     G[state][pat[j++]] = ++newstate;
	     state = newstate;
	     /* this is just a bug G[state][0] = -1; */
	   }
	G[state][0] = num;
   }

GAutom	MakeG(uchar **Ps, int jj, int m)
   {
	int	i, j;
        GAutom G = (GAutom) malloc ((m+1)*sizeof(*G));

        newstate = 0;  /* here, to allow reuse! */

	for ( i = 0; i <= m; i++ )
	   { G[i][0] = -1;
	     for ( j = 1; j < Sigma; j++ )
		  G[i][j] = -1;
	   }

	for ( i = 0; i < jj; i++ )
	     Enter(G,Ps[i],i);

	/* ommited, since we use it as a trie
	for ( i = 1; i < Sigma; i++ )
	     if (G[0][i] == -1) G[0][i] = 0;
	*/

	return G;
   }





/*multipat.c*/
	/* prepare to search patterns Ps[0]..Ps[r-1] */

void prepMulti (Multi *M, uchar **Ps, int r)

  { int c,i,j,totlen;

	/* compute the total and minimum length */
    M->minlen = totlen = strlen(Ps[0]);
    for (i=1;i<r;i++)
	{ int l = strlen(Ps[i]);
	  totlen += l;
	  if (l < M->minlen) M->minlen = l;
	}
    
	/* define the pessimistic d table */
    M->d = (int*) malloc (Sigma*sizeof(int));
    for (c=0;c<Sigma;c++) M->d[c] = M->minlen+1;
    for (i=0;i<r;i++)
      { for(j=0;j<M->minlen;j++) 
	  if (M->minlen-j < M->d[Ps[i][j]]) M->d[Ps[i][j]] = M->minlen-j;
      }

	/* compute the trie */
    M->G = MakeG (Ps,r,totlen);

  }

void initMulti (Multi *M)

  {
  }

	/* free data */

void freeMulti (Multi *M)

   { free (M->d); M->d = NULL;
     free (M->G); M->G = NULL;
   }

	/* searches M from text+n, returning where was it found (next position)
	   (the pattern must be in the text)
	*/

void searchMulti (Multi *M, register uchar *text, register int n, 
		  Tcode code, void *ptr)

   { bool ret;
     runMulti ((*M),text,n,ret = code(n,ptr),ret);
   }


/*cut.c*/
void makeCut (uchar *pattern, int m, int k, int jj, Cut *cut)

   { int i;
     int mdj,msj,mrj;

     cut->j = jj; cut->k = k/jj;
     mdj = m/jj; msj = (m+jj-1)/jj; mrj = m-jj*mdj;

          /* Partition in subpatterns */
     cut->aclen = ((int *) malloc ((1+jj)*sizeof(int)))+1;
     cut->len = (int*) malloc (jj*sizeof(int));
     cut->ps = (uchar**) malloc (jj*sizeof(uchar*));
     for (i=0;i<mrj;i++) cut->len[i] = msj;
     for (i=mrj;i<jj;i++) cut->len[i] = mdj;
     cut->aclen[-1] = 0;
     for (i=0;i<jj;i++)
         { cut->aclen[i] = cut->aclen[i-1]+cut->len[i];
           cut->ps[i] = (uchar*) malloc (cut->len[i]+1);
           strncpy (cut->ps[i],pattern+cut->aclen[i-1],cut->len[i]);
           cut->ps[i][cut->len[i]] = 0;
         }
   }

void freeCut (Cut *cut)

   { 
     if(cut==NULL) return;
     int i;
     for (i=0; i<cut->j; i++)
        safe_free (cut->ps[i]);
     if(cut->aclen!=NULL) safe_free (cut->aclen-1);
     cut->aclen = NULL;
     safe_free (cut->len); cut->len = NULL;
     safe_free (cut->ps); cut->ps = NULL;
   }


/*dynprog.c*/
	/* prepares to search pattern[0..m-1] with k errors */

void prepVer (VerData *D, uchar *pattern, int m, int k)

   { int j;
     D->m = m; D->k = k;
     D->pat = pattern;
     D->lastpos = -1;

	/* since the first n to searchVer may be negative,
	   initialize here anyway */
     initVer (D);
   }

	/* prepare for a fresh search */

void initVer (VerData *D)

   {
   }

	/* frees the data */

void freeVer (VerData *D)

   {
   }


	/* searches n..fin and executes code on matches */

void searchVerV (VerData *D, register uchar *text, register int n, 
		 register int fin, Tcode code, void *ptr)

   { bool ret;
     runVerV((*D),text,n,fin,ret = code(n,ptr),ret);
   }

	/* searches n... and executes code on matches */

void searchVer (VerData *D, register uchar *text, register int n, 
		Tcode code, void *ptr)

   { bool ret;
     runVer((*D),text,n,ret = code(n,ptr),ret);
   }







/* original search.c begins here */


void ExactSearch (register uchar *text, register int n, 
		 uchar *pat, int m)


   { register int i;
     register bool ret = false;
     Bmhs A;

     i = 0;
     prepBmhs (&A,pat,m,0);
     initBmhs (&A);

     runBmhs (A,text,i,
       { if (i > n) ret = true;
	 else nmatch++;
       } ,ret);

     freeBmhs (&A);
   }

PartExact A;
void PartExactSearch (register uchar *text, register int n, 
		     uchar *pat, int m, int k)

   { register int i;
     register bool ret = false;
     //PartExact A;

     i = 0;
     nmatch = 0;
     //prepPartExact (&A,pat,m,k);
     //initPartExact (&A);

     runPartExact (A,text,i,
       { if (i >= n) ret = true;
	 else nmatch++;
       } ,ret);


     //freePartExact (&A);
   }

void prep(CHARTYPE *p, unsigned mval, unsigned kval, unsigned qval) {
    m = mval;
    memcpy(pattern, p, m);
    prepPartExact (&A,p,m,kval);
    initPartExact (&A);
}

//void partexactheur (int m, int n, uchar *pattern, uchar *text, int k)
int exec(CHARTYPE *text, unsigned n, unsigned k, unsigned q) {
    A.dyn.lastpos = 0;
    strcpy (text+n,pattern);
    text[n+m] = 1;
    
    /* if k = 0 then exact search */
    if (k == 0)
	{ ExactSearch (text,n,pattern,m); return; }
    
    /* else, partition into exact match */
    PartExactSearch (text,n,pattern,m,k);
    
    
    return nmatch;
}

//MAIN("partexact",partexactheur);
