
#ifndef BASICSINCLUDED
#define BASICSINCLUDED

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

typedef int bool;
#define true 1
#define false 0

typedef unsigned long mask;
typedef unsigned char uchar;

#define Sigma 256			/* alphabet size */
#define W (8*sizeof(mask))		/* word length */

typedef mask Tsection[Sigma];   	/* T for a single cell */

	/* code to execute when finding a match. pos it the position
	   FOLLOWING the last matching position */

typedef int (*Tcode) (int pos, void *data);


#endif

#ifndef BMHSINCLUDED
#define BMHSINCLUDED


		/* boyer-moore-horspool-sunday search */

typedef struct
   { int *d;			/* the d table of bmhs */
     uchar *pat;		/* pattern */
     int m;			/* pattern length */
   } Bmhs;

	/* prepares to search for pattern[0..m-1] */

void prepBmhs (Bmhs *B, uchar *pattern, int m, int k);

	/* no init needed */

void initBmhs (Bmhs *B);

	/* frees data */

void freeBmhs (Bmhs *B);
     
	/* searches in text+n, executing code (n,ptr) at each occurrence.
	   code returns whether to stop the search. a match must exist.
	*/

void searchBmhs (Bmhs *B, uchar *text, int n, Tcode code, void *ptr);


	/* macro version. code sets ret to its return
	   value (break the search or not) */

#define runBmhs(B,text,n,code,ret) \
   { register char *_pat = B.pat; \
     register int _m = B.m; \
     register int *_d = B.d; \
     register uchar *_tt; \
     register int _l; \
    \
     _tt = text + n; \
     while (true) \
        { for (_l=0; _tt[_l] == _pat[_l]; _l++); \
          if (_l == _m)  /* found */ \
	     { n = _tt - text + _m; \
	       code; \
	       if (ret) break; \
	     } \
          _tt += _d[_tt[_m]]; \
        } \
   }


#endif


#ifndef CUTINCLUDED
#define CUTINCLUDED

	/* cuts a pattern into subpatterns */

typedef struct
   { int *aclen;                /* accum lengths of Ps[0..i] */
     int *len;                  /* length of Ps[i] */
     uchar **ps;                /* subpatterns */
     int j;                     /* number of subpatterns */
     int k;                     /* k/j */
   } Cut;

void makeCut (uchar *pattern, int m, int k, int jj, Cut *cut);

void freeCut (Cut *cut);

#endif

#ifndef DYNPROGINCLUDED
#define DYNPROGINCLUDED


		/* match verification with dynamic programming
		   (really, the O(kn) heuristic of Ukkonen).
		   it avoids retraversing the same text twice
	 	*/

typedef struct
   { int m,k;			/* m, k */
     uchar *pat;		/* pattern to search (minus 1) */
     int lastpos;
   } VerData;

	/* prepares to search pattern[0..m-1] with k errors */

void prepVer (VerData *D, uchar *pattern, int m, int k);

	/* prepares for a fresh search (not needed for searchVerV) */

void initVer (VerData *D);

	/* frees the data */

void freeVer (VerData *D);

        /* searches in text[n..fin-1], executing code(pos,ptr) each time
	   it finds a match. if code() returns true, it returns.
           it remembers the last verified position, to avoid
           re-verifying due to overlapping checks
        */

void searchVerV (VerData *D, uchar *text, int n, int fin, 
	         Tcode code, void *ptr);

	/* similar, but no end point and no reverification test.
	   here it is necessary to use initVerprog */

void searchVer (VerData *D, uchar *text, int n,
	        Tcode code, void *ptr);


	/* macro version. in this case, code
	   must set the variable ret to whether it wants to break 
	   the iteration or not. code can look at n to see the position
	*/

#define runVerV(D,text,n,fin,code,ret) \
    { register int _vmm; \
      register int _vi; \
      register int _vm=D.m; \
      register char *_vpat=D.pat; \
      register int _vk = D.k; \
      if (n <= D.lastpos) n = D.lastpos+1; \
      while(n < fin-_vm) { \
        _vmm = 0; \
        for (_vi = 0; _vi < _vm; _vi++) { \
            _vmm += (text[n+_vi] == _vpat[_vi] ? 0 : 1);  \
          if (_vmm > _vk) \
            break; \
        } \
        if (_vmm <= _vk) { \
          code; \
          if (ret) break; \
        } \
        D.lastpos = n; \
        n++; \
      }\
    }

#define runVer(D,text,n,code,ret) \
    { register int _vmm; \
      register int _vi; \
      register int _vp=n; \
      register int _vm=D.m; \
      register char *_vpat=D.pat; \
      register int _vk = D.k; \
      while(true) { \
        _vmm = 0; \
        for (_vi = 0; _vi < _vm; _vi++) { \
          _vmm += (text[_vp+_vi] == _vpat[_vi]); \
          if (_vmm > _vk) \
            break; \
        } \
        if (_vmm <= _vk) { \
          code; \
          if (ret) break; \
        } \
        _vp++; \
      }\
    }

#endif

#ifndef MULTIPATINCLUDED
#define MULTIPATINCLUDED

	/* a fast multipattern search heuristic, based on bmhs.
	   the d table is computed as the worst among the
	   patterns (length = minimum among patterns). instead
	   of matching text against pattern, we search the
	   text into a trie of the patterns 
	*/

typedef int (*GAutom)[Sigma];

typedef struct 
   { int *d;			/* d table */
     GAutom G;			/* the trie of the patterns */
     int minlen;		/* length of shorter pattern */
   } Multi;

	/* prepare to search patterns Ps[0]..Ps[r-1] */

void prepMulti (Multi *M, uchar **Ps, int r);

	/* no initialization needed */

void initMulti (Multi *M);

	/* free data */

void freeMulti (Multi *M);

	/* searches M from text+n, executing code(n,ptr) at each occurrence
	   beginning at n.  (the pattern must be in the text)
	*/

void searchMulti (Multi *M, uchar *text, int n, Tcode code, void *ptr);

	/* macro version */

#define runMulti(M,text,n,code,ret) \
   { register int *_d = M.d; \
     register GAutom _G = M.G; \
     register int _minlen = M.minlen; \
     register int _r,_state; \
     register uchar *_tt = text+n; \
     _r = 0; _state = 0; \
     while (true) \
       { _state = _G[_state][_tt[_r++]]; \
         if (_state > 0) \
            { if (_G[_state][0] != -1) /* found */ \
	         { n = _tt-(uchar*)text; \
		   code; \
		   if (ret) break; \
		 } \
	    } \
         else { _tt += _d[_tt[_minlen]]; _r = _state = 0; } \
       } \
   } 

#endif


#ifndef PARTEXACTINCLUDED
#define PARTEXACTINCLUDED


	/* problem partitioning into exact match */

typedef struct
   { Multi M;                   /* multipattern search */
     VerData dyn;               /* dynammic programing verification */
     Cut cut;                   /* subpatterns */
     int back;
     int forth;            /* precomputed values */
   } PartExact;         /* partitioning into exact search */

void prepPartExact (PartExact *P, uchar *pattern, int m, int k);

void initPartExact (PartExact *P);

void freePartExact (PartExact *P);

int searchPartExact (PartExact *P, uchar *text, int n, Tcode code, void *ptr);

#define runPartExact(P,text,n,code,ret) \
   { register int _pe_n = n,_pe_f; \
     register int _back = P.back; \
     register int _forth = P.forth; \
     register bool _ret = false; \
 \
     while (true) \
       { runMulti(P.M,text,_pe_n,,true); \
         _pe_f = _pe_n + _forth; n = _pe_n - _back; \
         runVerV (P.dyn,text,n,_pe_f,{code;_ret=ret;},ret); \
         if (_ret) break; \
         _pe_n++; \
       } \
   }

#endif
