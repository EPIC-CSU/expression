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
// $Id: RegisterFile.h,v 1.7 1998/03/27 00:01:18 wtang Exp $
//
// File:  		RegisterFile.h
// Created:		Tue Nov 19 , 96
// Last modified: 	Tue Nov 19 , 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARCHITECTURE

#ifndef __REGISTERFILE_H__
#define __REGISTERFILE_H__

#include "ArchitectureIncludes.h"
#include "RealRegArg.h"

class RegisterFile 
{
private:
 
#ifdef COUNT_NUMREGFILES
 static int _numRegFiles;
#endif
 
protected:
 char *_name;
 int _numRegisters;
 
 int _regFileID;
 
 int _valType;  // float or int.

 int _genericOrTarget; // register file if generic or target

 //
 // Currently the program initializes all regs to int. This must be
 // changed later.
 
public:
 RegisterFile()
 {
	 _name = _NULL_;
	 _numRegisters = 0;
	 
	 _regFileID = _INVALID_;
  
	 _valType = _INT_;

	 _genericOrTarget = _GENERIC_;
	 
#ifdef COUNT_NUMREGFILES
	 _numRegFiles++;
#endif
 }
 
 RegisterFile(char *name, int numRegs = 1, int regFileID = _INVALID_, int valType = _INT_, int genericOrTarget = _GENERIC_) :
 _numRegisters(numRegs), _regFileID(regFileID), _valType(valType), _genericOrTarget(genericOrTarget)
 {
	 _name = strdup(name);
  
#ifdef COUNT_NUMREGFILES
	 _numRegFiles++;
#endif
 }

 RegisterFile(const RegisterFile & b) : 
 _numRegisters(b._numRegisters), _regFileID(b._regFileID), _valType(b._valType), _genericOrTarget(b._genericOrTarget)
 {
	 _name = strdup(b._name);
  
#ifdef COUNT_NUMREGFILES
	 _numRegFiles++;
#endif
  }

 ~RegisterFile()
 {
  if (_name!=NULL)
   free(_name);
  
#ifdef COUNT_NUMREGFILES
	 _numRegFiles--;
#endif
 }
 
 const int size()
 {
	 return _numRegisters;
 }
 
 char *name()
 {
	 return _name;
 }
 
 int valType()
 {
	 return _valType;
 }
 
 void setValType(int valType)
 {
	 _valType = valType;
 }
 
 friend const int operator == (const RegisterFile & a,
  const RegisterFile & b)
 {
	 if (strcmp(a._name, b._name) == 0)
   return _YES_;
	 return _NO_;
 }
 
 friend const int operator == (const RegisterFile & a,
  const char *b)
 {
	 if (strcmp(a._name, b) == 0)
   return _YES_;
	 return _NO_;
 }
 
 void print(ostream & out)
 {
	 out << _name;
 }

 friend ostream & operator << (ostream & out, const RegisterFile & s)
 {
	 out << "Register File: ";
	 out << s._name;
  
	 return out;
 }
 
 RegisterFile *copy();
 
 int getID(int registerNumber)
 {
	 if (registerNumber >= _numRegisters)
  {
   return 0;
   //
   // Throw an exception.
  }
	 return (_regFileID + registerNumber);
 }
 
 int getID()
 {
	 return _regFileID;
 }
 
 void setID(int regFileID)
 {
	 _regFileID = regFileID;
 }
 
 int isValidID(int someRegNum)
 {
	 if ((someRegNum >= _regFileID) && (someRegNum < (_regFileID +
   _numRegisters)))
   return _YES_;
	 
	 return _NO_;
 }
 

 // Following two functions are added after categorizing the registerFileList
 // as genericRegFileList and targetRegFileList

 void setGenericOrTarget( int genericOrTarget )
 {
	 _genericOrTarget = genericOrTarget;
	 return;
 }

 int isGenericOrTarget()
 {
	 return _genericOrTarget;
 }

 BaseArgument *createRealReg(int index, int regNum);
 
#ifdef COUNT_NUMREGFILES
 const int numRegFiles()
 {
	 return _numRegFiles;
 }
#endif
 
};

#endif



