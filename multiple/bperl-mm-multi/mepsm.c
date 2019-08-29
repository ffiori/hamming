/*
ASSUMPTION: all patterns are separated by '\n' or '\0' and are the same length (>=4 and <=32).
*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include 	<smmintrin.h>
#include "bperl-mm-multi.h"

#define EXPRS4(x) ((*(base + (x)) & 6) << 5) | ((*(base + (x) - 1) & 6) << 3) | ((*(base + (x) - 2) & 6) << 1) | ((*(base + (x) - 3) & 6) >> 1)
#define EXPRS5(x) ((*(base + (x)) & 6) << 7) | EXPRS4((x)-1)
#define EXPRS6(x) ((*(base + (x)) & 6) << 9) | EXPRS5((x)-1)
#define EXPRS7(x) ((*(base + (x)) & 6) << 11) | EXPRS6((x)-1)
#define EXPRS8(x) ((*(base + (x)) & 6) << 13) | EXPRS7((x)-1)

#define COLLISIONS 0 //set to 1 to count collisions in hash preprocessing. ONLY ONE RUN!

//List funcs
typedef struct list
{
    struct list *next;
    int patt;
    int subpatt;
    int pos;
} LIST;

void freeList (LIST * f)
{
    if (f->next != NULL)
        freeList (f->next);
    free (f);
}

void freeFlist (LIST * flist[65536])
{
    int i;

    for (i = 0; i < 65536; i++)
        if (flist[i] != NULL)
            freeList (flist[i]);
}

LIST *flist[65536];
LIST *t;
unsigned short int signature;
unsigned int crc;
unsigned long long lcrc;        //for pattern length >=16
unsigned int *intptr;
unsigned long long *lptr;       //for pattern length >=16
unsigned long long crcAdditiveConstant = 0x000011115555AAAA;    //no effect on CRC
int i, j;
int shift;
char *possibleStart;

#if(MOD)
unsigned long long mask;
#endif

unsigned int offset;            //offset from the beginning of the text. Used for skipping what I've already seen
unsigned char **pats[MAXPATS];  //patterns. pats[i]: pattern i. pats[i][j]: subpattern j of pattern i.
int r;                          //number of patterns
int Plen;                       //length of pattern
unsigned int patnow;            //identifier of pattern that I'm preprocessing now

/*
Inserts a node in searchAns[patnow], keeping it ordered.
It runs in O(shift) and eliminates duplicates.
*/
void insert_RetList (int patnow, RetList * node)
{
    RetList *list = searchAns[patnow];

    if (list == NULL) {
        lastNode[patnow] = node;
        searchAns[patnow] = node;
        return;
    }

    for (list = lastNode[patnow]; list != NULL; list = list->prev) {
        if (list->pos == node->pos) {
            free (node);
            return;
        }                       //do nothing, duplicate entry
        if (list->pos < node->pos) {
            if (list == lastNode[patnow])
                lastNode[patnow] = node;
            else {
                node->next = list->next;
                list->next->prev = node;
            }

            list->next = node;
            node->prev = list;

            return;
        }
    }

    if (list == NULL) {
        searchAns[patnow]->prev = node;
        node->next = searchAns[patnow];
        searchAns[patnow] = node;
    }
}

#if(COLLISIONS)
unsigned long long col = 0;
char printed = 0;
#endif

// ***** preprocessing *****

// 4 <= Plen < 8
void prep4 (unsigned char **ps, int _, int kval, int qval)
{
#if(!MOD)
    shift = (Plen / 2 - 1) * 2; //original
#else
    //shift = Plen - sizeof(*intptr) + 1; //Plen - sizeof(hashed_block) +1
    shift = 1;
    mask = 0xffffffffffffffff;  //11111...
    mask = (unsigned long long) mask >> (unsigned long long) (sizeof (*lptr) * (sizeof (*lptr) - Plen));
    //printf("mask 0x%llx\n",mask);
#endif

    for (i = 0; i < r; i++) {   //for all patterns
        for (j = 0; j < shift; j++) {    //TODO check +!MOD
#if(!MOD)
            intptr = (unsigned long long *) &ps[i][shift - j];
            crc = _mm_crc32_u16 (crcAdditiveConstant, (*intptr & 0x0000ffff));
            //printf("mirando patt %s, crc %d\n",intptr,crc);
#else
#if(DNA)
            unsigned char *base = &pats[i * (Plen + 1) + shift - 1];

            if (Plen == 4)
                signature = EXPRS4 (Plen - 1);
            if (Plen == 5)
                signature = EXPRS5 (Plen - 1);
            if (Plen == 6)
                signature = EXPRS6 (Plen - 1);
            if (Plen == 7)
                signature = EXPRS7 (Plen - 1);
#else
            lptr = (unsigned long long *) &ps[i][shift - 1 - j];
            crc = _mm_crc32_u64 (crcAdditiveConstant, mask & *lptr);
#endif
#endif
            signature = (unsigned short int) crc;
#if(COLLISIONS)
//printf("r: %d, shift: %d\n",r,shift);
//col++;
            if (!printed && flist[signature])
                col++;
#endif
            t = (LIST *) malloc (sizeof (LIST));
            t->next = flist[signature];
            t->pos = j;
            t->patt = patnow;
            t->subpatt = i;
            flist[signature] = t;
        }

        //~ if (strlen (ps[i]) > Plen) {
//~ #if(!MOD)
            //~ intptr = (unsigned long long *) &ps[i][shift];
            //~ crc = _mm_crc32_u16 (crcAdditiveConstant, (*intptr & 0x0000ffff));
//~ #else
//~ #if(DNA)
            //~ unsigned char *base = &pats[i * (Plen + 1) + shift];

            //~ if (Plen == 4)
                //~ signature = EXPRS4 (Plen - 1);
            //~ if (Plen == 5)
                //~ signature = EXPRS5 (Plen - 1);
            //~ if (Plen == 6)
                //~ signature = EXPRS6 (Plen - 1);
            //~ if (Plen == 7)
                //~ signature = EXPRS7 (Plen - 1);
//~ #else
            //~ lptr = (unsigned long long *) &ps[i][shift];
            //~ crc = _mm_crc32_u64 (crcAdditiveConstant, mask & *lptr);
//~ #endif
//~ #endif
            //~ signature = (unsigned short int) crc;

//~ #if(COLLISIONS)
            //~ if (!printed && flist[signature])
                //~ col++;
//~ #endif

            //~ t = (LIST *) malloc (sizeof (LIST));
            //~ t->next = flist[signature];
            //~ t->pos = -1;
            //~ t->patt = patnow;
            //~ t->subpatt = i;
            //~ flist[signature] = t;
        //~ }
    }
}

// 8 <= Plen < 16
void prep8 (unsigned char **ps, int _, int kval, int qval)
{
#if(!MOD)
    shift = (Plen / 4 - 1) * 4; //original
#else
    shift = Plen - sizeof (*lptr) + 1;  //Plen - sizeof(hashed_block) + 1
#endif

    for (i = 0; i < r; i++) {   //for all patterns
        for (j = 0; j < shift; j++) {
#if(!MOD)
            intptr = (unsigned int *) &ps[i][shift - j];
            crc = _mm_crc32_u32 (crcAdditiveConstant, *intptr);
            signature = (unsigned short int) crc;
#else
#if(DNA)
            unsigned char *base = &pats[i * (Plen + 1) + shift - 1 - j];

            signature = EXPRS8 (7);
#else
            lptr = (unsigned long long *) &ps[i][shift - 1 - j];
            lcrc = _mm_crc32_u64 (crcAdditiveConstant, *lptr);
            signature = (unsigned short int) lcrc;
#endif
#endif
#if(COLLISIONS)
            if (!printed && flist[signature])
                col++;
#endif
            t = (LIST *) malloc (sizeof (LIST));
            t->next = flist[signature];
            t->pos = j;
            t->patt = patnow;
            t->subpatt = i;
            flist[signature] = t;
        }

        //~ if (strlen (ps[i]) > Plen) {
//~ #if(!MOD)
            //~ intptr = (unsigned int *) &ps[i][shift + 1];
            //~ crc = _mm_crc32_u32 (crcAdditiveConstant, *intptr);
            //~ signature = (unsigned short int) crc;
//~ #else
//~ #if(DNA)
            //~ unsigned char *base = &pats[i * (Plen + 1) + shift];

            //~ signature = EXPRS8 (7);
//~ #else
            //~ lptr = (unsigned long long *) &ps[i][shift];
            //~ lcrc = _mm_crc32_u64 (crcAdditiveConstant, *lptr);
            //~ signature = (unsigned short int) lcrc;
//~ #endif
//~ #endif

//~ #if(COLLISIONS)
            //~ if (!printed && flist[signature])
                //~ col++;
//~ #endif

            //~ t = (LIST *) malloc (sizeof (LIST));
            //~ t->next = flist[signature];
            //~ t->pos = -1;
            //~ t->patt = patnow;
            //~ t->subpatt = i;
            //~ flist[signature] = t;
        //~ }
    }
}

//DEPRECATED same as prep8 with MOD=1

// 16 <= Plen < 32
void prep16 (unsigned char **ps, int _, int kval, int qval)
{
#if(!MOD)
    shift = (Plen / 8 - 1) * 8; //original
#else
    shift = Plen - sizeof (*lptr) + 1;  //Plen - sizeof(hashed_block)
#endif

    for (i = 0; i < r; i++) {   //for all patterns
        for (j = 0; j < shift; j++) {
#if(!MOD)
            lptr = (unsigned long long *) &ps[i][shift - j];
            lcrc = _mm_crc32_u64 (crcAdditiveConstant, *lptr);
#else
#if(DNA)
            unsigned char *base = &pats[i * (Plen + 1) + shift - 1 - j];

            lcrc = EXPRS8 (7);
#else
            lptr = (unsigned long long *) &ps[i][shift - 1 - j];
            lcrc = _mm_crc32_u64 (crcAdditiveConstant, *lptr);
#endif
#endif
            signature = (unsigned short int) lcrc;
#if(COLLISIONS)
            if (!printed && flist[signature])
                col++;
#endif
            t = (LIST *) malloc (sizeof (LIST));
            t->next = flist[signature];
            t->pos = j;
            t->patt = patnow;
            t->subpatt = i;
            flist[signature] = t;
        }

        //~ if (strlen (ps[i]) > Plen) {
//~ #if(!MOD)
            //~ lptr = (unsigned long long *) &ps[i][shift - j];
            //~ lcrc = _mm_crc32_u64 (crcAdditiveConstant, *lptr);
//~ #else
//~ #if(DNA)
            //~ unsigned char *base = &pats[i * (Plen + 1) + shift - 1 - j];

            //~ lcrc = EXPRS8 (7);
//~ #else
            //~ lptr = (unsigned long long *) &ps[i][shift - 1 - j];
            //~ lcrc = _mm_crc32_u64 (crcAdditiveConstant, *lptr);
//~ #endif
//~ #endif
            //~ signature = (unsigned short int) lcrc;

//~ #if(COLLISIONS)
            //~ if (!printed && flist[signature])
                //~ col++;
//~ #endif

            //~ t = (LIST *) malloc (sizeof (LIST));
            //~ t->next = flist[signature];
            //~ t->pos = -1;
            //~ t->patt = patnow;
            //~ t->subpatt = i;
            //~ flist[signature] = t;
        //~ }
    }
}

void mepsm_prep (unsigned char **ps, int nsubpats, int _Plen, int _patnow)
{
    //int len = strlen(ps[0]);

    if (_Plen < 4 || _Plen > 32) {
        printf ("Pattern length %d out of range!\n", _Plen);
        exit (1);
        return;
    }

    r = nsubpats;
    patnow = _patnow;

    pats[patnow] = ps;
    Plen = _Plen;

    int len, kval, qval;

    if (Plen >= 16)
        prep16 (ps, len, kval, qval);
    else if (Plen >= 8)
        prep8 (ps, len, kval, qval);
    else if (Plen >= 4)
        prep4 (ps, len, kval, qval);
    else
        return;

    offset = shift - 1;         //TODO check!!
    //printf("shift: %d\n",shift);

#if(0 && COLLISIONS) //only valid for one pattern
    if (!printed) {
        printf ("Number of collisions: %-10lld", col);
        printed = 1;
    }
#endif
}

void mepsm_init ()
{
    freeFlist (flist);          //free flist in case prep executes many times
    memset (flist, 0, sizeof (LIST *) * 65536);
}

// ***** search *****
RetList search4 (char *buf, int Tlen, int *patans, int qval)
{                               // puts("search4");
    char *charPtr = buf + offset;

    //register int count = 0;

    register int Hlen;

#if(!MOD)
    Hlen = 2;                   //length of block to hash
#else
    Hlen = sizeof (*intptr);
#endif

    while (charPtr < buf + Tlen - (Hlen - 1)) { //there must be room for a hash_block
#if(!MOD)
        crc = _mm_crc32_u16 (crcAdditiveConstant, (*((unsigned int *) charPtr) & 0x0000ffff));
        signature = (unsigned short int) crc;
#else
#if(DNA)
        unsigned char *base = charPtr;

        if (Plen == 4)
            signature = EXPRS4 (Plen - 1);
        else if (Plen == 5)
            signature = EXPRS5 (Plen - 1);
        else if (Plen == 6)
            signature = EXPRS6 (Plen - 1);
        else if (Plen == 7)
            signature = EXPRS7 (Plen - 1);
#else
        lcrc = _mm_crc32_u64 (crcAdditiveConstant, mask & *((unsigned long long *) charPtr));
        signature = (unsigned short int) lcrc;
#endif
#endif

        t = flist[signature];
        while (t) {
#if(!MOD)
            possibleStart = charPtr - shift + t->pos;
#else
            possibleStart = charPtr - shift + 1 + t->pos;
#endif
            if (1 || 0 == memcmp (possibleStart, pats[t->patt][t->subpatt], Plen)) { //Plen?? si hay distintas len? No importa, total se rechequea después
                //RetList *newnode = malloc(sizeof(RetList));
                RetList newnode;

                newnode.patt = t->patt;
                newnode.subpatt = t->subpatt;
                newnode.pos = possibleStart - buf;
                offset = charPtr - buf + shift;
                t = t->next;
                return newnode;
            }
            t = t->next;
        }
        charPtr += shift;
    }

    RetList newnode;

    newnode.patt = -1;
    return newnode;
}

RetList search8 (char *buf, int Tlen, int *patans, int qval)
{                               //puts("search8");
    //RetList *ret = NULL;
    char *charPtr = &buf[offset];

    //register int count = 0;

    register int Hlen;

#if(!MOD)
    Hlen = sizeof (*intptr);    //length of block to hash
#else
    Hlen = sizeof (*lptr);
#endif

    while (charPtr < &buf[Tlen - (Hlen - 1)]) { //there must be room for a hash_block
#if(!MOD)
        crc = _mm_crc32_u32 (crcAdditiveConstant, *((unsigned int *) charPtr));
        signature = (unsigned short int) crc;
#else
#if(DNA)
        unsigned char *base = charPtr;
        signature = EXPRS8 (7);
#else
        lcrc = _mm_crc32_u64 (crcAdditiveConstant, *((unsigned long long *) charPtr));
        signature = (unsigned short int) lcrc;
#endif
#endif

        int lastpat = -1;

        t = flist[signature];
        while (t) {
            //if(t->subpatt==lastpat) { t=t->next; continue; }

#if(!MOD)
            possibleStart = charPtr - shift + t->pos;
#else
            possibleStart = charPtr - shift + 1 + t->pos;
#endif

            if (1 || 0 == memcmp (possibleStart, pats[t->patt][t->subpatt], Plen)) {
                //RetList *newnode = malloc(sizeof(RetList));
                RetList newnode;

                newnode.patt = t->patt;
                newnode.subpatt = t->subpatt;
                newnode.pos = possibleStart - buf;
                offset = charPtr - buf + shift;
                t = t->next;
                return newnode;
            }
            t = t->next;
        }
        charPtr += shift;
    }

    RetList newnode;

    newnode.patt = -1;
    return newnode;
}

//DEPRECATED same as search8 with MOD=1

RetList search16 (char *buf, int Tlen, int *patans, int qval)
{                               // puts("search16");
    //RetList *ret = NULL;
    char *charPtr = &buf[offset];

    //register int count = 0;

    register int Hlen;

    Hlen = sizeof (*lptr);      //length of block to hash
    //Hlen = sizeof(*lptr)*2;

    while (charPtr < &buf[Tlen - (Hlen - 1)]) { //there must be room for a hash_block (8 bytes in this case)

#if(DNA)
        unsigned char *base = charPtr;
        signature = EXPRS8 (7);
#else
        lcrc = _mm_crc32_u64 (crcAdditiveConstant, *((unsigned long long *) charPtr));
        signature = (unsigned short int) lcrc;
#endif

        t = flist[signature];
        while (t) {
#if(!MOD)
            possibleStart = charPtr - shift + t->pos;
#else
            possibleStart = charPtr - shift + 1 + t->pos;
#endif
            if (1 || 0 == memcmp (possibleStart, pats[t->patt][t->subpatt], Plen)) {
                //RetList *newnode = malloc(sizeof(RetList));
                RetList newnode;

                newnode.patt = t->patt;
                newnode.subpatt = t->subpatt;
                newnode.pos = possibleStart - buf;
                offset = charPtr - buf + shift;
                t = t->next;
                return newnode;
            }
            t = t->next;
        }
        charPtr += shift;
    }

    RetList newnode;

    newnode.patt = -1;
    return newnode;
}

//patans will contain the answer of which pattern contains the occurence
RetList mepsm_exec (char *buf, int Tlen)
{
#if(COLLISIONS)
    if (!printed) {
        printf ("Number of collisions: %-10lld", col);
        printed = 1;
    }
#endif

    int qval;

    char *charPtr = &buf[offset - shift];

    while (t != NULL) {
#if(!MOD)
        possibleStart = charPtr - shift + t->pos;
#else
        possibleStart = charPtr - shift + 1 + t->pos;
#endif
        if (1 || 0 == memcmp (possibleStart, pats[t->patt][t->subpatt], Plen)) {
            //RetList *newnode = malloc(sizeof(RetList));
            RetList newnode;

            newnode.patt = t->patt;
            newnode.subpatt = t->subpatt;
            newnode.pos = possibleStart - buf;
            t = t->next;
            return newnode;
        }
        t = t->next;
    }

    if (Plen >= 16)
        return search16 (buf, Tlen, qval, qval);
    else if (Plen >= 8)
        return search8 (buf, Tlen, qval, qval);
    else if (Plen >= 4)
        return search4 (buf, Tlen, qval, qval);
    else {
        RetList newnode;

        newnode.patt = -1;
        return newnode;
    }
}

void mepsm_set_offset (unsigned val)
{
    t = NULL;
    offset = shift - 1;
    //printf("shift: %d\n",shift);
    //offset = val;
}

int mepsm_get_shift ()
{
    return shift;
}
