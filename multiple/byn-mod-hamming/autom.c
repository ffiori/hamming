/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/

#include "autom.h"

#define max(a,b) ((a)<(b) ? (b) : (a))

void printbin(mask x, int pos)
{
	if(pos<=0) {return;}
	
	printbin(x>>1,pos-1);
	printf("%d",x&1);
	
	if(pos==sizeof(x)*8) printf("\n");
}

#define printbin(x) printbin((x),sizeof(x)*8)

        /* searches from text with automaton A. */

int searchFAutom (PartAutom * A, register uchar * text, int from, int to, int *matches)
{
	puts("WARNING! Entering old searchFAutom...");
    register mask x;
    register mask D = A->D.d0;
    register mask Din = A->Din;
    register mask m2 = A->msk[2];
    register mask m1 = A->msk[1];
    register mask m3 = m1 | m2;
    register mask G = A->Glast.d0;
    register mask Gact = A->Gany.d0;
    register int p0 = A->p[0];
    register int p3 = A->p[3];
    register mask *T = A->T.d1[0];
    register bool *S = A->S;
    register int n = from;
    int count = 0;

    /* k=m-1 not solved with this automaton */
    if (A->k == A->m - 1)
        return search1match (S, text, from, to, matches);

    if (!(~D & Gact))
        while (S[text[n]])
            n++;

    /* search loop */
    while (true) {
        x = (D >> p0) | T[text[n++]];
        D = ((D << 1) | m1) & ((D << p3) | m3) & (((x + m1) ^ x) >> 1) & Din;
        if (~D & Gact) {        /* active? */
            if (!(D & G)) {     /* found *//* clear the last diagonal */
                D |= m2;
                /* invoke code */
                if (n > 0) {    /* otherwise it was just retraining */
                    if (n <= to) {
                        if (matches != NULL)
                            matches[count] = n;
                        count++;
                    }
                    else
                        break;
                }
                /* may be inactive now (optional optimization) */
                if (!(~D & Gact))
                    while (S[text[n]])
                        n++;
            }
        }
        else
            while (S[text[n]])
                n++;
    }
    A->D.d0 = D;
    return count;
}

int searchAutom (PartAutom * A, register uchar * text, int from, int to, int *matches)
{
	puts("WARNING! Entering old searchAutom...");
    register mask x;
    register mask *T = A->T.d1[0];
    register mask D = A->D.d0;
    register mask Din = A->Din;
    register mask m2 = A->msk[2];
    register mask m1 = A->msk[1];
    register mask m3 = m1 | m2;
    register mask G = A->Glast.d0;
    register int p0 = A->p[0];
    register int p3 = A->p[3];
    register int n = from;
    int count = 0;

    /* k=m-1 not solved with this automaton */
    if (A->k == A->m - 1)
        return search1match (A->S, text, from, to, matches);

    /* search loop */
    while (true) {
        x = (D >> p0) | T[text[n++]];
        D = ((D << 1) | m1) & ((D << p3) | m3) & (((x + m1) ^ x) >> 1) & Din;
        if (!(D & G)) {         /* found *//* clear the last diagonal */
            D |= m2;
            /* fill match */
            if (n > 0) {        /* otherwise it was just retraining */
                if (n <= to) {
                    if (matches != NULL)
                        matches[count] = n;
                    count++;
                }
                else
                    break;
            }
        }
    }
    A->D.d0 = D;
    return count;
}


typedef unsigned long word;
//~ #define W (8 * (int)sizeof(word))

//~ int B; /* number of bits per state */
//~ word lim, ovmask, MASK, T[256];

size_t execSA(uchar *text, size_t n, int offset, PartAutom *M, int *matches)
{
    word state, overflow;
    size_t i, occ = 0;

    state = M->MASK & ~M->ovmask;
    overflow = M->ovmask;

    occ = 0;
    for (i = 0; i < n; i++) {
        state = ((state << M->B) + M->TSA[text[i]]) & M->MASK;
        overflow = ((overflow << M->B) | (state & M->ovmask)) & M->MASK;
        state &= ~M->ovmask;
        if ((state + overflow) < M->lim) {
            /* match at T[i-m+1..i] */
            if(matches != NULL)
				matches[occ] = i+1+offset;
            
            occ++;
        }
    }

    return occ;
}

#define DEBUG 0

int searchVAutom (PartAutom * A, register uchar * text, int from, int to, int *matches)
{
    //~ register mask x;
    //~ register mask *T = A->T.d1[0]; //T['a']=0, T[otra cosa]=219
    //~ register mask D = A->D.d0;
    //~ register mask Din = A->Din;
    //~ register mask m2 = A->msk[2]; // 0..0(1)^(k+1)
    //~ register mask m1 = A->msk[1]; // 0..010..010...
    //~ register mask m3 = m1 | m2; //no se usa
    //~ register mask G = A->Glast.d0; //OK 1<<k
    //~ register int p0 = A->p[0]; //OK = k+2
    //~ register int p3 = A->p[3]; //no se usa
    //~ register int n = from;
	//~ int k = p0-2; //WTF Fernando	
#if DEBUG
printf("searchVAutom() from %d to %d. diff %d. p0: %d\n",from,to,to-from,A->p[0]);
//~ printbin(A->MASK);
//~ printbin(A->ovmask);
//~ printbin(A->TSA['a']);
//~ printbin(A->TSA['b']);
#endif
	
	int count = 0;
	
	from = max(0,from);
	to = max(from,to);
	
	count = execSA(text+from, to-from, from, A, matches);
	//~ printf("count: %d\n",count);
	return count;
}




#if 0

int searchVAutom (PartAutom * A, register uchar * text, int from, int to, int *matches)
{
    register mask x;
    register mask *T = A->T.d1[0]; //T['a']=0, T[otra cosa]=219
    register mask D = A->D.d0;
    register mask Din = A->Din;
    register mask m2 = A->msk[2]; // 0..0(1)^(k+1)
    register mask m1 = A->msk[1]; // 0..010..010...
    register mask m3 = m1 | m2; //no se usa
    register mask G = A->Glast.d0; //OK 1<<k
    register int p0 = A->p[0]; //OK = k+2
    register int p3 = A->p[3]; //no se usa
    register int n = from;
    int count = 0;

#if DEBUG
printf("searchVAutom() from %d to %d. diff %d. p0: %d, G: %d\n",from,to,to-from,p0,G);
printf("m1: "); printbin(m1);
printf("m2: "); printbin(m2);
#endif

    /* k=m-1 not solved with this automaton */
    if (A->k == A->m - 1)
        return search1matchV (A->S, text, from, to, matches);

    /* search loop */
    int last = -100;

    while (n < to) {
		#if DEBUG
        
        printf("\nEstoy en n=%d:\n",n);
        
        mask Toriginal = T[text[n++]];
        mask Tnow = ((D >> p0) + m1) & Toriginal; //conservo sólo el 1 a la izq del primer uno de cada diagonal (que sería el mismatch)
        
        int shift = (A->m - A->k - 1) * (A->k + 2);
        Tnow |= (Toriginal >> shift) << shift; //primera diagonal
        
        printf("T[%c]\t",text[n-1]); printbin(Toriginal);
		printf("Tnow\t"); printbin(Tnow);
        printf("D\t"); printbin(D);
        //~ printf("mask\t",text[n-1]); printbin(m2<<shift);
        
        #else
        
        mask Tnow = T[text[n++]];
        
        #endif
        
        x = (D >> p0) | Tnow; // D >> (k+2), o sea que desplaza las diagonales a la derecha.
        
        D = 
			((D << 1) | m1) &   //mismatch (suma 1, necesario solamente para la primer diagonal)
			
            ((D << p3) | m3) & //represents insertions of chars in pattern
            
            (((x + m1) ^ x) >> 1) &     // matches and deletions. TODO work on this!! (suma 1 o mantiene igual, excepto en la primer diagonal)
            
            Din; //máscara que tiene 0s como separadores
        
        #if DEBUG
        printf("x\t"); printbin(x);
        printf("D after\t"); printbin(D);
        #endif
        
        if (!(D & G)) {         /* found */
            D |= m2; /* clear the last diagonal */
            /* fill match */
            if (n > 0) { //capaz >=m
                if (matches != NULL)
                    matches[count] = n;
                if (n != last + 1) //por qué este if ?? TODO
                    count++;

                last = n;
                printf("found %d\n",n-1);
            }
        }
    }
    A->D.d0 = D;
    return count;
}


#endif

/*

xxxxxthe hingxxxxx
           ..
           ^matches bcs of deletion in the pattern
            
*/
