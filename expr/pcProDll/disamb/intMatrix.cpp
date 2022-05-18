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

// intMatrix.c -- matrix of type integer
#ifndef WIN32
	#include <stdio.h>
	#include <math.h>
#endif

#include <stdlib.h>

#include "intMatrix.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

extern bool dp;
extern bool dt;


void intMatrix::Set(int i,int j,int v){
  *(_v+i*(ncol)+j)=v;
}

int intMatrix::Get(int i,int j){
  return *(_v+i*(ncol)+j);
}

bool intMatrix::isNul() const
{
  int* p=_v;
  int i,j;
  for(i=0; i<nrow; i++)
    for(j=0; j<ncol; j++)
      //if ( at(i,j)!=0 ) return 0;
      if ( *p++ ) return 0;
  return 1;
}

bool intMatrix::matIsNul() const
{
  int i,j;
  for(i=0; i<nRow(); i++)
    for(j=0; j<nCol()-1; j++)
      if (at(i,j)!=0) return 0;
  return 1;
}

bool intMatrix::vecIsNul() const
{
  int i;
  for(i=0; i<nRow(); i++)
    if (at(i,nCol()-1)!=0) return 0;
  return 1;
}

bool intMatrix::rowIsNul(int i) const
{
  int* p=_v+i*ncol;
  int j;
  for(j=0; j<ncol; j++)
    if ( *p++ ) return NO;
  return YES;
}

bool intMatrix::colIsNul(int j) const
{
  int* p=_v+j;
  int i;
  for(i=0; i<nrow; i++) {
    if ( *p ) return NO;
    p += ncol;
  }
  return YES;
}

intMatrix intMatrix::Row(int i) const
{
  intMatrix row(1,ncol);
  int j;
  int* p1=_v+i*ncol;  int* p2=row._v;
  for(j=0; j<ncol; j++) *p2++ = *p1++ ;
  return row;
}

intMatrix intMatrix::Col(int j) const
{
  intMatrix col(nrow,1);
  int* p1=_v+j;  int* p2=col._v;
  int i;
  for(i=0; i<nrow; i++) {
    *p2++ = *p1;
    p1 += ncol;
  }
  return col;
}


intMatrix intMatrix::diff() const
{
  intMatrix n(nRow(),nCol()-1,0);
  int i,j;
  for(j=1; j<nCol(); j++)
    for(i=0; i<nRow(); i++) n(i,j-1)=at(i,j)-at(i,0);
  return n;
}


intMatrix intMatrix::equality(intMatrix& m) const
// *this = (A|b) with b a column vector
// return all rows of *this for which Am==b  (equality)
{
  int i,j;
  if (nCol()!=m.nRow()-1) return intMatrix(0,0,0);
  int last = nCol()-1;
  intMatrix mul(*this * ( m % intMatrix(1,m.nCol()) ) );
  int totok=0;
  for(i=0; i<mul.nRow(); i++)
  { bool ok=1;
    for(j=0; j<mul.nCol(); j++)
      ok = ok && (mul(i,j)==at(i,last));
    if (ok) { mul(i,0)=1; totok++; } else mul(i,0)=0;
  }
  intMatrix eq(totok,nCol(),0);
  int rw=0;
  for(i=0; i<nRow(); i++)
    if (mul(i,0)==1) { eq.row(rw,*this,i); rw++; }
  return eq;
}


intMatrix intMatrix::affineMat() const
{
  int j;
  intMatrix m(nRow(),nCol()-1,0);
  for(j=0; j<nCol()-1; j++) m.col(j,*this,j);
  return m;
}

intMatrix intMatrix::affineVec() const
{
  intMatrix m(col(nCol()-1));
  return m;
}

intMatrix intMatrix::affineTr(const intMatrix& m) const
// this = A, m=(C|c) 
// returned D with D = C*A + (for each column of C*A) c. 
{
  int i,j;
  if (nRow()!=m.nCol()-1)
  { cerr << "ERROR: incorrect affine transformation \n"; exit(0); } 
  intMatrix C(m.affineMat()); intMatrix c(m.affineVec());
  intMatrix r(C*(*this));
  for(i=0; i<r.nRow(); i++)
    for(j=0; j<r.nCol(); j++) r(i,j)+=c(i,0);
  return r;
}

intMatrix intMatrix::affineMult(const intMatrix& m) const
{
  intMatrix A(affineMat()); intMatrix b(affineVec());
  intMatrix C(m.affineMat()); intMatrix c(m.affineVec());
  intMatrix D(C*A & (C*b+c));
  return D;
}

intMatrix intMatrix::extendIneqDim(int d, intMatrix* range) const
// this = (A|b) with Ax>=b
// returns dimension enlarged matrix with additional inequalities
{ 
  if (d<nCol()) return *this;
  int diff = d-nCol()+1;
  intMatrix pn(*this & intMatrix(nRow(),diff,0)); // patched matrix
  pn.switchCols(nCol()-1,pn.nCol()-1);  // reposition constants
  intMatrix extraInEq(2*diff,pn.nCol(),0);          // extra inequalities
  int k=0;
  int i;
  for(i=0; i<2*diff; i+=2)
  { extraInEq(i,nCol()+k-1) = 1; 
    if (range!=0) extraInEq(i,pn.nCol()-1) = range->at(nCol()+k-1,0);
    extraInEq(i+1,nCol()+k-1) = -1;
    if (range!=0) extraInEq(i+1,pn.nCol()-1) = range->at(nCol()+k-1,1);
    k++;
  }
  return pn % extraInEq;
}


intMatrix &intMatrix::rmNulRows() const 
{
/*
  intMatrix nulr(nrow,1,0);
  int k=0;
  for(i=0; i<nrow; i++)
    if (row(i).isNul()) { nulr(i,0)=1; k++; }
  intMatrix a(nrow-k,ncol,0);
  k=0;
  for(i=0; i<nrow; i++) if (nulr(i,0)==0) { a.row(k,row(i)); k++; }
  return a;
*/
  int* nulr = new int[nrow] ;
  int k=0;  int* p=nulr;
  int i,j;
  for(i=0; i<nrow; i++)
    if (rowIsNul(i)) { *p++=1; k++; }
    else *p++=0;
  intMatrix *a;
  a=new intMatrix(nrow-k,ncol);
  //intMatrix a(nrow-k,ncol);
  if (nrow==k || ncol==0) { delete nulr; return *a; }
  int* pa=a->_v;  p=_v;
  for(i=0; i<nrow; i++)
    if (nulr[i]==0)
      for(j=0; j<ncol; j++) *pa++=*p++;
    else p+=ncol;
  delete nulr;
  return *a;
}

intMatrix &intMatrix::rmNulCols() const 
{ // Remove null columns except the last one
/*
  intMatrix nulc(1,ncol,0);
  int k=0;
  for(i=0; i<ncol-1; i++)
    if (col(i).isNul()) { nulc(0,i)=1; k++; }
  intMatrix a(nrow,ncol-k,0);
  k=0;
  for(i=0; i<ncol; i++) if (nulc(0,i)==0) { a.col(k,col(i)); k++; }
  return a;
*/

  int* nulc = new int[ncol] ;
  int k=0;  int* p=nulc;
  int i,j;
  for(i=0; i<ncol-1; i++)
    if (colIsNul(i)) { *p++=1; k++; }
    else *p++=0;
  intMatrix *a;
  a=new intMatrix(nrow,ncol-k);
  //intMatrix a(nrow,ncol-k);
  if (nrow==0 || ncol==k) { delete nulc; return *a; }
  *p++=0;
  int* pa=a->_v;  p=_v;
  for(i=0; i<nrow; i++)
    for(j=0; j<ncol; j++)
      if (nulc[j]==0) *pa++=*p++;
      else p++;
  delete nulc;
  return *a;
}

intMatrix intMatrix::rmLastNulCols(int l) const 
{ // Remove last null columns (except the last one),
  //   and those null columns for which col>=l
  int* nulc = new int[ncol] ;
  int k=0;
  int i;
  int* p=nulc;  int j=ncol-1;
  for(i=0; i<ncol-1; i++)
    if (colIsNul(i)) { *p++=1; k++; }
    else { j=i; *p++=0; }
  i=0; p=nulc;
  while (i<j && i<l){
    if (*p==1) { *p=0; k--; }
    p++; i++;
  }
  intMatrix a(nrow,ncol-k);
  if (nrow==0 || ncol==k) { delete nulc; return a; }
  *(nulc+ncol-1)=0;
  int* pa=a._v;  p=_v;
  for(i=0; i<nrow; i++)
    for (j=0; j<ncol; j++)
      if (nulc[j]==0) *pa++=*p++;
      else p++;
  delete nulc;
  return a;
}


intMatrix intMatrix::extractGroupRows (int startRow, int nRows) const
{
  int i,j;
  intMatrix A(nRows,ncol,0) ;
  for(i=0 ; i<nRows ; i++) {
    if ( startRow + i >= nrow ) break ;
    for(j=0 ; j<ncol ; j++) A.at(i,j) = at(i+startRow,j) ;
  }
  return A ;
}

void intMatrix::sizeError(char* where,const intMatrix& m,int a, int b)
{
    cerr << where << ": " 
         << m.nRow() << "x" << m.nCol() << " Matrix expected, "
         << a << "x" << b << " found" << endl;
    abort();
}
void intMatrix::v_alloc(int a, int b)
{
    nrow = a; ncol = b;
#ifdef TRACE
cerr << "v_alloc: nrow=" << nrow << " ncol=" << ncol << endl;
#endif
    _v = new int[a*b];
}

intMatrix::intMatrix(int nr,int nc)
{
    v_alloc(nr,nc);
}

intMatrix::intMatrix(int nr,int nc,int* f)
{
    v_alloc(nr,nc);

    int i,j;
    int* p=_v;
    if ( f==0 )  // initilize 0 matrix
      for (i=0; i<nrow; i++)
        for (j=0; j<ncol; j++) *p++ = 0;
          //at(i,j) = 0;
    else if ( nrow==1 )  // initialize row
            for (j=0; j<ncol; j++) *p++ = f[j];
   	 else if ( ncol==1 )  // initialize column
      		for (i=0; i<nrow; i++) *p++ = f[i];
              else  // initialize matrix
      		for (i=0; i<nrow; i++)
        	  for (j=0; j<ncol; j++) *p++ = f[i*ncol+j];
          	    //at(i,j) = f[i*ncol+j];
}

void intMatrix::init(int f)
{
    int i,j;
      for (i=0; i<nrow; i++)
        for (j=0; j<ncol; j++)
          at(i,j) = f;
}
intMatrix::intMatrix(const intMatrix& m)
{
    v_alloc(m.nrow,m.ncol);
    *this = m;
}
intMatrix::intMatrix(int nr, const intMatrix& M)
{
  int i,j;
    v_alloc(nr,M.ncol);

    int* p1=_v;      int* p2=M._v;
    for(i=0; i<nrow; i++) {
	if (i==M.nrow) break;
	for(j=0; j<ncol; j++) *p1++=*p2++;
    }
}


intMatrix::intMatrix(const intMatrixRow& v)
{
  int j;
    v_alloc(1,v.nCol());
    for(j=0; j<ncol; j++) at(0,j) = v.at(j);
}
intMatrix::intMatrix(const intMatrixCol& v)
{
  int i;
    v_alloc(v.nRow(),1);
    for(i=0; i<nrow; i++) at(i,0) = v.at(i);
}
intMatrix::intMatrix(int k, int* f)
{
  int i,j;
    v_alloc(k,k);
    for(i=0; i<nrow; i++)
      for(j=0; j<ncol; j++)
        at(i,j) = (i==j)? f[i]:0;
}
intMatrix::~intMatrix()
{
    delete _v;
}

int& intMatrix::operator()(int irow, int icol) const
{
    if ( irow<0||irow>=nrow||icol<0||icol>=ncol ) {
      cerr << "at: [" 
           << irow << "," << icol
           << "] out of range [" 
           << nrow << "," << ncol 
           << "]" << endl << *this <<flush;
      exit(0);
      }
//    return at(irow,icol);
    return *(_v+irow*(ncol)+icol);
}

intMatrixRow intMatrix::row(int k) const
{
    return intMatrixRow(k,*this);
}
intMatrixRow intMatrix::row(int k,const intMatrixRow& v) const
{
  int i;
    for(i=0; i<ncol; i++ )
      at(k,i) = v.at(i);
    return v;
}
intMatrix intMatrix::rM(int k) const      // NEW
{
  return intMatrix(row(k));
}
void intMatrix::row(int k,const intMatrix& v, int j)       // NEW
{
  int i;
    for(i=0; i<ncol; i++ ) at(k,i) = v.at(j,i);
}
intMatrixCol intMatrix::col(int k) const
{
    return intMatrixCol(k,*this);
}
intMatrixCol intMatrix::col(int k,const intMatrixCol& v) const
{
  int i;
    for(i=0; i<nrow; i++ )
      at(i,k) = v.at(i);
    return v;
}
void intMatrix::col(int k,const intMatrix& v, int j)       // NEW
{
  int i;
    for(i=0; i<nrow; i++ ) at(i,k) = v.at(i,j);
}
intMatrix intMatrix::cM(int k) const      // NEW
{
  return intMatrix(col(k));
}

void intMatrix::operator=(const intMatrix& m)
{
  int i,j;
    //if (sameSize(m.nrow,m.ncol))
    if ( nrow == m.nrow  &&  ncol == m.ncol ) {
      int* p1=_v;  int* p2=m._v;
      for(i=0; i<nrow; i++)
        for(j=0; j<ncol; j++)  *p1++ = *p2++ ;
            //at(i,j) = m.at(i,j); 
    }
}

int intMatrix::operator==(const intMatrix& m) const
{
  int i,j;
    //if ( ! sameSize(m.nrow,m.ncol) ) return 0;
    if ( nrow != m.nrow  ||  ncol != m.ncol ) return 0;
    int* p1=_v;  int* p2=m._v;
    for(i=0; i<nrow; i++)
      for(j=0; j<ncol; j++)
        if ( *p1++ != *p2++ ) return 0;
    return 1;
}

int intMatrix::operator!=(const intMatrix& m) const
{
//return ! ( *this == m ) ;
    int i,j;
    if ( nrow != m.nrow  ||  ncol != m.ncol ) return 1;
    int* p1=_v;  int* p2=m._v;
    for(i=0; i<nrow; i++)
      for(j=0; j<ncol; j++)
        if ( *p1++ != *p2++ ) return 1;
    return 0;
}

int intMatrix::operator>(const intMatrix& m) const
{
    if ( nrow != m.nrow  ||  ncol != m.ncol ) return 0;
    bool GT=NO; int k;
    int* p1=_v;  int* p2=m._v;
    int i,j;
    for(i=0; i<nrow; i++)
      for(j=0; j<ncol; j++)
	if ( (k=*p1++ - *p2++) > 0 ) GT=YES;
        else if ( k<0 ) return 0;
    if ( !GT ) return 0;
    return 1;
}

intMatrix operator+(const intMatrix& m,const intMatrix& n)
{
    if ( ! m.sameSize(n.nRow(),n.nCol()) ) 
       intMatrix::sizeError("operator+",m,n.nRow(),n.nCol());
// C++2.0 bug
//    intMatrix rm(m.nRow(),m.nCol());
    int nr = m.nRow(), nc =m.nCol();
    intMatrix rm(nr,nc);
    int i,j;
    for(i=0; i<m.nRow(); i++)
      for(j=0; j<m.nCol(); j++)
        rm.at(i,j) = m.at(i,j)+n.at(i,j);
    return rm;
}
intMatrix operator-(const intMatrix& m,const intMatrix& n)
{
    if ( ! m.sameSize(n.nRow(),n.nCol()) ) 
      intMatrix::sizeError("operator-",m,n.nRow(),n.nCol());
// C++2.0 bug
//    intMatrix rm(m.nRow(),m.nCol());
    int nr = m.nRow(), nc =m.nCol();
    intMatrix rm(nr,nc);
    int i,j;
    for(i=0; i<m.nRow(); i++)
      for(j=0; j<m.nCol(); j++)
        rm.at(i,j) = m.at(i,j)-n.at(i,j);
    return rm;
}
intMatrix operator-(const intMatrix& m)
{
// C++2.0 bug
//    intMatrix rm(m.nRow(),m.nCol());
    int nr = m.nRow(), nc =m.nCol();
    intMatrix rm(nr,nc);
    int i,j;
    for(i=0; i<m.nRow(); i++)
      for(j=0; j<m.nCol(); j++)
        rm.at(i,j) = -m.at(i,j);
    return rm;
}
int det(const intMatrix& m)
{
  int i,j;
    if (m.nRow()!=m.nCol()) {
      cerr << "det: not a square matrix" << endl;;
      abort();
      }
    if (m.nRow()==1) return m.at(0,0);
    if (m.nRow()==2) return m.at(0,0)*m.at(1,1)-m.at(0,1)*m.at(1,0);
// FAULTY DET. FOR RANK=3 !!!
//    if (m.nRow()==3) 
//      return (  m.at(0,0)*m.at(1,1)*m.at(2,2)
//                   -m.at(0,1)*m.at(1,2)*m.at(2,0)
//                   +m.at(0,2)*m.at(2,1)*m.at(0,1) );
    if (m.nRow()==3)
        return ( m.at(0,0)*(m.at(1,1)*m.at(2,2)-m.at(1,2)*m.at(2,1))
                -m.at(0,1)*(m.at(1,0)*m.at(2,2)-m.at(1,2)*m.at(2,0))
                +m.at(0,2)*(m.at(1,0)*m.at(2,1)-m.at(1,1)*m.at(2,0)) ); 
    if ( m.isUpperTriangle() ) {
       int val = 1;
       for(i=0;i<m.nRow(); i++) val *=m.at(i,i);
       return val;
       }
    int val = 0;
    int sign = 1;
    for(j=0; j<m.nCol(); j++) {
      val += sign*m.at(0,j)*det(m.coFactor(0,j));
      sign *= -1;
      }
    return val;
}


intMatrix intMatrix::multipliedInv () const
{
  int i,j,k,l;
  if (nrow != ncol) {
    cerr << "Inv: not a square matrix" << endl;;
    abort();
  }
  int n = ncol ;
  int* *T = new int*[n] ;
  for(i=0 ; i<n ; i++ ) {
    T[i] = new int[n] ;
    for(j=0 ; j<n ; j++ )  T[i][j] = (*this).at(i,j) ;
  }

  int r = 0 ;

  int* *S = new int*[n] ;
  for ( i=0 ; i<n ; i++ ) {
    S[i] = new int[n] ;
    for(j=0 ; j<n ; j++ )  S[i][j] = i==j ? 1 : 0 ;
  }
  int* *t = new int*[n] ;
  for ( i=0 ; i<n ; i++ )  t[i] = new int[n] ;
  
  for ( i=0 ; i<n ; i++ )
  {
    int nr = 0 ;
    do {
      int k = r ;
      while ( T[i][k]==0 && k<n ) k = k+1 ;
      if ( k==n )  break ;
      int M = k ;
      nr = 1 ;
      while ( k < n-1 ) {
	k = k+1 ;
	if ( T[i][k] != 0 ) {
	   nr = nr+1 ;
	   if ( abs(T[i][k]) < abs(T[i][M]) )  M = k ;
	}
      }
      if ( nr > 1 ) {
	for(l=0 ; l<n ; l++ )
	  { for(j=0 ; j<n ; j++ )  t[l][j] = l==j ? 1 : 0 ; }
	for(k=r ; k<n ; k++ )
	  if ( k==M )  t[M][k] = 1 ;
	  else {
	    t[M][k] = T[i][k] / abs(T[i][M]) ;
	    if ( T[i][k] % abs(T[i][M]) > abs(T[i][M]) / 2 )  t[M][k]++ ;
	    if ( T[i][M] > 0 )  t[M][k] = -t[M][k] ;
	  }
	for(j=0 ; j<n ; j++ ) {
	  for ( l=0 ; l<n ; l++ )
	    if ( j!=M )  T[l][j] += T[l][M]*t[M][j] ;
	  for ( l=0 ; l<n ; l++ )
	    if ( j!=M )  S[l][j] += S[l][M]*t[M][j] ;
	}
      }
      else
	if ( M != r )
	  for(l=0 ; l<n ; l++ ) {
	    int aux = T[l][r] ;  T[l][r] = T[l][M] ;  T[l][M] = aux ;
	        aux = S[l][r] ;  S[l][r] = S[l][M] ;  S[l][M] = aux ;
	  }
    } while ( nr > 1 );

    if ( nr == 1 )  r++ ;
  }


  if ( r < n ) {
    cerr << "Inv: not a singular matrix" << endl;;
    abort();
  }

  int Delta = 1 ;
  for ( i=0 ; i<n ; i++ )  Delta *= T[i][i] ;
  Delta = abs(Delta) ;

  int* *invT = new int*[n] ;
  for ( i=0 ; i<n ; i++ )  invT[i] = new int[n] ;

  for(j=0 ; j<n ; j++ ) {
    for ( i=0 ; i<j ; i++ )  invT[i][j] = 0 ;
    invT[j][j] = Delta / T[j][j] ;
    for ( i=j+1 ; i<n ; i++ ) {
      invT[i][j] = 0 ;
      for(l=j ; l<i ; l++ )  invT[i][j] -= T[i][l]*invT[l][j]/T[i][i] ;
    }
  }

  intMatrix B(n,n,0) ; 
  for ( i=0 ; i<n ; i++ )
    for ( j=0 ; j<n ; j++ )
      for(k=0 ; k<n ; k++ )  B.at(i,j) += S[i][k] * invT[k][j] ;

  for ( i=0 ; i<n ; i++ )  { delete S[i] ; delete t[i] ; delete invT[i] ; delete T[i] ; }
  delete S ;  delete t ;  delete invT ;  delete T ;

  return B ;
}


int norm(const intMatrix& m)
{
  int i,j;
    int val =0;
    for(i=0; i<m.nRow(); i++)
      for(j=0; j<m.nCol(); j++)
        if ( abs(m.at(i,j))>val ) val = abs(m.at(i,j));
    return val;
}
intMatrix operator*(const intMatrix& m,const intMatrix& n)
{
  int i,j;
    if ( m.nCol()!=n.nRow() ) {
         cerr << "operator*: " << m.nCol() << "x* intMatrix expected " 
          << n.nRow() << "x* found:\n" << m << n << endl;
       exit(0);
       }
// C++2.0 bug
//    intMatrix rm(m.nRow(),n.nCol());
    int nr = m.nRow(), nc =n.nCol();
    intMatrix rm(nr,nc);
    for(i=0; i<rm.nRow(); i++)
      for(j=0; j<rm.nCol(); j++)
        rm.at(i,j) = m.row(i)^n.col(j);
    return rm;
}
intMatrix operator&(const intMatrix& m, const intMatrix& n)
{
  int i,j;
    if ( m.nRow()!=n.nRow() ) {
       cerr << "operator&: " << m.nRow() << " rows expected, "
        << n.nRow() << " found" << endl;
       exit(0);
       }
// C++2.0 bug
//    intMatrix rm(m.nRow(),m.nCol()+n.nCol());
    if (m.nCol()<1 && n.nCol()<1) return intMatrix(n.nRow(),0,0);
    if (m.nCol()<1) return n;
    if (n.nCol()<1) return m;
    int nr = m.nRow(), nc =m.nCol()+n.nCol();
    intMatrix rm(nr,nc);
    for(i=0; i<m.nCol(); i++ )
       rm.col(i,m.col(i));
    for(j=0; j<n.nCol(); j++ )
       rm.col(m.nCol()+j,n.col(j));
    return rm;      
}
intMatrix operator%(const intMatrix& m, const intMatrix& n)
{
  int i,j;
    if ( m.nCol()!=n.nCol() ) {
       cerr << "operator%: " << m.nCol() << " Columns expected, "
        << n.nCol() << " found" << endl;
       exit(0);
       }
// C++2.0 bug
//    intMatrix rm(m.nRow()+n.nRow(),m.nCol());
    if (m.nRow()<1 && n.nRow()<1) return intMatrix(0,n.nCol(),0);
    if (m.nRow()<1) return n;
    if (n.nRow()<1) return m;
    int nr = m.nRow()+n.nRow(), nc =m.nCol();
    intMatrix rm(nr,nc);
    for(i=0; i<m.nRow(); i++ )
       rm.row(i,m.row(i));
    for(j=0; j<n.nRow(); j++ )
       rm.row(m.nRow()+j,n.row(j));
    return rm;      
}
intMatrix intMatrix::t() const
{
  int i,j;
    intMatrix rm(ncol,nrow);
    for(i=0; i<rm.nrow; i++)
      for(j=0; j<rm.ncol; j++)
        rm.at(i,j) = at(j,i);
    return rm;
}

intMatrix operator*(int f,const intMatrix& m)
{
  int i,j;
// C++2.0 bug
//    intMatrix rm(m.nRow(),m.nCol());
    int nr = m.nRow(), nc =m.nCol();
    intMatrix rm(nr,nc);
    for(i=0; i<rm.nRow(); i++)
      for(j=0; j<rm.nCol(); j++)
        rm.at(i,j) = f*m.at(i,j);
    return rm;
}
void intMatrix::operator*=(int f)
{
  int i,j;
    for(i=0; i<nrow; i++)
      for(j=0; j<ncol; j++)
        at(i,j) *= f;
}
void intMatrix::switchRows(int i,int j)
{
    if (i!=j)
    { intMatrix tmp(row(i));
      row(i,row(j));
      row(j,tmp.row(0));    }
}
void intMatrix::switchCols(int i,int j)
{
    if (i!=j)
    { intMatrix tmp(col(i));
      col(i,col(j));
      col(j,tmp.col(0));    }
}
void intMatrix::combineRows(int i, int a, int j)
{
  int h;
    for(h=0; h<ncol; h++)
      at(i,h) = at(i,h) + a*at(j,h);
}

int intMatrix::isUpperTriangle() const
{
  int i,j;
    for(j=0; j<ncol; j++)
       for(i=j+1; i<nrow; i++)
         if ( at(i,j)!=0 ) return 0;
    return 1;
}

intMatrix intMatrix::coFactor(int irow, int jcol) const
{
  int i,j;
    if ( nrow==1||ncol==1 ) {
      cerr << "coFactor: can't coFactor row or column matrix" << endl;
      cerr << *this <<flush;
      exit(0);
      }
    intMatrix val(nrow-1,ncol-1);
    int getcol, getrow =0;
    for(i=0; i<val.nRow(); i++) {
      if ( getrow==irow ) ++getrow;
      if ( getrow==nrow ) break;
      getcol = 0;
      for(j=0; j<val.nCol(); j++) {
        if ( getcol==jcol ) ++getcol;
        if ( getcol==ncol ) continue;
        val.at(i,j) = at(getrow,getcol);
        ++getcol;
        }
      ++getrow;
      }
    return val;
}
intMatrix intMatrix::coFactor(int irow, int jcol,intMatrix& m) const
{
  int i,j;
    if ( nrow==1||ncol==1 ) {
      cerr << "coFactor: can't coFactor row or column matrix" << endl;
      cerr << *this <<flush;
      exit(0);
      }
    if ( m.nRow()!=nrow-1||m.nCol()!=ncol-1 ) {
      cerr << "coFactor: argument is wrong size" << endl;
      abort();
      } 
    intMatrix val = coFactor(irow,jcol);
    int putcol, putrow =0;
    for(i=0; i<m.nRow(); i++) {
      if ( putrow==irow ) ++putrow;
      if ( putrow==nrow ) break;
      putcol = 0;
      for(j=0; j<m.nCol(); j++) {
        if ( putcol==jcol ) ++putcol;
        if ( putcol==ncol ) continue;
        at(putrow,putcol) = m.at(i,j);
        ++putcol;
        }
      ++putrow;
      }
    return val;
}
void intMatrix::printOn(ostream& strm) const
{
  int i,j;
  intMatrix _max(1,ncol,0);
  for(j=0; j<ncol; j++)
  { for(i=0; i<nrow; i++) if(at(i,j)<0) _max(0,j)=1; }
  for(i=0; i<nrow; i++ )
  { strm<<"[ ";
    for(j=0; j<ncol; j++)
    { if ((_max(0,j)==1) && (at(i,j)>=0)) strm<<" "; strm<<at(i,j)<<" ";}
    if (i<nrow-1) strm<<"]\n"; else strm<<"]*\n";
  }
}
void intMatrix::dmpOn(ostream& strm, int indent) const
{
/*
  int i,j;
  char * offs(' ',indent);
  strm << offs<< "RxC: " << nRow() << " " << nCol() << endl;
  for(i=0; i<nRow(); i++)
  { strm << offs;
    for(j=0; j<nCol(); j++) strm << at(i,j) << " ";
    strm << endl;
  }
*/
}
//void Matrix::dmpOn(ostream& strm) const
//{
//    strm << "Matrix[" << nrow << " " << ncol << endl;
//    printOn(strm);
//    strm << "]" << endl;
//}
intMatrixRow::intMatrixRow(const intMatrixRow& r)
{
    pm = r.pm;
    _row = r._row;
}
intMatrixRow::intMatrixRow(int k,const intMatrix& m)
{
    pm = (intMatrix*)&m;
    _row = k;
}
// -------- added functionality
bool intMatrixRow::isNul() const
{
  int i;
  for(i=0; i<nCol(); i++) if ( at(i)!=0 ) return 0;
  return 1;
}
// ---------------------------
int intMatrixRow::operator^(const intMatrixCol& c) const
{
  int i;
    if ( nCol()!=c.nRow() ) {
       cerr << "operator^: 1x" << nCol()
        << " mismatched with " << c.nRow() 
        << "x1.\n" << *this << c << endl;
       abort();
       }   
    int val = 0;
    for(i=0; i< nCol(); i++ )
      val += at(i)*c.at(i);
    return val;    
}
void intMatrixRow::operator=(const intMatrixRow& r)
{
    if ( nCol()!=r.nCol() ) {
      cerr << "operator=: intMatrixRow of size " << nCol()
           << " expected, size " << r.nCol() << " found." << endl,
      abort();
      }
    pm = r.pm;
    _row = r._row;
}
void intMatrixRow::operator+=(const intMatrixRow& r)
{
  int i;
    for(i=0; i<nCol(); i++ )
      at(i) += r.at(i);
}
// -------- added functionality
void intMatrixRow::operator-=(const intMatrixRow& r)
{
  int i;
    for(i=0; i<nCol(); i++ )
      at(i) -= r.at(i);
}
void intMatrixRow::printOn(ostream& strm) const
{
    strm << "[ ";
    for( int i=0; i<nCol(); i++ )
    { if (at(i)>=0) strm<<" "; strm << at(i) << " "; }
    strm << "]";
}
intMatrixCol::intMatrixCol(const intMatrixCol& c)
{
    pm = c.pm;
    _col = c._col;
}
intMatrixCol::intMatrixCol(int k, const intMatrix& m)
{
    pm = (intMatrix*)&m;
    _col = k;
}
// -------- added functionality
bool intMatrixCol::isNul() const
{
  int i;
  for(i=0; i<nRow(); i++) if ( at(i) != 0 ) return 0;
  return 1;
}
void intMatrixCol::operator=(const intMatrixCol& c)
{
    if ( nRow()!=c.nRow() ) {
      cerr << "operator=: intMatrixCol of size " 
           << nRow() << " expected, size " 
           << c.nRow() << " found." << endl;
      abort();
      }
    pm = c.pm;
    _col = c._col;
}
void intMatrixCol::operator+=(const intMatrixCol& c)
{
  int i;
    for(i=0; i<nRow(); i++ )
      at(i) += c.at(i);
}
// -------- added functionality
void intMatrixCol::operator-=(const intMatrixCol& c)
{
  int i;
    for(i=0; i<nRow(); i++ )
      at(i) -= c.at(i);
}
// ---------------------------- 
void intMatrixCol::printOn(ostream& strm) const
{
    strm << "t[ ";
    for( int i=0; i<nRow(); i++ )
      strm << at(i) << " ";
    strm << "]";
}

ostream& operator<<(ostream& strm,const intMatrix& m)
{
    m.printOn(strm);
    return strm;
}
ostream& operator<<(ostream& strm,const intMatrixRow& m)
{
    m.printOn(strm);
    return strm;
}
ostream& operator<<(ostream& strm,const intMatrixCol& m)
{
    m.printOn(strm);
    return strm;
}

void intMatrix::put(void) const
{
  int* p=_v;
  int i,j;
  for(i=0; i<nrow; i++){
    for(j=0; j<ncol; j++){
      printf("%d \t",*p++);
    }
    printf("\n");
  }
}
