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
#ifndef BasicSet_H
#define BasicSet_H 1


class BasicSet;
#ifndef WIN32
typedef struct BasicSet * bset;
#else
typedef class BasicSet * bset;
#endif

class BasicSet {
	long int latticePoints ;
	intMatrix Coord  ; //T
	intMatrix Constr ; //A
public:
	BasicSet (const BasicSet &b);
	BasicSet (const intMatrix& , const intMatrix&) ;
	BasicSet (const intMatrix& , const intMatrix& , int points) ;

	long int nrPts() const { return latticePoints ; }
	const BasicSet* operator^(const BasicSet&) const;
	long int operator&(const BasicSet&) const;
	bool operator==(const BasicSet&) const;
	bool operator!=(const BasicSet& a) const { return !(*this==a); }
	void printOn(ostream& strm) const;
	void put(void) const;
};

BasicSet* ConstructBasicSet (intMatrix& T, intMatrix& A) ;
int CompactConstraints (intMatrix& A) ;
int DiophantineSystem (int m, int n, int* *T, int* u, int* *V, int* v) ;
long int CountPolytopePoints (intMatrix A) ;
bool LatticePoints (intMatrix& A) ;
int Fourier_MotzkinElimination (const intMatrix& A) ;
intMatrix EliminateCol (const intMatrix& A, int col) ;
int Range (const intMatrix& A) ;
int gcd (int a, int b) ;
long int CountImagePolytope (intMatrix T, intMatrix& P) ;
long int CountPrefixes (intMatrix& P, int r) ;

#endif
