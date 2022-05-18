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
#ifndef _vector_
#define _vector_

extern int Binomial ( int n, int p );
extern int Factorial ( int n );
extern int First_Non_Zero ( int *p, unsigned length );
extern int Gcd ( int a, int b );
extern int Inner_Product ( int *p1, int *p2, unsigned length );
extern chernikovaMatrix *Matrix_Alloc ( unsigned NbRows, unsigned NbColumns );
extern void Matrix_Free ( chernikovaMatrix *Mat );
extern void Matrix_Print ( char *Format, chernikovaMatrix *Mat );
extern void Matrix_Product ( chernikovaMatrix *mat1, chernikovaMatrix *mat2, chernikovaMatrix *mat3 );
//extern chernikovaMatrix *Matrix_Read ( /*void*/ );
extern chernikovaMatrix *Matrix_Read ( intMatrix& );
extern void Matrix_Vector_Product ( chernikovaMatrix *mat, int *p1, int *p2 );
extern void Vector_Add ( int *p1, int *p2, int *p3, unsigned length );
extern MyVector *Vector_Alloc ( unsigned length );
extern void Vector_AntiScale ( int *p1, int *p2, int lambda, unsigned
                               length );
extern void Vector_Combine ( int *p1, int *p2, int *p3, int lambda, int
                             mu, unsigned length );
extern Boolean Vector_Equal ( int *Vec1, int *Vec2, unsigned n );
extern void Vector_Free ( MyVector *vec );
extern int Vector_Gcd ( int *p, int *q, unsigned length );
extern void Vector_Map ( int *p1, int *p2, int *p3, unsigned length, int
                         (*f)() );
extern void Vector_Matrix_Product ( int *p1, chernikovaMatrix *mat, int *p2 );
extern int Vector_Max ( int *p, unsigned length );
extern int Vector_Min ( int *p, unsigned length );
extern int Vector_Min_Not_Zero ( int *p, unsigned length, int *index );
extern void Vector_Normalize ( int *p, int *q, unsigned length );
extern void Vector_Or ( int *p1, int *p2, int *p3, unsigned length );
extern void Vector_Print ( char *Format, MyVector *Vec );
extern MyVector *Vector_Read ( void );
extern int Vector_Reduce ( int *p, unsigned length, int (*f)() );
extern void Vector_Scale ( int *p1, int *p2, int lambda, unsigned length );
extern void Vector_Set ( int *p, int n, unsigned length );
extern void Vector_Sort ( int *Vec, unsigned n );
extern void Vector_Sub ( int *p1, int *p2, int *p3, unsigned length );

#endif
