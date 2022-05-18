#include <stdio.h>

extern void DEBUG_PRINT(int x);

main()
{
 
    long k , l , ipnt , ipntp , i;
    long loop, n, ii;
    int x[1001],v[1001];
 
    /*
     *******************************************************************
     *   Kernel 2 -- ICCG excerpt (Incomplete Cholesky Conjugate Gradient)
     *******************************************************************
     */
    

    for(i=0;i<10;i++){
      x[i]=v[i]=i;
    }

    loop= 1; n=2;
    for ( l=1 ; l<=loop ; l++ ) {
        ii = n;
        ipntp = 0;
        do {
            ipnt = ipntp;
            ipntp += ii;
            ii /= 2;
            i = ipntp - 1;
            for ( k=ipnt+1 ; k<ipntp ; k=k+2 ) {
                i++;
                x[i] = x[k] - v[k]*x[k-1] - v[k+1]*x[k+1];
            }
        } while ( ii>0 );
    }
    DEBUG_PRINT(x[2]);
}
 
/* End of File */
