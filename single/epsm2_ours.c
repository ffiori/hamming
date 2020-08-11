#include <memory.h>
#include <smmintrin.h>
#include <inttypes.h>

void prep(unsigned char *P, size_t m, size_t k);
size_t exec(unsigned char *T, size_t n, size_t k);

#define HASHSIZE 11

typedef union{
   __m128i* symbol16;
   unsigned char* symbol;
} TEXT;

typedef union{
              __m128i  v;
        unsigned  int  ui[4];
   unsigned short int  us[8];
        unsigned char  uc[16];
}VectorUnion;

typedef struct list
{
    struct list *next;
    int pos;
} LIST;

// Global (FF)
LIST* flist[2048]; //11 bit hash is gives the best result according to our tests, no shorter no longer
LIST *t, *t1;

unsigned int i,filter,shift;
unsigned long long crc, seed= 123456789, mask;
unsigned long long* ptr64;
unsigned long long* lastchunk;
unsigned char* charPtr;
int count,p,tmppatlen;

unsigned char* global_pattern; int global_patlen; // FF

VectorUnion P,zero; // search < 16
__m128i res,a,b,z,p128; // search < 16
// end Global

void prep_greater_equal_16(unsigned char* pattern, int patlen)
{
    global_pattern=pattern; // FF
    global_patlen=patlen; // FF
    
    shift = (patlen/8)-1;
    count=0; p=0; tmppatlen=(patlen/8)*8;
    mask = 2047;

    memset(flist,0,sizeof(LIST*)*2048);

    for (i=1; i<tmppatlen-7; i++)
    {
        ptr64 = (unsigned long long*)(&pattern[i]);
        crc = _mm_crc32_u64(seed,*ptr64);
        filter = (unsigned int)(crc & mask);

        if (flist[filter]==0)
        {
            flist[filter] = (LIST*)malloc(sizeof(LIST));
            if (flist[filter]){
                flist[filter]->next = 0;
                flist[filter]->pos  = i;
            }
        }
        else
        {
            t = flist[filter];
            while(t->next!=0) t = t->next;
            t->next = (LIST*)malloc(sizeof(LIST));
            if (t->next){
                t = t->next;
                t->next=0;
                t->pos = i;
            }
        }
    }
}

int search_greater_equal_16(unsigned char* pattern, int patlen, unsigned char* x, int textlen)
{
    lastchunk = (unsigned long long*)&x[((textlen-tmppatlen)/8)*8+1] ;
    ptr64     = (unsigned long long*)&x[(shift-1)*8];

    crc = _mm_crc32_u64(seed,*ptr64);
    filter = (unsigned int)(crc & mask);
    if (flist[filter]){
        charPtr = (unsigned char*)ptr64;
        t = flist[filter];
        while(t)
        {

            if (t->pos <= 8*(shift-1)){
                if (memcmp(pattern,charPtr - t->pos,patlen) == 0)
                    count++;
            }
            t=t->next;
        }
    }
    ptr64 += shift;

    crc = _mm_crc32_u64(seed,*ptr64);
    filter = (unsigned int)(crc & mask);
    if (flist[filter]){
        charPtr = (unsigned char*)ptr64;
        t = flist[filter];
        while(t)
        {   
            if (t->pos <= 8*(2*shift-1)){
                if (memcmp(pattern,charPtr - t->pos,patlen) == 0)
                    count++;
                }
                t=t->next;
            }
    }
    ptr64 += shift;

    while(ptr64 < lastchunk)
    {
        crc = _mm_crc32_u64(seed,*ptr64);
        filter = (unsigned int)(crc & mask);

        if (flist[filter])
        {
            charPtr = (unsigned char*)ptr64;
            t = flist[filter];
            while(t)
            {
                if (memcmp(pattern,charPtr - t->pos,patlen) == 0)
                    count++;
                t=t->next;
            }
        }
        ptr64 += shift;
    }

    ptr64 -= shift;
    charPtr = (unsigned char*)ptr64;
    charPtr += tmppatlen-1; //the first position unchecked where P may end

    while(charPtr < &x[textlen-1])
    {
        if (0== memcmp(pattern,charPtr-tmppatlen+1,patlen)) count++;
        charPtr++;
    }

    return count;
}

//void prep(unsigned char *P, size_t m, size_t k);
void prep(unsigned char* pattern, size_t patlen, size_t _k)
{
    global_pattern=pattern; // FF
    global_patlen=patlen; // FF
    if (global_patlen>=16)       return prep_greater_equal_16(pattern, patlen);

    //~ unsigned char* y0;
    //~ int i,j,k,count=0;
    //~ VectorUnion P,zero;
    //~ __m128i res,a,b,z,p;

    //~ __m128i* text = (__m128i*)x;
    //~ __m128i* end  = (__m128i*)(x+16*(textlen/16));
     //~ end--;

    // Only these two lines preprocessing (FF)
    zero.ui[0]=    zero.ui[1]=    zero.ui[2]=    zero.ui[3]=0;  z = zero.v;
    P.uc[0] = pattern[patlen-5];  P.uc[1] = pattern[patlen-4];  P.uc[2] = pattern[patlen-3];  P.uc[3] = pattern[patlen-2];  p128 = P.v;
}

// size_t exec(unsigned char *T, size_t n, size_t k);
size_t exec(unsigned char* x, size_t textlen, size_t _k)
{
    //~ if (patlen<2)         return search1(pattern, patlen, x, textlen);
    //~ if (patlen==2)        return search2(pattern, patlen, x, textlen);
    //~ if (patlen==3)        return search3(pattern, patlen, x, textlen);
    //~ if (patlen==4)        return search4(pattern, patlen, x, textlen);
    if (global_patlen>=16)       return search_greater_equal_16(global_pattern, global_patlen, x, textlen);

    unsigned char* y0;
    int i,j,k,count=0;
    //~ VectorUnion P,zero;
    //~ __m128i res,a,b,z,p128;
    
    __m128i* text = (__m128i*)x;
    __m128i* end  = (__m128i*)(x+16*(textlen/16));
     end--;

    // Only these two lines preprocessing (FF)
    //~ zero.ui[0]=    zero.ui[1]=    zero.ui[2]=    zero.ui[3]=0;  z = zero.v;
    //~ P.uc[0] = pattern[patlen-5];  P.uc[1] = pattern[patlen-4];  P.uc[2] = pattern[patlen-3];  P.uc[3] = pattern[patlen-2];  p = P.v;

    // SEARCH (FF)
    
    i = (global_patlen-1) / 16; // i points the first 16-byte block that P may end in
    i++;
    text += i;
    for (k=0; k<(i*16+8)-global_patlen+1; k++)  if (0 == memcmp(global_pattern,x+k,global_patlen)) count++;

    //the loop checks if pattern ends at the second half of text[i] or at the first half of text[i+1]
    while(text < end)
    {
        //check if P[(m-5) ... (m-2)] matches with T[i*16+4 ... i*16+7], T[i*16+5 ... i*16+8], .... , T[i*16+11 ... i*16+14]
        //note thet this corresponds P ends at T[i*16+8],T[i*16+9],...,T[i*16+15]
        res  = _mm_mpsadbw_epu8(*text, p128, 0x04);
        b    = _mm_cmpeq_epi16(res,z);
        j    = _mm_movemask_epi8(b);
        if (j)
        {
            y0 = (unsigned char*)(text) + 9 - global_patlen;
            if ( (j&3)==3 && !memcmp(global_pattern,y0,global_patlen)) count++;
            if ( (j&12)==12 && !memcmp(global_pattern,y0+1,global_patlen)) count++;
            if ( (j&48)==48 && !memcmp(global_pattern,y0+2,global_patlen)) count++;
            if ( (j&192)==192 && !memcmp(global_pattern,y0+3,global_patlen)) count++;
            if ( (j&768)==768 && !memcmp(global_pattern,y0+4,global_patlen)) count++;
            if ( (j&3072)==3072 && !memcmp(global_pattern,y0+5,global_patlen)) count++;
            if ( (j&12288)==12288 && !memcmp(global_pattern,y0+6,global_patlen)) count++;
            if ( (j&49152)==49152 && !memcmp(global_pattern,y0+7,global_patlen)) count++;
        }

        a   = _mm_blend_epi16(*text,*(text+1),0x0f);
        b   = _mm_shuffle_epi32(a,_MM_SHUFFLE(1,0,3,2));

        //check if P ends at T[(i+1)*16+8],T[(i+1)*16+9],...,T[(i+1)*16+15]
        res  = _mm_mpsadbw_epu8(b, p128, 0x04);
        b    = _mm_cmpeq_epi16(res,z);
        j    = _mm_movemask_epi8(b);

        if (j)
        {
            y0 = (unsigned char*)(text) + 9 + 8 - global_patlen;
            if ( (j&3)==3 && !memcmp(global_pattern,y0,global_patlen)) count++;
            if ( (j&12)==12 && !memcmp(global_pattern,y0+1,global_patlen)) count++;
            if ( (j&48)==48 && !memcmp(global_pattern,y0+2,global_patlen)) count++;
            if ( (j&192)==192 && !memcmp(global_pattern,y0+3,global_patlen)) count++;
            if ( (j&768)==768 && !memcmp(global_pattern,y0+4,global_patlen)) count++;
            if ( (j&3072)==3072 && !memcmp(global_pattern,y0+5,global_patlen)) count++;
            if ( (j&12288)==12288 && !memcmp(global_pattern,y0+6,global_patlen)) count++;
            if ( (j&49152)==49152 && !memcmp(global_pattern,y0+7,global_patlen)) count++;
        }
        text++;
    }

    for (k = ((unsigned char*)text)+8-x ; k < textlen ; k++)
    {
        if ( 0 == memcmp(global_pattern,&x[k-global_patlen+1],global_patlen) ) count++;
    }

    return count;
}

//~ #define SEARCH search
//~ #include "main-pm.c"
