/*

   Implementation of FAAST algorithm using the edit distance table for
   determining a shift which is the value of lower right corner of the edit
   distance table

   (C) Janne Auvinen

*/



#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>



#ifndef CHARTYPE
#define CHARTYPE unsigned char
#endif


#define CHAR_SET (CHAR_MAX-CHAR_MIN+1)

#define MAXPATTERN 200


#define MIN(x,y) (((x) < (y))?(x):(y))
#define MAX(x,y) (((x) > (y))?(x):(y))



static CHARTYPE P[MAXPATTERN];

static int h0[CHAR_SET];

static unsigned long power4;

static unsigned int k;
static unsigned int m;
static unsigned int x;

static int **D;

static int *stringArray;
static int *Dkx;
static int *MM;


// Forms a DNA string in numerical form from a given number using a division
// algorithm for forming of base four numbers

void createArray(unsigned long number) {

   double n;
   unsigned long i, j, remainder;
   int position, characterNumber;

   // printf("number %d\n", number);

   position = 0;
   remainder = 0;

   for (i = 0; i < k+x; i++)
     *(stringArray+i) = 0;

   if (number == 0)
     return;
   else if (number == 1) {
     *(stringArray+k+x-1) = 1;

     return;
   }

   for (i = power4/4; i >= 1; i /= 4) {
     // printf("number i %d\n", i);

     if (number >= i) {
       remainder = number;

       for (j = i; j >= 1; j /= 4) {
	 // printf("number j %d\n", j);

	 if (j == 1) {
	   characterNumber = remainder;
	   *(stringArray+position) = characterNumber;

	   break;
	 }

	 if (remainder >= j) {
	   n = remainder / j;
	   n = floor(n);
	   characterNumber = (int)n;

	   // printf("number %d\n", characterNumber);

	   *(stringArray+position) = characterNumber;
	   position++;
	   remainder -= characterNumber*j;

	   // printf("number remainder %d\n", remainder);
	 } else {
	   *(stringArray+position) = 0;
	   position++;
	 }
       }

       break;
     } else {
       *(stringArray+position) = 0;
       position++;
     }
   }
}

//static int firstrun=1;
CHARTYPE *lastPattern = NULL;

void prep(CHARTYPE *pattern, unsigned mval, unsigned kval, unsigned qval) {
   if(lastPattern==pattern)
      freeArrays();
   else lastPattern = pattern;

   unsigned long stringNumber;
   unsigned int shift;
   int a, i, j, increment;

   m = mval;
   k = kval;
   x = qval;

   memcpy(P, pattern, m);

   // printf("prep %d %d %d\n", m, k, x);

   power4 = 1;

   for (i = 0; i < k+x; i++)
     power4 *= 4;

   // printf("1 %d\n", power4);

   for (a = 0; a < CHAR_SET; a++)
     h0[a] = 0;

   h0['a'] = 0;
   h0['c'] = 1;
   h0['g'] = 2;
   h0['t'] = 3;
   h0['A'] = 0;
   h0['C'] = 1;
   h0['G'] = 2;
   h0['T'] = 3;

   stringArray = (int *)malloc(sizeof(int)*(k+x));

   // printf("2\n");

   Dkx = (int *)malloc(sizeof(int)*power4);
   MM = (int *)malloc(sizeof(int)*power4);

   // printf("3\n");

   D = (int **)malloc(sizeof(int *)*(k+x+1));

   for (i = 0; i < k+x+1; i++)
     D[i] = (int *)malloc(sizeof(int)*(m+1));

   // printf("edit\n");

   for (stringNumber = 0; stringNumber < power4; stringNumber++) {
     register int i, j;

     // shift = 0;
     shift = 1;

     createArray(stringNumber);

     for (i = 0; i < k+x+1; i++)
       D[i][0] = 0;

     for (j = 1; j < m+1; j++)
       D[0][j] = 0;

     for (j = 1; j < m+1; j++) {
       // printf("\n");

       for (i = 1; i < k+x+1; i++) {
	 if (*(stringArray+i-1) == h0[*(P+j-1)])
	   increment = 0;
	 else
	   increment = 1;

	 D[i][j] = D[i-1][j-1]+increment;

	 // printf("%d ", D[i][j]);
       }
     }

     /* if (k > 0) {
       for (j = 0; j < m+1; j++) {
	 printf("%d %d  ", j, D[k+x][j]);

	 if (D[k+x][j] <= k)
	   if (j == m)
	     shift = 1;
	   else
	     shift = m - j;
       }
     } else
       shift = 1; */

     // printf("\n");

     for (j = 0; j < m; j++) {
       // printf("%d %d  ", j, D[k+x][j]);

       if (D[k+x][j] <= k)
	 if (j == m)
	   shift = 1;
	 else if (j == 0 && k == 0)
	   shift = 1;
	 else
	   shift = m - j;
     }
     if (shift<1) shift=1;
     Dkx[stringNumber] = shift;
     MM[stringNumber] = D[k+x][m];
   }

   // printf("8\n");
}

int exec(CHARTYPE *base, unsigned n, unsigned kval, unsigned qval) {

   unsigned long index, power;
   int e, h, i, j, occurences, position;

   unsigned int k = kval;

   j = m-1;
   occurences=0;

   while (j < n) {

     index = 0;
     power = 1;

     for (position = j; position >= j-k-x+1; position--) {
       // printf("index %d %d %d %d\n", index, power, *(base+position), h0[*(base+position)]);

       index += h0[*(base+position)]*power;
       power *= 4;
     }

     // printf("index %d %d\n", index, Dkx[index]);

     e = MM[index];

     h = j-k-x;
     i = m-k-x-1;

     while (i >= 0 && e <= k) {
       if (*(base+h) != *(P+i)) e++;

       i--;
       h--;
     }

     if (e <= k)
       occurences++;
     j += Dkx[index];
   }

   // printf("9\n");

   //free(stringArray);

   // printf("10\n");

   //free(Dkx);

   //free(MM);

   // printf("11\n");

   //for (i = 0; i < k+x+1; i++)
    // free(D[i]);

   //free(D);

   // printf("12\n");

   return occurences;
}

void freeArrays()
{
  free(stringArray);
  free(Dkx);
  free(MM);
  for(int i = 0; i < k+x+1; i++)
    free(D[i]);
  free(D);
}
