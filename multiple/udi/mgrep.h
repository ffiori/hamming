/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/

/* 
 *
 * Multi Pattern Matcher 
 *
 */

#ifndef MGREP_H
#define MGREP_H

typedef signed char int8;
typedef unsigned char uint8;

typedef signed int int32;
typedef unsigned int uint32;

typedef signed short int int16;
typedef unsigned short int uint16;

typedef int bool;

#define true 1
#define false 0

#define MAXLINELEN  1024
#define W_DELIM	    128
#define L_DELIM     10 

#define NO_SKIP 1 //fernando. To count all matches in a line (i.e. doesn't skip the line)
//#define DELETIONS //uncomment to allow deletions. Commented counts only mismatches.

typedef struct 
{
    bool Report;
    bool Statistics;
    bool WordBound;
    bool NoUpper;
    bool WholeLine;
    bool Inverse;
    bool Count;
    bool Silent;
    bool FileNameOnly;
    bool Debug;
    bool NoMatching;
    int  Seed;
}
GLOBALFLAG;


/********************************************************
EXTERNS
********************************************************/

extern GLOBALFLAG GlobalFlag;

extern void FatalError(int errorno, char *mess, ... );
extern void AllocTables(uint32 hashtab, uint32 bittab);
extern void ProcessPatterns(uint8 *PatternStart, uint8 *PatternEnd);
extern int MatchText(uint8 *TextStart, uint8 *TextEnd);

#endif /* MGREP_H */

/********************************************************
  EOF
********************************************************/







