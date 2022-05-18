/* This code implements a Laplace algorithm to perform edge enhancement */
/* of northerly directional edges in an image. */

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


main()
{
  int i, j;
  int image1[100][100], image2[100][100];


  int temp;

  for(i=0;i<10;i++)
    for(j=0;j<10;j++){
	if (i+j < 2*i)
      		image1[i][j]=i*(i+5)-j*2;
	else
		image1[i][j]=i+j;
    }
  

  for (i=1; i<MAX; i++) {
    for (j=1; j<MAX; j++) {
      image2[i][j] = image1[i-1][j-1] +
		     -2 * image1[i][j-1] +
		     image1[i+1][j-1] +
		     -2 * image1[i-1][j] +
		     4 * image1[i][j] +
		     -2 * image1[i+1][j] +
		     image1[i-1][j+1] +
		     -2 * image1[i][j+1] +
		     image1[i+1][j+1];
    }
  }
      DEBUG_PRINT(image2[2][3]);
}  /* main */

