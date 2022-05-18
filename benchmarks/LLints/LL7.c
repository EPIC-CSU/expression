#include <stdio.h>

extern void DEBUG_PRINT(int x);

main()
{
        register int    m;
        int  cksum[21];
        int  r, t;
        int  x[1002], y[1002], z[1002], u[501];

        r = 4;        t = 276;        


	for(m=0;m<10;m++){
          x[m]=y[m]=z[m]=u[m]=m;
        }



 
/* loop 7      equation of state excerpt */
 
        for (m=1; m<=2; m++)
                x[m] = u[m]+r*(z[m]+r*y[m])+
                       t*(u[m+3]+r*(u[m+2]+r*u[m+1])+
                       t*(u[m+6]+r*(u[m+5]+r*u[m+4])));
 
        DEBUG_PRINT(x[6]);
}
