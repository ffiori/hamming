
        /* Codigo a agregar para contar los matches */

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

int GZL_SIGMA;

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

#define GZL_chartoint(c) ((c)=='\n') ? 0 : (((c)=='_') ? 1 : (((c) <= '9') ? ((c
) - '0' + 2):((c) - 'a' + 11)))
#define GZL_inttochar(i) ((i)==0) ? '\n' : (((i)==1) ? '_' : (((i) <= 11) ? ((i)
- 2 + '0'):((i) - 11 + 'a')))
#endif

int COUNT = 0;

#define MATCH(i) \
  { COUNT++; \
    printf ("Match at %i: %.10s\n",i,text+i-10);  \
  }

int PRINT = 1;

#define END_MATCH \
  if (PRINT) printf ("Total: %i\n",COUNT);

#define fatal_error(msg) \
   { printf (msg); \
     exit(1); \
   }

#if 0
int readpats (uchar * fname, uchar *** pat, uchar ** pats)
{
	FILE *f;
	struct stat sdata;
	int num, l, k, j;

	if (stat (fname, &sdata) != 0)
		fatal_error ("No pude stat archivo patrones\n");
	*pats = (uchar *) malloc (sdata.st_size + 2);
	if (*pats == NULL)
		fatal_error ("No pude alocar memoria\n");
	f = fopen (fname, "r");
	if (fread (*pats, sdata.st_size, 1, f) != 1)
		fatal_error ("No pude leer file de patrones\n");
	fclose (f);
	l = sdata.st_size;
	if ((*pats)[l - 1] != '\n')
		(*pats)[l++] = '\n';
	num = 0;
	for (k = 0; k < l; k++)
		if ((*pats)[k] == '\n') {
			(*pats)[k] = 0;
			num++;
		}
	*pat = (uchar **) malloc (num * sizeof (char *));
	if (*pat == NULL)
		fatal_error ("No pude alocar memoria\n");
	(*pat)[0] = *pats;
	j = 1;
	for (k = 0; j < num; k++)
		if (!(*pats)[k])
			(*pat)[j++] = (*pats) + k + 1;
	return num;
}
#endif

int readpats (int plen, uchar * pstring, uchar *** pat, uchar ** pats)
{
	//~ FILE *f;
	//~ struct stat sdata;
	int num, l, k, j;

	//~ if (stat (fname, &sdata) != 0) fatal_error ("No pude stat archivo patrones\n");
	*pats = (uchar *) malloc (plen + 2);
	//~ if (*pats == NULL) fatal_error ("No pude alocar memoria\n");
	//~ f = fopen (fname, "r");
	//~ if (fread (*pats, sdata.st_size, 1, f) != 1) fatal_error ("No pude leer file de patrones\n");
	//~ fclose (f);
	*pats = pstring;
	l = plen;
	
	if ((*pats)[l - 1] != '\n')
		(*pats)[l++] = '\n';
	num = 0;
	for (k = 0; k < l; k++)
		if ((*pats)[k] == '\n') {
			(*pats)[k] = 0;
			num++;
		}
	*pat = (uchar **) malloc (num * sizeof (char *));
	if (*pat == NULL) fatal_error ("No pude alocar memoria\n");
	(*pat)[0] = *pats;
	j = 1;
	for (k = 0; j < num; k++)
		if (!(*pats)[k])
			(*pat)[j++] = (*pats) + k + 1;
	return num;
}



#define TBLK 8123456
//now ~8MB, before was (128*1024)

#define MAIN(progname,invoc) \
main (int argc, char **argv) \
 \
   { struct stat sdata; \
     FILE *f; \
     int j,n,m,k,c; \
     uchar **pat,*pats,*text; \
 \
     if (argc < 5) \
        fatal_error ("Usage: " progname " <patfile> <k> <file> <sigma>\n"); \
if (argc>5) if (!strcmp(argv[5],"-p")) PRINT=1; \
     j = readpats (argv[1],&pat,&pats); \
     m = strlen(pats); \
     k = atoi(argv[2]); \
     c = atoi(argv[4]); \
     if ((k < 0) || (k >= m-1))  \
	fatal_error ("Debe ser 0 <= k < m-1\n"); \
     if (stat(argv[3],&sdata) != 0) \
        fatal_error ("No pude stat archivo\n"); \
     n = sdata.st_size; \
     text = (char*) malloc (TBLK+j*m+1); \
     if (text == NULL) \
        fatal_error ("No pude alocar memoria\n"); \
     f = fopen (argv[3],"r"); \
     while (n > 0) \
	{ int ln = TBLK; \
	  if (ln > n) ln = n; \
	  if (fread (text,ln,1,f) != 1) \
             fatal_error ("No pude leer file\n"); \
          invoc (j,m,ln,pat,text,k,c); \
	  n -= ln; \
	} \
     fclose(f); \
     END_MATCH; \
   }


#if 0
main (int argc, char **argv)
{
    struct stat sdata;
    FILE *f;
    int j, n, m, k, c;
    uchar **pat, *pats, *text;

    if (argc < 5)
        fatal_error ("Usage: <patfile> <k> <file> <sigma>\n");
    if (argc > 5)
        if (!strcmp (argv[5], "-p"))
            PRINT = 1;
    j = readpats (argv[1], &pat, &pats);
    m = strlen (pats);
    k = atoi (argv[2]);
    c = atoi (argv[4]);
    
    if ((k < 0) || (k >= m - 1)) fatal_error ("Debe ser 0 <= k < m-1\n");
    //~ if (stat (argv[3], &sdata) != 0) fatal_error ("No pude stat archivo\n");
    
    n = sdata.st_size;
    text = (char *) malloc (TBLK + j * m + 1);
    if (text == NULL)
        fatal_error ("No pude alocar memoria\n");
    f = fopen (argv[3], "r");
    while (n > 0) {
        int ln = TBLK;

        if (ln > n)
            ln = n;
        if (fread (text, ln, 1, f) != 1)
            fatal_error ("No pude leer file\n");
            
        partexactheur(j, m, ln, pat, text, k, c);
        
        n -= ln;
    }
    fclose (f);
    END_MATCH;
}

#endif
