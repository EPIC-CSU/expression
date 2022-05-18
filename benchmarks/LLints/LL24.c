#include <stdio.h>

extern void DEBUG_PRINT(int x);

main() 
{
    long  k , l , m, n;
    int x[1001];
    int i;
    /*
     *******************************************************************
     *   Kernel 24 -- find location of first minimum in array
     *******************************************************************
     */
 
    for(i=0;i<11;i++){
      x[i]=i;
    }

    n=10;
    x[n/2] = 1000;
    for ( l=1 ; l<=2 ; l++ ) {
        m = 0;
        for ( k=1 ; k<n ; k++ ) {
            if ( x[k] < x[l] ) m = k;
        }
    }
    DEBUG_PRINT(m);
}
