/*****************************************************************\
*                                                                *
*  Copyright (C) Regents of University of California, 2003       *
*  This source code is a part of EXPRESSION project and is       *
*  copyrighted by the Regents of the University of California,   *
*  Irvine.                                                       *
*  The authors hereby grant permission to use this description   *
*  and its documentation for educational and non-commercial      *
*  purpose under the BSD license                                 *
*  (http://www.cecs.uci.edu/~express/BSD_License.txt). 	         *
*  The source code comes with no warranty and no author or       *
*  distributor accepts any responsibility for the consequences   *
*  of its use. Permission is granted to distribute this file in  *
*  compiled or executable form under the same conditions that    *
*  apply for source code. Permission is granted	to anyone to     *
*  make or distribute copies of this source code, either as      *
*  received or modified, in any medium, provided that all        *
*  copyright notices, permission and non warranty notices are    *
*  preserved, and that the distributor grants the recipient      *
*  permission for further redistribution as permitted by this    *
*  document. No written agreement, license, or royalty fee is    *
*  required for authorized use of this software.                 *
*                                                                *
*******************************************************************/
#include "stdafx.h"
#include "GlobalIncludes.h"

#include <iostream.h>

#ifndef WIN32
#include <malloc.h>
#include <stdio.h>
#include <math.h>
#endif

#include "auxiliary.h"
#include "types.h"
#include "intMatrix.h"
#include "myvector.h"

#ifdef WIN32
//#include <stdlib.h>
#endif


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


/* put a one in the MSB of an int (portable) */
#define MSB 1<<(sizeof(int)*8-1)
#define Top MSB-1
#define True 1
#define False 0

#ifndef ASSERT_TRACE
	#define ASSERT_TRACE(c,e) if (!(c)) { cerr << endl << "WOA, NELLIE! " << __FILE__ << "(" << __LINE__ << "): " << e << endl; assert(0); }
#endif

int Factorial(int n)
{
  int fact, i;

  fact=1;
  for (i=1;i<=n;i++)
    fact*=i;
  return fact;
} /* Factorial */


int Binomial(int n,int p)
{
  int prod, i;
  
  if (n-p<p) p=n-p;
  if (p!=0) {
    prod=n-p+1;
    for (i=n-p+2;i<=n;i++)
      prod*=i;
    return prod/Factorial(p);
  } else return 1;
} /* Binomial */


int Gcd(int a,int b)
{
  int aux;
  
  while (a) { aux=b%a; b=a; a=aux; }
  return b;
} /* Gcd */


#define Vector_Copy(p1, p2, length) \
  bcopy((char *)p1, (char *)p2, (int)((length)*sizeof(int)))


#define Vector_Init(p1, length) \
  bzero((char *)(p1), (int)((length)*sizeof(int)))


MyVector *Vector_Alloc(unsigned length)
{
  MyVector *vec;
  vec=(MyVector *)malloc(sizeof(vec));
  vec->Size=length;
  vec->p=(int *)malloc(length * sizeof(int));
  return vec;
} /* Vector_Alloc */


void Vector_Free(MyVector *vec)
{
  free((char *)vec->p);
  free((char *)vec);
} /* Vector_Free */


void Vector_Set(int *p,int n,unsigned length)
{
  int *cp, i;
  
  cp=p;
  for (i=0;i<length;i++)
    *cp++=n;
} /* Vector_Set */


void Vector_Add(int *p1,int *p2,int *p3,unsigned length)
{
  int *cp1, *cp2, *cp3, i;

  cp1=p1;
  cp2=p2;
  cp3=p3;
  for (i=0;i<length;i++)
    *cp3++=*cp1++ + *cp2++;
} /* Vector_Add */


void Vector_Sub(int *p1,int *p2,int *p3,unsigned length)
{
  int *cp1, *cp2, *cp3, i;

  cp1=p1;
  cp2=p2;
  cp3=p3;
  for (i=0;i<length;i++)
    *cp3++= *cp1++ - *cp2++;
} /* Vector_Sub */


void Vector_Or(int *p1,int *p2,int *p3,unsigned length)
{
  int *cp1, *cp2, *cp3, i;

  cp1=p1;
  cp2=p2;
  cp3=p3;
  for (i=0;i<length;i++)
    *cp3++=*cp1++ | *cp2++;
} /* Vector_Or */


void Vector_Scale(int *p1,int *p2,int lambda,unsigned length)
{
  int *cp1, *cp2, i;
  
  cp1=p1;
  cp2=p2;
  for (i=0;i<length;i++) {
    *cp2++=*cp1++ * lambda;
  }
} /* Vector_Scale */


void Vector_AntiScale(int *p1,int *p2,int lambda,unsigned length)
{
  int *cp1, *cp2, i;
  
  cp1=p1;
  cp2=p2;
  for (i=0;i<length;i++) {
    *cp2++=*cp1++ / lambda;
  }
} /* Vector_Scale */


int Vector_Min(int *p,unsigned length)
{
  int *cp, min, i;

  cp=p;
  min=*cp++;
  for (i=1;i<length;i++) {
    if (min<*cp) min=*cp;
    cp++;
  }
  return min;
} /* Vector_Min */


int Vector_Max(int *p,unsigned length)
{
  int *cp, max, i;
  
  cp=p;
  max=*cp++;
  for (i=1;i<length;i++)
    if (max>*cp) max=*cp;
  return max;
} /* Vector_Max */


int Inner_Product(int *p1,int *p2,unsigned length)
{
  int i, ip;

  ip=*p1++ * *p2++;
  for (i=1;i<length;i++)
    ip*= *p1++ * *p2++;

  return ip;
} /* Inner_Product */

#ifdef CHECK_OVERFLOW

void Vector_Combine(int *p1,int *p2,int *p3,int lambda,int mu,unsigned length)
{ 
  int *cp1, *cp2, *cp3, t1, t2, t3, t4, t5, i;

  cp1=p1;
  cp2=p2;
  cp3=p3;
  for (i=0;i<length;i++)
  {  t4 = *cp1++;
     t5 = *cp2++;
     t1 = lambda * t4;
     t2 = mu * t5;
     if ( (t4 && (lambda != t1/t4)) ||
          (t5 && (mu     != t2/t5)) )
     {   /* multiplication overflow */
	fprintf(stderr, "? Vector_Combine: multiplication overflow\n");
     }
     t3 = t1 + t2;
     if (t1 != t3 - t2)
     {  /* addition overflow */
        fprintf(stderr, "? Vector_Combine: addition overflow\n");
     }
     *cp3++ = t3;
  }
} /* Vector_Combine */

#else

void Vector_Combine(int *p1,int* p2,int* p3,int lambda,int mu,unsigned length)
{
  int *cp1, *cp2, *cp3, i;

  cp1=p1;
  cp2=p2;
  cp3=p3;
  for (i=0;i<length;i++)
    *cp3++=lambda * *cp1++ + mu * *cp2++;
} /* Vector_Combine */

#endif

int Vector_Min_Not_Zero(int *p,unsigned length,int *index)
{
  int *cp, min, aux, i;
  
  cp=p;
  min=Top;
  for (i=0;i<length;i++) {
    if (*cp&&((aux=abs(*cp))<min)) {
      min=aux;
      *index=i;
    }
    cp++;
  }
  if (min==Top) min=1;
  return min;
} /* Vector_Min_Not_Zero */


int Vector_Gcd(int *p,int *q,unsigned length)
{
  int *cq, *cp, min, Index_Min, Not_Zero, i;

  cp=p;
  for (cq=q,i=0;i<length;i++)
    *cq++=abs(*cp++);
  do {
    if ((min=Vector_Min_Not_Zero(q, length, &Index_Min))!=1) {
      cq=q;
      Not_Zero=False;
      for (i=0;i<length;i++,cq++)
        if (i!=Index_Min)
          Not_Zero|=(*cq%=min);
    } else break;
  } while (Not_Zero);
  return min;
} /* Vector_Gcd */

void Vector_Normalize(int *p,int *q,unsigned length)
{
  int *cp, gcd, i;

  if ((gcd=Vector_Gcd(p, q, length))>=2) {
    cp=p;
    for (i=0;i<length;i++)
      *cp++/=gcd;
  }
} /* Vector_Normalize */


void Vector_Map(int *p1,int *p2,int *p3,unsigned length,int (*f)(int, int))
{
  int *cp1, *cp2, *cp3, i;
  
  cp1=p1;
  cp2=p2;
  cp3=p3;
  for (i=0;i<length;i++)
    *cp3=(*f)(*cp1++, *cp2++);
}/*  Vector_Map */


int Vector_Reduce(int *p,unsigned length,int (*f)(int, int))
{
  int *cp, i, r;
  
  cp=p;
  r=*cp++;
  for(i=1;i<length;i++)
    r=(*f)(r, *cp++);
  return r;
}  /*Vector_Insert */


chernikovaMatrix *Matrix_Alloc(unsigned NbRows,unsigned NbColumns)
{
  chernikovaMatrix *Mat;
  int **q, *p, i;

  Mat=(chernikovaMatrix *)malloc(sizeof(chernikovaMatrix));
  Mat->NbRows=NbRows;
  Mat->NbColumns=NbColumns;
  Mat->p=q=(int **)malloc(NbRows * sizeof(int *));
  Mat->p_Init=p=(int *)malloc(NbRows * NbColumns * sizeof(int));
  for (i=0;i<NbRows;i++) {
    *q++=p;
    p+=NbColumns;
  }
  return Mat;
} /* Matrix_Alloc */


void Matrix_Free(chernikovaMatrix *Mat)
{
  free((char *) Mat->p_Init);
  free((char *) Mat->p);
  free((char *) Mat);
} /* Matrix_Free */


void Matrix_Vector_Product(chernikovaMatrix *mat,int *p1,int *p2)
{
  int **cm, *q, *cp1, *cp2, NbRows, NbColumns, i, j;

  NbRows=mat->NbRows;
  NbColumns=mat->NbColumns;
  
  cm=mat->p;
  cp2=p2;
  for (i=0;i<NbRows;i++) {
    q=*cm++;
    cp1=p1;
    *cp2=*q++ * *cp1++;
    for (j=1;j<NbColumns;j++)
      *cp2+= *q++ * *cp1++;
    cp2++;
  }
} /* Matrix_Vector_Product */


void Vector_Matrix_Product(int *p1,chernikovaMatrix *mat,int *p2)
{
  int **cm, *cp1, *cp2, NbRows, NbColumns, i, j;
  
  NbRows=mat->NbRows;
  NbColumns=mat->NbColumns;
  cp2=p2;
  cm=mat->p;
  for (j=0;j<NbColumns;j++) {
    cp1=p1;
    *cp2=*(*cm+j) * *cp1++;
    for (i=1;i<NbRows;i++)
      *cp2+=*(*(cm+i)+j) * *cp1++;
    cp2++;
  }
} /* Vector_Matrix_Product */


void Matrix_Product(chernikovaMatrix *mat1,chernikovaMatrix *mat2,chernikovaMatrix *mat3)
{
  int **q1, **q2, *p1, *p3, Size, sum, i, j, k;
  unsigned NbRows, NbColumns;

  /* mat3 = Matrix_Alloc(NbRows=mat1->NbRows, NbColumns=mat2->NbColumns); */
  Size=mat1->NbColumns;
  
  p3=mat3->p_Init;
  q1=mat1->p;
  q2=mat2->p;
  for (i=0;i<NbRows;i++) {
    for (j=0;j<NbColumns;j++) {
      p1=*(q1+i);
      sum=0;
      for (k=0;k<Size;k++)
        sum+=*p1++ * *(*(q2+k)+j);
      *p3++=sum;
    }
  }
} /* Vector_Matrix_Product */


void Vector_Print(char *Format,MyVector *Vec)
{
  int i, *p;
  unsigned length;
  
  (void) fprintf(stderr, "%d\n", length=Vec->Size);
  p=Vec->p;
  for (i=0;i<length;i++)
    (void) fprintf(stderr, Format, *p++);
  (void) fprintf(stderr, "\n");
} /* Vector_Print */


MyVector *Vector_Read()
{
  MyVector *Vec;
  unsigned length;
  int *p, i;

  (void) scanf("%d", &length);
  Vec = Vector_Alloc(length);
  p=Vec->p;
  for (i=0;i<length;i++)
    (void) scanf("%d", p++);

  //NOS
  ASSERT_TRACE(0,"Forced EXIT!");
  return NULL;
} /* Vector_Read */


void Matrix_Print(char *Format,chernikovaMatrix *Mat)
{
  int *p, i, j;
  unsigned NbRows, NbColumns;

  fprintf(stderr,"%d %d\n", NbRows=Mat->NbRows, NbColumns=Mat->NbColumns);
  for (i=0;i<NbRows;i++) {
    p=*(Mat->p+i);
    for (j=0;j<NbColumns;j++)
      fprintf(stderr, Format, *p++);
      fprintf(stderr, "\n");
  }
} /* Matrix_Print */

/*
chernikovaMatrix *Matrix_Read()
{
  chernikovaMatrix *Mat;
  int *p, i, j;
  unsigned NbRows, NbColumns;

  (void) scanf("%d %d", &NbRows, &NbColumns);
  Mat = Matrix_Alloc(NbRows, NbColumns);
  p = Mat->p_Init;
  for (i=0;i<NbRows;i++)
    for (j=0;j<NbColumns;j++)
      (void) scanf("%d", p++);
  return Mat;
} // original Matrix_Read 
*/


chernikovaMatrix *Matrix_Read(intMatrix& A)
{
  chernikovaMatrix *Mat;
  int *p, i, j;
  unsigned NbRows, NbColumns;

  NbRows = A.nRow() ;  NbColumns = A.nCol()+1 ;
  Mat = Matrix_Alloc(NbRows, NbColumns);
  p = Mat->p_Init;
  for (i=0;i<NbRows;i++) {
    *p++ = 1 ; //  set inequality flags
    for (j=1;j<NbColumns-1;j++)
      *p++ = (int)A.at(i,j-1) ;
    *p++ = (int)-A.at(i,NbColumns-2) ;
  }
  return Mat;
}

int First_Non_Zero(int *p,unsigned length)
{
  int *cp, i;
  
  cp=p;
  for (i=0;i<length;i++)
    if (*cp++) break;
  return (i==length) ? -1 : i;
} /* First_Non_Null */

void Vector_Sort(int *Vec,unsigned n)
{
  int i, j, temp;
  int *current_node,
      *left_son,
      *right_son;

  for (i=(n-1)/2;i>=0;i--) {         /* phase 1 : build the heap */
    j=i;
    temp=*(Vec+i);
    while (j<=(n-1)/2) {             /* while not a leaf         */
      current_node=Vec+j;
      left_son=Vec+(j<<1)+1;
      if ((j<<1)+2>=n) {             /* only one son             */
        if (temp<*left_son) {
          *current_node=*left_son;
          j=(j<<1)+1;
        } else break;
      } else {                       /* two sons                 */
        right_son=left_son+1;
        if (*right_son<*left_son) {
          if (temp<*left_son) {
            *current_node=*left_son;
            j=(j<<1)+1;
          } else break;
        } else {
          if (temp<*right_son) {
            *current_node=*right_son;
            j=(j<<1)+2;
          } else break;
        }
      }
    }
    *current_node=temp;
  }
  for (i=n-1;i>0;i--) {              /* phase 2 : sort the heap */
    temp=*(Vec+i);
    *(Vec+i)=*Vec;
    j=0;
    while (j<i/2) {                  /* while not a leaf        */
      current_node=Vec+j;
      left_son=Vec+(j<<1)+1;
      if ((j<<1)+2>=i) {             /* only one son            */
        if (temp<*left_son) {
          *current_node=*left_son;
          j=(j<<1)+1;
        } else break;
      } else {                       /* two sons                */
        right_son=left_son+1;
        if (*right_son<*left_son) {
          if (temp<*left_son) {
            *current_node=*left_son;
            j=(j<<1)+1;
          } else break;
        } else {
          if (temp<*right_son) {
            *current_node=*right_son;
            j=(j<<1)+2;
          } else break;
        }
      }
    }
    *current_node=temp;
  }
} /* Vector_Sort */


Boolean Vector_Equal(int *Vec1,int *Vec2,unsigned n)
{
  int i, *p1, *p2;

  p1=Vec1;
  p2=Vec2;
  for (i=0;i<n;i++)
    if (*p1++!=*p2++) break;
  
  return (i==n);
} /* Vector_Equal */
