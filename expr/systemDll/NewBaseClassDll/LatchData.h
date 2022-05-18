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
/*************************************************************
 *file  : LatchData.h                                        *
 *authors : vgee,ashok                                       *
 *last modified :                                            *
 *                                                           *
 *contents : contains the Data classes which will reside in  *
 *           the latches.                                    *
 *compiled with : g++                                        *
 *for more info :                                            *
 *caution : always grep for TODO to check for incompleteness *
 *************************************************************/
#ifndef _LATCHDATA_H_
#define _LATCHDATA_H_

#include "AddrData.h"

/*************************************************************
 *class :                                                    *
 *        OneOperandData                                     *
 *description :                                              *
 *        this class contains single data element.           *
 *Note to consumers of this class :                          *
 *        This class is templated for your convenience butthe*
 *template class should always be a "derived class of class  *
 *BaseType" or "Val"  or "AddrDataUnion".  we cannot garuntee*
 *proper operation if you pass any other type of class.      *
 *                                                           *
 *Methods :                                                  *
 *Data Members :                                             *
 *ToDo List :                                                *                
 *************************************************************/

class SimpleData : public BaseType{
protected:
	int _opCode;
public:
	SimpleData(int opCode = _INVALID_) : _opCode(opCode){}
	~SimpleData(){}
	SimpleData(const SimpleData & b) : _opCode(b._opCode){}

	virtual void setOpCode(int &b){
		_opCode = b;
	}
	virtual int getOpCode(){
		return _opCode;
	}

	virtual BaseType *probeIRData() { return NULL; }
};

template <class S, class T>
class OneOperandData : public SimpleData{
 protected : // SimpleData contains the opcode.
 S _s1;
 T _t1;
 public :
 OneOperandData(): _s1(), _t1() {_opCode = NO_OP;}
 ~OneOperandData(){}
 OneOperandData(const OneOperandData<S, T> & b) {_opCode = b._opCode; _s1 = b._s1; _t1 = b._t1;}
 OneOperandData(const SimpleData & b)  { _opCode = (b._opCode); }
 // changed by Sumit on Oct 13, 1999 from 
 // OneOperandData(const SimpleData & b) { SimpleData::SimpleData(b); }

 virtual BaseType * copy()
 {
  BaseType * retPtr;  
  retPtr = new OneOperandData(*this);
  return retPtr;
 }

 virtual int is(BaseTypeEnum ctype) {return ((ctype == _ONEOPERANDDATA_) ? _YES_ : BaseType::is(ctype));}
 virtual void reset() {_s1.reset(); _t1.reset();}
 void operator =(const OneOperandData & b) {_opCode = b._opCode;_s1 = b._s1; _t1 = b._t1;}

 void print(ostream & out)
 {
  out << "OpCode: " << _opCode << " Op1: ";
  _s1.print(out);
  _t1.print(out);
 }

 virtual void setLatchData(OneOperandData<S, T>& b) {_s1 = b._s1; _t1 = b._t1;}
 virtual void setLatchData(S& b) {_s1 = b;}
 S& getLatchData() {return _s1;}

 virtual void setIRData(T & b) { _t1 = b;}
 virtual T & getIRData() { return _t1; }

 virtual BaseType *probeIRData() { return &(_t1); }
};

/*************************************************************
 *class :                                                    *
 *        TwoOperandData                                     *
 *description :                                              *
 *        This class has the opcode and operand fields this  *
 *class is in the output stage of Execute                    *
 *Note to consumers of this class :                          *
 *        This class is templated for your convenience butthe*
 *template class should always be a "derived class of class  *
 *BaseType" or "Val"  or "AddrDataUnion".  we cannot garuntee*
 *proper operation if you pass any other type of class.      *
 *(a sample derived class from "BaseType" is "AddrOperand".) *
 *                                                           *
 *Methods :     decodeInstruction()                          *
 *Data Members :                                             *
 *ToDo List :                                                *                
 *************************************************************/
template <class Q, class R, class T>
class TwoOperandData : public SimpleData{
 protected: // SimpleData contains the opcode.
 Q _d1;
 R _d2;
 T _t1;

 public:
 TwoOperandData() :_d1(),_d2(), _t1() {_opCode = NO_OP;}
 ~TwoOperandData() {}
 TwoOperandData(const TwoOperandData<Q,R, T>& b)
 {
  _opCode = b._opCode; 
  _d1 = b._d1; 
  _d2 = b._d2;
  _t1 = b._t1;
 }
 TwoOperandData(const SimpleData & b) { _opCode = (b._opCode); }
// changed by Sumit on Oct 13, 1999 from 
// TwoOperandData(const SimpleData & b) { SimpleData::SimpleData(b); }
  
 virtual BaseType * copy()
 {
  BaseType* retPtr;  
  retPtr = new TwoOperandData(*this);
  return retPtr;
 }

 virtual int is(BaseTypeEnum ctype) {return ((ctype == _TWOOPERANDDATA_)?_YES_:BaseType::is(ctype));}

 virtual void reset() 
 {
  _opCode = 0; //TODO
  _d1.reset();
  _d2.reset();
  _t1.reset();
 }

 void operator =(const TwoOperandData<Q,R, T>& b) 
 {
  _opCode = b._opCode;
  _d1 = b._d1;
  _d2 = b._d2;
  _t1 = b._t1;
 }

 void print(ostream & out)
 {
  out << "OpCode: " << _opCode << " Op1: ";
  _d1.print(out);
  out << " Op2: ";
  _d2.print(out);
  out << "IR: ";
  _t1.print(out);
 }

 virtual void setLatchData(Q& a, R& b, T& c)
 {
  _d1 = a; 
  _d2 = b;
  _t1 = c;
 }

 virtual void getLatchData(Q& a, R& b, T& c)
 {
  a = _d1; 
  b = _d2;
  c = _t1;
 }

 virtual void setOperand1(Q& a) {_d1 = a;}
 virtual void setOperand2(R& b) {_d2 = b;}
 virtual Q&   getOperand1() {return _d1;}
 virtual R&   getOperand2() {return _d2;}

 virtual void setIRData(T& b) {_t1 = b;}
 virtual T&   getIRData() {return _t1;}

 virtual BaseType *probeIRData() { return &(_t1); }
};

/*************************************************************
 *class :                                                    *
 *        ThreeOperandData                                   *
 *description :                                              *
 *        This class has the opcode and operand fields this  *
 *class is in the output stage of decode                     *
 *Note to consumers of this class :                          *
 *        This class is templated for your convenience butthe*
 *template class should always be a "derived class of class  *
 *BaseType" or "Val"  or "AddrDataUnion".  we cannot garuntee*
 *proper operation if you pass any other type of class.      *
 *                                                           *
 *Methods :     decodeInstruction()                          *
 *Data Members :                                             *
 *ToDo List :                                                *                
 *************************************************************/
template <class Q, class R, class S, class T>
class ThreeOperandData : public SimpleData{
 protected: // SimpleData contains the opcode.
 Q _d1;
 R _s1;
 S _s2;
 T _t1;

 public:
 ThreeOperandData() : _d1(), _s1(), _s2(), _t1() {_opCode = NO_OP ;}
 ~ThreeOperandData() {}
 ThreeOperandData(const ThreeOperandData<Q,R,S,T>& b)
 { 
  _opCode = b._opCode; 
  _d1 = b._d1; 
  _s1 = b._s1; 
  _s2 = b._s2;
  _t1 = b._t1;
 }

 ThreeOperandData(const SimpleData & b)  { _opCode = (b._opCode); }
// changed by Sumit on Oct 13, 1999 from 
// ThreeOperandData(const SimpleData & b) { SimpleData::SimpleData(b); }
  
 virtual BaseType * copy() 
 {
  BaseType * retPtr;  
  retPtr = new ThreeOperandData(*this);
  return retPtr;
 }

 virtual int is(BaseTypeEnum ctype) {return ((ctype == _THREEOPERANDDATA_) ? _YES_ : BaseType::is(ctype));}
 virtual void reset()
 {
   _opCode = NO_OP; // TODO: Define NOP properly. (i.e. I am not sure
		    // that this is exactly right.)
  _d1.reset();
  _s1.reset();
  _s2.reset();
  _t1.reset();
 }

 void operator = (const ThreeOperandData<Q, R, S, T> & b) 
 {
  _opCode = b._opCode;
  _d1 = b._d1;
  _s1 = b._s1;
  _s2 = b._s2;
  _t1 = b._t1;
 }

 friend int operator == (const ThreeOperandData<Q, R, S, T> & a, const ThreeOperandData<Q, R, S, T> & b) 
 {
	 if ((a._opCode == b._opCode)
		 && (a._d1 == b._d1)
		 && (a._s1 == b._s1)
		 && (a._s2 == b._s2)
		 && (a._t1 == b._t1))
		 return _YES_;
	 else
		 return _NO_;
 }

 void print(ostream & out)
 {
   
  out << "OpCode: " << _opCode << " Dest: R";
  _d1.print(out);
  out << " Src1: ";
  _s1.print(out);
  out << " Src2: ";
  _s2.print(out);
  out << " IR: ";
  _t1.print(out);  
 }
       
 virtual void setLatchData(Q& a, R& b, S& c, T& d)
 {
  _d1 = a; 
  _s1 = b; 
  _s2 = c;
  _t1 = d;
 }

 virtual void getLatchData(Q& a, R& b, S& c, T& d)
 {
  a = _d1; 
  b = _s1; 
  c = _s2;
  d = _t1;
 }

 virtual void setOperand1(Q& a) {_d1 = a; }
 virtual void setOperand2(R& b) {_s1 = b; }
 virtual void setOperand3(S& b) {_s2 = b; }
 virtual Q&   getOperand1() { return _d1; }
 virtual R&   getOperand2() { return _s1; }
 virtual S&   getOperand3() { return _s2; }

 virtual void setIRData(T& b) {_t1 = b; }
 virtual T&   getIRData() { return _t1; }
 
 virtual BaseType *probeIRData() { return &(_t1); }
};

/*************************************************************
 *class :                                                    *
 *        ListOperandData                                    *
 *description :                                              *
 *        This class has the opcode and operand fields this  *
 *class is in the output stage of decode                     *
 *Note to consumers of this class :                          *
 *        This class is templated for your convenience butthe*
 *template class should always be a "derived class of class  *
 *BaseType" or "Val"  or "AddrDataUnion".  we cannot garuntee*
 *proper operation if you pass any other type of class.      *
 *                                                           *
 *Methods :     decodeInstruction()                          *
 *Data Members :                                             *
 *ToDo List :                                                *                
 *************************************************************/
template <class Q, class R, class T>
class ListOperandData : public SimpleData {
 protected: // SimpleData contains the opcode.
 Vector<Q> _d;
 Vector<R> _s;

 T _t1;

 public:
 ListOperandData() : _d(), _s(), _t1() {_opCode = NO_OP ;}
 ~ListOperandData() 
 {
	 _d.clear();
	 _s.clear();
 }

 ListOperandData(const ListOperandData<Q,R,T>& b)
 { 
  _opCode = b._opCode; 
  _d = b._d; 
  _s = b._s; 
  _t1 = b._t1;
 }

 ListOperandData(SimpleData & b)  { _opCode = (b.getOpCode()); }
// changed by Sumit on Nov 4, 1999 from 
// ListOperandData(const SimpleData & b) {SimpleData(b);}
 
 virtual BaseType * copy() 
 {
  BaseType * retPtr;  
  retPtr = new ListOperandData(*this);
  return retPtr;
 }

 virtual int is(BaseTypeEnum ctype) {return ((ctype == _LISTOPERANDDATA_) ? _YES_ : BaseType::is(ctype));}
 virtual void reset()
 {
   _opCode = NO_OP; // TODO: Define NOP properly. (i.e. I am not sure
		    // that this is exactly right.)
  _d.clear();
  _s.clear();
  _t1.reset();
 }

 void operator = (const ListOperandData<Q, R, T> & b) 
 {
  _opCode = b._opCode;
  _d = b._d;
  _s = b._s;
  _t1 = b._t1;
 }

 void print(ostream & out)
 {
	 out << "OpCode: " << _opCode << " Dests: ";
	 
	 Vector<Q> :: iterator d1, d2;
	 d1 = _d.begin();
	 d2 = _d.end();
	 
	 while (d1 != d2)
	 {
		 (*d1).print(out);
		 out << ", ";
		 d1++;
	 }
	 
	 out << " Srcs: ";
	 
	 Vector<R> :: iterator s1, s2;
	 s1 = _s.begin();
	 s2 = _s.end();
	 
	 while (s1 != s2)
	 {
		 (*s1).print(out);
		 out << ", ";
		 s1++;
	 }
	 
	 out << " IR: ";
	 _t1.print(out);
	 
 }
       
 virtual void setLatchData(Vector<Q> & a, Vector<R> & b, T & d)
 {
  _d = a; 
  _s = b; 
  _t1 = d;
 }

 virtual void getLatchData(Vector<Q> & a, Vector<R> & b, T& d)
 {
  a = _d; 
  b = _s; 
  d = _t1;
 }

 virtual void addOperand1(Q& a) {_d.push_back(a); }
 virtual void addOperand2(R& b) {_s.push_back(b); }

 virtual Vector<Q> & getOperand1() { return _d; }

 virtual int hasFirstOperand1()
 {
	 if (!_d.empty()) return _YES_;

	 return _NO_;
 }
 virtual Q & getFirstOperand1() { return _d.front(); }
 virtual Vector<R> & getOperand2() { return _s; }

 virtual void setIRData(T& b) {_t1 = b; }
 virtual T&   getIRData() { return _t1; }

 virtual BaseType *probeIRData() { return &(_t1); }
};

#endif


