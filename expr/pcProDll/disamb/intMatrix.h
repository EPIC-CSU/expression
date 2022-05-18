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
#ifndef intMatrixH
#define intMatrixH 1

#define YES 1
#define NO  0

class intMatrixCol;
class intMatrixRow;

class intMatrix;
#ifndef WIN32
typedef struct intMatrix * intmat;
#else
typedef class intMatrix * intmat;
#endif

class intMatrix{
    int nrow, ncol;
    void v_alloc(int,int);
public:
    int* _v;
    intMatrix(int nr=0, int nc=0);
    intMatrix(int nr, int nc, int* f);
    intMatrix(const intMatrix&);
    intMatrix(int nr, const intMatrix& M);
    intMatrix(const intMatrixRow&);
    intMatrix(const intMatrixCol&);
    intMatrix(int,int*);
    ~intMatrix();
    void init(int =0);
    void Set(int i,int j,int v);
    int Get(int i,int j);
    virtual void deepenShallowCopy() {}
    bool isNul() const;
    bool matIsNul() const;
    bool vecIsNul() const;
    bool rowIsNul (int i) const;
    bool colIsNul (int j) const;
    intMatrix Row(int i) const;
    intMatrix Col(int j) const;
    int* addrRow(int i) const { return _v+i*ncol; }
    int det(const intMatrix&);
    intMatrix multipliedInv () const;
    int norm(const intMatrix&);
    intMatrix diff() const;
    int dim() const;
    intMatrix equality(intMatrix&) const;
    intMatrix affineMat() const;
    intMatrix affineVec() const;
    intMatrix affineTr(const intMatrix&) const;
    intMatrix affineMult(const intMatrix&) const;
    intMatrix extendIneqDim(int,intMatrix* range = 0) const;
    intMatrix &rmNulRows() const;
    intMatrix &rmNulCols() const;
    intMatrix rmLastNulCols(int l) const;
    intMatrix extractGroupRows (int startRow, int nRows) const;
    int& at(int irow,int icol) const { return operator()(irow,icol); }
    int& operator()(int irow,int icol) const; 
    intMatrixRow row(int) const;
    intMatrixRow row(int,const intMatrixRow&) const;
    intMatrixCol col(int) const;
    intMatrixCol col(int,const intMatrixCol&) const;
    intMatrix rM(int k) const;
    intMatrix cM(int k) const;
    void row(int, const intMatrix&, int);
    void col(int, const intMatrix&, int);
    int sameSize(int a,int b) const { return (nrow==a&&ncol==b); }
    int nRow() const { return nrow; }
    int nCol() const { return ncol; }
    void operator=(const intMatrix&);
    int operator==(const intMatrix&) const;
    int operator!=(const intMatrix&) const;
    int operator>(const intMatrix&) const;
    intMatrix t() const;  // transpose
    void operator*=(int);
    void switchRows(int,int);
    void switchCols(int,int);
    void combineRows(int i,int b,int j); // row(i) += b*row(j)
    void combineCols(int i,int b,int j); // col(i) += b*col(j)
    int isUpperTriangle() const;
    intMatrix coFactor(int,int) const;
    intMatrix coFactor(int,int,intMatrix&) const;
    virtual void printOn(ostream&) const;
    virtual void dmpOn(ostream&, int indent=0) const;
    static void sizeError(char* where,const intMatrix&,int,int);
    void put(void) const;
};

//determinent
extern int det(const intMatrix&);
extern int norm(const intMatrix&);
extern intMatrix operator+(const intMatrix&,const intMatrix&);
extern intMatrix operator-(const intMatrix&,const intMatrix&);
extern intMatrix operator*(const intMatrix&,const intMatrix&);
extern intMatrix operator-(const intMatrix&);
// concatenate columns
extern intMatrix operator&(const intMatrix&,const intMatrix&);

// concatenate rows
extern intMatrix operator%(const intMatrix&,const intMatrix&);

// scalar multiply    
extern intMatrix operator*(int,const intMatrix&);
    
class intMatrixCol {
    intMatrix* pm;
    int _col;    // index of this column
public:
    intMatrixCol(int,const intMatrix&);
    intMatrixCol(const intMatrixCol&);
    bool isNul() const;
    int nRow() const { return pm->nRow(); }
    int& at(int i) const { return pm->at(i,_col); }
    int& operator()(int i) const { return pm->operator()(i,_col); }
    void operator=(const intMatrixCol&);
    void operator+=(const intMatrixCol&);
    void operator-=(const intMatrixCol&);
    virtual void printOn(ostream&) const;
};

class intMatrixRow {
    intMatrix* pm;
    int _row;    // index of this row
public:
    intMatrixRow(int,const intMatrix&);
    intMatrixRow(const intMatrixRow&);
    bool isNul() const;
    int nCol() const { return pm->nCol(); }
    int& at(int i) const { return pm->at(_row,i); }
    int& operator()(int i) const { return pm->operator()(_row,i); }
    int operator^(const intMatrixCol&) const; // dot product
    void operator=(const intMatrixRow&);
    void operator+=(const intMatrixRow&);
    void operator-=(const intMatrixRow&);
    virtual void printOn(ostream&) const;
};

extern ostream& operator<<(ostream& strm,const intMatrix& m);
extern ostream& operator<<(ostream& strm,const intMatrixRow& m);
extern ostream& operator<<(ostream& strm,const intMatrixCol& m);
#endif/*intMatrixH*/


