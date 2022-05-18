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

#include <iostream.h>
#include <math.h>
#ifndef WIN32
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#endif

#include "auxiliary.h"
#include "types.h"
#include "intMatrix.h"
#include "BasicSet.h"
#include "polyhedron.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#define DEFAULT_CAPACITY 256


int min,max;

BasicSet::BasicSet(const BasicSet &b) : Coord(b.Coord), Constr(b.Constr)
{
  latticePoints = CountImagePolytope(Coord,Constr); 
} 

BasicSet::BasicSet (const intMatrix& m1, const intMatrix& m2) : Coord(m1), Constr(m2)
{
  latticePoints = CountImagePolytope(Coord,Constr); 
} 


BasicSet::BasicSet (const intMatrix& m1, const intMatrix& m2, int points) : Coord(m1), Constr(m2)
{
  latticePoints = points ; 
} 

const BasicSet* BasicSet::operator^(const BasicSet& bset2) const
{
  int m  = Coord.nRow() ;
  int n1 = Coord.nCol() - 1 ;
  int n2 = bset2.Coord.nCol() - 1 ;
  int n  = n1 + n2 ;
  int l1 = Constr.nRow() ;
  int l2 = bset2.Constr.nRow() ;
  int L  = l1 + l2 ;
  int i,j,k;

  if ( n==0 ) {
    for ( i=0 ; i<m ; i++ )
      if ( Coord(i,0) != bset2.Coord(i,0) )  return NULL ;
    return this;
  }

  int* *T = new int*[m] ;
  for ( i=0 ; i<m ; i++ )  T[i] = new int[n] ;
  int*  u = new int [m] ;
  int* p1 = Coord._v ;  int* p2 = bset2.Coord._v ;
  for ( i=0 ; i<m ; i++ ) {
    int* t = T[i] ;
    //for ( int j=0 ; j<n1 ; j++ )  T[i][j] =+this->Coord(i,j) ;
    //for ( j=n1 ; j<n ; j++ )      T[i][j] =-bset2.Coord(i,j-n1) ;
    //u[i] = -this->Coord(i,n1) + bset2.Coord(i,n2) ;
    int j;
    for ( j=0 ; j<n1 ; j++ )  *t++ = *p1++ ;
    for ( j=n1 ; j<n ; j++ )  	  *t++ =-*p2++ ;
    u[i] = *p2++ - *p1++ ;
  }

  int* *V = new int*[n] ;
  int*  v = new int [n] ;
  int   p = DiophantineSystem (m,n,T,u,V,v) ;
  for ( i=0 ; i<m ; i++ )  delete T[i] ;
  delete T ;  delete u ;

  if ( p == -1 )  { delete V ; delete v ; return NULL ; }
  else {
    intMatrix T(m,p+1,0) ;
    if ( p == 0 )  L = 0 ;
    intMatrix A(L,p+1,0) ;
    BasicSet* bset ;
    if ( p == 0 ) {
      delete V ;
      int* p1 = Constr._v ;  int* p2 ;
      for ( i=0 ; i<l1 ; i++ ) {
	int S = 0 ;  p2 = v ;
	//for ( int j=0 ; j<n1 ; j++ )  S += this->Constr(i,j) * v[j] ;
	//if ( S < this->Constr(i,n1) )  { delete v ; return NULL ; }
	for ( j=0 ; j<n1 ; j++ )  S += *p1++ * *p2++ ;
	if ( S < *p1++ )  { delete v ; return NULL ; }
      }
      p1 = bset2.Constr._v ;
      for ( i=0 ; i<l2 ; i++ ) {
	int S = 0 ;  p2 = v+n1 ;
	//for ( int j=0 ; j<n2 ; j++ )  S += bset2.Constr(i,j) * v[j+n1] ;
	//if ( S < bset2.Constr(i,n2) )  { delete v ; return NULL ; }
	for ( j=0 ; j<n2 ; j++ )  S += *p1++ * *p2++ ;
	if ( S < *p1++ )  { delete v ; return NULL ; }
      }
     
      p1 = Coord._v ;  int* t = T._v ; 
      for ( i=0 ; i<m ; i++ ) {
	//for ( int j=0 ; j<n1 ; j++ )  T(i,0) += this->Coord(i,j) * v[j] ;
	//T(i,0) += this->Coord(i,n1) ;
	p2 = v ;
	for ( j=0 ; j<n1 ; j++ )  *t += *p1++ * *p2++ ;
	*t++ += *p1++ ;
      }
      delete v ;
      bset = new BasicSet(T,A) ;
    }
    else {  //	p > 0
      int* t = T._v ;  int* p1 = Coord._v ;  int* p2 ;
      for ( i=0 ; i<m ; i++ ) {
	for ( j=0 ; j<p ; j++ ) {
	  //for ( k=0 ; k<n1 ; k++ )  T(i,j) += this->Coord(i,k) * V[k][j] ;
	  for ( k=0 ; k<n1 ; k++ )  *t += *p1++ * V[k][j] ;
	  t++ ;  p1 -= n1 ;
	}
	//for ( k=0 ; k<n1 ; k++ )  T(i,p) += this->Coord(i,k) * v[k] ;
	p2 = v ;
	for ( k=0 ; k<n1 ; k++ )  *t += *p1++ * *p2++ ;
	//T(i,p) += this->Coord(i,n1) ;
	*t++ += *p1++ ;
      }
      
      t  = A._v ;  
      p1 = Constr._v ;
      for ( i=0 ; i<l1 ; i++ ) {
	for ( j=0 ; j<p ; j++ ) {
	  //for ( k=0 ; k<n1 ; k++ )  A(i,j) += this->Constr(i,k) * V[k][j] ;
	  for ( k=0 ; k<n1 ; k++ )  *t += *p1++ * V[k][j] ;
	  t++ ;  p1 -= n1 ;
	}
	//for ( k=0 ; k<n1 ; k++ )  A(i,p) -= this->Constr(i,k) * v[k] ;
	p2 = v ;
	for ( k=0 ; k<n1 ; k++ )  *t -= *p1++ * *p2++ ;
	//A(i,p) += this->Constr(i,n1) ;
	*t++ += *p1++ ;
      }
      p1 = bset2.Constr._v ;
      for ( i=l1 ; i<L ; i++ ) {
	for ( j=0 ; j<p ; j++ ) {
	  //for ( k=0 ; k<n2 ; k++ )  A(i,j) += bset2.Constr(i-l1,k) * V[k+n1][j] ;
	  for ( k=0 ; k<n2 ; k++ )  *t += *p1++ * V[k+n1][j] ;
	  t++ ;  p1 -= n2 ;
	}
	//for ( k=0 ; k<n2 ; k++ )  A(i,p) -= bset2.Constr(i-l1,k) * v[k+n1] ;
	p2 = v+n1 ;
	for ( k=0 ; k<n2 ; k++ )  *t -= *p1++ * *p2++ ;
	//A(i,p) += bset2.Constr(i-l1,n2) ;
	*t++ += *p1++ ;
      }
 
      for ( i=0 ; i<n ; i++ )  delete V[i] ;
      delete V ;  delete v ;

      if ( ! LatticePoints(A) )  return NULL ;
      int j;
      for ( j=p-1 ; j>=0 ; j-- )
	if ( T.colIsNul(j) ) break ;
      if ( j < 0 )  bset = ConstructBasicSet (T,A) ;
      else {
        intMatrix* a = new intMatrix(EliminateCol(A,j)) ;  j-- ;
        for (   ; j>=0 ; j-- )
	  if ( T.colIsNul(j) ) {
	    intMatrix* b = new intMatrix(EliminateCol(*a,j)) ;
	    delete a ;  a = b ;
	  }
        if ( a->nCol() == 1 ) {
	  for ( i=0 ; i < a->nRow() ; i++ )
	    if ( a->at(i,0) > 0 )  { delete a ;  return NULL ; }
	  bset = new BasicSet(T.Col(p),intMatrix(0,1)) ;
        }
        else  bset = ConstructBasicSet (T.rmNulCols(),*a) ;
        delete a ;
      }
      if ( bset == NULL )  return NULL ;
    }

    if ( bset->nrPts() == 0 )  { delete bset ;  return NULL ; }
    if ( n1 == 0  ||  bset->nrPts() == nrPts() )  { delete bset ;  return this ;   }
    if ( n2 == 0  ||  bset->nrPts() == bset2.nrPts() )  { delete bset ;  return &bset2 ; }
    return bset ;
  }
}

long int BasicSet::operator&(const BasicSet& bset2) const
//  Previously, it checked whether the intersection is non-empty 
//   (without the creation of the intersection bset); the goal was to check the existence of dependences
//  Now, it returns the intersection size (number of dependences)
{
  int m  = Coord.nRow() ;
  int n1 = Coord.nCol() - 1 ;
  int n2 = bset2.Coord.nCol() - 1 ;
  int n  = n1 + n2 ;
  int l1 = Constr.nRow() ;
  int l2 = bset2.Constr.nRow() ;
  int L  = l1 + l2 ;
  int i,j,k;

  if ( n==0 ) {
    for ( i=0 ; i<m ; i++ )
      if ( Coord(i,0) != bset2.Coord(i,0) )  return 0;  // previous:return NULL
    return 1 ;  // return this; (Previously)
  }

  int* *T = new int*[m] ;
  for ( i=0 ; i<m ; i++ )  T[i] = new int[n] ;
  int*  u = new int [m] ;
  int* p1 = Coord._v ;  int* p2 = bset2.Coord._v ;
  for ( i=0 ; i<m ; i++ ) {
    int* t = T[i] ;
    int j;
    for ( j=0 ; j<n1 ; j++ )  *t++ = *p1++ ;
    for ( j=n1 ; j<n ; j++ )  	  *t++ =-*p2++ ;
    u[i] = *p2++ - *p1++ ;
  }

  int* *V = new int*[n] ;
  int*  v = new int [n] ;
  int   p = DiophantineSystem (m,n,T,u,V,v) ;
  for ( i=0 ; i<m ; i++ )  delete T[i] ;
  delete T ;  delete u ;

  if ( p == -1 )  { delete V ; delete v ; return 0; } // return NULL ; 
  else {
    intMatrix T(m,p+1,0) ;
    if ( p == 0 )  L = 0 ;
    intMatrix A(L,p+1,0) ;
    if ( p == 0 ) {
      delete V ;
      int* p1 = Constr._v ;  int* p2 ;
      int i;
      for ( i=0 ; i<l1 ; i++ ) {
	int S = 0 ;  p2 = v ;
	for ( j=0 ; j<n1 ; j++ )  S += *p1++ * *p2++ ;
	if ( S < *p1++ )  { delete v ; return 0; } // return NULL ; }
      }
      p1 = bset2.Constr._v ;
      for ( i=0 ; i<l2 ; i++ ) {
	int S = 0 ;  p2 = v+n1 ;
	for ( j=0 ; j<n2 ; j++ )  S += *p1++ * *p2++ ;
	if ( S < *p1++ )  { delete v ; return 0; } // return NULL ; }
      }
     
      p1 = Coord._v ;  int* t = T._v ; 
      for ( i=0 ; i<m ; i++ ) {
	p2 = v ;
	for ( j=0 ; j<n1 ; j++ )  *t += *p1++ * *p2++ ;
	*t++ += *p1++ ;
      }
      delete v ;
      return 1 ;  // return this; (Previously)
    }
    else {  //	p > 0
      int* t = T._v ;  int* p1 = Coord._v ;  int* p2 ;
      for ( i=0 ; i<m ; i++ ) {
	for ( j=0 ; j<p ; j++ ) {
	  for ( k=0 ; k<n1 ; k++ )  *t += *p1++ * V[k][j] ;
	  t++ ;  p1 -= n1 ;
	}
	p2 = v ;
	for ( k=0 ; k<n1 ; k++ )  *t += *p1++ * *p2++ ;
	*t++ += *p1++ ;
      }
      
      t  = A._v ;  
      p1 = Constr._v ;
      for ( i=0 ; i<l1 ; i++ ) {
	for ( j=0 ; j<p ; j++ ) {
	  for ( k=0 ; k<n1 ; k++ )  *t += *p1++ * V[k][j] ;
	  t++ ;  p1 -= n1 ;
	}
	p2 = v ;
	for ( k=0 ; k<n1 ; k++ )  *t -= *p1++ * *p2++ ;
	*t++ += *p1++ ;
      }
      p1 = bset2.Constr._v ;
      for ( i=l1 ; i<L ; i++ ) {
	for ( j=0 ; j<p ; j++ ) {
	  for ( k=0 ; k<n2 ; k++ )  *t += *p1++ * V[k+n1][j] ;
	  t++ ;  p1 -= n2 ;
	}
	p2 = v+n1 ;
	for ( k=0 ; k<n2 ; k++ )  *t -= *p1++ * *p2++ ;
	*t++ += *p1++ ;
      }
 
      for ( i=0 ; i<n ; i++ )  delete V[i] ;
      delete V ;  delete v ;

      if ( ! LatticePoints(A) )  return 0; // return NULL ;
      for ( j=p-1 ; j>=0 ; j-- )
	if ( T.colIsNul(j) ) break ;
      if ( j < 0 ) {
	CompactConstraints(A) ;
	return CountImagePolytope(T,A.rmNulRows()) ;
      }        // return this ; (if only the EXISTENCE of dependences is necessary)
      intMatrix* a = new intMatrix(EliminateCol(A,j)) ;  j-- ;
      for (   ; j>=0 ; j-- )
	if ( T.colIsNul(j) ) {
	  intMatrix* b = new intMatrix(EliminateCol(*a,j)) ;
	  delete a ;  a = b ;
	}
      if ( a->nCol() == 1 ) {
	for ( i=0 ; i < a->nRow() ; i++ )
	  if ( a->at(i,0) > 0 )  { delete a ;  return 0; } // return NULL ; }
	delete a ;  return 1 ;
      }

      CompactConstraints(*a) ;     //return this ; if only the EXISTENCE of dependences is necessary
      intMatrix P(a->rmNulRows()) ;     delete a ;
      return CountImagePolytope(T.rmNulCols(),P) ; // The number of dependences is returned
    }
  }
}





BasicSet* ConstructBasicSet (intMatrix& T, intMatrix& A)
{
  int m = T.nRow() ;
  int L = A.nRow() ;
  int n = A.nCol() - 1 ;
  int i,j,k,r;
  //if ( m==0 && L==0 )  return NULL ;

//  More efficient "cleaning" of matrix A  (divide each row by its GCD, 
//      and shrink the matrices when iterator values can be determined)

    if ( L>0 && n>0 ) {
      int* pa = A._v ;

      for ( i=0 ; i<L ; i++ ) {
	int GCD = 0 ;
	for ( j=0 ; j<n ; j++ )  GCD = gcd ( abs(*pa++) , GCD ) ;
	if ( GCD > 0 ) {
	  GCD = gcd ( abs(*pa) , GCD ) ;
	  if ( GCD > 1 )
	    {  pa -= n ;  for ( j=0 ; j<=n ; j++ )  *pa++ /= GCD ;  }
	  else pa++ ;
	}
	else if ( 0 < *pa ) return NULL ;
	     else *pa++ = 0 ;
      }
      
      for ( i=0 ; i<L ; i++ )
	if ( ! A.rowIsNul(i) ) {
	  int* pi = A.addrRow(i) ;  //  ... = &A(i,0)
	  int* p1 = pi ;  int* p2 = pi+n-1 ;
	  while ( ! *p1++ ) ;  p1-- ;
	  while ( ! *p2-- ) ;  p2++ ;
	  if ( p1==p2 ) {
	    if ( *p1 != 1  &&  *p1 != -1 ) {
	      *(pi+n) = (int)ceil ( float(*(pi+n)) / float(abs(*p1)) ) ;
	      if ( *p1 > 0 )  *p1 = 1 ;  else  *p1 = -1 ;
	    }
	    int val =  *p1 == 1  ?  *(pi+n) : -(*(pi+n)) ;
	    for ( k=i+1 ; k<L ; k++ )
	      if ( ! A.rowIsNul(k) ) {
	        int* pk = A.addrRow(k) ; // ... = &A(k,0)
	  	int* p3 = pk ;  int* p4 = pk+n-1 ;
	  	while ( ! *p3++ ) ;  p3-- ;
	  	while ( ! *p4-- ) ;  p4++ ;
		if ( p3==p4 ) {
	          if ( *p3 != 1  &&  *p3 != -1 ) {
	            *(pk+n) = (int)ceil ( float(*(pk+n)) / float(abs(*p3)) ) ;
	            if ( *p3 > 0 )  *p3 = 1 ;  else  *p3 = -1 ;
	          }
		  if ( p3-pk==p1-pi && *p1==-*p3 && *(pi+n)==-*(pk+n)) {
		    int j = p1-pi ;
	    	    for (r=0; r<m; r++){ T(r,n) += val*T(r,j) ;  T(r,j) = 0 ; }
	    	    for (r=0 ;r<L ;r++){ A(r,n) -= val*A(r,j) ;  A(r,j) = 0 ; }
		  }
		}
	      }
	  }
	}
    }

  if ( ! CompactConstraints(A) )  return NULL ;

  intMatrix B = A.rmNulRows().rmNulCols() ;
  if ( B.nCol() == 1 )  return  new BasicSet(T.rmNulCols(),intMatrix(0,1)) ;
  if ( B.nRow() <= B.nCol() )
    return  new BasicSet(T.rmNulCols(),B) ;
//
  chernikovaMatrix* M = Matrix_Read(B) ;
  Polyhedron* P = Constraints2Polyhedron(M,DEFAULT_CAPACITY) ;
  int d = P->Dimension + 1 ;
  if ( P->NbEq == d )  return NULL ; // Empty polyhedron
  intMatrix C(P->NbConstraints + P->NbEq , d) ;
  int* pc = C._v ;  //int r = 0 ;
  for ( i=0 ; i < P->NbConstraints ; i++ ) {
    int* p = P->Constraint[i] + 1 ;
    for ( j=1 ; j<d ; j++ )  *pc++ = *p++ ;  // C(r,j-1) = P->Constraint[i][j] ;
    *pc++ = -*p ;  // C(r,d-1) = -P->Constraint[i][d] ;  r++ ;
    if ( P->Constraint[i][0] == 0 )
      for ( j=0 ; j<d ; j++ )  *pc++ = -*(pc-d) ;  //{ C(r,j) = -C(r-1,j) ;  r++ ; }
  }
  Matrix_Free(M) ;  Domain_Free(P) ;
  return ( new BasicSet(T.rmNulCols(),C) ) ;
}



int CompactConstraints (intMatrix& A)
{
  int L = A.nRow() ;
  int n = A.nCol() - 1 ;
  int i,j,k;
    if ( L>0 && n>0 ) {
      int* pa = A._v ;

      for ( i=0 ; i<L ; i++ ) {
	int GCD = 0 ;
	for ( j=0 ; j<n ; j++ )  GCD = gcd ( abs(*pa++) , GCD ) ;
	if ( GCD > 0 ) {
	  GCD = gcd ( abs(*pa) , GCD ) ;
	  if ( GCD > 1 )
	    {  pa -= n ;  for ( j=0 ; j<=n ; j++ )  *pa++ /= GCD ;  }
	  else pa++ ;
	}
	else if ( 0 < *pa ) return 0 ;
	     else *pa++ = 0 ;
      }
      
      for ( i=0 ; i<L ; i++ )  // The obsolete rows of A are made null
	if ( ! A.rowIsNul(i) ) {
	  int* pi = A.addrRow(i) ;  // ... = &A(i,0)
	  for ( j=i+1 ; j<L ; j++ )
	    if ( ! A.rowIsNul(j) ) {
	      int* pj = A.addrRow(j) ;  // ... = &A(j,0)
	      int* p1 = pi ;  int* p2 = pj ;
	      for ( k=0 ; k<n ; k++ )  if ( *p1++ != *p2++ ) break ;
	      if ( k==n ) {
		if ( *p1 < *p2 )  *p1 = *p2 ;
		p2 = pj ;
		for ( k=0 ; k<=n ; k++ )  *p2++ = 0 ;
	      }
	    }
	}
    }

    return 1 ;
}


int DiophantineSystem (int m, int n, int* *T, int* u, int* *V, int* v)
{
  int nulc = 0 ;  int* nulCol = new int[n] ;
  int k = 0 ,i,j,l;

  for ( j=0 ; j<n ; j++ ) {
    for ( i=0 ; i<m ; i++ )  if ( T[i][j] != 0 ) break ;
    if ( i==m )  {  nulCol[j] = 1 ;  nulc++ ;  }
    else {
      nulCol[j] = 0 ;
      if ( k<j )  for ( i=0 ; i<m ; i++ )  T[i][k] = T[i][j] ;
      k++ ;
    }
  }
  n -= nulc ;


  int* *S = new int*[n] ;
  int* *t = new int*[n] ;
  for ( i=0 ; i<n ; i++ ) {
    S[i] = new int[n] ;
    t[i] = new int[n] ;
    for ( j=0 ; j<n ; j++ )  S[i][j] = i==j ? 1 : 0 ;
  }
  int r = 0 ;

  for ( i=0 ; i<m ; i++ )
  {
    int nr = 0 ;
    do {
      for ( k=r ; k<n ; k++ )  if ( T[i][k] != 0 ) break ;
      if ( k==n )  break ;
      int M = k ;
      nr = 1 ;
      while ( k < n-1 ) {
	k++ ;
	if ( T[i][k] != 0 ) {
	   nr++ ;
	   if ( abs(T[i][k]) < abs(T[i][M]) )  M = k ;
	}
      }
      if ( nr > 1 ) {
	for ( l=0 ; l<n ; l++ )
	  for ( j=0 ; j<n ; j++ )  t[l][j] = l==j ? 1 : 0 ;
	int* pt = &t[M][r] ;
	int* pT = &T[i][r] ;  int* pM = &T[i][M] ;
	for ( k=r ; k<n ; k++ ) {
	  if ( k!=M ) { 
	    //t[M][k] = floor ( float(T[i][k]) / abs(T[i][M]) ) ;
	    *pt = (int)floor ( float(*pT) / float(abs(*pM)) ) ;
	    //if ( T[i][k] % abs(T[i][M]) > abs(T[i][M])/2 )  t[M][k]++ ;
	    if ( *pT % abs(*pM) > abs(*pM)>>1 )  (*pt)++ ;
	    //if ( T[i][M] > 0 )  t[M][k] = -t[M][k] ;
	    if ( *pM > 0 )  *pt = - *pt ;
	  }
	  pt++ ;  pT++ ;
	}
	for ( j=0 ; j<n ; j++ ) {
	  for ( l=0 ; l<m ; l++ )
	    if ( j!=M )  T[l][j] += T[l][M]*t[M][j] ;
	  for ( l=0 ; l<n ; l++ )
	    if ( j!=M )  S[l][j] += S[l][M]*t[M][j] ;
	}
      }
      else
	if ( M != r ) {
	  for ( l=0 ; l<m ; l++ ) {
	    int aux = T[l][r] ;  T[l][r] = T[l][M] ;  T[l][M] = aux ;
	  }
	  for ( l=0 ; l<n ; l++ ) {
	    int aux = S[l][r] ;  S[l][r] = S[l][M] ;  S[l][M] = aux ;
	  }
        }
    } while ( nr > 1 );

    if ( nr == 1 )  r++ ;
  }

  r = 0 ;
  for ( i=0 ; i<m ; i++ ) {
    for ( j=0 ; j<r ; j++ )  u[i] -= T[i][j] * u[j] ;
    if ( r < n )
      if ( T[i][r] != 0 ) {
        if ( u[i]%T[i][r] != 0 )  { r=0 ; break; }
        u[r] = u[i] / T[i][r] ;  r++ ;
      }
      else  { if ( u[i] != 0 )  { r=0 ; break; } }
    else  if ( u[i] != 0 )  { r=0 ; break; }
  }

  if ( r != 0 ) {
    for ( i=0 ; i<n+nulc ; i++ )  V[i] = new int[n-r] ;

    for ( i=0 ; i<n ; i++ ) {
      v[i] = 0 ;
      for ( j=0 ; j<r ; j++ )  v[i] += S[i][j] * u[j] ;
      for ( j=0 ; j<n-r ; j++ )  V[i][j] = S[i][j+r] ;
    }

    i = n-1 ;  k = i+nulc ;
    while ( i < k )
      if ( nulCol[k] == 1 ) {
	for ( j=0 ; j<n-r ; j++ )  V[k][j] = 0 ;
	v[k] = 0 ;  k-- ;
      }
      else {
	for ( j=0 ; j<n-r ; j++ )  V[k][j] = V[i][j] ;
	v[k] = v[i] ;  i-- ;  k-- ;
      }
  }

  for ( i=0 ; i<n ; i++ ) { delete S[i] ;  delete t[i] ;  }
  delete S ;  delete t  ;  delete nulCol ;

  if ( r== 0 )  return -1 ;
  else  return n-r ;
}

/*
int gcd (int a, int b)
{
  int r ;
  if ( a<b )  { int temp = a ;  a = b ;  b = temp ; }
  if ( b == 0 )  return a ;
  while ( (r=a%b) != 0 )  { a = b ;  b = r ; }
  return b ;
}
*/

long int CountPolytopePoints (intMatrix A)
{
  int flag ;
  int m = A.nRow() ;	int n = A.nCol() - 1 ;
  int i,j,k;

  if ( n == 0 )  return 1 ;
  if ( n == 1 )  return Range (A) ;

  long int Delta = 1 ;  int aux[2] ;
  for ( j=0 ; j<n ; j++ ) {
    int k = 0 ;
    for ( i=0 ; i<m ; i++ )
      if ( A(i,j) != 0 )
	if ( k==2 ) break ;
	else { aux[k] = i ;  k++ ; }
    if ( i<m || k==0 ) continue ;
    if ( k==1 ) return -1 ;
    i = aux[0] ;
    int* p1 = &A(i,0) ;
    while ( ! *p1++ ) ;  p1-- ;
    int* p2 = &A(i,n-1) ;
    while ( ! *p2-- ) ;  p2++ ;
    if ( p1 != p2 ) continue ;
    k = aux[1] ;
    p1 = &A(k,0) ;
    while ( ! *p1++ ) ;  p1-- ;
    p2 = &A(k,n-1) ;
    while ( ! *p2-- ) ;  p2++ ;
    if ( p1 != p2 ) continue ;

    intMatrix temp(2,2) ;  p1 = temp._v ;
    *p1++ = A(i,j) ;  *p1++ = A(i,n) ;
    *p1++ = A(k,j) ;  *p1++ = A(k,n) ;
    A(i,j) = 0 ;  A(i,n) = 0 ;
    A(k,j) = 0 ;  A(k,n) = 0 ;
    Delta *= Range(temp) ;
    if ( Delta < 0 ) return -1 ;
  }

  intMatrix B = A.rmNulRows().rmNulCols() ;
  m = B.nRow() ;	n = B.nCol() - 1 ;

  if ( n == 0 )  return Delta ;
  if ( n == 1 ) { 
    Delta *= Range (B) ;
    if ( Delta >= 0 ) return Delta ;
    else return -1 ;
  }


//  Start the proper computation process for the (eventually simplified) matrix

    if ( ( flag = Fourier_MotzkinElimination (B) )  <= 0 )  return flag ;

    int liminf = min ;
    int limsup = max ;
    intMatrix C(m,n) ;
    //for ( j=0 ; j<n-1 ; j++ )  C.col(j,B,j+1) ;
    int* pc = C._v ;  int* pb = B._v ;
    for ( i=0 ; i<m ; i++ ) {
      pb++ ;
      for ( j=0 ; j<n-1 ; j++ )  *pc++ = *pb++ ;
      pc++ ;  pb++ ;
    }
    
    long int N = 0 ;
    for ( k=liminf ; k<=limsup ; k++ ) {
      for ( i=0 ; i<m ; i++ )  C(i,n-1) = B(i,n) - k*B(i,0) ;
      N += CountPolytopePoints(C) ;
    }
    return Delta*N ;
}



bool LatticePoints (intMatrix& A)
{
  int flag ;
  int m = A.nRow() ;	int n = A.nCol() - 1 ;
  int i,j,k;

  if ( n == 0 )  return YES ;
  if ( n == 1 )  return Range (A) ;

    if ( ( flag = Fourier_MotzkinElimination (A) )  <= 0 )  return flag ;

    int liminf = min ;
    int limsup = max ;
    intMatrix C(m,n) ;
    int* pc = C._v ;  int* pa = A._v ;
    for ( i=0 ; i<m ; i++ ) {
      pa++ ;
      for ( j=0 ; j<n-1 ; j++ )  *pc++ = *pa++ ;
      pc++ ;  pa++ ;
    }
    
    for ( k=liminf ; k<=limsup ; k++ ) {
      for ( i=0 ; i<m ; i++ )  C(i,n-1) = A(i,n) - k*A(i,0) ;
      if ( LatticePoints (C) )  return YES ;
    }
    return NO ;
}



int Fourier_MotzkinElimination (const intMatrix& A)
{
  // Variant valid only for polytopes (empty or not)
  
  int n = A.nCol() - 2 ;
  if ( n > 0 )  return Fourier_MotzkinElimination( EliminateCol(A,n) ) ;
  else 		return Range(A) ;
}

	    

intMatrix EliminateCol (const intMatrix& A, int col)
{
  int m = A.nRow() ;  int n = A.nCol() ;
  int i,j;
  
  int* pCol = A._v + col ;
  int* lastAddr = &A(m-1,n-1) ;
  int p = 0 ;  int q = 0 ;  int s = 0 ;

  for ( ; pCol<=lastAddr ; pCol+=n )
    if ( *pCol > 0 ) p++ ;
    else if ( *pCol < 0 ) q++ ;
	 else s++ ;

  int* *B = new int*[p*q+s] ;  int r = 0 ;
  int* pi = A._v ;

  for ( ; pi<=lastAddr ; pi += n ) {
    int icol = *(pi+col) ;
    if ( icol == 0 ) {
      int* p1 = pi ;
      int* p2 = new int[n] ;
      B[r] = p2 ;  r++ ;
      for ( j=0 ; j<n ; j++ )  *p2++ = *p1++ ;
    }
    else
      if ( icol > 0 ) {
	int* pk = A._v ;
	for ( ; pk<=lastAddr ; pk += n ) {
	  int kcol = *(pk+col) ;
	  if ( kcol < 0 ) {
	    int* p1 = pi ;  int* p2 = pk ;
	    int* p0 = new int[n] ;  B[r] = p0 ;
	    for ( j=0 ; j<n ; j++ )  *p0++ = *p2++ * icol - *p1++ * kcol ;

	    int GCD = 0 ;  p0 = B[r] ;
	    for ( j=0 ; j<n-1 ; j++ )  GCD = gcd( abs(*p0++) , GCD ) ;
	    if ( GCD > 0 ) {
	      GCD = gcd ( abs(*p0++) , GCD ) ;
	      if ( GCD > 1 ) {
		p0 = B[r] ;
		for ( j=0 ; j<n ; j++ )  *p0++ /= GCD ;
	      }
	      r++ ;
	    }
	    else if ( *p0++ <= 0 )  delete B[r] ;
		 else   r++ ;
	  }
	}
      }
  }

  intMatrix C(r,n-1) ;
  int* p1 = C._v ;
  for ( i=0 ; i<r ; i++ ) {
    int* p2 = B[i] ;
    for ( j=0 ; j<col ; j++ )  *p1++ = *p2++ ;
    p2++ ;
    for ( j=col+1 ; j<n   ; j++ )  *p1++ = *p2++ ;
    delete B[i] ;
  }
  delete B ;
  return C ;
}

	    

int Range (const intMatrix& A)
{
    int m = A.nRow() ;
    int i;

    min = INT_MIN ;  max = INT_MAX ;
    int* p = A._v ;
    for ( i=0 ; i<m ; i++ ) {
      if ( *p != 0 ) {
	int val = (int)ceil( float(*(p+1)) / float(abs(*p)) ) ;
	if       ( *p > 0  &&   val > min )  min = +val ;
	else  if ( *p < 0  &&  -val < max )  max = -val ;
      }
      else  if ( 0 < *(p+1) )  return 0 ;
      p++ ;  p++ ;
    }

    if ( max == INT_MAX  ||  min == INT_MIN )  return -1 ;
    if ( max >= min )  return max-min+1 ;
    else  return 0 ;
}



long int CountImagePolytope (intMatrix T, intMatrix& P)
{
  int m = T.nRow() ;
  int n = T.nCol() - 1 ;
  if ( n == 0 )  return 1 ;

  int i,j,k,l;
  int r = 0 ;

  intMatrix S(n,n,0) ;
  for ( i=0 ; i<n ; i++ )  S(i,i) = 1 ;
  intMatrix t(n,n) ;

  for ( i=0 ; i<m ; i++ )
  {
    int nr = 0 ;
    do {
      for ( k=r ; k<n ; k++ )  if ( T(i,k) != 0 ) break ;
      if ( k==n )  break ;
      int M = k ;
      nr = 1 ;
      while ( k < n-1 ) {
	k++ ;
	if ( T(i,k) != 0 ) {
	   nr++ ;
	   if ( abs(T(i,k)) < abs(T(i,M)) )  M = k ;
	}
      }

      if ( nr > 1 ) {
	int* pt = t._v ;
	for ( l=0 ; l<n ; l++ )
	  { for ( j=0 ; j<n ; j++ )   *pt++ = l==j ? 1 : 0 ; }
	  //for ( j=0 ; j<n ; j++ )  t(l,j) = l==j ? 1 : 0 ;
	pt = &t(M,r) ;  
	int* pT = &T(i,r) ;  int* pM = &T(i,M) ;
	for ( k=r ; k<n ; k++ ) {
	  if ( k!=M ) { 
	    *pt = (int)floor ( float(*pT) / float(abs(*pM)) ) ;
	    //t(M,k) = floor ( float(T(i,k)) / abs(T(i,M)) ) ;
	    if ( *pT % abs(*pM) > abs(*pM)>>1 )  (*pt)++ ;
	    //if ( T(i,k) % abs(T(i,M)) > abs(T(i,M)) / 2 )  t(M,k)++ ;
	    if ( *pM > 0 )  *pt = - *pt ;
	    //if ( T(i,M) > 0 )  t(M,k) = -t(M,k) ;
	  }
	  pt++ ;  pT++ ;
	}
	for ( j=0 ; j<n ; j++ ) {
	  for ( l=0 ; l<m ; l++ )
	    if ( j!=M )  T(l,j) += T(l,M) * t(M,j) ;
	  for ( l=0 ; l<n ; l++ )
	    if ( j!=M )  S(l,j) += S(l,M) * t(M,j) ;
	}
      }
      else
	if ( M != r ) {
	  for ( l=0 ; l<m ; l++ ) {
	    int aux = T(l,r) ;  T(l,r) = T(l,M) ;  T(l,M) = aux ;
	  }
	  for ( l=0 ; l<n ; l++ ) {
	    int aux = S(l,r) ;  S(l,r) = S(l,M) ;  S(l,M) = aux ;
	  }
        }
    } while ( nr > 1 );

    if ( nr == 1 )  r++ ;
  }

  if ( r==n )  return CountPolytopePoints(P) ;

  int L = P.nRow() ;
  intMatrix A(L,n+1,0) ;
  for ( i=0 ; i<L ; i++ ) {
    for ( j=0 ; j<n ; j++ )
      for ( k=0 ; k<n ; k++ )  A(i,j) += P(i,k) * S(k,j) ;
    A(i,n) = P(i,n) ;
  }

  return CountPrefixes(A,r) ;
}



long int CountPrefixes (intMatrix& A, int r)
{
  int i,j,k;

  if ( r==0 )
    if ( LatticePoints(A) ) return 1 ;
    else return 0 ;

  int m = A.nRow() ;	int n = A.nCol() - 1 ;

  Fourier_MotzkinElimination (A) ;

  int liminf = min ;
  int limsup = max ;
  intMatrix C(m,n) ;
  int* pc = C._v ;  int* pa = A._v ;
  for ( i=0 ; i<m ; i++ ) {
    pa++ ;
    for ( j=0 ; j<n-1 ; j++ )  *pc++ = *pa++ ;
    pc++ ;  pa++ ;
  }
    
  long int N = 0 ;
  for ( k=liminf ; k<=limsup ; k++ ) {
    for ( i=0 ; i<m ; i++ )  C(i,n-1) = A(i,n) - k*A(i,0) ;
    N += CountPrefixes(C,r-1) ;
  }
  return N ;
}



bool BasicSet::operator==(const BasicSet& bset) const
// Test two instances of BasicSet for equality
{
  if ( latticePoints != bset.nrPts() )  return NO ;
  const BasicSet* bset1 = *this ^ bset ;
  if ( bset1 != this )  { delete bset1 ; return NO ; }
  else return YES ;
}

void BasicSet::printOn(ostream& strm) const
// Print this object on an ostream
{
	strm << "\n\nTransformation matrix ( " << Coord.nRow() << " x " << Coord.nCol() << " )\n" ;
	strm << Coord ;
	strm << "\nRestriction matrix ( " << Constr.nRow() << " x " << Constr.nCol() << " )\n" ;
	strm << Constr ;
	strm << endl << "Lattice points :  " ;
	strm << latticePoints << endl << endl ;
}

void BasicSet::put(void) const{
  cout << "\n---------------------------";
  cout << "\nTransformation matrix:\n";
  Coord.put();
  cout << "\nRestriction matrix:\n";
  Constr.put();
  cout << "\n---------------------------";
}



