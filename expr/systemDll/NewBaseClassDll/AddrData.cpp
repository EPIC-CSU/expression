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
 *file  : AddrData.cpp                                       *
 *authors : ashok,vgee, sudeep                                       *
 *last modified :                                            *
 *                                                           *
 *contents:definitions of the methods of AddrData classes    *
 *comments:-                                                 *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

#ifdef WIN32
#pragma warning(disable:4786)
#endif


#include "stdafx.h"
#include "AddrData.h"
#include <math.h>
#include "DataConversion.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

BaseType & BaseType::getDataObject()
{
	// Error: this function should never be executed
	cout << "Error: Call to base class function" << endl;
	return (*this);
}

BaseType & BaseType::getAddrObject()
{
	// Error: this function should never be executed
	cout << "Error: Call to base class function" << endl;
	return (*this);
}

long BaseType::convertAddrMechanism()
{
	// Error: this function should never be executed
	cout << "Error: Call to base class function" << endl;
	return (-1);
}

AddrDataUnion::AddrDataUnion()
{
	Val data;
	AddrOperand address;

	_type = _NOTYPE_;
	
	_dataAddr.data = data;
	_dataAddr.address = address;
}


Val::Val()
{
  _as.dbl = 0.0;
  _valType = _ANY; 
  _size = _INVALID_;
}

void Val::zero()
{
  _as.dbl = 0.0;
  _valType = _ANY;
  _size = _INVALID_;
}

Val::Val(char v) 
{  
  _as.dbl = 0.0; 
  _valType = _CHAR; 
  _as.byte = v; 
  _size = _INVALID_;
}

void Val::changeValue(char v) 
{  
  _as.dbl = 0.0; 
  _valType = _CHAR; 
  _as.byte = v; 
  _size = _INVALID_;
}

void Val::changeValue(unsigned char v) 
{  
  _as.dbl = 0.0; 
  _valType = _UCHAR; 
  _as.byte = v; 
  _size = _INVALID_;
}

Val::Val(bool v) 
{  
  _as.dbl = 0.0;
  _valType = _BOOL;
  _as.bl = v; 
  _size = _INVALID_;
}

void Val::changeValue(bool v) 
{  
  _as.dbl = 0.0;
  _valType =_BOOL;
  _as.bl = v; 
  _size = _INVALID_;
}

Val::Val(short v) 
{  
  _as.dbl = 0.0;
  _valType = _SHORT;
  _as.short_int = v; 
  _size = _INVALID_;
}

void Val::changeValue(short v) 
{  
  _as.dbl = 0.0;
  _valType = _SHORT;
  _as.short_int = v; 
  _size = _INVALID_;
}

Val::Val(int v) 
{
  _as.dbl = 0.0;
  _valType = _INT;
  _as.integer = v; 
  _size = _INVALID_;
}

void Val::changeValue(int v) 
{
  _as.dbl = 0.0;
  _valType = _INT;
  _as.integer = v; 
  _size = _INVALID_;
}

Val::Val(unsigned int v) 
{  
  _as.dbl = 0.0;
  _valType = _UINT;
   _as.uinteger = v; 
  _size = _INVALID_;
}

void Val::changeValue(unsigned int v) 
{  
  _as.dbl = 0.0;
  _valType = _UINT;
   _as.uinteger = v; 
  _size = _INVALID_;
}

Val::Val(long int v) 
{
  _as.dbl = 0.0;
  _valType = _INT;
  _as.integer = v;
  _size = _INVALID_;
}

void Val::changeValue(long int v) 
{
  _as.dbl = 0.0;
  _valType = _INT;
  _as.integer = v;
  _size = _INVALID_;
}

void Val::changeValue(unsigned long int v) 
{
  _as.dbl = 0.0;
  _valType = _UINT;
  _as.uinteger = v;
  _size = _INVALID_;
}

Val::Val(float v) 
{
  _as.dbl = 0.0;
  _valType = _FLT;
  _as.flt = v; 
  _size = _INVALID_;
}

void Val::changeValue(float v) 
{
  _as.dbl = 0.0;
  _valType = _FLT;
  _as.flt = v; 
  _size = _INVALID_;
}

Val::Val(double v) 
{
  _as.dbl = 0.0;
  _valType = _DBL;
  _as.dbl = v; 
  _size = _INVALID_;
}

void Val::changeValue(double v) 
{
  _as.dbl = 0.0;
  _valType = _DBL;
  _as.dbl = v; 
  _size = _INVALID_;
}

Val::Val(char *v) 
{
  _as.dbl = 0.0;
  _valType = _INTERNAL_LABEL;
  _as.label = v; 
  _size = _INVALID_;
}

void Val::changeValue(char *v)
{
	_as.dbl = 0.0;
	_valType = _INTERNAL_LABEL;
	_as.label = v;
  _size = _INVALID_;
}

Val::Val(unsigned char *word, int size) 
{
	_as.dbl = 0.0;
	_valType = _EIGHT_BYTES;
	_size = size;
	
	if (word == NULL) return;
	
	for (int i = 0; i < size; i++)
	{
		_as.word[i] = word[i];
	}
}

void Val::changeValue(unsigned char *word, int size)
{
	_as.dbl = 0.0;
	_valType = _EIGHT_BYTES;
	_size = size;
	
	if (word == NULL) return;
	
	for (int i = 0; i < size; i++)
	{
		_as.word[i] = word[i];
	}
}

void Val::getValue(unsigned char *word)
{
	if (_valType != _EIGHT_BYTES)
	{
		// error condition
		//
		cout << "Error: Tried to get a non EIGHT_BYTES val as eight bytes";
		assert(0);
	}

	if (word == NULL) return;
	if ((_size < 0) || (_size > MAX_NUM_BYTES))
	{
		// error condition.
		//
		cout << "Panic: getValue found wrong size paramenter for word in val" << endl;
		assert(0);
	}

	for (int i = 0; i < _size; i++)
	{
		word[i] = _as.word[i];
	}
}

Val::Val(FILE *fPtr)
{
	_as.uinteger = (unsigned)fPtr;
	_valType = _FILE_PTR;
  _size = _INVALID_;
}

void Val::changeValue(FILE *fPtr)
{
	_as.uinteger = (unsigned)fPtr;
	_valType = _FILE_PTR;
  _size = _INVALID_;
}

Val::Val(unsigned short v)
{
	_as.uinteger = (unsigned)v;
	_valType = _UINT;
  _size = _INVALID_;
}

void Val::changeValue(unsigned short v)
{
	_as.uinteger = (unsigned)v;
	_valType = _UINT;
  _size = _INVALID_;
}

Val::Val(VALUE_TYPE v)
{
  _as.dbl = 0.0;
  _valType = v;
  _size = _INVALID_;
}

FILE *Val::asFPTR()
{
	if ((_valType == _FILE_PTR) || (_valType == _UINT))
		return ((FILE *)(_as.uinteger));

	// Note: Normally, we'd expect the valType to be either a FILE_PTR or UINT.
	// However, loads from memory typically are IVLOADs that load integers.
	// So, the FILE_PTR might be disguised as an int.
	//
	if (_valType == _INT)
		return ((FILE *)(_as.integer));

	return NULL;
}

Val& Val::Mask(int mask)
{
 assert(0); //NOS please rewrite this function, it causes memory leaks
#ifdef __UNIX__
   switch(_valType) { 
    case _CHAR: 
      return *(new Val(mask & _as.byte)); 
    case _SHORT: 
      return *(new Val(mask & _as.short_int)); 
	case _USHORT: 
      return *(new Val(mask & _as.short_int)); 
    case _INT: 
      return *(new Val(mask & _as.integer)); 
    case _UINT: 
      return *(new Val(mask & _as.uinteger)); 
    case _FLT: 
      //return Val(mask & _as.flt);
	break;
    case _DBL: 
      //return Val(mask & _as.dbl);
	break;
    default: 
      panic("bad Value type\n"); 
  }
  panic("bad Value type\n"); 
  return *(new Val(0));
#else
  switch(_valType) { 
    case _CHAR: 
      return *(new Val(mask & _as.byte)); 
    case _SHORT: 
      return *(new Val(mask & _as.short_int)); 
	case _USHORT: 
      return *(new Val(mask & _as.short_int)); 
    case _INT: 
      return *(new Val(mask & _as.integer)); 
    case _UINT: 
      return *(new Val(mask & _as.uinteger)); 
    case _FLT: 
      //return Val(mask & _as.flt);
		;
    case _DBL: 
      //return Val(mask & _as.dbl);
		;
    default: 
      panic("bad Value type\n"); 
  } 
  panic("bad Value type\n"); 
  return *(new Val(0));
#endif
}

Val& Val::cast_to(VALUE_TYPE t)
{
  switch(t) {
    case _BOOL:
      if (_valType == _CHAR)
        _as.bl = (bool) _as.byte;
      else if (_valType == _SHORT)
        _as.bl = (bool) _as.short_int;
	  else if (_valType == _USHORT)
        _as.bl = (bool) _as.short_int;
	  else if (_valType == _UINT)
		  _as.bl = (bool) _as.uinteger;
	  else if (_valType == _INT)
		  _as.bl = (bool) _as.integer;
        _valType = _BOOL;
        break;
    case _INT:
      if (_valType == _FLT)
        _as.integer = (int) _as.flt;
      else if (_valType == _DBL)
        _as.integer = (int) _as.dbl;
      else if (_valType == _CHAR)
        _as.integer = (int) _as.byte;
      else if (_valType == _SHORT)
        _as.integer = (int) _as.short_int;
	  else if (_valType == _USHORT)
        _as.integer = (int) _as.short_int;
	  else if (_valType == _UINT)
		  _as.integer = (int) _as.uinteger;
	  else if (_valType == _FILE_PTR)
		  _as.integer = (int) _as.uinteger;
      else if (_valType != _INT)
        panic("bad cast type for Val");
        _valType = _INT;
        break;
    case _UINT:
      if (_valType == _FLT)
        _as.uinteger = (unsigned int) _as.flt;
      else if (_valType == _DBL)
        _as.uinteger = (unsigned int) _as.dbl;
      else if (_valType == _CHAR)
        _as.uinteger = (unsigned int) _as.byte;
      else if (_valType == _SHORT)
        _as.uinteger = (unsigned int) _as.short_int;
	  else if (_valType == _USHORT)
        _as.uinteger = (unsigned int) _as.short_int;
      else if (_valType == _INT)
        _as.uinteger = (unsigned int) _as.integer;
      else if ((_valType != _UINT) && (_valType != _FILE_PTR))
        panic("bad cast type for Val");
        _valType = _UINT;
        break;
    case _SHORT:
      if (_valType == _FLT)
        _as.short_int = (short) _as.flt;
      else if (_valType == _DBL)
        _as.short_int = (short) _as.dbl;
      else if (_valType == _CHAR)
        _as.short_int = (short) _as.byte;
      else if (_valType == _INT)
        _as.short_int = (short) _as.integer;
      else if (_valType == _UINT)
        _as.short_int = (short) _as.uinteger;
      else if (_valType != _SHORT)
        panic("bad cast type for Val");
        _valType = _SHORT;
        break;
    case _USHORT:
      if (_valType == _FLT)
        _as.short_int = (unsigned short) _as.flt;
      else if (_valType == _DBL)
        _as.short_int = (unsigned short) _as.dbl;
      else if (_valType == _CHAR)
        _as.short_int = (unsigned short) _as.byte;
      else if (_valType == _SHORT)
        _as.short_int = (unsigned short) _as.short_int;
      else if (_valType == _INT)
        _as.short_int = (unsigned short) _as.integer;
      else if (_valType == _UINT)
        _as.short_int = (unsigned short) _as.uinteger;
      else if (_valType != _USHORT)
        panic("bad cast type for Val");
        _valType = _USHORT;
        break;
    case _FLT:
      if (_valType == _INT)
        _as.flt = (float) _as.integer;
      else if (_valType == _UINT)
        _as.flt = (float) _as.uinteger;
	  else if (_valType == _SHORT)
        _as.flt = (float) _as.short_int;
	  else if (_valType == _USHORT)
        _as.flt = (float) _as.short_int;
      else if (_valType == _DBL)
        _as.flt = (float) _as.dbl;
      else if (_valType != _FLT)
        panic("bad cast type for Val");
        _valType = _FLT;
        break;
    case _DBL:
      if (_valType == _INT)
        _as.dbl = (double) _as.integer;
      else if (_valType == _UINT)
        _as.dbl = (double) _as.uinteger;
	  else if (_valType == _SHORT)
        _as.dbl = (double) _as.short_int;
	  else if (_valType == _USHORT)
        _as.dbl = (double) _as.short_int;
      else if (_valType == _FLT)
        _as.dbl = (double) _as.flt;
      else if (_valType != _DBL)
        panic("bad cast type for Val");
        _valType = _DBL;
        break;
    case _CHAR:
      if (_valType == _INT)
        _as.byte = (char) _as.integer;
      else if (_valType == _UINT)
        _as.byte = (char) _as.uinteger;
      else if (_valType == _SHORT)
        _as.byte = (char) _as.short_int;
      else if (_valType != _CHAR)
        panic("bad cast type for Val");
        _valType = _CHAR;
        break;
    default:
        panic("bad cast type for Val");
  }
  return *this;
}

Val& Val::unsigned_cast_to( VALUE_TYPE t )
{
	_ASSERT(0);
	//anybody calling this function? 
  switch(t) {
    case _INT:
      if (_valType == _CHAR)
        _as.integer = (unsigned char) _as.byte;
      else if (_valType == _SHORT)
	_as.integer = (unsigned short) _as.short_int;
      else if (_valType != _INT)
        panic("bad cast type for Val");
	_valType = _INT;
	break;
    case _UINT:
      if (_valType == _CHAR)
        _as.uinteger = (unsigned char) _as.byte;
      else if (_valType == _SHORT)
        _as.uinteger = (unsigned short) _as.short_int;
      else if (_valType == _INT)
        _as.uinteger = (unsigned int) _as.integer;
      else if (_valType != _INT)
        panic("bad cast type for Val");
	_valType = _UINT;
	break;
    case _SHORT:
	case _USHORT:
      if (_valType == _CHAR)
        _as.short_int = (unsigned char) _as.byte;
      else if (_valType == _INT)
        _as.short_int = (unsigned int) _as.integer;
      else if (_valType == _UINT)
        _as.short_int = (unsigned int) _as.uinteger;
      else if (_valType != _SHORT)
        panic("bad cast type for Val");
	_valType = _SHORT;
	break;
    case _CHAR:
      if (_valType == _INT)
        _as.byte = (unsigned int) _as.integer;
      else if (_valType == _UINT)
        _as.byte = (unsigned int) _as.uinteger;
      else if (_valType == _SHORT)
        _as.byte = (unsigned short) _as.short_int;
      else if (_valType != _CHAR)
        panic("bad cast type for Val");
	_valType = _CHAR;
	break;
    default:
      panic("bad cast type for Val");
  }
  return *this;
}

Val& Val::operator=(const Val& b) 
{
  _valType = b._valType;
	  
  _as.bl = b._as.bl;
  _as.byte = b._as.byte;
  _as.short_int = b._as.short_int;
  _as.integer = b._as.integer;
  _as.uinteger = b._as.uinteger;
  _as.flt = b._as.flt;
  _as.dbl = b._as.dbl;
  _as.label = b._as.label;
  
  return *this;
}

/********************************************************/

Val Val::operator +(const Val& b) const 
{ 
    if (_valType == b._valType) 
  {
	switch(_valType)
	{
	case _CHAR: 
		return Val(_as.byte + b._as.byte); 
	case _SHORT: 
	case _USHORT: 
      return Val(_as.short_int + b._as.short_int); 
    case _INT: 
      return Val(_as.integer + b._as.integer); 
    case _UINT: 
      return Val(_as.uinteger + b._as.uinteger); 
    case _FLT: 
      return Val(_as.flt + b._as.flt); 
    case _DBL: 
      return Val(_as.dbl + b._as.dbl); 
    default: 
      panic("bad Value type\n"); 
	}
  }
	else if ((_valType == _INT) && (b._valType == _FILE_PTR))
	{
		return Val(_as.integer + b._as.uinteger);
	}
	else if ((_valType == _FILE_PTR) && (b._valType == _INT))
	{
		return Val(_as.uinteger + b._as.integer);
	}
	else if ((_valType == _INT) && (b._valType == _UINT))
	{
		return Val(_as.integer + b._as.uinteger);
	}
	else if ((_valType == _UINT) && (b._valType == _INT))
	{
		return Val(_as.uinteger + b._as.integer);
	}
  else if ((_valType == _INT) && (b._valType == _DBL))
  {
	  return Val(_as.integer + b._as.dbl);
  }
  else if ((_valType == _DBL) && (b._valType == _INT))
  {
	  return Val(b._as.integer + _as.dbl);
  }
  else if ((_valType == _FLT) && (b._valType == _INT))
  {
	  return Val(b._as.integer + _as.flt);
  }
  else if ((_valType == _INT) && (b._valType == _FLT))
  {
	  return Val(_as.integer + b._as.flt);
  }
  else if ((_valType == _INT) && (b._valType == _SHORT))
  {
	  return Val(_as.integer + b._as.short_int);
  }
  else if ((_valType == _SHORT) && (b._valType == _INT))
  {
	  return Val(_as.short_int + b._as.integer);
  }
  else
  {
	  cout << "Warning: operator + does not yet support this combination of operands" << endl;
  }
  _ASSERT(0);
  return (Val(-1));
}

Val Val::operator -(const Val& b) const 
{ 
	  if (_valType == b._valType) 
  {
	switch(_valType)
	{
	case _CHAR: 
		return Val(_as.byte - b._as.byte); 
	case _SHORT: 
	case _USHORT: 
      return Val(_as.short_int - b._as.short_int); 
    case _INT: 
      return Val(_as.integer - b._as.integer); 
    case _UINT: 
      return Val(_as.uinteger - b._as.uinteger); 
    case _FLT: 
      return Val(_as.flt - b._as.flt); 
    case _DBL: 
      return Val(_as.dbl - b._as.dbl); 
    default: 
      panic("bad Value type\n"); 
	}
  }
	else if ((_valType == _INT) && (b._valType == _FILE_PTR))
	{
		return Val(_as.integer - b._as.uinteger);
	}
	else if ((_valType == _FILE_PTR) && (b._valType == _INT))
	{
		return Val(_as.uinteger - b._as.integer);
	}
	else if ((_valType == _INT) && (b._valType == _UINT))
	{
		return Val(_as.integer - b._as.uinteger);
	}
	else if ((_valType == _INT) && (b._valType == _SHORT))
	{
		return Val(_as.integer - b._as.short_int);
	}
	else if ((_valType == _UINT) && (b._valType == _INT))
	{
		return Val(_as.uinteger - b._as.integer);
	}
  else if ((_valType == _INT) && (b._valType == _DBL))
  {
	  return Val(_as.integer - b._as.dbl);
  }
  else if ((_valType == _DBL) && (b._valType == _INT))
  {
	  return Val(_as.dbl - b._as.integer);
  }
  else if ((_valType == _FLT) && (b._valType == _INT))
  {
	  return Val(_as.flt - b._as.integer);
  }
  else if ((_valType == _INT) && (b._valType == _FLT))
  {
	  return Val(_as.integer - b._as.flt);
  }
  else if ((_valType == _INT) && (b._valType == _SHORT))
  {
	  return Val(_as.integer - b._as.short_int);
  }
  else if ((_valType == _SHORT) && (b._valType == _INT))
  {
	  return Val(_as.short_int - b._as.integer);
  }
  else
  {
	  cout << "Warning: operator - does not yet support this combination of operands" << endl;
	  _ASSERT(0);
  }
  _ASSERT(0);
  return (Val(-1));
}

Val Val::operator *(const Val& b) const 
{ 
  if (_valType == b._valType) 
  {
	switch(_valType)
	{
	case _CHAR: 
		return Val(_as.byte * b._as.byte); 
	case _SHORT: 
	case _USHORT: 
      return Val(_as.short_int * b._as.short_int); 
    case _INT: 
      return Val(_as.integer * b._as.integer); 
    case _UINT: 
      return Val(_as.uinteger * b._as.uinteger); 
    case _FLT: 
      return Val(_as.flt * b._as.flt); 
    case _DBL: 
      return Val(_as.dbl * b._as.dbl); 
    default: 
      panic("bad Value type\n"); 
	}
  }
	else if ((_valType == _INT) && (b._valType == _UINT))
	{
		return Val(_as.integer * b._as.uinteger);
	}
	else if ((_valType == _UINT) && (b._valType == _INT))
	{
		return Val(_as.uinteger * b._as.integer);
	}
  else if ((_valType == _INT) && (b._valType == _DBL))
  {
	  return Val(_as.integer * b._as.dbl);
  }
  else if ((_valType == _DBL) && (b._valType == _INT))
  {
	  return Val(b._as.integer * _as.dbl);
  }
  else if ((_valType == _DBL) && (b._valType == _FLT))
  {
	  return Val(b._as.flt * _as.dbl);
  }
  else if ((_valType == _FLT) && (b._valType == _DBL))
  {
	  return Val(b._as.dbl * _as.flt);
  }
  else if ((_valType == _FLT) && (b._valType == _INT))
  {
	  return Val(b._as.integer * _as.flt);
  }
  else if ((_valType == _INT) && (b._valType == _FLT))
  {
	  return Val(_as.integer * b._as.flt);
  }
  else if ((_valType == _INT) && (b._valType == _SHORT))
  {
	  return Val(_as.integer * b._as.short_int);
  }
  else if ((_valType == _SHORT) && (b._valType == _INT))
  {
	  return Val(_as.short_int * b._as.integer);
  }
  else
  {
	  cout << "Warning: operator * does not yet support this combination of operands" << endl;
	  _ASSERT(0);
  }
  _ASSERT(0);
  return (Val(-1));
}

Val Val::operator /(const Val& b) const 
{ 
  // What do we do if there is a divide by zero exception?
  // As of now, we check the second arg to see if it is 0.

  if (b._valType==_INT && (b._as.integer == 0) ||
	  b._valType==_FLT && (b._as.flt == 0.0) || 
	  b._valType==_DBL && (b._as.dbl == 0.0))
  {
	  cout << "Warning: DIV recieved Zero divisor" << endl;

//	  _ASSERT(0);
	  return (Val(-1));
  }
  if (_valType == b._valType) 
  {
	  switch(_valType)
	  { 
	  case _CHAR: 
		  return Val(_as.byte / b._as.byte); 
	  case _SHORT: 
	  case _USHORT: 
		  return Val(_as.short_int / b._as.short_int); 
	  case _INT: 
		  return Val(_as.integer / b._as.integer); 
	  case _UINT: 
		  return Val(_as.uinteger / b._as.uinteger); 
	  case _FLT: 
		  return Val(_as.flt / b._as.flt); 
	  case _DBL: 
		  return Val(_as.dbl / b._as.dbl); 
	  default: 
		  panic("bad Value type\n"); 
	  } 
  }
	else if ((_valType == _INT) && (b._valType == _UINT))
	{
		return Val(_as.integer / b._as.uinteger);
	}
	else if ((_valType == _UINT) && (b._valType == _INT))
	{
		return Val(_as.uinteger / b._as.integer);
	}
  else if ((_valType == _INT) && (b._valType == _DBL))
  {
	  return Val(_as.integer / b._as.dbl);
  }
  else if ((_valType == _DBL) && (b._valType == _INT))
  {
	  return Val(b._as.integer / _as.dbl);
  }
  else if ((_valType == _DBL) && (b._valType == _FLT))
  {
	  return Val(b._as.flt / _as.dbl);
  }
  else if ((_valType == _FLT) && (b._valType == _DBL))
  {
	  return Val(b._as.dbl / _as.flt);
  }
  else if ((_valType == _FLT) && (b._valType == _INT))
  {
	  return Val(b._as.integer / _as.flt);
  }
  else if ((_valType == _INT) && (b._valType == _FLT))
  {
	  return Val(_as.integer / b._as.flt);
  }
  else if ((_valType == _INT) && (b._valType == _SHORT))
  {
	  return Val(_as.integer / b._as.short_int);
  }
  else if ((_valType == _SHORT) && (b._valType == _INT))
  {
	  return Val(_as.short_int / b._as.integer);
  }
  else
  {
	  cout << "Warning: operator / does not yet support this combination of operands" << endl;
	  _ASSERT(0);
  }
  _ASSERT(0);
  return (Val(-1));
}

Val Val::operator %(const Val& b) const 
{ 
  if (_valType != b._valType) 
     panic("inValid Value types in Val arithmetic"); 

  // check to see if b is 0 (zero)
  //
  // if b is zero, then the % operator is not defined.
  // return the original value instead. (i.e. return this->value)

  switch(_valType) { 
    case _CHAR: 
      return Val(_as.byte % b._as.byte); 
    case _SHORT: 
	case _USHORT: 
		if (b._as.short_int == 0) return Val(_as.short_int);
      return Val(_as.short_int % b._as.short_int); 
    case _INT: 
		if (b._as.integer == 0) return Val(_as.integer);
      return Val(_as.integer % b._as.integer); 
    case _UINT: 
		if (b._as.uinteger == 0) return Val(_as.uinteger);
      return Val(_as.uinteger % b._as.uinteger); 
    default: 
      panic("bad Value type\n"); 
  } 
  _ASSERT(0);
  return (Val(-1));
}

/************************************************/

int Val::operator <(const Val& b) const 
{ 
	// First, consider _FILE_PTR to be equivalent to _INT
	//
	if (((_valType == _FILE_PTR) && (b._valType == _INT)) 
		|| ((b._valType == _FILE_PTR) && (_valType == _INT)))
	{
		return (_as.integer < b._as.integer);
	}

	// Sometimes, uint may be compared against int
	//
	if ((_valType == _INT) && (b._valType == _UINT))
	{
		return ((int)_as.integer < (int)b._as.uinteger);
	}
	else if ((_valType == _UINT) && (b._valType == _INT))
	{
		return ((int)_as.uinteger < (int)b._as.integer);
	}

	// Sometimes, byte values (i.e. ascii eqvivalent of characters)
	// may be compared against integer values.
	//
	if ((_valType == _CHAR) && (b._valType == _INT))
	{
		return (_as.byte < b._as.integer);
	}
	else if ((_valType == _INT) && (b._valType == _CHAR))
	{
		return (_as.integer < b._as.byte);
	}

	// Sometimes, there may be integer and float comparisions.
	//
	if ((_valType == _INT) && (b._valType == _FLT))
	{
		return (_as.integer < b._as.flt);
	}
	else if ((_valType == _FLT) && (b._valType == _INT))
	{
		return (_as.flt < b._as.integer);
	}
	else if ((_valType == _INT) && (b._valType == _DBL))
	{
		return (_as.integer < b._as.dbl);
	}
	else if ((_valType == _DBL) && (b._valType == _INT))
	{
		return (_as.dbl < b._as.integer);
	}

	// Sometimes, there may be float vs double comparisions.
	//
	if ((_valType == _DBL) && (b._valType == _FLT))
	{
		return (_as.dbl < b._as.flt);
	}
	else if ((_valType == _FLT) && (b._valType == _DBL))
	{
		return (_as.flt < b._as.dbl);
	}

//	if (_valType != b._valType) 
//    panic("inValid Value types in Val arithmetic"); 

	switch(_valType) { 
    case _CHAR: 
      return (_as.byte < b._as.byte); 
    case _SHORT: 
	case _USHORT: 
      return (_as.short_int < b._as.short_int); 
    case _INT: 
      return (_as.integer < b._as.integer); 
    case _UINT: 
      return (_as.uinteger < b._as.uinteger); 
    case _FLT: 
      return (_as.flt < b._as.flt); 
    case _DBL: 
      return (_as.dbl < b._as.dbl); 
    default: 
      panic("bad Value type\n"); 
  } 
  _ASSERT(0);
  return (-1);
}

int Val::operator >(const Val& b) const 
{ 
	// First, consider _FILE_PTR to be equivalent to _INT
	//
	if (((_valType == _FILE_PTR) && (b._valType == _INT)) 
		|| ((b._valType == _FILE_PTR) && (_valType == _INT)))
	{
		return (_as.integer > b._as.integer);
	}

	// Sometimes, uint may be compared against int
	//
	if ((_valType == _INT) && (b._valType == _UINT))
	{
		return ((int)_as.integer > (int)b._as.uinteger);
	}
	else if ((_valType == _UINT) && (b._valType == _INT))
	{
		return ((int)_as.uinteger > (int)b._as.integer);
	}

	// Sometimes, byte values (i.e. ascii eqvivalent of characters)
	// may be compared against integer values.
	//
	if ((_valType == _CHAR) && (b._valType == _INT))
	{
		return (_as.byte > b._as.integer);
	}
	else if ((_valType == _INT) && (b._valType == _CHAR))
	{
		return (_as.integer > b._as.byte);
	}

	// Sometimes, there may be integer and float comparisions.
	//
	if ((_valType == _INT) && (b._valType == _FLT))
	{
		return (_as.integer > b._as.flt);
	}
	else if ((_valType == _FLT) && (b._valType == _INT))
	{
		return (_as.flt > b._as.integer);
	}
	else if ((_valType == _INT) && (b._valType == _DBL))
	{
		return (_as.integer > b._as.dbl);
	}
	else if ((_valType == _DBL) && (b._valType == _INT))
	{
		return (_as.dbl > b._as.integer);
	}

	// Sometimes, there may be float vs double comparisions.
	//
	if ((_valType == _DBL) && (b._valType == _FLT))
	{
		return (_as.dbl > b._as.flt);
	}
	else if ((_valType == _FLT) && (b._valType == _DBL))
	{
		return (_as.flt > b._as.dbl);
	}

//	if (_valType != b._valType) 
//     panic("inValid Value types in Val arithmetic"); 

  switch(_valType) { 
    case _CHAR: 
      return (_as.byte > b._as.byte); 
    case _SHORT: 
	case _USHORT: 
      return (_as.short_int > b._as.short_int); 
    case _INT: 
      return (_as.integer > b._as.integer); 
    case _UINT: 
      return (_as.uinteger > b._as.uinteger); 
    case _FLT: 
      return (_as.flt > b._as.flt); 
    case _DBL: 
      return (_as.dbl > b._as.dbl); 
    default: 
      panic("bad Value type\n"); 
  } 
  _ASSERT(0);
  return (-1);
}

int Val::operator ==(const Val& b) const 
{ 
	// First, consider _FILE_PTR to be equivalent to _INT
	//
	if (((_valType == _FILE_PTR) && (b._valType == _INT)) 
		|| ((b._valType == _FILE_PTR) && (_valType == _INT)))
	{
		return (_as.integer == b._as.integer);
	}

	// Sometimes, uint may be compared against int
	//
	if ((_valType == _INT) && (b._valType == _UINT))
	{
		return ((int)_as.integer == (int)b._as.uinteger);
	}
	else if ((_valType == _UINT) && (b._valType == _INT))
	{
		return ((int)_as.uinteger == (int)b._as.integer);
	}

	// Sometimes, byte values (i.e. ascii eqvivalent of characters)
	// may be compared against integer values.
	//
	if ((_valType == _CHAR) && (b._valType == _INT))
	{
		return (_as.byte == b._as.integer);
	}
	else if ((_valType == _INT) && (b._valType == _CHAR))
	{
		return (_as.integer == b._as.byte);
	}

	// Sometimes, there may be integer and float comparisions.
	//
	if ((_valType == _INT) && (b._valType == _FLT))
	{
		return (_as.integer == b._as.flt);
	}
	else if ((_valType == _FLT) && (b._valType == _INT))
	{
		return (_as.flt == b._as.integer);
	}
	else if ((_valType == _INT) && (b._valType == _DBL))
	{
		return (_as.integer == b._as.dbl);
	}
	else if ((_valType == _DBL) && (b._valType == _INT))
	{
		return (_as.dbl == b._as.integer);
	}

	// Sometimes, there may be float vs double comparisions.
	//
	if ((_valType == _DBL) && (b._valType == _FLT))
	{
		return (_as.dbl == b._as.flt);
	}
	else if ((_valType == _FLT) && (b._valType == _DBL))
	{
		return (_as.flt == b._as.dbl);
	}

//	if (_valType != b._valType) 
//     panic("inValid Value types in Val arithmetic"); 

	switch(_valType) { 
    case _CHAR: 
      return (_as.byte == b._as.byte); 
    case _SHORT: 
	case _USHORT: 
      return (_as.short_int == b._as.short_int); 
    case _INT: 
      return (_as.integer == b._as.integer); 
    case _UINT: 
      return (_as.uinteger == b._as.uinteger); 
    case _FLT: 
      return (_as.flt == b._as.flt); 
    case _DBL: 
      return (_as.dbl == b._as.dbl); 
    default: 
      panic("bad Value type\n"); 
  } 
  _ASSERT(0);
  return (-1);
}

int Val::operator !=(const Val& b) const 
{ 
	// First, consider _FILE_PTR to be equivalent to _INT
	//
	if (((_valType == _FILE_PTR) && (b._valType == _INT)) 
		|| ((b._valType == _FILE_PTR) && (_valType == _INT)))
	{
		return (_as.integer != b._as.integer);
	}

	// Sometimes, uint may be compared against int
	//
	if ((_valType == _INT) && (b._valType == _UINT))
	{
		return ((int)_as.integer != (int)b._as.uinteger);
	}
	else if ((_valType == _UINT) && (b._valType == _INT))
	{
		return ((int)_as.uinteger != (int)b._as.integer);
	}

	// Sometimes, byte values (i.e. ascii eqvivalent of characters)
	// may be compared against integer values.
	//
	if ((_valType == _CHAR) && (b._valType == _INT))
	{
		return (_as.byte != b._as.integer);
	}
	else if ((_valType == _INT) && (b._valType == _CHAR))
	{
		return (_as.integer != b._as.byte);
	}

	// Sometimes, there may be integer and float comparisions.
	//
	if ((_valType == _INT) && (b._valType == _FLT))
	{
		return (_as.integer != b._as.flt);
	}
	else if ((_valType == _FLT) && (b._valType == _INT))
	{
		return (_as.flt != b._as.integer);
	}
	else if ((_valType == _INT) && (b._valType == _DBL))
	{
		return (_as.integer != b._as.dbl);
	}
	else if ((_valType == _DBL) && (b._valType == _INT))
	{
		return (_as.dbl != b._as.integer);
	}

	// Sometimes, there may be float vs double comparisions.
	//
	if ((_valType == _DBL) && (b._valType == _FLT))
	{
		return (_as.dbl != b._as.flt);
	}
	else if ((_valType == _FLT) && (b._valType == _DBL))
	{
		return (_as.flt != b._as.dbl);
	}

//	if (_valType != b._valType) 
//     panic("inValid Value types in Val arithmetic"); 

	switch(_valType) { 
    case _CHAR: 
      return (_as.byte != b._as.byte); 
    case _SHORT: 
	case _USHORT: 
      return (_as.short_int != b._as.short_int); 
    case _INT: 
      return (_as.integer != b._as.integer); 
    case _UINT: 
      return (_as.uinteger != b._as.uinteger); 
    case _FLT: 
      return (_as.flt != b._as.flt); 
    case _DBL: 
      return (_as.dbl != b._as.dbl); 
    default: 
      panic("bad Value type\n"); 
  } 
  _ASSERT(0);
  return (-1);
}

int Val::operator >=(const Val& b) const 
{ 
	// First, consider _FILE_PTR to be equivalent to _INT
	//
	if (((_valType == _FILE_PTR) && (b._valType == _INT)) 
		|| ((b._valType == _FILE_PTR) && (_valType == _INT)))
	{
		return (_as.integer >= b._as.integer);
	}

	// Sometimes, uint may be compared against int
	//
	if ((_valType == _INT) && (b._valType == _UINT))
	{
		return ((int)_as.integer >= (int)b._as.uinteger);
	}
	else if ((_valType == _UINT) && (b._valType == _INT))
	{
		int temp=(int)_as.uinteger >= (int)b._as.integer;
		return temp;
	}

	// Sometimes, byte values (i.e. ascii eqvivalent of characters)
	// may be compared against integer values.
	//
	if ((_valType == _CHAR) && (b._valType == _INT))
	{
		return (_as.byte >= b._as.integer);
	}
	else if ((_valType == _INT) && (b._valType == _CHAR))
	{
		return (_as.integer >= b._as.byte);
	}

	// Sometimes, there may be integer and float comparisions.
	//
	if ((_valType == _INT) && (b._valType == _FLT))
	{
		return (_as.integer >= b._as.flt);
	}
	else if ((_valType == _FLT) && (b._valType == _INT))
	{
		return (_as.flt >= b._as.integer);
	}
	else if ((_valType == _INT) && (b._valType == _DBL))
	{
		return (_as.integer >= b._as.dbl);
	}
	else if ((_valType == _DBL) && (b._valType == _INT))
	{
		return (_as.dbl >= b._as.integer);
	}

	// Sometimes, there may be float vs double comparisions.
	//
	if ((_valType == _DBL) && (b._valType == _FLT))
	{
		return (_as.dbl >= b._as.flt);
	}
	else if ((_valType == _FLT) && (b._valType == _DBL))
	{
		return (_as.flt >= b._as.dbl);
	}

//	if (_valType != b._valType) 
//     panic("inValid Value types in Val arithmetic"); 

  switch(_valType) { 
    case _CHAR: 
      return (_as.byte >= b._as.byte); 
    case _SHORT: 
	case _USHORT: 
      return (_as.short_int >= b._as.short_int); 
    case _INT: 
      return (_as.integer >= b._as.integer); 
    case _UINT: 
      return (_as.uinteger >= b._as.uinteger); 
    case _FLT: 
      return (_as.flt >= b._as.flt); 
    case _DBL: 
      return (_as.dbl >= b._as.dbl); 
    default: 
      panic("bad Value type\n"); 
  } 
  _ASSERT(0);
  return (-1);
}

int Val::operator <=(const Val& b) const 
{ 
	// First, consider _FILE_PTR to be equivalent to _INT
	//
	if (((_valType == _FILE_PTR) && (b._valType == _INT)) 
		|| ((b._valType == _FILE_PTR) && (_valType == _INT)))
	{
		return (_as.integer <= b._as.integer);
	}

	// Sometimes, uint may be compared against int
	//
	if ((_valType == _INT) && (b._valType == _UINT))
	{
		return ((int)_as.integer <= (int)b._as.uinteger);
	}
	else if ((_valType == _UINT) && (b._valType == _INT))
	{
		return ((int)_as.uinteger <= (int)b._as.integer);
	}

	// Sometimes, byte values (i.e. ascii eqvivalent of characters)
	// may be compared against integer values.
	//
	if ((_valType == _CHAR) && (b._valType == _INT))
	{
		return (_as.byte <= b._as.integer);
	}
	else if ((_valType == _INT) && (b._valType == _CHAR))
	{
		return (_as.integer <= b._as.byte);
	}

	// Sometimes, there may be integer and float comparisions.
	//
	if ((_valType == _INT) && (b._valType == _FLT))
	{
		return (_as.integer <= b._as.flt);
	}
	else if ((_valType == _FLT) && (b._valType == _INT))
	{
		return (_as.flt <= b._as.integer);
	}
	else if ((_valType == _INT) && (b._valType == _DBL))
	{
		return (_as.integer <= b._as.dbl);
	}
	else if ((_valType == _DBL) && (b._valType == _INT))
	{
		return (_as.dbl <= b._as.integer);
	}

	// Sometimes, there may be float vs double comparisions.
	//
	if ((_valType == _DBL) && (b._valType == _FLT))
	{
		return (_as.dbl <= b._as.flt);
	}
	else if ((_valType == _FLT) && (b._valType == _DBL))
	{
		return (_as.flt <= b._as.dbl);
	}

//	if (_valType != b._valType) 
//     panic("inValid Value types in Val arithmetic"); 

  switch(_valType) { 
    case _CHAR: 
      return (_as.byte <= b._as.byte); 
    case _SHORT: 
	case _USHORT: 
      return (_as.short_int <= b._as.short_int); 
    case _INT: 
      return (_as.integer <= b._as.integer); 
    case _UINT: 
      return (_as.uinteger <= b._as.uinteger); 
    case _FLT: 
      return (_as.flt <= b._as.flt); 
    case _DBL: 
      return (_as.dbl <= b._as.dbl); 
    default: 
      panic("bad Value type\n"); 
  } 
  _ASSERT(0);
  return (-1);
}

int Val::operator &&(const Val& b) const 
{ 
//  if (_valType != b._valType) 
//     panic("inValid Value types in Val arithmetic"); 
  switch(_valType) { 
    case _CHAR: 
      return (_as.byte && b._as.byte); 
    case _SHORT: 
	case _USHORT: 
      return (_as.short_int && b._as.short_int); 
    case _INT: 
      return (_as.integer && b._as.integer); 
    case _UINT: 
      return (_as.uinteger && b._as.uinteger); 
    case _FLT: 
      return (_as.flt && b._as.flt); 
    case _DBL: 
      return (_as.dbl && b._as.dbl); 
    default: 
      panic("bad Value type\n"); 
  } 
  _ASSERT(0);
  return (-1);
}

int Val::operator ||(const Val& b) const 
{ 
//  if (_valType != b._valType) 
//     panic("inValid Value types in Val arithmetic"); 
  switch(_valType) { 
    case _CHAR: 
      return (_as.byte || b._as.byte); 
    case _SHORT: 
	case _USHORT:
      return (_as.short_int || b._as.short_int); 
    case _INT: 
      return (_as.integer || b._as.integer); 
    case _UINT: 
      return (_as.uinteger || b._as.uinteger); 
    case _FLT: 
      return (_as.flt || b._as.flt); 
    case _DBL: 
      return (_as.dbl || b._as.dbl); 
    default: 
      panic("bad Value type\n"); 
  } 
  _ASSERT(0);
  return (-1);
}

/**************************************************/

Val Val::operator <<(const Val& b) const 
{ 
//  if (_valType != b._valType) 
//    panic("inValid Value types in Val arithmetic"); 
  switch(_valType) { 
    case _CHAR: 
      return Val(_as.byte << b._as.byte); 
    case _SHORT: 
	case _USHORT:
      return Val(_as.short_int << b._as.short_int); 
    case _INT: 
      return Val(_as.integer << b._as.integer); 
    case _UINT: 
      return Val(_as.uinteger << b._as.uinteger); 
    default: 
      panic("bad Value type\n"); 
  } 
  _ASSERT(0);
  return (Val(-1));
}

Val Val::operator >>(const Val& b) const 
{ 
//  if (_valType != b._valType) 
//    panic("inValid Value types in Val arithmetic"); 
  switch(_valType) { 
    case _CHAR: 
      return Val(_as.byte >> b._as.byte); 
    case _SHORT: 
	case _USHORT:
      return Val(_as.short_int >> b._as.short_int); 
    case _INT: 
      return Val( ((signed int) _as.integer) >> b._as.integer); 
    case _UINT: 
      return Val(_as.uinteger >> b._as.uinteger); 
    default: 
      panic("bad Value type\n"); 
  } 
  _ASSERT(0);
  return (Val(-1));
}

Val Val::operator &(const Val& b) const 
{ 

	if ((_valType == _CHAR) && (b._valType == _INT))
	{
		return Val(_as.byte & b._as.integer);
	}
	else if ((_valType == _INT) && (b._valType == _CHAR))
	{
		return Val(_as.integer & b._as.byte);
	}

//  if (_valType != b._valType) 
//    panic("inValid Value types in Val arithmetic"); 
  switch(_valType) { 
    case _CHAR: 
      return Val(_as.byte & b._as.byte); 
    case _SHORT: 
	case _USHORT:
      return Val(_as.short_int & b._as.short_int); 
    case _INT: 
      return Val(_as.integer & b._as.integer); 
    case _UINT: 
      return Val(_as.uinteger & b._as.uinteger); 
    default: 
      panic("bad Value type\n"); 
  } 
  _ASSERT(0);
  return (Val(-1));
}

Val Val::operator |(const Val& b) const 
{ 
	if ((_valType == _CHAR) && (b._valType == _INT))
	{
		return Val(_as.byte | b._as.integer);
	}
	else if ((_valType == _INT) && (b._valType == _CHAR))
	{
		return Val(_as.integer | b._as.byte);
	}

//  if (_valType != b._valType) 
//    panic("inValid Value types in Val arithmetic"); 
  switch(_valType) { 
    case _CHAR: 
      return Val(_as.byte | b._as.byte); 
    case _SHORT: 
	case _USHORT:
      return Val(_as.short_int | b._as.short_int); 
    case _INT: 
      return Val(_as.integer | b._as.integer); 
    case _UINT: 
      return Val(_as.uinteger | b._as.uinteger); 
    default: 
      panic("bad Value type\n"); 
  } 
  _ASSERT(0);
  return (Val(-1));
}

Val Val::operator ^(const Val& b) const 
{ 
	if ((_valType == _CHAR) && (b._valType == _INT))
	{
		return Val(_as.byte ^ b._as.integer);
	}
	else if ((_valType == _INT) && (b._valType == _CHAR))
	{
		return Val(_as.integer ^ b._as.byte);
	}

//  if (_valType != b._valType) 
//    panic("inValid Value types in Val arithmetic"); 
  switch(_valType) { 
    case _CHAR: 
      return Val(_as.byte ^ b._as.byte); 
    case _SHORT: 
	case _USHORT:
      return Val(_as.short_int ^ b._as.short_int); 
    case _INT: 
      return Val(_as.integer ^ b._as.integer); 
    case _UINT: 
      return Val(_as.uinteger ^ b._as.uinteger); 
    default: 
      panic("bad Value type\n"); 
  } 
  _ASSERT(0);
  return (Val(-1));
}

/**************************************************/

Val Val::operator ~() const { 
  switch(_valType) { 
    case _CHAR: 
      return Val(~ _as.byte); 
    case _SHORT: 
	case _USHORT:
      return Val(~ _as.short_int); 
    case _INT: 
      return Val(~ _as.integer); 
    case _UINT: 
      return Val(~ _as.uinteger); 
    default: 
      panic("bad Value type\n"); 
  } 
  _ASSERT(0);
  return (Val(-1));
}

/**************************************************/

Val Val::ash(const Val &b)
{
	int size;

	switch(_valType) {
	case _CHAR:
		{
		size = sizeof(char);
		char mask = 0x1 << ((size * 8) - 1);
		char lastBit = _as.byte & mask;
		if (lastBit == 0)
		{
			return Val(_as.byte >> b._as.byte);
		}
		else
		{
			mask = lastBit;
			for (int i = 0; i < b._as.byte; i++)
			{
				lastBit = lastBit >> 1;
				mask = mask | lastBit;
			}
			return Val((_as.byte >> b._as.byte) | mask);
		}
		break;
		}
	case _SHORT:
	case _USHORT:
		{
		size = sizeof(short);
		short mask = 0x1 << ((size * 8) - 1);
		short lastBit = _as.short_int & mask;
		if (lastBit == 0)
		{
			return Val(_as.short_int >> b._as.short_int);
		}
		else
		{
			mask = lastBit;
			for (int i = 0; i < b._as.short_int; i++)
			{
				lastBit = lastBit >> 1;
				mask = mask | lastBit;
			}
			return Val((_as.short_int >> b._as.short_int) | mask);
		}
		break;
		}
	case _INT:
		{
		size = sizeof(long);
		long mask = 0x1 << ((size * 8) - 1);
		long lastBit = _as.integer & mask;
		if (lastBit == 0)
		{
			return Val(_as.integer >> b._as.integer);
		}
		else
		{
			mask = lastBit;
			for (int i = 0; i < b._as.integer; i++)
			{
				lastBit = lastBit >> 1;
				mask = mask | lastBit;
			}
			return Val((_as.integer >> b._as.integer) | mask);
		}
		break;
		}
	case _UINT:
		{
		size = sizeof(unsigned);
		unsigned mask = 0x1 << ((size * 8) - 1);
		unsigned lastBit = _as.uinteger & mask;
		if (lastBit == 0)
		{
			return Val(_as.uinteger >> b._as.uinteger);
		}
		else
		{
			mask = lastBit;
			for (int i = 0; i < b._as.uinteger; i++)
			{
				lastBit = lastBit >> 1;
				mask = mask | lastBit;
			}
			return Val((_as.uinteger >> b._as.uinteger) | mask);
		}
		break;
		}
	default:
		panic("bad value type\n");
	}
	_ASSERT(0);
	return Val(-1);
}

Val Val::rr(const Val &b)
{
	int size;
	switch (_valType) {
	case _CHAR:
		{
		size = sizeof(char);
		char mask = 0x1 << (size * 8 - 1);
		char result = _as.byte;
		char firstBit;
		for (int i = 0; i < b._as.byte; i++)
		{
			// Extract out the first bit
			firstBit = result & 1;
			result = result >> 1;
			if (firstBit)
			{
				result = result | mask;
			}
		}
		return Val(result);
		}
	case _SHORT:
	case _USHORT:
		{
		size = sizeof(short);
		short mask = 0x1 << (size * 8 - 1);
		short result = _as.byte;
		char firstBit;
		for (int i = 0; i < b._as.short_int; i++)
		{
			// Extract out the first bit
			firstBit = result & 1;
			result = result >> 1;
			if (firstBit)
			{
				result = result | mask;
			}
		}
		return Val(result);
		}
	case _INT:
		{
		size = sizeof(long);
		long mask = 0x1 << (size * 8 - 1);
		long result = _as.integer;
		char firstBit;
		for (int i = 0; i < b._as.integer; i++)
		{
			// Extract out the first bit
			firstBit = result & 1;
			result = result >> 1;
			if (firstBit)
			{
				result = result | mask;
			}
		}
		return Val(result);
		}
	case _UINT:
		{
		size = sizeof(unsigned);
		unsigned mask = 0x1 << (size * 8 - 1);
		unsigned result = _as.uinteger;
		char firstBit;
		for (int i = 0; i < b._as.uinteger; i++)
		{
			// Extract out the first bit
			firstBit = result & 1;
			result = result >> 1;
			if (firstBit)
			{
				result = result | mask;
			}
		}
		return Val(result);
		}
	}
	_ASSERT(0);
	return Val(-1);
}

/**************************************************/

Val Val::operator -() const 
{ 
  switch(_valType) {
  case _BOOL:
	  if (_as.bl == true)
		  return Val(false);
	  else
		  return Val(true);
    case _CHAR: 
      return Val(- _as.byte); 
    case _SHORT: 
	case _USHORT:
      return Val(- _as.short_int); 
    case _INT: 
      return Val(- _as.integer); 
    case _UINT: 
      return Val(- _as.uinteger); 
    case _FLT: 
      return Val(- _as.flt); 
    case _DBL: 
      return Val(- _as.dbl); 
    default: 
      panic("bad Value type\n"); 
  } 
  _ASSERT(0);
  return (Val(-1));
}

/**************************************************/

Val Val::negate() const
{
	switch(_valType)
	{
	case _BOOL:
		if (_as.bl == true)
		  return Val(false);
	  else
		  return Val(true);
	case _CHAR:
		if (_as.byte != 0)
			return Val(0);
		else
			return Val(1);
	case _SHORT:
	case _USHORT:
		if (_as.short_int!= 0)
			return Val(0);
		else
			return Val(1);
	case _INT:
		if (_as.integer != 0)
			return Val(0);
		else
			return Val(1);
	case _UINT:
		if (_as.uinteger != 0)
			return Val(0);
		else
			return Val(1);
	case _FLT:
		if (_as.flt != 0.0)
			return Val(0.0);
		else
			return Val(1.0);
	case _DBL:
		if (_as.dbl != 0.0)
			return Val(0.0);
		else
			return Val(1.0);
	default:
		panic("bad Value type\n");
	}
	_ASSERT(0);
	return (Val(-1));
}

/**************************************************/

Val Val::squareroot() const 
{ 
  switch(_valType) { 
    case _CHAR: 
      return Val(sqrt(_as.byte)); 
    case _SHORT: 
	case _USHORT:
      return Val(sqrt(_as.short_int)); 
    case _INT: 
      return Val(sqrt(_as.integer)); 
    case _UINT: 
      return Val(sqrt(_as.uinteger)); 
    case _FLT: 
      return Val(sqrt(_as.flt)); 
    case _DBL: 
      return Val(sqrt(_as.dbl)); 
    default: 
      panic("bad Value type\n"); 
  } 
  _ASSERT(0);
  return (Val(-1));
}

/**************************************************/

Val Val::exponential() const 
{ 
  switch(_valType) { 
    case _CHAR: 
      return Val(exp(_as.byte)); 
    case _SHORT: 
	case _USHORT:
      return Val(exp(_as.short_int)); 
    case _INT: 
      return Val(exp(_as.integer)); 
    case _UINT: 
      return Val(exp(_as.uinteger)); 
    case _FLT: 
      return Val(exp(_as.flt)); 
    case _DBL: 
      return Val(exp(_as.dbl)); 
    default: 
      panic("bad Value type\n"); 
  } 
  _ASSERT(0);
  return (Val(-1));
}

/**************************************************/

Val Val::and(Val & b)
{
	if (_valType == b._valType)
	{
		switch(_valType)
		{
		case _BOOL:
			if ((_as.bl == true) && (b._as.bl == true))
				return Val(true);
			else
				return Val(false);
		case _CHAR:
			if ((_as.byte != 0) && (b._as.byte != 0))
				return Val(1);
			else
				return Val(0);
		case _SHORT:
		case _USHORT:
			if ((_as.short_int != 0) && (b._as.short_int != 0))
				return Val(1);
			else
				return Val(0);
		case _INT:
			if ((_as.integer != 0) && (b._as.integer != 0))
				return Val(1);
			else
				return Val(0);
		case _FLT:
			if ((_as.flt != 0.0) && (b._as.flt != 0.0))
				return Val(1.0);
			else
				return Val(0.0);
		case _DBL:
			if ((_as.dbl != 0.0) && (b._as.dbl != 0.0))
				return Val(1.0);
			else
				return Val(0.0);
		default:
			panic("And operator recieved bad Value type\n");
		}
	}
	_ASSERT(0);
	return (Val(-1));
}

/**************************************************/

int Val::isFalse()
{
	switch(_valType)
	{
	case _BOOL:
		if (_as.bl == false) return _YES_;
		break;
	case _CHAR:
		if (_as.byte == 0) return _YES_;
		break;
	case _SHORT:
	case _USHORT:
		if (_as.short_int == 0) return _YES_;
		break;
	case _INT:
		if (_as.integer == 0) return _YES_;
		break;
	case _FLT:
		if (_as.flt == 0.0) return _YES_;
		break;
	case _DBL:
		if (_as.dbl == 0.0) return _YES_;
		break;
	default:
		panic("And operator recieved bad Value type\n");
	}
	return _NO_;
}

int Val::isAscii()
{
	if ((_valType == _USHORT) || (_valType == _SHORT) || (_valType == _UINT) || (_valType == _INT))
	{
		if (__isascii(_as.integer)) return _YES_;
	}
	if (_valType == _CHAR)
	{
		int tmp = _as.byte;
		if (__isascii(tmp)) return _YES_;
	}
	return _NO_;
}

int Val::getAscii()
{
	if ((_valType == _USHORT) || (_valType == _SHORT) || (_valType == _UINT) || (_valType == _INT))
	{
		if (__isascii(_as.integer)) return _as.integer;
	}
	if (_valType == _CHAR)
	{
		int tmp = _as.byte;
		if (__isascii(tmp)) return tmp;
	}
	return _INVALID_;
}

unsigned char Val::getByte()
{
	unsigned char retVal = '\0';

	if ((_valType == _USHORT) || (_valType == _SHORT) || (_valType == _UINT) || (_valType == _INT))
	{
		if (__isascii(_as.integer))
		{
			retVal = _as.integer;
		}
	}
	if (_valType == _CHAR)
	{
		retVal = _as.byte;
	}
	return retVal;
}

void Val::setValue(char *Value)
{
  istrstream s(Value);
  switch(_valType) {
    case _CHAR:
      s>>_as.byte; break;
    case _SHORT:
	case _USHORT:
      s >> _as.short_int; break;
    case _INT:
      s >> _as.integer; break;
    case _UINT:
      s >> _as.uinteger; break;
    case _FLT:
      s >> _as.flt; break;
    case _DBL:
    case _ANY:
      s >> _as.dbl; break;
    case _INTERNAL_LABEL:
      s >> _as.label; break;
    default:
      panic("Val::operator<<:  bad _valType\n");
  }
}

void Val::print(ostream& out,VALUE_TYPE type) 
{
  if (type==_ANY)
    type = _valType;
  switch(type) {
    case _CHAR:
      out << (int)_as.byte; break;
    case _SHORT:
	case _USHORT:
      out << _as.short_int; break;
    case _INT:
      out << _as.integer; break;
    case _UINT:
      out << _as.uinteger; break;
    case _FLT:
      out << _as.flt; break;
    case _DBL:
    case _ANY:
      out << _as.dbl; break;
    case _INTERNAL_LABEL:
      out << _as.label; break;
	case _FILE_PTR:
		out << "(FILE *) " << _as.integer; break;
    default:
      panic("Val::operator<<:  bad _valType\n");
  }
}

void Val::printTyped(ostream& out) 
{
  switch(_valType) {
    case _CHAR:
      out << "(B)"<< (int)_as.byte; break;
	case _UCHAR:
      out << "(UB)"<< (int)_as.byte; break;
    case _SHORT:
	  out << "(S)"<< _as.short_int; break;
	case _USHORT:
      out << "(US)"<< _as.short_int; break;
    case _INT:
      out << "(I)"<< _as.integer; break;
    case _UINT:
      out << "(UI)"<< _as.uinteger; break;
    case _FLT:
      out << "(F)"<< _as.flt; break;
    case _DBL:
      out << "(D)"<< _as.dbl; break;
    case _INTERNAL_LABEL:
      out << "(L)"<< _as.label; break;
	case _ANY:
	  out << "(ANY)"<< _as.dbl; break;
	case _FILE_PTR:
	  out << "(FP)"<< _as.uinteger; break;
	case _EIGHT_BYTES:
		out << "(EB)"; break;
    default:
      panic("Val::operator<<:  bad _valType\n");
  }

}

void Val::compose(Vector<Val> & listOfData){
	*this = listOfData.front();
}

void Val::decompose(Vector<Val> & listOfData){
	listOfData.push_back(*this);
}

void Val::getListOfAddr(BaseType & startAddr, Vector<AddrOperand> & listOfAddr){
	listOfAddr.push_back((AddrOperand&)startAddr);
}

int Val::isExternalVectorType()
{
	if (_valType == _EXTERNAL_VECTOR) return _YES_;
	return _NO_;
}

// ToDo, TODO, TBD : SAH Sun, Feb 27, 00 : Complete this method.
//
void Val::writeToString(MyString & s)
{
	char buf[80];

	switch (_valType)
	{
	case _BOOL:
		s.concat("L ");
		sprintf(buf, "%d", _as.bl);
		s.concat(buf);
		break;
	case _CHAR:
		s.concat("B ");
		sprintf(buf, "%c", _as.byte);
		s.concat(buf);
		break;
	case _SHORT:
	case _USHORT:
		s.concat("S ");
		sprintf(buf, "%d", _as.short_int);
		s.concat(buf);
		break;
	case _INT:
		s.concat("I ");
		sprintf(buf, "%d", _as.integer);
		s.concat(buf);
		break;
	case _UINT:
		s.concat("U ");
		sprintf(buf, "%d", _as.uinteger);
		s.concat(buf);
		break;
	case _FLT:
		s.concat("F ");
		sprintf(buf, "%f", _as.flt);
		s.concat(buf);
		break;
	case _DBL:
		s.concat("D ");
		sprintf(buf, "%g", _as.dbl);
		s.concat(buf);
		break;
	case _ANY:
		s.concat("A ");
		sprintf(buf, "%g", _as.dbl);
		s.concat(buf);
		break;
	case _INTERNAL_LABEL:
		s.concat("R ");
		s.concat(_as.label);
		break;
	case _EXTERNAL_VECTOR:
		s.concat("V ");
		break;
	case _FILE_PTR:
		s.concat("P ");
		sprintf(buf, "%d", _as.uinteger);
		s.concat(buf);
		break;
	default:
		panic("Val::writeToString recieved bad _valType\n");
	}
}

// ToDo, TODO, TBD : SAH Sun, Feb 27, 00 : Complete this method.
//
void Val::readFromString(MyString & s)
{
	MyString temp = strtok(s.getStr(), " ");

	int tempInt;

	if (temp == "L")
	{
		_valType = _BOOL;

		// The next field of s is a bool.
		//
		tempInt = atoi(strtok(NULL, "\n"));
		if (tempInt) _as.bl = true;
		_as.bl = false;

	}
	if (temp == "B")
	{
		_valType = _CHAR;

		// The next field of s is a byte.
		//
		char *tstr = strtok(NULL, "\n");
		if (tstr != NULL)
		{
			_as.byte = tstr[0];
		}
	}
	if (temp == "S")
	{
		_valType = _SHORT;

		// The next field of s is a short.
		//
		_as.short_int = atoi(strtok(NULL, "\n"));
	}
	if (temp == "I")
	{
		_valType = _INT;

		// The next field of s is a integer.
		//
		_as.integer = atoi(strtok(NULL, "\n"));
	}
	if (temp == "U")
	{
		_valType = _UINT;

		// The next field of s is a uinteger.
		//
		_as.uinteger = atoi(strtok(NULL, "\n"));
	}
	if (temp == "F")
	{
		_valType = _FLT;

		// The next field of s is a float.
		//
		_as.flt = atof(strtok(NULL, "\n"));
	}
	if (temp == "D")
	{
		_valType = _DBL;

		// The next field of s is a double.
		//
		_as.dbl = atof(strtok(NULL, "\n"));
	}
	if (temp == "A")
	{
		_valType = _ANY;

		// The next field of s is a double.
		//
		_as.dbl = atof(strtok(NULL, "\n"));
	}
	if (temp == "R")
	{
		_valType = _INTERNAL_LABEL;

		// The next field of s is a char *string.
		//
		_as.label = strdup(strtok(NULL, "\n"));
	}
	if (temp == "V")
	{
		_valType = _EXTERNAL_VECTOR;
	}
	if (temp == "P")
	{
		_valType = _FILE_PTR;

		// The next field of s is an integer.
		//
		_as.integer = atoi(strtok(NULL, "\n"));
	}
}

void Val::addBytes(Vector<Val> & byteList)
{
	Vector<Val>::iterator i1;
	int i = 0;

	if (byteList.size() > MAX_NUM_BYTES) // error condition
	{
		cout << "Panic: Tried to greater than MAX_NUM_BYTES (8) number of bytes to val as word" << endl;
		assert(0);
	}

	_valType = _EIGHT_BYTES;

	for (i1 = byteList.begin(); i1 != byteList.end(); i1++)
	{
		if (((*i1)._valType != _UCHAR) && ((*i1)._valType != _CHAR)) // error condition.
		{
			cout << "Panic: Tried to compose bytes from an invalid data type" << endl;
			assert(0);
		}
		_as.word[i] = (*i1).getByte();
		i++;
	}
	_size = i;
}

void Val::getBytes(Vector<Val> & byteList)
{
	int i;

	if (_valType != _EIGHT_BYTES) // error condition
	{
		cout << "Panic: Tried to get bytes from a val that was not of _EIGHT_BYTES type" << endl;
		assert(0);
	}

	for (i = 0; i < MAX_NUM_BYTES; i++)
	{
		Val tmp((char)_as.word[i]);
		byteList.push_back(tmp);
	}
}


//changes the value from the previous type to the new type t
//t: input, the new type to change the value to 
//not for dlb, flt, and other types
void Val::changeType(VALUE_TYPE t){
	switch(this->_valType){
	case _CHAR://from CHAR 
	case _UCHAR:
		switch(t){
		case _CHAR://to char
			this->_as.byte=(char)(this->_as.byte);
			this->_valType=_CHAR;
			break;
		case _UCHAR://to uchar
			this->_as.byte=(unsigned char)(this->_as.byte);
			this->_valType=_UCHAR;
			break;
		case _SHORT:
			this->_as.short_int=(short)(this->_as.byte);
			this->_valType=_SHORT;
			break;
		case _USHORT:
			this->_as.short_int=(unsigned short)((unsigned char)(this->_as.byte));
			this->_valType=_USHORT;
			break;
		case _INT:
			this->_as.integer=(int)(this->_as.byte);
			this->_valType=_INT;
			break;
		case _UINT:
			this->_as.uinteger=(unsigned int)((unsigned char)(this->_as.byte));
			this->_valType=_UINT;
			break;
		case _LONG:
			this->_as.integer=(long)(this->_as.byte);
			this->_valType=_LONG;
			break;
		case _ULONG:
			this->_as.uinteger=(unsigned long)((unsigned char)(this->_as.byte));
			this->_valType=_ULONG;
			break;
		default:
			_ASSERT(0);
			break;
		}
		break;
	case _SHORT:
	case _USHORT:
		switch(t){
		case _CHAR://to char
			this->_as.byte=(char)(this->_as.short_int);
			this->_valType=_CHAR;
			break;
		case _UCHAR://to uchar
			this->_as.byte=(unsigned char)((unsigned short)(this->_as.short_int));
			this->_valType=_UCHAR;
			break;
		case _SHORT:
			this->_as.short_int=(short)(this->_as.short_int);
			this->_valType=_SHORT;
			break;
		case _USHORT:
			this->_as.short_int=(unsigned short)(this->_as.short_int);
			this->_valType=_USHORT;
			break;
		case _INT:
			this->_as.integer=(int)(this->_as.short_int);
			this->_valType=_INT;
			break;
		case _UINT:
			this->_as.uinteger=(unsigned int)((unsigned short)(this->_as.short_int));
			this->_valType=_UINT;
			break;
		case _LONG:
			this->_as.integer=(long)(this->_as.short_int);
			this->_valType=_LONG;
			break;
		case _ULONG:
			this->_as.uinteger=(unsigned long)((unsigned short)(this->_as.short_int));
			this->_valType=_ULONG;
			break;
		default:
			_ASSERT(0);
			break;
		}
		break;
	case _INT:
	case _LONG:
		switch(t){
		case _CHAR://to char
			this->_as.byte=(char)(this->_as.integer);
			this->_valType=_CHAR;
			break;
		case _UCHAR://to uchar
			this->_as.byte=(unsigned char)(this->_as.integer);
			this->_valType=_UCHAR;
			break;
		case _SHORT:
			this->_as.short_int=(short)(this->_as.integer);
			this->_valType=_SHORT;
			break;
		case _USHORT:
			this->_as.short_int=(unsigned short)(this->_as.integer);
			this->_valType=_USHORT;
			break;
		case _INT:
			this->_as.integer=(int)(this->_as.integer);
			this->_valType=_INT;
			break;
		case _UINT:
			this->_as.uinteger=(unsigned int)(this->_as.integer);
			this->_valType=_UINT;
			break;
		case _LONG:
			this->_as.integer=(long)(this->_as.integer);
			this->_valType=_LONG;
			break;
		case _ULONG:
			this->_as.uinteger=(unsigned long)(this->_as.integer);
			this->_valType=_ULONG;
			break;
		default:
			_ASSERT(0);
			break;
		}
		break;
	case _UINT:
	case _ULONG:
		switch(t){
		case _CHAR://to char
			this->_as.byte=(char)(this->_as.uinteger);
			this->_valType=_CHAR;
			break;
		case _UCHAR://to uchar
			this->_as.byte=(unsigned char)(this->_as.uinteger);
			this->_valType=_UCHAR;
			break;
		case _SHORT:
			this->_as.short_int=(short)(this->_as.uinteger);
			this->_valType=_SHORT;
			break;
		case _USHORT:
			this->_as.short_int=(unsigned short)(this->_as.uinteger);
			this->_valType=_USHORT;
			break;
		case _INT:
			this->_as.integer=(int)(this->_as.uinteger);
			this->_valType=_INT;
			break;
		case _UINT:
			this->_as.uinteger=(unsigned int)(this->_as.uinteger);
			this->_valType=_UINT;
			break;
		case _LONG:
			this->_as.integer=(long)(this->_as.uinteger);
			this->_valType=_LONG;
			break;
		case _ULONG:
			this->_as.uinteger=(unsigned long)(this->_as.uinteger);
			this->_valType=_ULONG;
			break;
		default:
			_ASSERT(0);
			break;
		}
		break;
	default:
		_ASSERT(0);
	}
}



int isIntegerType(Val & a)
{
	if ((a._valType == _SHORT) || (a._valType == _UINT) || (a._valType == _INT))
		return _YES_;

	return _NO_;
}

int getIntegerValue(Val & a)
{
	// if val is int, short, byte return the integer value.

//	if (a._valType == _CHAR) return a._as.byte;
	if (a._valType == _SHORT) return a._as.short_int;
	if (a._valType == _UINT) return a._as.uinteger;
	if (a._valType == _INT) return a._as.integer;
//	if (a._valType == _FILE_PTR) return a._as.uinteger;

	// error condition.
	assert(0);
	return -1;
}

int isFloatType(Val & a)
{
	if ((a._valType == _FLT) || (a._valType == _DBL)) return _YES_;

	return _NO_;
}

double getFloatValue(Val & a)
{
	// if val is float, double return the double value.

	if (a._valType == _FLT) return a._as.flt;
	if (a._valType == _DBL) return a._as.dbl;

	// error condition.
	assert(0);
	return -1.0;
}

int isStringType(Val & a)
{
	if (a._valType == _INTERNAL_LABEL) return _YES_;

	return _NO_;
}

char *getStringValue(Val & a)
{
	// if val is internal_label, return char * pointer;

	if (a._valType == _INTERNAL_LABEL) return a._as.label;

	// error condition.
	assert(0);
	return NULL;
}

int isAddrType(Val & a)
{
	// if val is of uinteger, integer type return _YES_

	if ((a._valType == _UINT) || (a._valType == _INT))
		return _YES_;

	return _NO_;
} 

// 32-bit a, 32-bit b.
//
// a = a1 + a2<<16.
// b = b1 + b2<<16.
//
// d = mult64(a, b) = a1*b1 + (a2*b1 + a1*b2)<<16 + (a2*b2)<<32
//                  = a1b1 + a2b1a1b2<<16 + a2b2<<32
// d = d1 + 32<<d2;
//
// d1 = a1b1 + a2b1a1b2<<16;
// d2 = a2b2 + a2b1a1b2>>16 + carry;
//
// carry = 1 if ((d1 < a1b1) || (d1 < (a2b1a1b2<<16)))
//       = 0 otherwise.
//
// Note: I am note sure that this is the right way to do this.
// I am also not sure if this handles all the cases correctly (e.g. multiplying 2 large -ves)
//
// Addendum (Sudeep - 05/22/03) multiplication of -ve integers was not being properly handled
// Rectified

Val mult64(Val & a, Val & b)
{
	Val retVal;

		int isIntType = 0, isUIntType = 0, sign1 = 0, sign2 = 0;

	if ((a._valType == _INT) && (b._valType == _INT))
		isIntType = 1;

	if ((a._valType == _UINT) && (b._valType == _UINT))
		isUIntType = 1;

	if (isIntType)
	{
	    unsigned int ta, tb, a1, a2, b1, b2, a2b1a1b2, d1, d2;

		if (a._as.integer&0x80000000) // -ve
		{
			ta = (~a._as.integer) + 1;
			sign1 = 1;
		} else {
			ta = a._as.integer;
		}

		if (b._as.integer&0x80000000) // -ve
		{
			tb = (~b._as.integer) + 1;
			sign2 = 1;
		} else {
			tb = b._as.integer;
		}


		a1 = (ta<<16)>>16;
		a2 = (ta>>16);

		b1 = (tb<<16)>>16;
		b2 = (tb>>16);

		a2b1a1b2 = a2*b1 + a1*b2;

		d1 = a1*b1 + (a2b1a1b2<<16);
		d2 = a2*b2 + (a2b1a1b2>>16);

		if ((d1 < a1*b1) || (d1 < (a2b1a1b2<<16)))
			d2++;

		if (sign1 ^ sign2)
		{
			// take 2's complement

			d1 = ~d1;
			d2 = ~d2;

			if (d1 & 0xffffffff)
			{
				d2 = d2 + 1;
			}

			d1 = d1 + 1;
		}

		Val v1((int)d1);
		Val v2((int)d2);
		int sz;

		unsigned char array1[MAX_NUM_BYTES], array2[MAX_NUM_BYTES];
		DataToArray(v1, array1, sz);
		DataToArray(v2, array2, sz);

		retVal._valType = _EIGHT_BYTES;

		for (int i = 0; i < 4; i++)
			retVal._as.word[i] = array1[i];
		for (i = 0; i < 4; i++)
			retVal._as.word[i+4] = array2[i];
	}
	else if (isUIntType)
	{
		unsigned int a1, a2, b1, b2, a2b1a1b2, d1, d2;

		a1 = (a._as.uinteger<<16)>>16;
		a2 = (a._as.uinteger>>16);

		b1 = (b._as.uinteger<<16)>>16;
		b2 = (b._as.uinteger>>16);

		a2b1a1b2 = a2*b1 + a1*b2;

		d1 = a1*b1 + (a2b1a1b2<<16);
		d2 = a2*b2 + (a2b1a1b2>>16);

		if ((d1 < a1*b1) || (d1 < (a2b1a1b2<<16)))
			d2++;

		Val v1((int)d1);
		Val v2((int)d2);
		int sz;

		unsigned char array1[MAX_NUM_BYTES], array2[MAX_NUM_BYTES];
		DataToArray(v1, array1, sz);
		DataToArray(v2, array2, sz);

		retVal._valType = _EIGHT_BYTES;

		for (int i = 0; i < 4; i++)
			retVal._as.word[i] = array1[i];
		for (i = 0; i < 4; i++)
			retVal._as.word[i+4] = array2[i];
	}
	else
		assert(0);


/*	int isIntType = 0;

	if ((a._valType == _INT) && (b._valType == _INT))
		isIntType = 1;

	if ((a._valType == _UINT) && (b._valType == _UINT))
		isIntType = 1;

	if (isIntType)
	{
		unsigned int a1, a2, b1, b2, a2b1a1b2, d1, d2;

		a1 = (a._as.uinteger<<16)>>16;
		a2 = (a._as.uinteger>>16);

		b1 = (b._as.uinteger<<16)>>16;
		b2 = (b._as.uinteger>>16);

		a2b1a1b2 = a2*b1 + a1*b2;

		d1 = a1*b1 + (a2b1a1b2<<16);
		d2 = a2*b2 + (a2b1a1b2>>16);

		if ((d1 < a1*b1) || (d1 < (a2b1a1b2<<16)))
			d2++;

		Val v1((int)d1);
		Val v2((int)d2);
		int sz;

		unsigned char array1[MAX_NUM_BYTES], array2[MAX_NUM_BYTES];
		DataToArray(v1, array1, sz);
		DataToArray(v2, array2, sz);

		retVal._valType = _EIGHT_BYTES;

		for (int i = 0; i < 4; i++)
			retVal._as.word[i] = array1[i];
		for (i = 0; i < 4; i++)
			retVal._as.word[i+4] = array2[i];
	}
	else
		assert(0);

 */ 

	return retVal;
}


// 32-bit a, 32-bit b.
// quotient is in LO, and remainder is in HI.
//
Val div64(Val & a, Val & b)
{
	Val retVal;

	if ((a._valType == _INT) && (b._valType == _INT))
	{
		int q, r;
		q = (a._as.integer)/(b._as.integer);
		r = (a._as.integer)%(b._as.integer);

		Val v1(q);
		Val v2(r);
		int sz;

		unsigned char array1[MAX_NUM_BYTES], array2[MAX_NUM_BYTES];
		DataToArray(v1, array1, sz);
		DataToArray(v2, array2, sz);

		retVal._valType = _EIGHT_BYTES;

		for (int i = 0; i < 4; i++)
			retVal._as.word[i] = array1[i];
		for (i = 0; i < 4; i++)
			retVal._as.word[i+4] = array2[i];
	}
	else
		assert(0);

	return retVal;
}
