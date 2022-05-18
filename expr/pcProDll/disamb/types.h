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
typedef int Boolean;

typedef struct my_vector {
  unsigned Size;
  int *p;
} MyVector;

typedef struct matrix {
  unsigned NbRows, NbColumns;
  int **p;
  int *p_Init;
} chernikovaMatrix;

/* NOTES on struct polyhedron
   Constraints are Equations and Inequalities
   Rays are        Lines     and Ray/Vertices
   The first bit in each vector is a type indicator.
   0 = Equation/Line
   1 = Inequality/Ray/Vertex
   This allows Constrains and Rays to be mixed... this is
   how they come in from the user.
*/

typedef struct polyhedron
{ struct polyhedron *next;
  unsigned Dimension, NbConstraints, NbRays, NbEq, NbBid;
  int **Constraint;
  int **Ray;
  int *p_Init;
} Polyhedron;
