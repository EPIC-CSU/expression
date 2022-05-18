#include <stdio.h>

extern void DEBUG_PRINT(int x);

main()
{ 
    long argument , k , l , i, j;
    int di, dk, dn, t, s;
    int g[1001],x[1001],xx[1001],y[1001],z[1001];
    int u[1001],v[1001],w[1001],vx[1001];

    for(i=0;i<11;i++)
      g[i]=x[i]=xx[i]=y[i]=z[i]=u[i]=v[i]=w[i]=vx[i]=i+2;



    /*
     *******************************************************************
     *   Kernel 20 -- Discrete ordinates transport, conditional recurrence on xx
     *******************************************************************
     */
 
	dk = 0;
    t = 0;
    s = 0;

    for ( l=1 ; l<= 2 ; l++ ) {
        for ( k=0 ; k<5 ; k++ ) {
            di = y[k] - g[k] / ( xx[k] + dk );
            dn = 0;
            if ( di ) {
                dn = z[k]/di ;
                if ( t < dn ) dn = t;
                if ( s > dn ) dn = s;
            }
            x[k] = ( ( w[k] + v[k]*dn )* xx[k] + u[k] ) / ( vx[k] + v[k]*dn );
            xx[k+1] = ( x[k] - xx[k] )* dn + xx[k];
        }
    }
    DEBUG_PRINT(xx[3]);
}
