#include <stdio.h>

extern void DEBUG_PRINT(int x);

main()
{
  int j, l, kn, jn, k, loop, n;
  int s,t;
  int za[7][101], zb[7][101], zm[7][101], zp[7][101], zq[7][101];
  int zr[7][101], zu[7][101], zv[7][101], zz[7][101];
  int i;


  for(i=0;i<7;i++){
    for(j=0;j<11;j++){
      za[i][j]=zb[i][j]=zm[i][j]=zp[i][j]=zq[i][j]=zr[i][j]=zu[i][j]=zv[i][j]=zz[i][j]=i+j;
    }
  }



  /*
   *******************************************************************
   *   Kernel 18 - 2-D explicit hydrodynamics fragment
   *******************************************************************
   */

  loop= 2;
  n= 10;
  for ( l=1 ; l<=loop ; l++ ) 
  {
    t = 3;
    s = 4;
    kn = 2;
    jn = n;
    for ( k=1 ; k<kn ; k++ ) 
    {
      for ( j=1 ; j<jn ; j++ ) 
      {
        za[k][j] = ( zp[k+1][j-1] +zq[k+1][j-1] -zp[k][j-1] -zq[k][j-1] )*
                   ( zr[k][j] +zr[k][j-1] ) / ( zm[k][j-1] +zm[k+1][j-1]);
        zb[k][j] = ( zp[k][j-1] +zq[k][j-1] -zp[k][j] -zq[k][j] ) *
                   ( zr[k][j] +zr[k-1][j] ) / ( zm[k][j] +zm[k][j-1]);
      }
    }
    for ( k=1 ; k<kn ; k++ ) 
    {
      for ( j=1 ; j<jn ; j++ ) 
      {
        zu[k][j] += s*( za[k][j]   *( zz[k][j] - zz[k][j+1] ) -
                        za[k][j-1] *( zz[k][j] - zz[k][j-1] ) -
                        zb[k][j]   *( zz[k][j] - zz[k-1][j] ) +
                        zb[k+1][j] *( zz[k][j] - zz[k+1][j] ) );
        zv[k][j] += s*( za[k][j]   *( zr[k][j] - zr[k][j+1] ) -
                        za[k][j-1] *( zr[k][j] - zr[k][j-1] ) -
                        zb[k][j]   *( zr[k][j] - zr[k-1][j] ) +
                        zb[k+1][j] *( zr[k][j] - zr[k+1][j] ) );
      }
    }
    for ( k=1 ; k<kn ; k++ ) 
    {
      for ( j=1 ; j<jn ; j++ ) 
      {
        zr[k][j] = zr[k][j] + t*zu[k][j];
        zz[k][j] = zz[k][j] + t*zv[k][j];
      }
    }
  }
  DEBUG_PRINT(zz[5][6]);
}
