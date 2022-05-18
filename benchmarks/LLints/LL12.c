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

/* loop 12     first diff. */
 
        for (k=1; k<=9; k++)
                x[k] = y[k+1]-y[k];
 
        DEBUG_PRINT(x[6]);
}
