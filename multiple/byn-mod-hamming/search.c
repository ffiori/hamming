/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/

#include "partexact.h"
#include "countcode.c"
//~ #include "main.c"

extern uchar Map[256];

//~ void partexactheur (int r, int m, int n, uchar ** pattern, register uchar * text, int k, int c)
//~ {
    //~ static PartExact P[10];
    //~ int i, nn = n;
    //~ static bool first = true;
    //~ static int rp, ii;

    //~ if (first) {
        //~ for (i = 0; i < 256; i++)
            //~ Map[i] = i;
        //~ rp = r;
        //~ if (c == 32) {
            //~ if (m == 30) {
                //~ if (k == 4)
                    //~ rp = 28;
                //~ else if (k == 8)
                    //~ rp = 20;
            //~ }
        //~ }
        //~ else if (c == 13) {
            //~ if (m == 30) {
                //~ if (k == 4)
                    //~ rp = 22;
                //~ else if (k == 8)
                    //~ rp = 16;
            //~ }
        //~ }
        //~ i = 0;
        //~ while (i + 2 * rp <= r) {
            //~ prepPartExact (P + i / rp, pattern + i, rp, m, k);
            //~ i += rp;
        //~ }
        //~ ii = i / rp;
        //~ prepPartExact (P + ii, pattern + i, r - i, m, k);
        //~ first = false;
    //~ }

    //~ for (i = 0; i < r; i++) {
        //~ strcpy (text + nn, pattern[i]);
        //~ nn += strlen (pattern[i]);
    //~ }
    //~ text[nn] = 1;
    
    //~ //queda text = <texto>\n<pat0><pat1>...<patr-1>1

    //~ for (i = 0; i <= ii; i++) {
        //~ initPartExact (P + i);
        //~ COUNT = searchPartExact (P + i, text, 0, n, NULL);      //entra acá "partexact.c"
    //~ }
    //~ /* freePartExact (&P); */
//~ }

//~ MAIN ("partexact", partexactheur);      //llama a partexactheur



PartExact P[10];
int ii;

void preppartexactheur (int r, int m, uchar ** pattern, int k, int c)
{
    //~ static PartExact P[10];
    //~ int i, nn = n;
    int i;
    //~ static bool first = true;
    //~ static int rp, ii;
    static int rp;

    //~ if (first) {
    
	for (i = 0; i < 256; i++)
		Map[i] = i;
	rp = r;
	if (c == 32) {
		if (m == 30) {
			if (k == 4)
				rp = 28;
			else if (k == 8)
				rp = 20;
		}
	}
	else if (c == 13) {
		if (m == 30) {
			if (k == 4)
				rp = 22;
			else if (k == 8)
				rp = 16;
		}
	}
	i = 0;
	while (i + 2 * rp <= r) {
		prepPartExact (P + i / rp, pattern + i, rp, m, k);
		i += rp;
	}
	ii = i / rp;
	prepPartExact (P + ii, pattern + i, r - i, m, k);
    
        //~ first = false;
    //~ }

    //~ for (i = 0; i < r; i++) {
        //~ strcpy (text + nn, pattern[i]);
        //~ nn += strlen (pattern[i]);
    //~ }
    //~ text[nn] = 1;

    //queda text = <texto>\n<pat0><pat1>...<patr-1>1

    //~ for (i = 0; i <= ii; i++) {
        //~ initPartExact (P + i);
        //~ COUNT = searchPartExact (P + i, text, 0, n, NULL);      //entra acá "partexact.c"
    //~ }
    /* freePartExact (&P); */
}

void prep(char *ps, int plen, int k, int q)
{
	freePartExact (&P); // to make multiple runs
	
    //~ struct stat sdata;
    //~ FILE *f;
    int j, m;
    uchar **pat, *pats;

    //~ if (argc < 5) fatal_error ("Usage: <patfile> <k> <file> <sigma>\n");
    //~ if (argc > 5) if (!strcmp (argv[5], "-p")) PRINT = 1;
    
    j = readpats (plen, ps, &pat, &pats);
    m = strlen (pats);
    
    //~ k = atoi (argv[2]);
    //~ c = atoi (argv[4]);
    
    if ((k < 0) || (k >= m - 1)) fatal_error ("Debe ser 0 <= k < m-1\n");
    //~ if (stat (argv[3], &sdata) != 0) fatal_error ("No pude stat archivo\n");
    
    //~ n = sdata.st_size;
    //~ text = (char *) malloc (TBLK + j * m + 1);
    //~ if (text == NULL) fatal_error ("No pude alocar memoria o no hay texto\n");
    //~ f = fopen (argv[3], "r");
    //~ while (n > 0) {
        //~ int ln = TBLK;
        //~ if (ln > n) ln = n;
        //~ if (fread (text, ln, 1, f) != 1) fatal_error ("No pude leer file\n");
        //~ partexactheur(j, m, ln, pat, text, k, c);
        //~ n -= ln;
    //~ }
    //~ fclose (f);
    //~ END_MATCH;
    
    preppartexactheur(j, m, pat, k, Sigma);
}

int exec (uchar * text, int n, int k, int q)
{
    //~ static PartExact P[10];
    //~ int i, nn = n;
    //~ static bool first = true;
    //~ static int rp, ii;

    //~ if (first) {
        //~ for (i = 0; i < 256; i++)
            //~ Map[i] = i;
        //~ rp = r;
        //~ if (c == 32) {
            //~ if (m == 30) {
                //~ if (k == 4)
                    //~ rp = 28;
                //~ else if (k == 8)
                    //~ rp = 20;
            //~ }
        //~ }
        //~ else if (c == 13) {
            //~ if (m == 30) {
                //~ if (k == 4)
                    //~ rp = 22;
                //~ else if (k == 8)
                    //~ rp = 16;
            //~ }
        //~ }
        //~ i = 0;
        //~ while (i + 2 * rp <= r) {
            //~ prepPartExact (P + i / rp, pattern + i, rp, m, k);
            //~ i += rp;
        //~ }
        //~ ii = i / rp;
        //~ prepPartExact (P + ii, pattern + i, r - i, m, k);
        //~ first = false;
    //~ }

    //~ for (i = 0; i < r; i++) {
        //~ strcpy (text + nn, pattern[i]);
        //~ nn += strlen (pattern[i]);
    //~ }
    //~ text[nn] = 1;
    
    //queda text = <texto>\n<pat0><pat1>...<patr-1>1
    int i;
    COUNT = 0;

    for (i = 0; i <= ii; i++) {
        initPartExact (P + i);
        COUNT += searchPartExact (P + i, text, 0, n, NULL);      //entra acá "partexact.c". += by Fernando.
    }
    /* freePartExact (&P); */
    
    return COUNT;
}
