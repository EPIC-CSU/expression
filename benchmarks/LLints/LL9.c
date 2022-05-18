#include <stdio.h>

extern void DEBUG_PRINT(int x);

main()
{
        register int    i;
        int  cksum[21];
        int  bm28, bm27, bm26, bm25, bm24, bm23, bm22, c0;
        int  px[16][101];
	int j;

        bm28 = 1;      bm27 = 2;       bm26 = 3;
        bm25 = 4;      bm24 = 5;       bm23 = 6;
        bm22 = 7;      c0 = 8;        

	for(i=0;i<16;i++){
          for(j=0;j<11;j++){
            px[i][j]=i+j;
          }
        }

/* loop 9      integrate predictors */
 
        for (i=1; i<=10; i++)
                px[1][i] = bm28*px[13][i] + bm27*px[12][i] +
                        bm26*px[11][i] + bm25*px[10][i] + bm24*px[9][i] +
                        bm23*px[8][i] + bm22*px[7][i] +
                        c0*(px[5][i] + px[6][i]) + px[3][i];
 
        DEBUG_PRINT(px[1][10]);
}
