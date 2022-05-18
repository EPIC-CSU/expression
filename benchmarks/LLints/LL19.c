#include <stdio.h>

extern void DEBUG_PRINT(int x);

main()
{ 
    long argument , k , l , i, kb5i;
    int sa[101], sb[101], b5[101], stb5;
 
    for(i=0;i<11;i++)
      sa[i]=sb[i]=b5[i]=i;

    kb5i = 0;
    stb5 = 0;

    for ( l=1 ; l<=2 ; l++ ) {
        for ( k=0 ; k<10 ; k++ ) {
            b5[k+kb5i] = sa[k] + stb5*sb[k];
            stb5 = b5[k+kb5i] - stb5;
        }
        for ( i=1 ; i<=10 ; i++ ) {
            k = 101 - i ;
            b5[k+kb5i] = sa[k] + stb5*sb[k];
            stb5 = b5[k+kb5i] - stb5;
        }
    }
    DEBUG_PRINT(b5[7]);
}
