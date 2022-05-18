#include <stdio.h>

extern void DEBUG_PRINT(int x);

main()
{
  int i,j,l,ink,loop,n;
  int scale,xnm,e6,xnc,e3,xnei;
  int vsp[101],vstp[101],vlin[101],vlr[101],ve3[101],vxne[101],vxnd[101];
    /*
     *******************************************************************
     *   Kernel 17 -- implicit, conditional computation
     *******************************************************************
     */

  for(i=0;i<10;i++){
    vsp[i]=vstp[i]=vlin[i]=vlr[i]=ve3[i]=vxne[i]=vxnd[i] = i;
  }




    loop=2;
    n=10;
    for ( l=1 ; l<=2 ; l++ ) {
        i = n-1;
        j = 0;
        ink = -1;
        scale = 5.0 / 3.0;
        xnm = 1.0 / 3.0;
        e6 = 1.03 / 3.07;
        goto l61;
l60:    e6 = xnm*vsp[i] + vstp[i];
        vxne[i] = e6;
        xnm = e6;
        ve3[i] = e6;
        i += ink;
        if ( i==j ) goto l62;
l61:    e3 = xnm*vlr[i] + vlin[i];
        xnei = vxne[i];
        vxnd[i] = e6;
        xnc = scale*e3;
        if ( xnm > xnc ) goto l60;
        if ( xnei > xnc ) goto l60;
        ve3[i] = e3;
        e6 = e3 + e3 - xnm;
        vxne[i] = e3 + e3 - xnei;
        xnm = e6;
        i += ink;
        if ( i != j ) goto l61;
l62:;
    }
    DEBUG_PRINT(vxne[6]);
}
