#include <stdio.h>

extern void DEBUG_PRINT(int x);

main()
{
    long argument , k , l , j , i;
    int qa;
    int za[7][101], zr[7][101], zb[7][101];
    int zu[7][101], zv[7][101], zz[7][101];

    for(i=0;i<7;i++)
      for(j=0;j<11;j++){
        za[i][j]=zr[i][j]=zb[i][j]=zu[i][j]=zv[i][j]=zz[i][j]=i+j;
      }
 
    for ( l=1 ; l<=2 ; l++ ) {
        for ( j=1 ; j<6 ; j++ ) {
            for ( k=1 ; k<10 ; k++ ) {
                qa = za[j+1][k]*zr[j][k] + za[j-1][k]*zb[j][k] +
                     za[j][k+1]*zu[j][k] + za[j][k-1]*zv[j][k] + zz[j][k];
                za[j][k] += 1*( qa - za[j][k] );
            }
        }
    }
    DEBUG_PRINT(za[5][6]);
}
