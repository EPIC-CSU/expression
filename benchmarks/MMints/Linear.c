/* This code implements a general linear recurrence solver. */

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



main()
{
    long argument , k , l , i, kb5i;
    int sa[101], sb[101], b5[101], stb5=0;
    int temp;

    for(i=0;i<10;i++){
      sa[i]=sb[i]=b5[i]=i;
    }


    kb5i = 0;
    for ( l=1 ; l<=2 ; l++ ) { /*1000*/
        for ( k=0 ; k<5 ; k++ ) { /*101*/
            b5[k+kb5i] = sa[k] + stb5*sb[k];
            stb5 = b5[k+kb5i] - stb5;
        }
        for ( i=1 ; i<=5 ; i++ ) { /*101*/
            k = 5 - i ; /*101*/
            b5[k+kb5i] = sa[k] + stb5*sb[k];
            stb5 = b5[k+kb5i] - stb5;
        }
    }
	    DEBUG_PRINT(stb5);
}
