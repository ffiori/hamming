/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/

/* 
 * Approximate Multi Pattern Matcher 
 *
 * magrep
 */

/*
#define CACHESIZE (1<<16)
#define LINESIZE  32
*/

#define DEBUG_PATTERNTAB(a) 

#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include "mgrep.h"

#ifndef NLENGTH
#define NLENGTH 5
#define HASHTABBITS (19)
#define FASTTABBITS (16)
#endif

#define HASHTABSIZE (1<<HASHTABBITS)
#define FASTTABSIZE (1<<FASTTABBITS)
#define HASHMASK    (HASHTABSIZE-1)
#define FASTMASK    (FASTTABSIZE-1)


#define NSHIFT 1


#define THE_WRD(base,no) ((uint32*)base)[(no)>>5]
#define THE_BIT(no)      (1 << ((no) & 0x1f))           
#define SET_BIT(base,no) THE_WRD(base,no) |=  THE_BIT(no)
#define CLR_BIT(base,no) THE_WRD(base,no) &= ~THE_BIT(no)
#define GET_BIT(base,no) (THE_WRD(base,no) &  THE_BIT(no))

#if   (NLENGTH == 4)
#define DECLARE_BYTES uint32 b01,b12,b23,b34,b4,hash
#elif (NLENGTH == 5)
#define DECLARE_BYTES uint32 b01,b12,b23,b34,b45,b5,hash
#elif (NLENGTH == 6)
#define DECLARE_BYTES uint32 b01,b12,b23,b34,b45,b56,b6,hash
#endif

#if   (NLENGTH == 4)

#define COMPUTE_BYTES(pointer) \
            b01 = Global.RandomTab[(pointer)[0]];\
            b12 = Global.RandomTab[(pointer)[1]];\
            b23 = Global.RandomTab[(pointer)[2]];\
            b34 = Global.RandomTab[(pointer)[3]];\
            b4  = Global.RandomTab[(pointer)[4]];\
            hash =  b01; hash <<= NSHIFT;\
            hash ^= b12; hash <<= NSHIFT;\
            hash ^= b23; hash <<= NSHIFT;\
            hash ^= b34; \
            b01 ^= b12; b12 ^= b23; b23 ^= b34; b34 ^= b4;\
            b01 <<= 3 * NSHIFT; b12 <<= 2 * NSHIFT; b23 <<= 1 * NSHIFT;

#elif    (NLENGTH == 5)         

#define COMPUTE_BYTES(pointer) \
            b01 = Global.RandomTab[(pointer)[0]];\
            b12 = Global.RandomTab[(pointer)[1]];\
            b23 = Global.RandomTab[(pointer)[2]];\
            b34 = Global.RandomTab[(pointer)[3]];\
            b45 = Global.RandomTab[(pointer)[4]];\
            b5  = Global.RandomTab[(pointer)[5]];\
            hash =  b01; hash <<= NSHIFT;\
            hash ^= b12; hash <<= NSHIFT;\
            hash ^= b23; hash <<= NSHIFT;\
            hash ^= b34; hash <<= NSHIFT;\
            hash ^= b45; \
            b01 ^= b12; b12 ^= b23; b23 ^= b34; b34 ^= b45; b45 ^= b5;\
            b01 <<= 4 * NSHIFT; b12 <<= 3 * NSHIFT; b23 <<= 2 * NSHIFT; b34 <<= 1 * NSHIFT

#elif (NLENGTH == 6)

#define COMPUTE_BYTES(pointer) \
            b01 = Global.RandomTab[(pointer)[0]];\
            b12 = Global.RandomTab[(pointer)[1]];\
            b23 = Global.RandomTab[(pointer)[2]];\
            b34 = Global.RandomTab[(pointer)[3]];\
            b45 = Global.RandomTab[(pointer)[4]];\
            b56 = Global.RandomTab[(pointer)[5]];\
            b6  = Global.RandomTab[(pointer)[6]];\
            hash =  b01; hash <<= NSHIFT;\
            hash ^= b12; hash <<= NSHIFT;\
            hash ^= b23; hash <<= NSHIFT;\
            hash ^= b34; hash <<= NSHIFT;\
            hash ^= b45; hash <<= NSHIFT;\
            hash ^= b56; \
            b01 ^= b12; b12 ^= b23; b23 ^= b34; b34 ^= b45; b45 ^= b56; b56 ^= b6;\
            b01 <<= 5 * NSHIFT; b12 <<= 4 * NSHIFT; b23 <<= 3 * NSHIFT;\
            b34 <<= 2 * NSHIFT; b45 <<= 1 * NSHIFT

#endif             

#if   (NLENGTH == 4)

#define UPDATE_BYTES(pointer) \
            b01 = b12 << NSHIFT;\
            b12 = b23 << NSHIFT;\
            b23 = b34 << NSHIFT;\
            b34 = b4;\
            b4  = Global.RandomTab[(pointer)[5]];\
            b34 ^= b4;

#elif   (NLENGTH == 5)

#define UPDATE_BYTES(pointer) \
            b01 = b12 << NSHIFT;\
            b12 = b23 << NSHIFT;\
            b23 = b34 << NSHIFT;\
            b34 = b45 << NSHIFT;\
            b45 = b5;\
            b5  = Global.RandomTab[(pointer)[6]];\
            b45 ^= b5;

#elif   (NLENGTH == 6)

#define UPDATE_BYTES(pointer) \
            b01 = b12 << NSHIFT;\
            b12 = b23 << NSHIFT;\
            b23 = b34 << NSHIFT;\
            b34 = b45 << NSHIFT;\
            b45 = b56 << NSHIFT;\
            b56 = b6;\
            b6  = Global.RandomTab[(pointer)[7]];\
            b56 ^= b6;
#endif


#define safe_free(x) { if((x)!=NULL) free((x)); }

struct
{
    uint32 nPatterns;
    uint32 Shortest;
    uint32 Longest;
    uint32 nRounds;
    uint32 nHashHits;
    uint32 nCandidatePatterns;
    uint32 nCheckedPatterns;
    uint32 nCompleteMissmatches;
    uint32 nMatches;
    uint32 FastCheckTab[ FASTTABSIZE/32 ];
    //uint8 *HashTab[ HASHTABSIZE];
    uint32 HashTab[HASHTABSIZE]; //stores pattern position in PatternTab[i]
    uint32 RandomTab[256];
    uint8 **PatternTab;
    uint8 *PatternSpoolArea;
}
Global;

#define MAXP 112345
uint8 * seen[MAXP]; //last position where there was a match for a given pattern

/* CACHE SIMULATOR */

#ifdef CACHESIZE

struct
{
    int hits;
    int misses;
    int line[CACHESIZE/LINESIZE];
}Cache = {0,0};

void CacheAccess(void *ptr)
{
    int norm = (long)ptr / LINESIZE;
    
    int i = ( (long)ptr & (CACHESIZE-1)) / LINESIZE;

    if( Cache.line[i] == norm )
    {
        Cache.hits++;
        
    }
    else
    {
        Cache.misses++;
        Cache.line[i] = norm;
    }

}

void CacheStatistics()
{
    printf("Hits: %d  Misses: %d   Rate: %d\n",
           Cache.hits,
           Cache.misses,
           Cache.hits / ((Cache.hits+Cache.misses)/100));
}

    

#endif

uint32 RandomSeed;

int Random()
{
    RandomSeed = RandomSeed * 1103515245 + 12345;
    return RandomSeed;
}


void SetSeed (int seed)
{
    RandomSeed = seed;
}


void InitRandomTab()
{
    int i;
    
    SetSeed(666);
    
    for( i=0; i< 256; i++ )
    {
        Global.RandomTab[i] = Random();
    }
}

uint32 Hash(uint8 *pattern, int magic)
{
    int hash = 0;
    int i;

    
    for( i=0; i <= NLENGTH; i++ )
    {
        if( i == magic )
            continue;

        hash <<= NSHIFT;        
        hash ^= Global.RandomTab[ pattern[i] ];
    }

    return hash;
}

/********************************************************
Initialize Tables 
********************************************************/
#define NOTHING -1

void InitTables()
{
    int i;
    
    /* allocate a new pattern table */

    InitRandomTab();
        
    /*
     * initialize FastCheckTab and CollisionTab
     */

    for( i=0; i < HASHTABSIZE; i++ ) Global.HashTab[i] = NOTHING;
    for( i=0; i < FASTTABSIZE/32; i++ ) Global.FastCheckTab[i] = 0;
    
    /*
     * compute hash table
     */

    for( i=0; i < Global.nPatterns; i++ )
    {
        int hash2;
        
        DECLARE_BYTES;

        COMPUTE_BYTES(Global.PatternTab[i]);

        /* 01234 */

        SET_BIT(Global.FastCheckTab, hash & FASTMASK);
        hash2 = hash & HASHMASK;
        while( NOTHING != Global.HashTab[ hash2 ] )
            hash2 = (hash2+1) & HASHMASK;
        //Global.HashTab[ hash2 ] = Global.PatternTab[i];
        Global.HashTab[ hash2 ] = i;

#if( NLENGTH >= 6 )
        /* 01235 */

        hash ^= b56;

        SET_BIT(Global.FastCheckTab, hash & FASTMASK);
        hash2 = hash & HASHMASK;
        while( NOTHING != Global.HashTab[ hash2 ] )
            hash2 = (hash2+1) & HASHMASK;
        //Global.HashTab[ hash2 ] = Global.PatternTab[i];
        Global.HashTab[ hash2 ] = i;
#endif
        
#if( NLENGTH >= 5 )        
        /* 01235 */

        hash ^= b45;

        SET_BIT(Global.FastCheckTab, hash & FASTMASK);
        hash2 = hash & HASHMASK;
        while( NOTHING != Global.HashTab[ hash2 ] )
            hash2 = (hash2+1) & HASHMASK;
        //Global.HashTab[ hash2 ] = Global.PatternTab[i];
        Global.HashTab[ hash2 ] = i;
#endif
        
        /* 01245 */
        
        hash ^= b34;

        SET_BIT(Global.FastCheckTab, hash & FASTMASK);
        hash2 = hash & HASHMASK;
        while( NOTHING != Global.HashTab[ hash2 ] )
            hash2 = (hash2+1) & HASHMASK;
        //Global.HashTab[ hash2 ] = Global.PatternTab[i];
        Global.HashTab[ hash2 ] = i;

        /* 01345 */
        
        hash ^= b23;

        SET_BIT(Global.FastCheckTab, hash & FASTMASK);
        hash2 = hash & HASHMASK;
        while( NOTHING != Global.HashTab[ hash2 ] )
            hash2 = (hash2+1) & HASHMASK;
        //Global.HashTab[ hash2 ] = Global.PatternTab[i];
        Global.HashTab[ hash2 ] = i;

        /* 02345 */
        
        hash ^= b12;

        SET_BIT(Global.FastCheckTab, hash & FASTMASK);
        hash2 = hash & HASHMASK;
        while( NOTHING != Global.HashTab[ hash2 ] )
            hash2 = (hash2+1) & HASHMASK;
        //Global.HashTab[ hash2 ] = Global.PatternTab[i];
        Global.HashTab[ hash2 ] = i;

        /* 12345 */
        
        hash ^= b01;

        SET_BIT(Global.FastCheckTab, hash & FASTMASK);
        hash2 = hash & HASHMASK;
        while( NOTHING != Global.HashTab[ hash2 ] )
            hash2 = (hash2+1) & HASHMASK;
        //Global.HashTab[ hash2 ] = Global.PatternTab[i];
        Global.HashTab[ hash2 ] = i;
    }

    /*
     * Print Statistics
     */

#ifdef GATHER_STATS
    
    if( GlobalFlag.Statistics )
    {
        int count,count2;

        for( count = i = 0; i < HASHTABSIZE; i++ )
        {
            
            if( NOTHING != Global.HashTab[i] )
            {
                /*putchar('8');*/
                count++; 
            }
            /*
              else putchar(' ');
              if( i % 80 == 0 )
              putchar('\n');
              */     
        }
        
        

        printf("Hashtable Size: 2^%d = %d, used entries: %d \n",
               HASHTABBITS, HASHTABSIZE, count);

        for( count2 = i = 0; i < HASHTABSIZE; i++ )
        {
            int hash = i;
            while( NOTHING != Global.HashTab[hash] )
                count2++, hash = (hash+1) & HASHMASK;
        }

        /*
        printf("Average Length of Chain * 1000: %d\n",
               (count2 * 1000) / count  );
        */
        
        for( count = i = 0; i < FASTTABSIZE; i++ )
            if( GET_BIT(Global.FastCheckTab,i) )
                count++;

        printf("Fasttable Size: 2^%d = %d; used entries: %d\n",
               FASTTABBITS, FASTTABSIZE, count);
    }
#endif
    
}

/********************************************************

********************************************************/

void ProcessPatterns(uint8 *PatternStart, uint8 *PatternEnd)
{
    uint8 *cp;
    int   i;

    /* count the number of patterns */
    
    for( cp = PatternStart, i = 0; cp < PatternEnd; cp++ )
	if( *cp == '\0')
	    i++;

    Global.nPatterns = i;

    /* allocate arrays that depend on the number of patterns */
    safe_free(Global.PatternTab);
    safe_free(Global.PatternSpoolArea);
 
    Global.PatternTab = (uint8 **) malloc( Global.nPatterns * sizeof(uint8 *) );
    Global.PatternSpoolArea = (uint8 *) malloc( (PatternEnd - PatternStart) + Global.nPatterns * 6 );

    /* initialize PatternTab and determine shortest pattern*/

    Global.Shortest = INT_MAX;
    Global.Longest = 0;

    cp = Global.PatternSpoolArea;

    *cp++ = 0;                                       /* sentinels everywhere */
    *cp++ = 0;
    *cp++ = 0;
    *cp++ = 0;

    for( i=0; i < Global.nPatterns; i++ )
    {
	int len;

	Global.PatternTab[i] = cp;

	if( GlobalFlag.WordBound )
	    *cp++ = W_DELIM; 

	if( GlobalFlag.WholeLine )
	    *cp++ = L_DELIM;

	//while( *PatternStart != '\n' )
	while( *PatternStart != '\0' )
	    *cp++ = *PatternStart++;

	PatternStart++;                            /* skip newline */

	if( GlobalFlag.WordBound )
	    *cp++ = W_DELIM; 

	if( GlobalFlag.WholeLine )
	    *cp++ = L_DELIM;

	len = cp - Global.PatternTab[i];

	if( len < Global.Shortest )
	    Global.Shortest = len;

	if( len > Global.Longest )
	    Global.Longest = len;
	
	do
	    *cp++ = '\0';
	while( ((long)(cp) & 3) != 0 );  /* fill to 4-aligned address */

	DEBUG_PATTERNTAB(printf("Placed pattern %d (%s) at %p\n",
				i,Global.PatternTab[i],Global.PatternTab[i]) );
    }

#ifdef GATHER_STATS    
    if( GlobalFlag.Statistics )
    {
        printf("Patterns: %d Shortest: %d Longest: %d Effective: %d Length: %d\n",
               Global.nPatterns,Global.Shortest,Global.Longest,
               Global.nPatterns *(NLENGTH+1), NLENGTH);
    }
#endif
    
    InitTables();
    
/*
    for( i=0; i < Global.nPatterns * 4; i++ )
    {
	uint8 *pattern = Global.PatternTab[i];
	printf("%05d %04x %.4s %02d %s\n",
	       i,Hash4(ReadNotAligned(pattern)),pattern+offset,pattern);
    }
*/
}    


//TODO inline
inline uint8 *CheckCollisions(uint32 hash, uint8 *text)
{
    //uint8 *pattern;
    uint32 pattern;
    
#ifdef GATHER_STATS
    uint8 CompleteMissmatch;

    Global.nHashHits++;
#endif
    
    int matches = 0;
    
    while( NOTHING != (pattern = Global.HashTab[ hash ]) )
    {
        uint8 *the_txt = text,
              *the_pat = Global.PatternTab[pattern],
              *the_txt2,
              *the_pat2;

#ifdef CACHESIZE
        CacheAccess( (int *) &Global.HashTab[hash] );
        CacheAccess( (int *)pattern );
#endif
        
        hash = (hash+1) & HASHMASK;
        if(seen[pattern] == text) continue;
        
#ifdef NO_VERIFY
        Global.nCheckedPatterns++;
        continue;
#endif

#ifdef GATHER_STATS
        CompleteMissmatch = 1;
        Global.nCheckedPatterns++;
#endif
        
        while( *the_txt == *the_pat )
            the_txt++, the_pat++;
        
        if( *the_pat == 0 ){ //equal
                matches++;
                seen[pattern] = text;
                //return pattern;
                continue;
        }

#ifdef DELETIONS
        the_txt2 = the_txt + 1;
        the_pat2 = the_pat;
        
        while( *the_txt2 == *the_pat2 )
            the_txt2++, the_pat2++;
        
        if( *the_pat2 == 0 ){
                matches++;
                seen[pattern] = text;
                //return pattern;
                continue;
        }

  #ifdef GATHER_STATS    
        if( the_txt2 - text >= NLENGTH )
            CompleteMissmatch = 0;
  #endif                 

        the_txt2 = the_txt;
        the_pat2 = the_pat + 1;
        
        while( *the_txt2 == *the_pat2 )
            the_txt2++, the_pat2++;
        
        if( *the_pat2 == 0 ){
                matches++;
                seen[pattern] = text;
                //return pattern;
                continue;
        }

  #ifdef GATHER_STATS    
        if( the_pat2 - pattern >= NLENGTH )
            CompleteMissmatch = 0;
  #endif

#endif
        
        the_txt2 = the_txt + 1;
        the_pat2 = the_pat + 1;
        
        while( *the_txt2 == *the_pat2 )
            the_txt2++, the_pat2++;
        
        if( *the_pat2 == 0 ){ // 1 mismatch
                matches++;
                seen[pattern] = text;
                //return pattern;
                continue;
        }

#ifdef GATHER_STATS    
        if( the_pat2 - pattern >= NLENGTH )
            CompleteMissmatch = 0;

        if( CompleteMissmatch  )
            Global.nCompleteMissmatches++;
#endif
        
    }
    
    return matches;
}


#define MATCH_FOUND { Global.nMatches+=matches; }

/********************************************************

********************************************************/
int MatchText(uint8 *TextStart, uint8 *TextEnd)
{    
    uint8 * pattern;
    register DECLARE_BYTES;

    Global.nMatches = 0;
    Global.nHashHits = 0;
    Global.nCheckedPatterns = 0;
    
    int i;
    for( i=0; i < Global.nPatterns; i++ ) seen[i] = NULL;
    
    while( TextStart < TextEnd ) 
    {

        /*********** INIT MATCH ***************/

        COMPUTE_BYTES(TextStart);
        
        /*********** FIND MATCH ***************/
        int matches = 0;
        
        while( TextStart < TextEnd ) 
        {
#ifdef GATHER_STATS   
            Global.nRounds++;
#endif

#ifdef CACHESIZE
            CacheAccess( TextStart );
            CacheAccess( &THE_WRD(Global.FastCheckTab, hash & FASTMASK) );
#endif
            
            /*********** CHECK FOR MATCH ***************/
            if( GET_BIT(Global.FastCheckTab, hash & FASTMASK)  &&
                (matches = CheckCollisions(hash & HASHMASK,TextStart)) )
                MATCH_FOUND;

#if (NLENGTH >= 6)
            hash ^= b56;

#ifdef CACHESIZE
            CacheAccess( &THE_WRD(Global.FastCheckTab, hash & FASTMASK) );
#endif            
            if( GET_BIT(Global.FastCheckTab, hash & FASTMASK)  &&
                (matches = CheckCollisions(hash & HASHMASK,TextStart)) )
                MATCH_FOUND;
#endif
            
#if (NLENGTH >= 5)
            hash ^= b45;

#ifdef CACHESIZE
            CacheAccess( &THE_WRD(Global.FastCheckTab, hash & FASTMASK) );
#endif
            
            if( GET_BIT(Global.FastCheckTab, hash & FASTMASK)  &&
                (matches = CheckCollisions(hash & HASHMASK,TextStart)) )
                MATCH_FOUND;
#endif
            hash ^= b34;

#ifdef CACHESIZE
            CacheAccess( &THE_WRD(Global.FastCheckTab, hash & FASTMASK) );
#endif
            
            if( GET_BIT(Global.FastCheckTab, hash & FASTMASK)  &&
                (matches = CheckCollisions(hash & HASHMASK,TextStart)) )
                MATCH_FOUND;

            hash ^= b23;

#ifdef CACHESIZE
            CacheAccess( &THE_WRD(Global.FastCheckTab, hash & FASTMASK) );
#endif
            
            if( GET_BIT(Global.FastCheckTab, hash & FASTMASK)  &&
                (matches = CheckCollisions(hash & HASHMASK,TextStart)) )
                MATCH_FOUND;


            hash ^= b12;

#ifdef CACHESIZE
            CacheAccess( &THE_WRD(Global.FastCheckTab, hash & FASTMASK) );
#endif
            
            if( GET_BIT(Global.FastCheckTab, hash & FASTMASK)  &&
                (matches = CheckCollisions(hash & HASHMASK,TextStart)) )
                MATCH_FOUND;

            hash ^= b01;
            
            
            if( GET_BIT(Global.FastCheckTab, hash & FASTMASK)  &&
                (matches = CheckCollisions(hash & HASHMASK,TextStart)) )
                MATCH_FOUND;
                
            /*********** UPDATE HASH VALUES ***************/

            UPDATE_BYTES(TextStart);            

            TextStart += 1;
        }

        /*********** MATCH FOUND ***************/

        if( TextStart < TextEnd )
        {
            Global.nMatches+=matches;
#ifdef NO_SKIP
            //TextStart+=3;
            TextStart+=1;
#else           
            if( GlobalFlag.Report )
            {
                printf("%d: ",matches);
            }

            if( GlobalFlag.Silent )
            {
                TextStart++;
                while(*TextStart++ != '\n')
                    ;
            }
            else
            {
                while(*TextStart != '\n')
                    TextStart--;
                TextStart++;
                do
                {
                    putchar( *TextStart );
                }
                while( *TextStart++ != '\n' );
            }
#endif            
        }
        
    }
    
    if( GlobalFlag.Count )
	printf("%d\n",Global.nMatches);

#ifdef GATHER_STATS    
    if( GlobalFlag.Statistics )
    {
        printf("Rounds: %9d Hits: %9d Checked: %9d Unnecessary: %9d Matches: %9d\n",
               Global.nRounds,Global.nHashHits,Global.nCheckedPatterns,
               Global.nCompleteMissmatches,Global.nMatches);
    }
#endif

#ifdef CACHESIZE
    CacheStatistics();
#endif

    return Global.nMatches;
}

/********************************************************
  EOF
********************************************************/

































