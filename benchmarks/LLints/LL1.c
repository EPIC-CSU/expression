/*#include <stdio.h> */

extern void DEBUG_PRINT(int x);

main()
{
        register int    k;
        int  cksum[21];
        int  x[1002], y[1002], z[1002];
        int  r, t;
        register int q;

        r = 4;        t = 276;        
 
/* loop 1      hydro excerpt */
 
        q = 0;

	for(k=1; k<=20; k++){
          x[k]=y[k]=z[k]=k;
        }



        for (k=1; k<=4; k++)
          {
                x[k] = q+y[k]*(r*z[k+10]+t*z[k+11]);
          }

        DEBUG_PRINT(x[4]);
}
