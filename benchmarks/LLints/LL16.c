#include <stdio.h>

extern void DEBUG_PRINT(int x);

main()
{
 
  int n, loop, i, ii, lb, k2, k3, l, m, i1, j2, k, j4, j5;
  int zone[300];
  int plan[300], d[300], tmp;
  int r, s, t;

  /*
   *******************************************************************
   *   Kernel 16 -- Monte Carlo search loop
   *******************************************************************
   */

   for ( i=0; i < 30; i++ ) {
        zone[i] = 1;
        /***************
	zone[0] = 1; zone[1] = 1; zone[2] = 1; zone[3] = 1; zone[4] = 1;
	zone[5] = 1; zone[6] = 1; zone[7] = 1; zone[8] = 1; zone[9] = 1;
	zone[10] = 1; zone[11] = 1; zone[12] = 1; zone[13] = 1; zone[14] = 1;
	zone[15] = 1; zone[16] = 1; zone[17] = 1; zone[18] = 1; zone[20] = 1;
	zone[21] = 1; zone[22] = 1; zone[23] = 1; zone[24] = 1; zone[25] = 1;
	zone[26] = 1; zone[27] = 1; zone[28] = 1; zone[30] = 1; zone[31] = 1;
	zone[32] = 1; zone[33] = 1; zone[34] = 1; zone[35] = 1; zone[36] = 1;
	zone[37] = 1; zone[38] = 1; zone[39] = 1;
        *****/
   }

  loop= 2;
  n= 10;
  ii = n / 3;
  lb = ii + ii;
  k3 = k2 = 0;
  for ( l=1 ; l<=loop ; l++ ) 
  {
    i1 = m = 1;
	do{
    j2 = ( n + n )*( m - 1 ) + 1;
    for ( k=1 ; k<=n ; k++ ) 
    {
      k2++;
      j4 = j2 + k + k;
      j5 = zone[j4-1];
      if ( j5 < n ) 
      {
        if ( j5+lb < n ) 
        {              /* 420 */
          tmp = plan[j5-1] - t;       /* 435 */
        } 
        else 
        {
          if ( j5+ii < n ) 
          {          /* 415 */
            tmp = plan[j5-1] - s;   /* 430 */
          } 
          else 
          {
            tmp = plan[j5-1] - r;   /* 425 */
          }
        }
      } 
      else if( j5 == n ) 
      {
        break;                          /* 475 */
      } 
      else 
      {
        k3++;                           /* 450 */
        tmp=(d[j5-1]-(d[j5-2]*(t-d[j5-3])*(t-d[j5-3])+(s-d[j5-4])*
            (s-d[j5-4])+(r-d[j5-5])*(r-d[j5-5])));
      }
      if ( tmp < 0 ) 
      {
        if ( zone[j4-2] < 0 )           /* 445 */
          continue;                   /* 470 */
        else if ( !zone[j4-2] )
          break;                      /* 480 */
      } 
      else if ( tmp ) 
      {
        if ( zone[j4-2] > 0 )           /* 440 */
          continue;                   /* 470 */
        else if ( !zone[j4-2] )
          break;                      /* 480 */
      } 
      else break;                       /* 485 */
      m++;                                /* 455 */
      if ( m > zone[0] )
        m = 1;                          /* 460 */
      if ( i1-m )                         /* 465 */
		  break;
      else
        break;
    }
	}
	while(i1-m);
  }
  DEBUG_PRINT(zone[20]);
}
