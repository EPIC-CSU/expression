#include <stdio.h>

extern void DEBUG_PRINT(int x);
 
main()
{
        register int    lw;
        register int    j, l;
        int  cksum[21];
        int  x[1002], y[1002];

	for(l=0;l<100;l++){
          x[l]=y[l]=l;
        }



 
/* loop 4      banded linear equarions */
 
        for (l=7; l<=7; l+=50) {
                lw=l;
                for (j=30; j<=70; j+=5)
                        x[l-1] -= x[lw++]*y[j];
                x[l-1] = y[5]*x[l-1];
                }
        DEBUG_PRINT(x[56]);
 
}
