#include <stdio.h>

extern void DEBUG_PRINT(int x);

main()
{
        register int    i1, i2, ip, j1, j2;
        int  y[102], z[102];
        int  b[65][9], c[65][9], p[5][102];
        int  h[65][9];
        int     e[193], f[193];
	int i,j;
	
	for(i=0;i<6;i++){
          for(j=0;j<6;j++){
            b[i][j]=c[i][j]=p[i][j]=h[i][j]=i;
          }
        }
          
	for(i=0;i<102;i++){  
	  y[i]=z[i]=i;
	}

	for(i=0;i<130;i++){  
	  e[i]=f[i]=i;
	}



/* loop 13      2-d particle pusher */
 
        for (ip=1; ip<=10; ip++)        {
                i1 = p[1][ip];  
                j1 = p[2][ip];
                p[3][ip] += b[i1][j1];
                p[4][ip] += c[i1][j1];
                p[1][ip] += p[3][ip];
                p[2][ip] += p[4][ip];
                i2 = (int) p[1][ip];
                j2 = (int) p[2][ip];
                p[1][ip] += y[i2+3];
                p[2][ip] += z[j2+3];
                i2 += e[i2+3];         
                j2 += f[j2+3];
                h[i2][j2] += 1;
        }
 
        DEBUG_PRINT(h[5][5]);
}
