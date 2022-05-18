#include <stdio.h>

extern void DEBUG_PRINT(int x);
 
main()
{
        register int    i;
        int  x[1002], y[1002], z[1002];
 
	for(i=0;i<10;i++){
          x[i]=y[i]=z[i]=i;
        }


/* loop 5      tri-diagonal elimination, below diagonal */
 
        for (i=2; i<=9; i+=1)  {
                x[i] = z[i]*(y[i]-x[i-1]);
                }
        DEBUG_PRINT(x[9]);
 
}
