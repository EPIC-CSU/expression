/* This is a Successive Over-Relaxation (SOR) algorithm found in the */
/* Numerical Recipies in C book, page 869. It is used in evaluating  */
/* partial differential equations.

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




#define N 5 /*100*/


main()
{
  int a[10][10], b[10][10], c[10][10], d[10][10], e[10][10], f[10][10];/*N*/
  int omega=1, resid=2, u[10][10]; /*N*/
  int j, l;
  int temp,i;

  for(i=0;i<10;i++){
    for(j=0;j<10;j++){
      a[i][j]=b[i][j]=c[i][j]=d[i][j]=e[i][j]=f[i][j]=u[i][j]=i;
    }
  }
  

  for (j=2; j<N; j++)
    for (l=1; l<N; l+=2) {
      resid = a[j][l]*u[j+1][l] +
	      b[j][l]*u[j-1][l] +
	      c[j][l]*u[j][l+1] +
	      d[j][l]*u[j][l-1] +
	      e[j][l]*u[j][l]   -
	      f[j][l];
      u[j][l] -= omega*resid/e[j][l];
    }
      DEBUG_PRINT(u[3][3]);
}  /* main */
