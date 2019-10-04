/*
ASSUMPTION: all patterns are separated by '\n' or '\0' and are the same length (>=4 and <=32).
*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include 	<smmintrin.h>
#include <xmmintrin.h>
#include <immintrin.h>
#include <x86intrin.h>
#include "bperl-mm.h"
#define max(a,b) (a)>(b) ? (a) : (b)

#define MEMCMP_FUNC simd_memcmp16  // string comparison function to use
#define SKIP_CMP 0  // set to 1 to skip string comparisons and go straight to approximate search in case of a hash match

#define DNA 1 //tuning for DNA, actually for English too.

#if(DNA)

#define MOD 1
#define HYBRID 1 //set to 1 //uses MOD only for Plen<8 (sets MOD=0 for Plen>=8)

#else

#define MOD 1
#define HYBRID 0

#endif

#define COLLISIONS 0

//List funcs
typedef struct list {
   struct list *next;
   int patt;
   int pos;
} LIST;

void freeList(LIST *f) {
	if(f->next != NULL) freeList(f->next);
	free(f);
}

void freeFlist(LIST *flist[65536]){
	int i;
	for(i=0; i<65536; i++)
		if(flist[i]!=NULL) freeList(flist[i]);
}

LIST *flist[65536];
LIST *t;
unsigned short int  signature;
unsigned int  crc;
unsigned long long  lcrc; //for pattern length >=16
unsigned int* intptr;
unsigned long long* lptr; //for pattern length >=16
unsigned long long  crcAdditiveConstant = 0x000011115555AAAA;//no effect on CRC
int        i,j,Hlen;
unsigned int        shift;
unsigned char*      possibleStart;

#if(MOD)
unsigned long long Mask;
#endif

unsigned int offset; //offset from the beginning of the text. Used for skipping what I've already seen
unsigned char **pats; //patterns
int r; //number of patterns
int Plen; //length of pattern

#if(COLLISIONS)
unsigned long long col;
char printed=0;
#endif



#define EQUAL 0
#define DIFFER 1
#define BYTES_PER_SIMD_REG 32
#define MASK_ALL_1 (((u_int32_t)1 << BYTES_PER_SIMD_REG)-1)
u_int8_t simd_memcmp(unsigned char *x, unsigned char *y, int sz)
{
    __m256i x_ptr, y_ptr;

    //~ for(; sz>=BYTES_PER_SIMD_REG; sz-=BYTES_PER_SIMD_REG){
        //~ x_ptr = _mm256_loadu_si256 ((__m256i *) x);
        //~ y_ptr = _mm256_loadu_si256 ((__m256i *) y);
        //~ u_int32_t t = _mm256_movemask_epi8 (_mm256_cmpeq_epi8 (x_ptr, y_ptr));
        //~ if(t != MASK_ALL_1) return DIFFER;
        //~ x+=BYTES_PER_SIMD_REG;
        //~ y+=BYTES_PER_SIMD_REG;
    //~ }

    //~ if(sz){ //compare sz bytes
        u_int32_t mask = ((1 << sz) - 1);
        x_ptr = _mm256_loadu_si256 ((__m256i *) x);
        y_ptr = _mm256_loadu_si256 ((__m256i *) y);
        u_int32_t t = _mm256_movemask_epi8 (_mm256_cmpeq_epi8 (x_ptr, y_ptr));
        //~ printf("t %d, mask %u\n",t,mask);
        //~ printBits(4,&t);
        //~ printBits(4,&mask);
        //~ u_int32_t aver = t&mask; printBits(4,&aver);
        if((t&mask) != mask) return DIFFER;
        else return EQUAL;
    //~ }

    //~ return EQUAL;
}

#define BYTES_PER_SIMD_REG 16
#define MASK_ALL_1 (((u_int32_t)1 << BYTES_PER_SIMD_REG)-1)
u_int8_t simd_memcmp16(unsigned char *x, unsigned char *y, int sz)
{
    __m128i x_ptr, y_ptr;

    u_int16_t mask = ((1 << sz) - 1);
    x_ptr = _mm_loadu_si128 ((__m128i *) x);
    y_ptr = _mm_loadu_si128 ((__m128i *) y);
    u_int16_t t = _mm_movemask_epi8 (_mm_cmpeq_epi8 (x_ptr, y_ptr));

#if(EQUAL==0)
    return (t&mask) != mask;
#else
    return (t&mask) == mask;
#endif
}


// ***** preprocessing *****

// 4 <= Plen < 8
void prep4(unsigned char **ps, int len, int  kval, int qval)
{
#if(!MOD)
    shift = (Plen/2 - 1)*2; //original
#else
    //shift = Plen - sizeof(*intptr) + 1; //Plen - sizeof(hashed_block) +1
    shift=1;
    Mask=0xffffffffffffffff; //11111...
    Mask = (unsigned long long)Mask>>(unsigned long long)(sizeof(*lptr)*(sizeof(*lptr)-Plen));
    //printf("mask 0x%llx\n",mask);
#endif

#if(!MOD)
    Hlen = 2; //length of block to hash
#else
    Hlen = sizeof(*intptr);
#endif

	for(i=0;i<r;i++){//for all patterns
	   //for(j=0;j<shift;j++){
       for(j=shift-1;j>=0;j--){
#if(!MOD)
		   intptr = (unsigned long long*) &pats[i][shift-j];
           crc = *intptr;
#else
		   lptr = (unsigned long long*) &pats[i][shift-1-j];
		   crc = _mm_crc32_u64(crcAdditiveConstant, Mask & *lptr);
#endif
		   signature = (unsigned short int) crc;
#if(COLLISIONS)
if(!printed && flist[signature]) col++;
#endif
		   t = (LIST*)malloc(sizeof(LIST));
		   t->next = flist[signature];
		   t->pos  = j;
		   t->patt = i;
		   flist[signature] = t;
	   }
	}
}

// 8 <= Plen < 16
void prep8(unsigned char **ps, int len, int  kval, int qval)
{
#if(!MOD || HYBRID)
    //shift = (Plen/4 - 1)*4; //original
    shift = Plen - sizeof(*intptr) + 1;
#else
    shift = Plen - sizeof(*lptr) + 1; //Plen - sizeof(hashed_block) + 1
#endif

#if(!MOD || HYBRID)
    Hlen = sizeof(*intptr); //length of block to hash
#else
    Hlen = sizeof(*lptr);
#endif

    for(i=0;i<r;i++){//for all patterns
        //for(j=0;j<shift;j++){
        for(j=shift-1;j>=0;j--){
#if(!MOD || HYBRID)
            intptr = (unsigned int*) &pats[i][shift-1-j];
            crc = _mm_crc32_u32(crcAdditiveConstant, *intptr);
            signature = (unsigned short int) crc;
#else
            lptr = (unsigned long long*) &pats[i][shift-1-j];
            lcrc = _mm_crc32_u64(crcAdditiveConstant, *lptr);
            signature = (unsigned short int) lcrc;
#endif
#if(COLLISIONS)
if(!printed && flist[signature]) col++;
#endif
            t = (LIST*)malloc(sizeof(LIST));
            t->next = flist[signature];
            t->pos  = j;
            t->patt = i;
            flist[signature] = t;
        }
    }
}

//DEPRECATED same as prep8 with MOD=1

// 16 <= Plen < 32
void prep16(unsigned char **ps, int len, int  kval, int qval)
{
    shift = Plen - sizeof(*lptr) + 1; //Plen - sizeof(hashed_block) + 1 
    //shift = Plen - 2*sizeof(*lptr) + 1;

    Hlen = sizeof(*lptr);

    for(i=0;i<r;i++){//for all patterns
        //for(j=0;j<shift;j++){
        for(j=shift-1;j>=0;j--){
            lptr = (unsigned long long*) &pats[i][shift-1-j];
            lcrc = _mm_crc32_u64(crcAdditiveConstant, *lptr);
            //unsigned long long block = *lptr ^ *(lptr+1);
            //lcrc = _mm_crc32_u64(crcAdditiveConstant, block);
            signature = (unsigned short int) lcrc;
#if(COLLISIONS)
if(!printed && flist[signature]) col++;
#endif
            t = (LIST*)malloc(sizeof(LIST));
            t->next = flist[signature];
            t->pos  = j;
            t->patt = i;
            flist[signature] = t;
        }
	}
}


void mepsm_prep(unsigned char **ps, int npats, int len)
{
    //int len = strlen(ps[0]);
    if(len<4 || len>32){
        printf("Pattern length %d out of range!\n",len);
        exit(1);
        return;
    }

    r = npats;

	freeFlist(flist); //free flist in case prep executes many times
	memset(flist,0,sizeof(LIST*)*65536);

	pats = ps;
	Plen = len;
	/*
	for(Plen = 0; *ps!=0; ++ps) ++Plen;
	r = (len+1) / (Plen+1); //len+1 in case last pattern finishes in '\0' instead of '\n'
	*/
#if(COLLISIONS)
col=0;
#endif
int kval,qval;
    if (Plen>=16)
        prep16(pats, len, kval, qval);
    else  if (Plen>=8)
        prep8(pats, len, kval, qval);
    else  if (Plen>=4)
        prep4(pats, len, kval, qval);
    else return;
#if(COLLISIONS)
if(!printed){
    printf("Number of collisions: %-10lld",col);
    printed=1;
}
#endif
}



//***** search *****
ExactAns search4(unsigned char *buf, int Tlen, int _, int qval)
{ 
    unsigned char* charPtr = &buf[offset];
    ExactAns ans;
    
    while(charPtr<&buf[Tlen-(Hlen-1)]){ //there must be room for a hash_block
#if(!MOD)
        //crc = _mm_crc32_u16(crcAdditiveConstant,(*((unsigned int*)charPtr) & 0x0000ffff));
        crc = *((unsigned int*)charPtr);
        signature = (unsigned short int) crc;
#else
        //crc = *((unsigned int*)charPtr);
        lcrc = _mm_crc32_u64(crcAdditiveConstant, Mask & *((unsigned long long*)charPtr));
        signature = (unsigned short int) lcrc;
#endif

        t = flist[signature];
        while(t){
#if(!MOD)
            possibleStart = charPtr-shift+t->pos;
#else
            possibleStart = charPtr-shift+1+t->pos;
#endif
            if (SKIP_CMP || 0==MEMCMP_FUNC(possibleStart, pats[t->patt], Plen)){
                //count++; //position of occurrence can be computed via (possibleStart-y)
                offset = charPtr - buf + shift;
                ans.pos = possibleStart-buf;
                ans.subpatt = t->patt;
                t=t->next;
                return ans;
            }
            t = t->next;
        }
        charPtr+=shift;
    }
	
    ans.pos = -1;
    ans.subpatt = -1;
	return ans;
}

ExactAns search8(unsigned char *buf, int Tlen, int _, int qval)
{
    unsigned char* charPtr = &buf[offset];
    ExactAns ans;
    
    while(charPtr<&buf[Tlen-(Hlen-1)]){ //there must be room for a hash_block
#if(!MOD || HYBRID)
        crc = _mm_crc32_u32(crcAdditiveConstant,*((unsigned int*)charPtr));
        signature = (unsigned short int) crc;
#else
        lcrc = _mm_crc32_u64(crcAdditiveConstant,*((unsigned long long*)charPtr));
        signature = (unsigned short int) lcrc;
#endif

        t = flist[signature];
        while(t){
            possibleStart = charPtr-shift+1+t->pos; //t->pos is negative
            if (SKIP_CMP || 0==MEMCMP_FUNC(possibleStart, pats[t->patt], Plen)){
                offset = charPtr - buf + shift;
                ans.pos = possibleStart-buf;
                ans.subpatt = t->patt;
                t=t->next;
                return ans;
            }
            t = t->next;
        }
        charPtr+=shift;
    }

    ans.pos = -1;
    ans.subpatt = -1;
	return ans;
}

//DEPRECATED same as search8 with MOD=1

ExactAns search16(unsigned char *buf, int Tlen, int _, int qval)
{
	ExactAns ans;
    unsigned char* charPtr = &buf[offset];

    //Hlen = sizeof(*lptr)*2;
    
    while(charPtr<&buf[Tlen-(Hlen-1)]){ //there must be room for a hash_block (8 bytes in this case)
        lcrc = _mm_crc32_u64(crcAdditiveConstant,*((unsigned long long*)charPtr));

//        lptr = (unsigned long long*)charPtr;
//        unsigned long long block = *lptr ^ *(lptr+1);
//        lcrc = _mm_crc32_u64(crcAdditiveConstant, block);

        signature = (unsigned short int) lcrc;
        t = flist[signature];
        while(t){
            possibleStart = charPtr-shift+1+t->pos;
            if (SKIP_CMP || 0==MEMCMP_FUNC(possibleStart, pats[t->patt], Plen)){
                offset = charPtr - buf + shift;
                ans.pos = possibleStart-buf;
                ans.subpatt = t->patt;
                t=t->next;
                return ans;
            }
            t = t->next;
        }
        charPtr+=shift;
    }

    ans.pos = -1;
    ans.subpatt = -1;
	return ans;
}

ExactAns mepsm_exec(unsigned char *buf, int Tlen)
{
	ExactAns ans;
#if 1 //turn to 0 to check only first coincident q-gram
    char* charPtr = &buf[offset-shift];    
    while(t!=NULL){
        possibleStart = charPtr-shift+1+t->pos;
        if (SKIP_CMP || 0==MEMCMP_FUNC(possibleStart, pats[t->patt], Plen)){
                ans.pos = possibleStart-buf;
                ans.subpatt = t->patt;
                t=t->next;
                return ans;
        }
        t = t->next;
    }
#endif

    int kval,qval;
    if (Plen>=16)
        return search16(buf, Tlen, kval, qval);
    else  if (Plen>=8)
        return search8(buf, Tlen, kval, qval);
    else  if (Plen>=4)
        return search4(buf, Tlen, kval, qval);
    else{
		ans.pos = -1;
		ans.subpatt = -1;
		return ans;
	}
}

void mepsm_set_offset(unsigned val){ t=NULL; offset = Plen-Hlen; }

int mepsm_get_shift(){ return shift; }
