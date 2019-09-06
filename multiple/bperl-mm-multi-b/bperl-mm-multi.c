/* BPERL (Navarro)
 * byn/bperl/search.c
 * The original idea is from Baeza-Yates and Perleberg.
 *
 * It is a particular case of pattern partitioning, where j = k+1, and hence
 * the search is made with 0 errors. We use a boyer-moore-horspool-sunday 
 * algorithm extended tu multipattern searching.
 */

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "bperl-mm-multi.h"
#include "ans2b.h"

#define CHARTYPE unsigned char

//#define MOD 0
//#define DNA 0 //changes MEPSM fingerprint, set to 1 if you're working with DNA

//static CHARTYPE pattern[MAXPATTERN];
CHARTYPE *pats;
int npats;
static unsigned int m;          //pattern length
PartExact *A;

int nmatch;

/*partexact.c*/
void prepPartExact (PartExact * P, uchar * pattern, int m, int k)
{
    freePartExact (P);
    int i;

    for (i = 0; i < npats; i++) {
        /* cut into subpatterns */
        makeCut (pattern + i * (m + 1), m, k, k + 1, &P[i].cut);

        /* prepare for multipattern search */
        //prepMulti(&P->M, P->cut.ps, k + 1);

//        printf("P[i].cut.ps[0] %s, P[i].cut.j %d, P[i].cut.len[0] %d, len[1] %d, aclen[0] %d, aclen[1] %d\n",P[i].cut.ps[0], P[i].cut.j, P[i].cut.len[0], P[i].cut.len[1], P[i].cut.aclen[0], P[i].cut.aclen[1]);
        int minlen = P[i].cut.len[0];
        int j;

        for (j = 1; j < P[i].cut.j; j++)
            minlen = min (minlen, P[i].cut.len[j]);

//	for(j=0; j<P[i].cut.j; j++) printf("pattern %s, subpattern %s\n",pattern+i*(m+1),P[i].cut.ps[j]);

        mepsm_prep (P[i].cut.ps, P[i].cut.j, minlen, i);
        /*
           typedef struct
           { int *aclen;                /* accum lengths of Ps[0..i] 
           int *len;                  /* length of Ps[i] 
           uchar **ps;                /* subpatterns
           int j;                     /* number of subpatterns 
           int k;                     /* k/j 
           } Cut;
         */

        /* prepare for dynamic programming */
        if (m > 32)
            prepVer (&P[i].dyn, pattern + i * (m + 1), m, k);

        /* precompute some values */
        //~ P[i].back = m - P[i].cut.len[k] + !MOD; //TODO check
        //~ _back = A[patnow].cut.aclen[list.subpatt - 1];
        
        P[i].forth = malloc(sizeof(int) * P[i].cut.j);
        P[i].back = malloc(sizeof(int) * P[i].cut.j);
        for (j = 0; j < P[i].cut.j; j++){
			P[i].forth[j] = m - P[i].cut.aclen[j-1];
            //~ P[i].back[j] = P[i].cut.aclen[j-1];
            P[i].back[j] = m - P[i].cut.len[k];
        }
    }

    if (m <= 32)
        ans_prep (pattern, m, k, npats);
}

void initPartExact (PartExact * P)
{
    initMulti (&P->M);
}

int searchPartExact (PartExact * P, register uchar * text, register int n, Tcode code, void *ptr)
{
    bool ret;
    runPartExact ((*P), text, n, ret = code (n, ptr), ret);
}

void freePartExact (PartExact * P)
{
	safe_free(P->forth);
	safe_free(P->back);
    freeVer (&P->dyn);
    //freeMulti(&P->M);
    freeCut (&P->cut);
    mepsm_init ();
}

/*makeg.c*/

/*
 * Construccion de la funcion GoTo.
 */

/* typedef int (*GAutom)[Sigma]; *//* en el char 0 guarda el Out */

int newstate;

void Enter (GAutom G, uchar * pat, int num)
{
    int state = 0, j;

    /*
     * Avanzamos en el automata segun el patron.
     */

    for (j = 0; pat[j] && (G[state][pat[j]] != -1); j++)
        state = G[state][pat[j]];

    /*
     * Insertamos solo si no existia este patron.
     */

    while (pat[j]) {
        G[state][pat[j++]] = ++newstate;
        state = newstate;
    }
    G[state][0] = num;
}

GAutom MakeG (uchar ** Ps, int jj, int m)
{
    int i, j;
    GAutom G = (GAutom) malloc ((m + 1) * sizeof (*G));

    newstate = 0;               /* here, to allow reuse! */

    for (i = 0; i <= m; i++) {
        G[i][0] = -1;
        for (j = 1; j < Sigma; j++)
            G[i][j] = -1;
    }

    for (i = 0; i < jj; i++)
        Enter (G, Ps[i], i);

    return G;
}

void initMulti (Multi * M)
{
}

void freeMulti (Multi * M)
{
    if (M == NULL)
        return;
    safe_free (M->d);
    M->d = NULL;
    safe_free (M->G);
    M->G = NULL;
}

/*cut.c*/
void makeCut (uchar * pattern, int m, int k, int jj, Cut * cut)
{
    int i;
    int mdj, msj, mrj;

    cut->j = jj;
    cut->k = k / jj;
    mdj = m / jj;
    msj = (m + jj - 1) / jj;
    mrj = m - jj * mdj;

    /* Partition in subpatterns */
    cut->aclen = ((int *) malloc ((1 + jj) * sizeof (int))) + 1;
    cut->len = (int *) malloc (jj * sizeof (int));
    cut->ps = (uchar **) malloc (jj * sizeof (uchar *));
    for (i = 0; i < mrj; i++)
        cut->len[i] = msj;
    for (i = mrj; i < jj; i++)
        cut->len[i] = mdj;
    cut->aclen[-1] = 0;
    for (i = 0; i < jj; i++) {
        cut->aclen[i] = cut->aclen[i - 1] + cut->len[i];
        cut->ps[i] = (uchar *) malloc (cut->len[i] + 1);
        strncpy (cut->ps[i], pattern + cut->aclen[i - 1], cut->len[i]);
        cut->ps[i][cut->len[i]] = 0;
    }
}

void freeCut (Cut * cut)
{
    if (cut == NULL)
        return;
    int i;

    for (i = 0; i < cut->j; i++)
        safe_free (cut->ps[i]);
    if (cut->aclen != NULL)
        safe_free (cut->aclen - 1);
    cut->aclen = NULL;
    safe_free (cut->len);
    cut->len = NULL;
    safe_free (cut->ps);
    cut->ps = NULL;
}

/*dynprog.c*/
        /* prepares to search pattern[0..m-1] with k errors */

void prepVer (VerData * D, uchar * pattern, int m, int k)
{
    int j;

    D->m = m;
    D->k = k;
    D->pat = pattern;
    D->lastpos = -1;

    /* since the first n to searchVer may be negative,
       initialize here anyway */
    initVer (D);                //does nothing ?
}

        /* prepare for a fresh search */

void initVer (VerData * D)
{
}

        /* frees the data */

void freeVer (VerData * D)
{
    return;
}

        /* searches n..fin and executes code on matches */

void searchVerV (VerData * D, register uchar * text, register int n, register int fin, Tcode code, void *ptr)
{
    bool ret;
    runVerV ((*D), text, n, fin, ret = code (n, ptr), ret);
}

        /* searches n... and executes code on matches */

void searchVer (VerData * D, register uchar * text, register int n, Tcode code, void *ptr)
{
    bool ret;
    runVer ((*D), text, n, ret = code (n, ptr), ret);
}

void PartExactSearch (register uchar * text, register int n, uchar * _, int m, int k)
{
    int i = 0;

    nmatch = 0;

    int _pe_n;
    int patnow;

int shift = mepsm_get_shift();

    for (patnow = 0; patnow < npats; patnow++)
        A[patnow].dyn.lastpos = -1;

    RetList list;

//~ int lastbegin = -1;

    if (m <= 32) {
        int(*ans_exec)(CHARTYPE *, int, int, int);
        if(m<=16){
            ans_exec = ans_exec_short;
        }
        else{
            ans_exec = ans_exec_long;
        }
        
        while (true) {
            list = mepsm_exec(text, n-1); 
            if(list.patt==-1) return;

            patnow=list.patt;

            _pe_n = list.pos;
            register int _back = m - A[patnow].cut.len[k] + shift; 
            register int _forth = m-A[patnow].cut.aclen[list.subpatt-1]; 
           
            int _pe_f = _pe_n + _forth;
            i = _pe_n - _back;
        
            register int begin, end;
            //must check between i and _pe_f
            begin = max(0, max(i,A[patnow].dyn.lastpos-m+1));
            end = min(n-1, _pe_f);

            if(end-begin+1 < m) continue;
            A[patnow].dyn.lastpos = end;
            nmatch += ans_exec(text+begin, end-begin+1, k, patnow);
            
        }
    }
    else { //dynamic Ukkonen algorithm (because ANS2 is not useful anymore).
        while (true) {
            /*
               _pe_n: indica el inicio de la ocurrencia de un subpatrón
               i: indica el inicio desde donde se debe empezar a chequear si está el patrón con k mismatches
               _pe_f = _pe_n + _forth
               _forth = m
               _back: cuánto tengo que retrodecer para empezar a verificar el patrón entero.
             */

            list = mepsm_exec (text, n - 1);
            if (list.patt == -1)
                return;

            patnow = list.patt;

            _pe_n = list.pos;
            register int _back = A[patnow].back;        // + shift; 
            //~ register int _forth = A[patnow].forth;
            register int _forth = A[patnow].forth[list.subpatt];

            int _pe_f = _pe_n + _forth;

            i = _pe_n - _back;

            //runVerV
            register bool _ret = false;
            register bool ret = false;

            register int _vmm;
            register int _vi;
            register int _vm = A[patnow].dyn.m;
            register char *_vpat = A[patnow].dyn.pat;
            register int _vk = A[patnow].dyn.k;

            if (i <= A[patnow].dyn.lastpos)
                i = A[patnow].dyn.lastpos + 1;
            while (i < _pe_f - _vm) {
                _vmm = 0;
                for (_vi = 0; _vi < _vm; _vi++) {
                    _vmm += (text[i + _vi] == _vpat[_vi] ? 0 : 1);
                    if (_vmm > _vk)
                        break;
                }

                if (_vmm <= _vk) {
                    if (i + m > n - 1)
                        ret = true;     //n-1 because of the final \n that Hume-Sunday puts
                    else {
                        nmatch++;
                        //printf("match at %d PartExactSearch, pat %d\n",i,patnow);
                    }
                    _ret = ret;
                    if (ret)
                        break;
                }
                A[patnow].dyn.lastpos = i;
                i++;
            }

            //free(list);
        }
    }
}

void prep (CHARTYPE * ps, unsigned len, unsigned kval, unsigned qval)
{
    //m = mval;
    //memcpy(pattern, p, m);
    pats = ps;
    for (m = 0; *ps != 0; ++ps)
        ++m;
    npats = (len + 1) / (m + 1);        //len+1 in case last pattern finishes in '\0' instead of '\n'

    safe_free (A);
    A = malloc (npats * sizeof (PartExact));
    prepPartExact (A, pats, m, kval);
//    initPartExact(A); //does nothing ?
}

//void partexactheur (int m, int n, uchar *pattern, uchar *text, int k)
int exec (CHARTYPE * text, unsigned n, unsigned k, unsigned q)
{
    mepsm_set_offset (-1);

    int i;

    /* if k = 0 then exact search */
    if (k == 0) {
        //ExactSearch(text, n, NULL, m);
        puts ("no exact search!");
        return;
    }

    /* else, partition into exact match */
    PartExactSearch (text, n, NULL, m, k);

    return nmatch;
}

//MAIN("partexact",partexactheur);
