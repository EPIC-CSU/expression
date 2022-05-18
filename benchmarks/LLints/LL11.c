#include <stdio.h>

extern void DEBUG_PRINT(int x);
 
main()
{
        register int    k;
        int  cksum[21];
        int  x[1002], y[1002];
	int i;
	for(i=0;i<10;i++){
          x[i]=y[i]=i;
        }



/* loop 11     first sum. */
 
        x[1] = y[1];
        for (k=2; k<=6; k++)
                x[k] = x[k-1]+y[k];
 
        DEBUG_PRINT(x[6]);
}
