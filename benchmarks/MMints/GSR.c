/* Red-black Gauss-Seidel relaxation method taken from */
/* Numerical Recipes in C, 2nd edition page 881        */

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

/* #define _SUN_ */ 

#ifdef _SUN_
void DEBUG_PRINT(int v ){
  printf("%d\n",v);
}
#else
void DEBUG_PRINT(int v );
#endif




#define MAX 5 /*100*/


main()
{
  int u[100][100], rhs[100][100];
  int n=MAX;

  int i, ipass, isw, j, jsw=1;
  int h, h2;
  int temp;

  for(i=0;i<20;i++){
    for(j=0;j<20;j++){
      u[i][j]=rhs[i][j]=i;
    }
  }

  h = 0;
  h2 = h*h;
  for (ipass=1; ipass<=2; ipass++, jsw=3-jsw) {    /* Red and black sweeps */
    isw = jsw;
    for (j=2; j<n; j++, isw=3-isw)
      for (i=isw+1; i<n; i+=2){
	/* the Gauss-Seidel formula */
	u[i][j] = 2 * (u[i+1][j]+u[i-1][j]+u[i][j+1]+u[i][j-1]-h2*rhs[i][j]);
      }
  }
	DEBUG_PRINT(u[3][2]);
}  /* relax */




