#include <stdio.h>

extern void DEBUG_PRINT(int x);
 
main()
{
        register int    nl1, nl2;
        register int    i, kx, ky;
        int    cksum[21];
        int  a11, a12, a13, sig, a21, a22, a23, a31, a32, a33;
        register int du1, du2, du3;
        int  u1[6][23][3], u2[6][23][3], u3[6][23][3];
	int j, k;
        a11 = 5;
        a12 = 33;      a13 = 25;       sig = 8;
        a21 = 20;      a22 = 167;      a23 = 141;
        a31 = 125;     a32 = 111;      a33 = 10;
 

	for(i=0;i<6;i++)
          for(j=0;j<4;j++)
            for(k=0;k<3;k++)
              u1[i][j][k]=u2[i][j][k]=u3[i][j][k]=i;


/* loop 8      p.d.e. integration */
 
        nl1 = 1;        nl2 = 2;
        for (kx=2; kx<=3; kx++)  {
                for (ky=2; ky<=2; ky++) {
                        du1 = u1[kx][ky+1][nl1]-u1[kx][ky-1][nl1];
                        du2 = u2[kx][ky+1][nl1]-u2[kx][ky-1][nl1];
                        du3 = u3[kx][ky+1][nl1]-u3[kx][ky-1][nl1];
                        u1[kx][ky][nl2] = u1[kx][ky][nl1]+a11*du1+
                                a12*du2+a13*du3+sig*(u1[kx+1][ky][nl1]
                                -2*u1[kx][ky][nl1]+u1[kx-1][ky][nl1]);
                        u2[kx][ky][nl2] = u2[kx][ky][nl1]+a21*du1+
                                a22*du2+a23*du3+sig*(u2[kx+1][ky][nl1]
                                -2*u2[kx][ky][nl1]+u2[kx-1][ky][nl1]);
                        u3[kx][ky][nl2] = u3[kx][ky][nl1]+a31*du1+
                                a32*du2+a33*du3+sig*(u3[kx+1][ky][nl1]
                                -2*u3[kx][ky][nl1]+u3[kx-1][ky][nl1]);
                        }
                }
        DEBUG_PRINT(u3[2][2][2]);
}
