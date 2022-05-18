#include <stdio.h>

extern void DEBUG_PRINT(int x);

main()
{
    long argument , k , l , i, j;
    int px[101][25], vy[101][25], cx[101][25];

    for(i=0;i<11;i++)
      for(j=0;j<2;j++){
        px[i][j]=vy[i][j]=cx[i][j]=i+j;
      }

    for ( l=1 ; l<=1 ; l++ ) {
        for ( k=0 ; k<1 ; k++ ) {
            for ( i=0 ; i<2 ; i++ ) {
                for ( j=0 ; j<2; j++ ) {
                    px[j][i] += vy[k][i] * cx[j][k];
                }
            }
        }
    }
    DEBUG_PRINT(px[2][1]);
}
