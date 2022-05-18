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
// $Id: RealRegArg.h,v 1.11 1998/03/27 00:01:16 wtang Exp $
//
// File:  		RealRegArg.h
// Created:		Fri Nov 01 , 96
// Last modified: 	Fri Nov 01 , 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARGUMENT

#ifndef __REALREGARG_H__
#define __REALREGARG_H__

#include <string.h>

#include "RegisterArg.h"
#include "BaseArgProperty.h"
#include "BaseArgument.h"
#include "disamb.h"
#include "IburgDefs.h"

#define _INVALID_ -1

typedef int RegFileNum;
typedef int RegNum;

// extern RegisterFileList *globalRegFileList;


// During conversion to _SSA_ form, each variable (register argument)
// gets a unique name (e.g. R1 to R1.1). We want to keep the
// original name of the argument (i.e. we do not want to mess with
// the "_regName" member data). Thus, the new name of the argument
// is kept as a SSAArgProperty. Also, SSAArgProperty provides us with a
// convinient handle to generate new names. See SSAArgProperty.h
// for more info.

extern int doneRA;

class RealRegArg : public RegisterArg
{
private:
	
#ifdef COUNT_REALREGS
	static int _numRealRegs;	// See Comment 1 below.
#endif
	
protected:
	
	RegFileNum _regFile;
	//
	// Identifies the register file to which this register belongs.
	// Assumption - If two registers belong to different register
	// files, then they can never be equivalent. i.e. no sharing of
	// registers between register files.
	
	/*
	inline RegisterFile & _registerFile()
	{
	return (*globalRegFileList)[_regFile];
	}
	*/
	
public:
	RealRegArg(char *regName = _NULL_, int regNum = _INVALID_, int regFile = _INVALID_) ;

	virtual ~RealRegArg();

	BaseArgument *copy();
	
	BaseArgument *copyBody();
	
#ifdef COUNT_REALREGS
	static int numRealRegs()
	{
		return _numRealRegs;
	}
#endif
 
 
 int isRealRegArg()
 {
	 return _YES_;
}
   
 virtual int isEq(BaseArgument *arg);

   friend const int operator == (RealRegArg & a,RealRegArg & b)
      {
			if (a._regFile != b._regFile)
				return _NO_;
			if(strcmp(a.regFilename(),b.regFilename())!=0)
				return _NO_;
	 
			// As of now, if the two real regs do not have the same
			// register numbers, they are considered to be different hard
			// regs. (assuming they are in the same register file).
			// 
			// If sharing of hard regs is allowed (like in sliding
			// window), then this may require changes.
			//
			if (a._symVal != b._symVal)
				return _NO_;

			BaseArgProperty *prop1,*prop2;
			prop1=a.getPtrToProperty(_SSA_);
	 		prop2=b.getPtrToProperty(_SSA_);
	 		if(prop1==NULL && prop2==NULL) return _YES_;
	 		if(prop1==NULL || prop2==NULL) return _NO_;
	 		if(((SSAArgProperty*)prop1)->getSSANum() == ((SSAArgProperty*)prop2)->getSSANum()) return _YES_;
	 		else
				return _NO_;
      }

   virtual void print();

   /* 
   **** The next two methods are the same as the register arg methods ****

  virtual void printCToString(MyString & s);
   virtual void irToC(FilePointer & fp, int indent);
 
   */

   virtual void printC();
   virtual void print(ostream& out);
   virtual void printC(ostream& out);
   
	    
// registerFileList is a list of register files provided by the
// architecture. _regFile serves as an index into the list.
//
   friend ostream & operator << (ostream & out, const RealRegArg & s)
      {
//	 out << (*globalRegFileList)[s._regFile] << endl;
	 out << "Register Number: " << s._symVal;
	 
	 return out;
 }
 
 void setRegFile(int regFileIndex)
 {
	 _regFile = regFileIndex;
 }
 
 const int regFileIndex()
 {
	 return _regFile;
 }
 
 const int regNumber()
 {
	 return _symVal;
 }
 
 int getIDWithoutSSA();

 int getUniqID();

 // type is one of:
 // _INT_, _FLOAT_
 //
 int getValType();

 // change reg for Thumb set
 void changeReg(char *rName, int rNum, int rFile);
 
 const char *regFilename()
 {
	 // perform lookup in the globalRegFileList and return the name
	 // of the register file.
  
	 return _regText;
      }
   
   const char *regName()
      {
	 // gather the register File name, then concatenate the
	 // register number in an appropriate manner.
	   
	 char *buffer;
	 buffer=new char[strlen(regFilename())+20];
	 sprintf(buffer,"%s%d",regFilename(),regNumber());
	 return buffer;
      }
   
   virtual int isIntegerReg();
   
   virtual int isFloatReg();

   virtual expr BuildExpr(BaseOperation *o);
   
   virtual char *Name(void);
   
   virtual NODEPTR_TYPE BuildIburgTree(BaseOperation *op,int essential);
   
   void changeReg(BaseArgument *arg);


};

#endif

// Comment 1:
//
// _numRealRegs is used to keep a count of the number of RealRegArg
// objects in existence at any point. COUNT_REALREGS can be either
// defined or undefined in the file "ArgumentIncludes.h". The count
// information will be used for debugging (and/or statistical)
// purposes. It might be a good idea to allow the user to specify
// whether he needs this info. 
//



