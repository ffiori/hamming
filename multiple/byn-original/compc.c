/* This code is from Gonzalo Navarro, 1998.
   It can be freely used for academic teaching/learning and research
   purposes. Usage for direct or indirect commercial advantage is not
   allowed without written permission from the authors.
*/


#include <stdio.h>

main ()

  { double h[256];
    int i;
    double s,l;

    for (i=0;i<256;i++) h[i] = 0;
    l = 0;

    while (!feof(stdin))
	{ h[(unsigned char)getchar()]++;
          l++;
	}
    
    s = 0; 
    for (i=0;i<256;i++) s += h[i]*h[i];

    printf ("sigma = %f\n",l*l/s);
  }
