/* This code implements an image compression scheme by estimating the */
/* current cell based on the neighbors values. It then stores the difference */
/* between the prediction and the actual value */

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
   unsigned int a[MAX][MAX], prediction;

   int temp;

   for(i=0;i<10;i++)
     for(j=0;j<10;j++){
       a[i][j]=i+j;
     }

 
   for (i=1; i<MAX; i++) {
     for (j=1; j<MAX; j++) {
       prediction =  2 * a[i-1][j-1] + a[i-1][j] + a[i][j-1];
       a[i][j] = a[i][j] - prediction;
     }
   }
       DEBUG_PRINT(a[2][2]);
 }
