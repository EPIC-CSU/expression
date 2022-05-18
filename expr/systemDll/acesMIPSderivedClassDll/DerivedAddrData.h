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
 *file  : DerivedAddrData.cpp                                *
 *created : 3 May, 2000.                                     *
 *authors : Prabhat, Srikanth                                *
 *last modified :                                            *
 *                                                           *
 *contents:contains class methods for Derived Address and    *
 *         Data classes                                      *
 *compiled with : VC++                                       *
 *for more info :                                            *
 *************************************************************/

#ifndef _DERIVEDADDRDATA_H_
#define _DERIVEDADDRDATA_H_


#include "AddrData.h"
#include "DerivedClassIncludes.h"

#include "..\..\pcProDll\include\BaseInstruction.h"
#include "..\..\pcProDll\include\BaseOperation.h"

typedef class BaseInstruction *InstrPtr;
typedef class BaseOperation *OperPtr;


class MyBool: public BaseType
{
public:
	bool _id;

	MyBool(){_id = false;}
	MyBool(bool a){_id = a;}
	MyBool(const MyBool &a){_id = a._id;}
	~MyBool(){}

	void operator=(const MyBool & a)
	{
		_id = a._id;
	}

	void reset()
	{
		_id = false;
	}
};

class MyInt: public BaseType
{
public:
	int _id;

	MyInt(){_id = -1;}
	MyInt(int a){_id = a;}
	MyInt(const MyInt &a){_id = a._id;}
	~MyInt(){}

	void operator=(const MyInt & a)
	{
		_id = a._id;
	}

	void reset()
	{
		_id = -1;
	}
	
	void print(ostream &out)
	{
		out << _id;
	}
};

class MyBusyElem: public BaseType
{
public:
	bool _id;

	int _fu; // This field stores the FU that is currently computing the value (and hence keeping this register busy).

	MyBusyElem() : _id(false), _fu(_INVALID_) {}
	MyBusyElem(bool a, int fu) : _id(a), _fu(fu) {}
	MyBusyElem(const MyBusyElem &a) : _id(a._id), _fu(a._fu) {}
	~MyBusyElem(){}

	void operator=(const MyBusyElem & a)
	{
		_id = a._id;
		_fu = a._fu;
	}

	void reset()
	{
		_id = false;
		_fu = _INVALID_;
	}
};

/*************************************************************
 *class : PCType                                             *
 *description :                                              *
 *  This class contains the PC(program counter).             *
 *Methods :                                                  *   
 *ToDo List :                                                *
 *************************************************************/
class PCType : public BaseType{
 private:
   InstrPtr _pc;
 public:
 PCType(){}
 ~PCType(){}
 PCType(const PCType& b) : _pc(b._pc){}
 PCType(const InstrPtr& b): _pc(b){}
 
  virtual BaseType * copy()
  {
   BaseType * retPtr;  
   retPtr = new PCType(*this);
   return retPtr;
  }
  virtual int is(BaseTypeEnum ctype)
    { return ((ctype == _PCTYPE_) ? _YES_ : BaseType::is(ctype));}
  friend int operator ==(const PCType &a, const PCType &b){return _NO_;}
  virtual void print(ostream &out)
    {
      out << "The value of the pc is :" << _pc ;
    }
  virtual int isEq(PCType * ptr){return _NO_;}
  void operator=(const PCType &a){_pc = a._pc;}

  virtual void reset(){ _pc = NULL; }
  InstrPtr& getPC(){return _pc;}
  void setPC(InstrPtr & b) { _pc = b; }

  int isValid(){ return ((_pc == NULL) ? _NO_ : _YES_); }
};

/*************************************************************
 *class : PgmAddr                                            *
 *description :                                              *
 *  This class contains the program Addresses for PC.        *
 *Methods :                                                  *   
 *ToDo List :                                                *
 *************************************************************/
class PgmAddr : public BaseType{
private:
	long _addr;
public:
	PgmAddr(){}
	
	~PgmAddr(){}
	
	PgmAddr(const PgmAddr& b)   
	{ 
		_addr = b._addr; 
	}

	PgmAddr(const long b) 
	{ 
		_addr = b; 
	}
	
	virtual BaseType * copy()
	{
		BaseType * retPtr;  
		retPtr = new PgmAddr(*this);
		return retPtr;
	}
	
	virtual int is(BaseTypeEnum ctype)
	{ 
		return ((ctype == _PGMADDR_) ? _YES_ : BaseType::is(ctype));
	}

	friend int operator ==(const PgmAddr &a, const PgmAddr &b)
	{
		return _NO_;
	}
	
	virtual void print(ostream &out)
    {
		out << "The value of the address is :" << _addr ;
    }

	virtual int isEq(PgmAddr * ptr)
	{
		return _NO_;
	}
	
	void operator=(const PgmAddr &a)
	{ 
		_addr = a._addr;
	}
	
	virtual void reset()
	{ 
		_addr = 0; 
	}

	long getAddr()
	{
		return _addr;
	}

	void setPgmAddr(long b) 
	{ 
		_addr = b; 
	}
	
	int isValid()
	{ 
		return ((_addr < 0) ? _NO_ : _YES_); 
	}
};


/*************************************************************
 *class : InstrStr                                           *
 *description :                                              *
 *  This class contains the Instruction string      .        *
 *Methods :                                                  *   
 *ToDo List :                                                *
 *************************************************************/
class InstrStr : public BaseType{
private:
	vector<MyString> _instrns;
public:
	InstrStr(){}
	
	~InstrStr(){}
	
	InstrStr(const InstrStr& b) {
		_instrns = b._instrns;
	}
	
	InstrStr(const vector<MyString> & b) {
		_instrns = b;
	}
	
	virtual BaseType * copy()
	{
		BaseType * retPtr;  
		retPtr = new InstrStr(*this);
		return retPtr;
	}
	
	virtual int is(BaseTypeEnum ctype)
	{ 
		return ((ctype == _INSTRSTR_) ? _YES_ : BaseType::is(ctype));
	}

	friend int operator ==(const InstrStr &a, const InstrStr &b)
	{
		return _NO_;
	}
	
	virtual void print(ostream &out)
    {
		out << " This print function is yet to be implemented...";
		out << "The value of the InstrStr is: ";
		// TODO: run an interator over the vector.
		// _str.print(out);
    }

	virtual int isEq(InstrStr * ptr)
	{
		return _NO_;
	}
	
	void operator=(const InstrStr &a)
	{ 
		_instrns = a._instrns;
	}
	
	virtual void reset()
	{ 
		_instrns.clear(); 
	}

	vector<MyString> getStr()
	{
		return _instrns;
	}

	void setInstrStr(vector<MyString> b) 
	{ 
		_instrns = b; 
	}
	
	int isValid()
	{ 
		return ((_instrns.empty() == true) ? _NO_ : _YES_); 
	}
};



/*************************************************************
 *class : PCTypeAddrType                                     *
 *description :                                              *
 *  This class contains the PC(program counter).             *
 *Methods :                                                  *   
 *ToDo List :                                                *
 *************************************************************/
class PCTypeAddrType : public AddrOperand {
 private:
   InstrPtr _pc;
 public:
 PCTypeAddrType() : AddrOperand(), _pc(NULL) {}
 ~PCTypeAddrType(){}
 PCTypeAddrType(const PCTypeAddrType& b) : _pc(b._pc)
		{AddrOperand(b._type,b._addr);}
 PCTypeAddrType(const InstrPtr& b): _pc(b),AddrOperand(){}
 
  virtual BaseType * copy()
  {
   BaseType * retPtr;  
   retPtr = new PCTypeAddrType(*this);
   return retPtr;
  }

  friend int operator ==(const PCTypeAddrType &a, const PCTypeAddrType &b){return _NO_;}
  virtual void print(ostream &out)
    {
      out << "The value of the pc is :" << _pc ;
	  AddrOperand::print(out);
    }
  virtual int isEq(PCTypeAddrType * ptr){return _NO_;}
  void operator=(const PCTypeAddrType &a)
  {
	  _pc = a._pc;
	  _type = a._type;
	  _addr = a._addr;
  }

  virtual void reset(){ _pc = NULL; AddrOperand::reset();}
  InstrPtr& getPC(){return _pc;}
  void setPC(InstrPtr & b) { _pc = b; }

  int isValid(){ return ((_pc == NULL) ? _NO_ : _YES_); }
};


extern int getRegFileType(int regNum);
extern void getValRName(int symTabNum, MyString & str);


/*************************************************************
 *class :                                                    *
 *        PredicateAddrDataUnion                             *
 *description :                                              *
 *        the information stored in a latch could be of two  *
 *types "Val" and "Addr". we support only three address code *
 *therefore everylatch will have _src1,_src2,_dest of type   *
 *"Val" or "Addr" or both(determined dynamically by enum).the*
 *"both" part is handled by this  class.                     *
 *                                                           *
 *Methods :                                                  *
 *Data Members :                                             *
 *ToDo List :                                                *                
 *************************************************************/
class PredicateAddrDataUnion : public BaseType{
protected :
 ArgType _type; // See ArchClassIncludes.h for a description of this type.
 struct DataAddr 
 {
  Val data;
  AddrOperand address;
 } 
  _dataAddr;
 bool _negate;
 
 public :
  PredicateAddrDataUnion() : _type(_NOTYPE_), _negate(false){}
  ~PredicateAddrDataUnion(){}
  PredicateAddrDataUnion(const PredicateAddrDataUnion & b) : _type(b._type), _dataAddr(b._dataAddr), _negate(b._negate){}
  
  virtual int is(BaseTypeEnum ctype)
  { return ((ctype == _PREDICATEADDRDATAUNION_) ? _YES_ : (BaseType::is(ctype)));}
  
  virtual BaseType * copy()
  {
   BaseType * retPtr;  
   retPtr = new PredicateAddrDataUnion(*this);
   return retPtr;
  }

  virtual void print(ostream& out)
  {
	  if(_ADDRTYPE_ == _type)
	  {
		  //out << "Addr: "; 
		  (_dataAddr.address).print(out);
	  }
	  if(_VALTYPE_ == _type)
	  {
		  (_dataAddr.data).printTyped(out);
	  }
	  //out << " NEGATE: " << _negate;
  }

  virtual void reset(){_type = _NOTYPE_;}
  
  virtual int isEmpty()
  {
	  if (_type == _NOTYPE_) return _YES_;
	  return _NO_;
  }
  
  PredicateAddrDataUnion& operator=(const PredicateAddrDataUnion &a)
  {
   _type = a._type;
   _negate = a._negate;
   _dataAddr.data = a._dataAddr.data;
   _dataAddr.address = a._dataAddr.address;
   return *this;
  }
  
  PredicateAddrDataUnion & operator = (AddrDataUnion & a)
  {
	  _type = a.getType();
	  _dataAddr.data = a.getData();
	  _dataAddr.address = a.getAddr();

	  return *this;
  }

  PredicateAddrDataUnion and(PredicateAddrDataUnion & a)
  {
	  // Create a predicate and init it to true
	  //
	  PredicateAddrDataUnion retVal;
	  Val b(true);
	  retVal.setDataObject(b);

	  if ((_type != _VALTYPE_) || (a._type != _VALTYPE_))
	  {
		  // error condition.
		  //
		  cout << "Warning: The and operator in PredicateAddrDataUnion recieved invalid arguments" << endl;
		  return retVal;
	  }

	  Val t1, t2;

	  t1 = _dataAddr.data;
	  t2 = a._dataAddr.data;

	  if (_negate) t1 = t1.negate();
	  if (a._negate) t2 = t2.negate();

	  b = t1.and(t2);
	  
	  retVal.setData(b);
	  retVal.setNegate(false);

	  return retVal;
  }

  virtual AddrOperand & getAddr(){ return _dataAddr.address;}
  virtual Val & getData(){ return _dataAddr.data;}
  virtual void setType(const ArgType  a) { _type = a;}
  virtual ArgType  getType(){ return _type;}
  virtual void setAddr(AddrOperand & b){ _dataAddr.address = b; _type = _ADDRTYPE_;}
  virtual void setData(Val & a){ _dataAddr.data = a; _type = _VALTYPE_; }
  virtual long convertAddrMechanism(){ return _dataAddr.address.convertAddrMechanism();} 
  virtual bool getNegate() { return _negate;}
  virtual void setNegate(bool newNegate) { _negate = newNegate;}
  /*  virtual BaseType & getAddrObject(){ // TODO }
  virtual setAddrObject(BaseType & a){ // TODO }
  virtual BaseType & getDataObject(){ // TODO }
  virtual setDataObject(BaseType & a){ // TODO }
  */
};

#endif

