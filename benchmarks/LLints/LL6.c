#include <stdio.h>

extern void DEBUG_PRINT(int x);

main()
{
  int b[64][64], w[1001];
  long  l, i, k;
  long loop, n;
 
   for(i=0;i<6;i++){
    for(l=0;l<6;l++){
      b[i][l]=l;
    }
   }

   for(i=0;i<10;i++){
     w[i]=i;
   }

  /*
  *******************************************************************
  *   Kernel 6 -- general linear recurrence equations
  *******************************************************************
  */
 
  for ( l=1 ; l<=2 ; l++ ) {
      for ( i=1 ; i<6 ; i++ ) {
          for ( k=0 ; k<i ; k++ ) {
              w[i] += b[k][i] * w[(i-k)-1];
          }
      }
  }
  DEBUG_PRINT(w[6]);
}
