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
 *file  : AddrData.h                                         *
 *authors : vgee,ashok                                       *
 *last modified :                                            *
 *                                                           *
 *contents : contains the BaseType class definitions  and    *
 *           associated classes.                             *
 *compiled with : g++                                        *
 *for more info :                                            *
 *caution : always grep for TODO to check for incompleteness *
 *************************************************************/
#ifndef _ADDRDATA_H_
#define _ADDRDATA_H_

#include <iostream.h>
#include "stdhdr.h"
#include "ArchGlobalIncludes.h"
#include "ArchSTLIncludes.h"
#include "ArchClassIncludes.h"

#ifdef WIN32
	#include <strstrea.h>
#else
	#include <strstream.h>
#endif

const int MAX_NUM_BYTES = 8;
typedef int SignalClass;

/*************************************************************
 *class : BaseType                                           *
 *description :                                              *
 *this is the basetype from which all addr and data classes  *
 *have been derived. this is very generic.                   *
 *Methods :                                                  *   
 *ToDo List :                                                *
 *the Addr related functions have to be made pure virtual.for*
 *those classes which are of type "Data" these funcs will    *
 *return NULL or error.                                      *
 *************************************************************/
class BaseType {
 public:
 BaseType(){}
 ~BaseType(){}
 BaseType(const BaseType&){}
 
 virtual BaseType * copy()
 {
   BaseType * retPtr;  
   retPtr = new BaseType(*this);
   return retPtr;
 }
  virtual int is(BaseTypeEnum ctype)
    { return ((ctype == _BASETYPE_) ? _YES_ : _NO_);}
  friend int operator ==(const BaseType &a, const BaseType &b){return _NO_;}
  virtual void print(ostream &out){}
  virtual int isEq(BaseType * ptr){return _NO_;}
  virtual void operator=(const BaseType &a){}

  virtual void setDataObject(const BaseType &){}
  virtual BaseType & getDataObject();

  virtual void setAddrObject(const BaseType &){}
  virtual BaseType & getAddrObject();
  virtual long convertAddrMechanism(); 

  virtual int isEmpty(){ return _YES_; }
  virtual void reset() { }
};

/*************************************************************
 *class : LatchAddr                                          *
 *description :LatchAddr is used by a CompoundLatch to traver*
 *             -se the list of latches it has.               *  
 *                                                           *
 *Methods :     convertAddrMechanism()                       *
 *Data Members :                                             *
 *ToDo List :                                                *              
 *************************************************************/
class LatchAddr : public BaseType{
  protected :
    unsigned long _index;
  public :
  LatchAddr(){};
  ~LatchAddr(){};
  LatchAddr(const LatchAddr & a){_index = a._index;}

  virtual BaseType * copy()
  {
   BaseType * retPtr;  
   retPtr = new BaseType(*this);
   return retPtr;
  }
  virtual int is(BaseTypeEnum ctype)
    {return ((ctype == _LATCHADDR_) ? _YES_ :(BaseType :: is(ctype)));}
  friend int operator ==(const LatchAddr &a, const LatchAddr &b){return _NO_;}
  virtual int isEq(BaseType * ptr){return _NO_;} //TODO
  virtual void print(ostream& out) 
    {
      out << "LatchAddr :" << _index;
    }
  virtual void reset(){_index = 0;}
  virtual int isEmpty(){ return _NO_; }
  void operator =(const LatchAddr & b){ _index = b._index;}

  virtual void setAddrObject(const BaseType & b){_index = ((LatchAddr&) b)._index;} 
  virtual void setLatchAddr(long a) {_index = a; }
  virtual long getLatchAddr() { return _index; }
  /*  virtual BaseType & getAddrObject(){ return _index;} */
  virtual long convertAddrMechanism(){return _index;}
};    

/*************************************************************
 *class : AddrOperand                                        *
 *description : This class stores the decoded operand. it has*
 *              info on the actual data and its type.Also see*
 *              ArchClassIncludes.h for a description for    *
 *              enum types like AddressMode.                 *
 *                                                           *
 *Methods :     convertAddrMechanism()                       *
 *Data Members :                                             *
 *ToDo List :                                                *              
 *************************************************************/
class AddrOperand : public BaseType{
  protected:
  AddressMode _type;
  long   _addr;
  FieldType _fType;
  public :
  AddrOperand(const AddrOperand & b):_type(b._type),_addr(b._addr), _fType(b._fType){}
  AddrOperand(AddressMode type = _NONE_, long addr = 0, FieldType fType = _ADDR_INT_): _type(type),_addr(addr), _fType(fType){}
  ~AddrOperand(){}

  virtual void print()
  {
	  printf("Addr: %d\n", _addr);
  }

  virtual void print(ostream& out)
    {
      out << _addr << "(T: " << _type << ")";
    }

  virtual void reset(){_type = _NONE_;}
  virtual int isEmpty()
  {
	  if (_type == _NONE_) return _YES_;
	  return _NO_;
  }
  virtual int is(BaseTypeEnum ctype)
    {return ((ctype == _ADDROPERAND_) ? _YES_ :(BaseType :: is(ctype)));}
  friend int operator ==(const AddrOperand &a, const AddrOperand &b)
  {
	  // Note: As of now, we do not check the types to see if they are equal.
	  if ((a._type == b._type) && (a._addr == b._addr))
		  return _YES_;
	  else
		  return _NO_;
  }
  friend bool operator <(const AddrOperand &a, const AddrOperand &b)
  {
	  // Note: As of now, we do not check the types to see if they are equal.
	  if ((a._type == b._type) && (a._addr < b._addr))
		  return true;
	  else
		  return false;
  }
 
  virtual int isEq(BaseType * ptr){return _NO_;} //TODO
  void operator =(const AddrOperand & b)
    {
    _type = b._type; 
    _addr=b._addr; 
	_fType = b._fType;
    }

  void setAddrOperand(AddressMode type, long b, FieldType fType = _ADDR_INT_)
    {
      _type = type;
      _addr=b; 
	  _fType = fType;
    }

  void inc(int increment = 1)
    { 
    _addr=_addr + increment; 
    }

  /*virtual BaseType & getAddrObject(){return _addr;}*/
  virtual long getAddrOperand(){return _addr;}

  virtual void setAddrObject(BaseType & b)
  {
    _type = ((AddrOperand&) b)._type;
    _addr = ((AddrOperand&) b)._addr;
    _fType = ((AddrOperand&) b)._fType;

  }
  virtual void setType(AddressMode type) { _type = type; }
  AddressMode getType() { return _type;}
  FieldType getFieldType() { return _fType;}
  virtual void setFieldType(FieldType fType) { _fType = fType; }

  virtual long convertAddrMechanism()
  {
   if(_type == _REG_) return _addr;
   if (_type == _DIRECTMEMORY_) return _addr;

   // Error condition.
   //
   cout << "Error: ConvertAddrmechanism not defined for callee type: " << _type << endl;
   return (-1);
  }
};

// #pragma interface

#define byte_val _as.byte
#define int_val _as.integer
#define flt_val _as.flt
#define dbl_val _as.dbl
#define vdata _as.dbl

// _INTERNAL_LABEL should never be treated as a normal VALUE_TYPE, so put
// it after NUM_VALUE_TYPES.  Size types must always follow data types,
// and must be grouped together in increasing order of size.
enum VALUE_TYPE {
				//Standard Data Types
				_BOOL,				//Not remember right now
				_CHAR,				//represents a char (used to be _BYTE, but has been changed to _CHAR)
				_UCHAR,				//represents an unsigned char (used to be _UNSINGED_BYTE but has been changed to _UCHAR)
				_SHORT,				//short
				_USHORT,			//unsigned short
				_INT,				//int
				_UINT,				//unsigned int
				_LONG,				//long (not used for now, since on PC long is the same as int)
				_ULONG,				//unsigned long (not used for now, since on PC long is the same as int)
				_FLT,				//float
				_DBL,				//double

				_ANY, //uninitialized data

				// C6x Data Types (to be cleand up later, since there is some overlapping with the standard ones)
 				_HALF_WORD, 
				_ONE_WORD, 
				_TWO_WORDS, 
				_UNSIGNED_HW,			 

				
				// Special types for 
				_NUM_VALUE_TYPES,	//may not be used by nobody:-)
				_INTERNAL_LABEL,	//represents a char string (char*) - used for computations
				_EXTERNAL_VECTOR,	//a hack for bcopy or memcopy (it contains a vector of values)
				_FILE_PTR,			//used in function calls (fopen, fclose)

				//Type to model the CPU to Memory Data Bus (64 bits)
				_EIGHT_BYTES
};

/*************************************************************
 *class : Val                                                *
 *description : This is "type" stored in a "storage" class.  *
 *                                                           *
 *Methods :                                                  *   
 *ToDo List :                                                *
 *************************************************************/
class Val : public BaseType{
public:
   VALUE_TYPE _valType;
   union {
      bool bl;
      char byte;
	  unsigned char word[MAX_NUM_BYTES];
      short short_int;
      long integer;    
      unsigned uinteger;    
      float flt;      
      double dbl;    
      char *label;    // used for builtin functions to the simulator
      } _as;
   int _size; // _size is needed only when the type is _EIGHT_BYTES

   Val();
   Val(char v);
   void changeValue(char v);
   void changeValue(unsigned char v);
   Val(bool v);
   void changeValue(bool v);
   Val(short v);
   void changeValue(short v);
   Val(int v);
   void changeValue(int v);
   Val(unsigned int v);
   void changeValue(unsigned int v);
   Val(long int v);
   void changeValue(long int v);
   void changeValue(unsigned long int v);
   Val(float v);
   void changeValue(float v);
   Val(double v);
   void changeValue(double v);
   Val(char *v);
   void changeValue(char *v);

   Val (unsigned char *word, int size = MAX_NUM_BYTES); // word is expected to be an array of 8 unsigned chars
   void changeValue(unsigned char *word, int size = MAX_NUM_BYTES);
   void getValue(unsigned char *word);

   // Note: SAH, Tue Apr 11, 2000
   //
   // As of now, unsigned short is stored as an unsigned int. 
   // We need to create a new type "_USHORT"
   Val(unsigned short v);
   void changeValue(unsigned short v);

   void changeType(VALUE_TYPE t);
   
   Val(VALUE_TYPE v);

   // Note: SetValue is different from changeValue.
   // ChangeValue assigns the char * as an _INTERNAL_LABEL,
   // SetValue looks at the existing _valType and changes the string into that type.
   //
   void setValue(char *value);
   void zero();

   Val(FILE *fPtr);
   void changeValue(FILE *fPtr);

   FILE *asFPTR();

   Val& operator=(const Val& b) ;
   Val operator+(const Val& b) const;
   Val operator-(const Val& b) const;
   Val operator*(const Val& b) const;
   Val operator/(const Val& b) const;
   Val operator%(const Val& b) const;
   int operator<(const Val& b) const;
   int operator>(const Val& b) const;
   int operator==(const Val& b) const;
   int operator!=(const Val& b) const;
   int operator>=(const Val& b) const;
   int operator<=(const Val& b) const;
   int operator&&(const Val& b) const;
   int operator||(const Val& b) const;
   Val operator<<(const Val&b) const;
   Val operator>>(const Val&b) const;
   Val operator&(const Val&b) const;
   Val operator|(const Val&b) const;
   Val operator^(const Val&b) const;
   Val operator ~() const;
   Val operator -() const;

   Val ash(const Val &b);
   Val rr(const Val &b);
   
   Val negate() const;
   Val squareroot() const;
   Val exponential() const;

   Val and(Val & b);

   // returns _YES_ if the value stored is false, 0 or 0.0
   //
   int isFalse();

   // returns _YES_ if the value stored is int, and is in the ascii range
   // i.e. from 0x00 - 0x7f
   //
   int isAscii();
   int getAscii();

   unsigned char getByte();

   Val& Mask(int);
   Val& cast_to( VALUE_TYPE t );
   Val& unsigned_cast_to( VALUE_TYPE t );
   void print(ostream&out,VALUE_TYPE type=_ANY);
   void compose(Vector<Val> & listOfData);
   void decompose(Vector<Val> & listOfData);
   void getListOfAddr(BaseType & startAddr, Vector<AddrOperand> & listOfAddr);
   void Val::printTyped(ostream& out); //TODO. ask asok about this func.
   virtual int is(BaseTypeEnum ctype)
    {return ((ctype == _VAL_) ? _YES_ :(BaseType :: is(ctype)));}
   void setType(VALUE_TYPE t) {_valType = t;}
   VALUE_TYPE getType(void) {return _valType;}

   int isFractionalType()
   {
	   if ((_valType == _FLT) || (_valType == _DBL)) return _YES_;
	   return _NO_;
   }

   void reset()
   {
	   _valType = _ANY;
	   _as.dbl = 0.0;
   }
	
   virtual int isEmpty()
   {
	   if (_valType == _ANY) return _YES_;
	   return _NO_;
   }

   int getSizeInBytes();
   int isExternalVectorType();

   void writeToString(MyString & s);
   void readFromString(MyString & s);

   void addBytes(Vector<Val> & byteList);
   void getBytes(Vector<Val> & byteList);
};

extern int isIntegerType(Val & a);
extern int getIntegerValue(Val & a);
extern int isFloatType(Val & a);
extern double getFloatValue(Val & a);
extern int isStringType(Val & a);
extern char *getStringValue(Val & a);
extern int isAddrType(Val & a);

// This performs the MIPS multiply
// 32 * 32 = 2 32-bit registers.
//
// The return Val is of _EIGHT_BYTES type. 
// The lower 4 bytes hold the lower 32-bit and the upper 4 bytes hold the upper 32-bit of the value.
//
extern Val mult64(Val & a, Val & b);

// This performs the MIPS divide
// result = 32-bit register, remainder = 32-bit register.
//
// The return Val is of _EIGHT_BYTES type.
// The lower 4 bytes hold the result, the upper 4 bytes hold the remainder.
//
extern Val div64(Val & a, Val & b);


class ValArray : public BaseType
{
public:
	Vector<Val> _valArray;

 ValArray(){}
 ~ValArray(){ _valArray.clear();}
 ValArray(const ValArray& b){ _valArray = b._valArray;}
 
 virtual BaseType * copy()
 {
   BaseType * retPtr;  
   retPtr = new ValArray(*this);
   return retPtr;
 }
  virtual int is(BaseTypeEnum ctype)
    { return ((ctype == _VALARRAY_) ? _YES_ : (BaseType::is(ctype)));}
#ifndef __UNIX__
  friend int operator ==(const BaseType &a, const BaseType &b){return _NO_;}
#endif
  virtual void print(ostream &out){}
  virtual int isEq(BaseType * ptr){return _NO_;}
  virtual void operator=(const ValArray &a){_valArray = a._valArray;}

  virtual void setDataObject(const BaseType &){}
  virtual BaseType & getDataObject();

  virtual void setAddrObject(const BaseType &){}
  virtual BaseType & getAddrObject();
  virtual long convertAddrMechanism(); 

  virtual int isEmpty(){ return _YES_; }
  virtual void reset() { }
};


/*************************************************************
 *class :                                                    *
 *        AddrDataUnion                                      *
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
class AddrDataUnion : public BaseType{
  protected :
  ArgType _type; // See ArchClassIncludes.h for a description of this type.
  struct DataAddr {
    Val data;
    AddrOperand address;
  } _dataAddr;

  public :
  AddrDataUnion();
  ~AddrDataUnion(){}
  AddrDataUnion(const AddrDataUnion & b) : _type(b._type), _dataAddr(b._dataAddr){}

  virtual int is(BaseTypeEnum ctype)
    { return ((ctype == _ADDRDATAUNION_) ? _YES_ : (BaseType::is(ctype)));}

  virtual BaseType * copy()
  {
   BaseType * retPtr;  
   retPtr = new AddrDataUnion(*this);
   return retPtr;
  }
  virtual void print(ostream& out)
    {
      if(_ADDRTYPE_ == _type)
	{
	 (_dataAddr.address).print(out);
	}
      if(_VALTYPE_ == _type)
	{
         (_dataAddr.data).print(out);
	}
    }
  virtual void reset(){_type = _NOTYPE_;}

  virtual int isEmpty()
  {
	  if (_type == _NOTYPE_) return _YES_;
	  return _NO_;
  }

  AddrDataUnion& operator=(const AddrDataUnion &a)
  {
    _type = a._type;
    _dataAddr.data = a._dataAddr.data;
    _dataAddr.address = a._dataAddr.address;
    return *this;
  }
  
  virtual AddrOperand & getAddr(){ return _dataAddr.address;}
  virtual Val & getData(){ return _dataAddr.data;}
  virtual void setType(const ArgType  a) { _type = a;}
  virtual ArgType  getType(){ return _type;}
  virtual void setAddr(AddrOperand & b){ _dataAddr.address = b; _type = _ADDRTYPE_;}
  virtual void setData(Val & a){ _dataAddr.data = a; _type = _VALTYPE_; }
  virtual long convertAddrMechanism(){ return _dataAddr.address.convertAddrMechanism();} 
  /*  virtual BaseType & getAddrObject(){ // TODO }
      virtual setAddrObject(BaseType & a){ // TODO }
      virtual BaseType & getDataObject(){ // TODO }
      virtual setDataObject(BaseType & a){ // TODO }
  */
};

#endif


