#include <stdio.h>

extern void DEBUG_PRINT(int x);
 
main()
{
         int    k;
        int  cksum[21];
         int q;
        int  x[1002], z[1002];

	 for(k=0;k<10;k++)
{
          x[k]=z[k]=k;
}

 
/* loop 3      inner prod */
 
        q = 0;
        for (k=1; k<=3; k++)
          {
            q += z[k]*x[k];
          }
        cksum[3] = q;

        DEBUG_PRINT(cksum[3]);
}
