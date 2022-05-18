#include <stdio.h>

extern void DEBUG_PRINT(int x);

main()
{
    long argument, n, k , l;
    int x[1001], y[1001], u[1001], v[1001], w[1001];
    int expmax;
    int i;

    for(i=0;i<11;i++){
      x[i]=y[i]=u[i]=v[i]=w[i]=i+10;
    }



    n= 10;
    expmax = 20;
    u[n-1] = 9*expmax*v[n-1];
    for ( l=1 ; l<=2 ; l++ ) {
        for ( k=0 ; k<n ; k++ ) {
	  /*            y[k] = u[k] / v[k];
			w[k] = x[k] / ( exp( y[k] ) -1 );*/

	    y[k] = u[k] * v[k];
            w[k] = x[k] * ( y[k] - 1 );

        }
    }
    DEBUG_PRINT(w[9]);
}
