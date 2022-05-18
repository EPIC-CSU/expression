#include <stdio.h>

extern void DEBUG_PRINT(int x);

main()
{
  int loop, n;
  int l, ng, nz, j, k;
  int ar, br, s, t, r;
  int vf[7][101], vg[7][101], vh[7][101], vs[7][101], vy[25][100];

  int i,tmp;

  for(i=0;i<7;i++)
    for(j=0;j<11;j++){
      vf[i][j]=vg[i][j]=vh[i][j]=vs[i][j]=vy[i][j]=i+j;
    }


  /*
   *******************************************************************
   *   Kernel 15 -- Casual Fortran.  Development version
   *******************************************************************
   */

  loop= 1;
  n= 10;
  for ( l=1 ; l<=loop ; l++ )
  {
    ng = 3;
    nz = n;
    ar = 5;
    br = 7;
    for ( j=1 ; j<ng ; j++ )
    {
      for ( k=1 ; k<nz ; k++ )
      {
        if ( (j+1) >= ng )
        {
          vy[j][k] = 0;
          continue;
        }
        if ( vh[j+1][k] > vh[j][k] )
        {
          t = ar;
        }
        else
        {
          t = br;
        }
        if ( vf[j][k] < vf[j][k-1] )
        {
          if ( vh[j][k-1] > vh[j+1][k-1] )
            r = vh[j][k-1];
          else
            r = vh[j+1][k-1];
          s = vf[j][k-1];
        }
        else
        {
          if ( vh[j][k] > vh[j+1][k] )
            r = vh[j][k];
          else
            r = vh[j+1][k];
          s = vf[j][k];
        }
        vy[j][k] = ( (int)vg[j][k]*vg[j][k] + r*r )* t * s;
        if ( (k+1) >= nz )
        {
          vs[j][k] = 0;
          continue;
        }
        if ( vf[j][k] < vf[j-1][k] )
        {
          if ( vg[j-1][k] > vg[j-1][k+1] )
            r = vg[j-1][k];
          else
            r = vg[j-1][k+1];
          s = vf[j-1][k];
          t = br;
        }
        else
        {
          if ( vg[j][k] > vg[j][k+1] )
            r = vg[j][k];
          else
            r = vg[j][k+1];
          s = vf[j][k];
          t = ar;
        }
        vs[j][k] = ( (int)vh[j][k]*vh[j][k] + r*r )* t * s;
      }
    }
  }
  DEBUG_PRINT(vs[6][5]);
}
