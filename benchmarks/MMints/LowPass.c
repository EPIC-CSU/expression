/* This code applies a low-pass filter to an image stored in array a. */
/* Low-pass filters accentuate low frequencies in an image---that is, */
/* the resulting image has lower changes between neighboring color values */
/* This code has been parameterized for arbitrary coefficients */


/* 
-- Submitted By: David Kolson (dkolson@ics.uci.edu) - 08 Dec 94
--
-- Verification Information:
--
--                  Verified     By whom?           Date         Simulator
--                  --------   ------------        --------     ------------
--  Syntax            yes     Preeti R. Panda      17 Jan 95         -
--  Functionality     yes     David Kolson             ?             -
*/

#include <stdio.h>

/* #define _SUN_  */

#ifdef _SUN_
void DEBUG_PRINT(int v ){
  printf("%d\n",v);
}
#else
void DEBUG_PRINT(int v );
#endif


#define	MAX	5 /*100*/
 
 main ()
 {
   int i, j;
   int c0=1, c1=2, c2=3, c3=4, c4=5, c5=6, c6=7, c7=8, c8=9;
   int a[100][100];/*MAX*/
   int temp;

   for(i=0;i<10;i++){
     for(j=0;j<10;j++){
       a[i][j]=i+j;
     }
   }

 
   for (i=1; i<MAX; i++) {
     for (j=1; j<MAX; j++) {
        a[i][j] = c0 * a[i-1][j-1] + c1 * a[i-1][j] + c2 * a[i-1][j+1] +
                  c3 * a[i][j-1]   + c4 * a[i][j]   + c5 * a[i][j+1]   +
                  c6 * a[i+1][j-1] + c7 * a[i+1][j] + c8 * a[i+1][j+1];
     }
   }
	DEBUG_PRINT(a[1][1]);
 }
 

