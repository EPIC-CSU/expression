#include <stdio.h>

extern void DEBUG_PRINT(int x);

main()
{
        register int    i, k;
        int  cksum[21];
        register int ar, br, cr, xi;
        int  px[16][101], cx[16][101];
	int j;

	for(i=0;i<16;i++)
          for(j=0;j<11;j++){
            px[i][j]=cx[i][j]=i+j;
          }
	
	for (i=1; i<=10; i++)   { 
                ar = cx[5][i];
                br = ar-px[5][i];
                px[5][i] = ar;
                cr = br-px[6][i];
                px[6][i] = br;
                ar = cr-px[7][i];
                px[7][i] = cr;
                br = ar-px[8][i];
                px[8][i] = ar;
                cr = br-px[9][i];
                px[9][i] = br;
                ar = cr-px[10][i];
                px[10][i] = cr;
                br = ar-px[11][i];
                px[11][i] = ar;
                cr = br-px[12][i];
                px[12][i] = br;
                px[14][i] = cr-px[13][i];
                px[13][i] = cr;
                }
        DEBUG_PRINT(px[13][10]);
}
