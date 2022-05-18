#include <stdio.h>

extern void DEBUG_PRINT(int x);

main()
{
        register int    loop, n, k, l;
        int  flx, rx1;
        int ix[1001], ir[1001];
        int  xi[1001], ex[1001], rx[1001], rh[1001];
        int  ex1[1001], dex[1001], dex1[1001], vx[1001], xx[1001], grd[1001];
	int i;

        flx = 4;
        rx1 = 6;
        loop= 2;
        n= 10;

	for(i=0;i<11;i++){
          xi[i]=ix[i]=ir[i]=ex[i]=rx[i]=rh[i]=ex1[i]=dex[i]=dex1[i]=vx[i]=xx[i]=grd[i]=i;
        }


 
/* loop 14      1-d particle pusher */
 
    for ( l=1 ; l<=loop ; l++ ) {
        for ( k=0 ; k<n ; k++ ) {
            vx[k] = 0;
            xx[k] = 0;
            ix[k] = grd[k];
            xi[k] = ix[k];
            ex1[k] = ex[ ix[k] - 1 ];
            dex1[k] = dex[ ix[k] - 1 ];
        }
        for ( k=0 ; k<n ; k++ ) {
            vx[k] = vx[k] + ex1[k] + ( xx[k] - xi[k] )*dex1[k];
            xx[k] = xx[k] + vx[k]  + flx;
            ir[k] = xx[k];
            rx[k] = xx[k] - ir[k];
            ir[k] = ( ir[k] & 2048-1 ) + 1;
            xx[k] = rx[k] + ir[k];
        }
        for ( k=0 ; k<n ; k++ ) {
            rh[ ir[k]-1 ] += 1 - rx[k];
            rh[ ir[k]   ] += rx[k];
        }
    }
    DEBUG_PRINT(rh[6]);
}
