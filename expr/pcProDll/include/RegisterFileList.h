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
// $Id: RegisterFileList.h,v 1.10 1998/03/27 00:01:20 wtang Exp $
//
// File:  		RegisterFileList.h
// Created:		Tue Nov 19 , 96
// Last modified: 	Tue Nov 19 , 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARCHITECTURE

#ifndef __REGISTERFILELIST_H__
#define __REGISTERFILELIST_H__

// Assuming that the number of Register files is not known in advance.
// If this assumption is changed (most likely), then we can create a
// static array to hold the register files.

#include "stdafx.h"
#include "ArchitectureIncludes.h"
#include "RegisterFile.h"
#include "Array.h"
#include "ArrayIterator.h"

#include <vector>

#ifdef vector
#undef vector
#endif 
#define vector std::vector

#ifndef ASSERT_TRACE
#define ASSERT_TRACE(c,e) if (!(c)) { cerr << endl << "WOA, NELLIE! " << __FILE__ << "(" << __LINE__ << "): " << e << endl; assert(0); }
#endif

const int REG_START_NUM = 0;

typedef vector<RegisterFile> RegFileVectorType;

class RegisterFileList
{
private:
protected:
	Array<RegisterFile> *_regFileList;
	//
	// Creating a register file list of size 0 and increment capacity 1.
	// i.e initially assume that we have zero register files and
	// that the minimum increment to the list is 1 register file.  
	
	static int _totNumOfRegisters;
	
public:
	
	RegisterFileList(int startSize = 0);
	
	~RegisterFileList()
	{
		_regFileList->removeAllElements();
	}
	
	// See comment for the method below.
	//
	void createRegFile(char *name, int numRegs, int valType, int genericOrTarget = _GENERIC_);
	
	
	// Call this function to add a register file to the list.
	// In the beginning, we are going to create this list from the target
	// specification. 
	//
	// Note that we are calling appendElement rather than insertElement,
	// because we know that our table will not be having any blank slots
	// in the middle. i.e. once a register file has been added, it will
	// not be removed (normally).
	//
	void addRegFile(RegisterFile & regFile)
	{
		regFile.setID(_totNumOfRegisters + REG_START_NUM);
		_totNumOfRegisters += regFile.size();
		
		_regFileList->appendElement(regFile);
		
	}
	
	// This function is needed to initialize the RealRegisterArguments.
	//
	int getIndex(char *regFileName)
	{
		ArrayIterator<RegisterFile> *regFileIter;
		if (_regFileList != NULL)
		{
			regFileIter = _regFileList->elements();
			
			while (regFileIter->hasMoreElements())
			{
				if ((regFileIter->currentElement()) == regFileName){
					int temp = regFileIter->currentIndex();
					delete regFileIter;
					return temp;
				}
				regFileIter->nextElement();
			}
			delete regFileIter;
		}
		
		return 0;
		//
		// Should actually generate an exception. (NoSuchElement
		//					    Exception) 
	}
	
	// Note: It would be very ineffecient to call this method every time
	// to get the ID of a particular register.
	// See the next method "getBaseID" on how to do it efficiently.
	//
	int getID(int regNum ,char *regFileName)
	{ 
		ArrayIterator<RegisterFile> *regFileIter;
		if (_regFileList != NULL)
		{
			regFileIter = _regFileList->elements();
			
			while (regFileIter->hasMoreElements())
			{
				if ((regFileIter->currentElement()) == regFileName){
					int temp = ((regFileIter->currentElement()).getID(regNum));
					delete regFileIter;
					return temp;
				}	     
				regFileIter->nextElement();
			}
			delete regFileIter;
		}
		
		return 0;
		//
		// Should actually generate an exception. (NoSuchElement
		//					    Exception) 
		
	}
	
	// getBaseID :
	// return the Base ID of the register file (which is identified
	//					 	by a name)
	//
	// Note: If a particular register has name "regName" and number
	// "regNumber", its ID is : getBaseID(regName) + regNumber. Also
	// note that regName is actually the name of a register File. Thus,
	// if the IDs of a large number of registers are needed. Keep a
	// table of baseIDs for each register file and then add the
	// register number to the baseID.
	//
	int   getBaseID(char *regFileName)
	{
		ArrayIterator<RegisterFile> *regFileIter;
		regFileIter = _regFileList->elements();
		
		while (regFileIter->hasMoreElements())
		{
			if ((regFileIter->currentElement()) == regFileName){
				int temp = ((regFileIter->currentElement()).getID());
				delete regFileIter;
				return temp;
			}
			regFileIter->nextElement();
		}
		
		return 0;
		//
		// Must be changed to an exception throw.
		
	}
	
	int getBaseID(int regFileIndex)
	{
		return ((_regFileList->elementAt(regFileIndex)).getID());
	}
	
	BaseArgument *getRealReg(int regID)
	{
		ArrayIterator<RegisterFile> *regFileIter;
		regFileIter = _regFileList->elements();
		
		int actRegNum, index;
		
		while (regFileIter->hasMoreElements())
		{
			if ((regFileIter->currentElement()).isValidID(regID))
			{
				actRegNum = regID - (regFileIter->currentElement()).getID();
				index = regFileIter->currentIndex();
				BaseArgument *temp=((regFileIter->currentElement()).createRealReg(index, actRegNum));
				delete regFileIter;
				return temp;
			}
			regFileIter->nextElement();
		}
		
		delete regFileIter;
		
		// Throw exception. regID is not a vaild ID
		
		//NOS
		ASSERT_TRACE(0,"Forced EXIT!");
		return NULL;
	}
	
	RegisterFile & getRegFile(int regID)
	{
		ArrayIterator<RegisterFile> *regFileIter;
		regFileIter = _regFileList->elements();
		
		RegisterFile *retReg=NULL;
		
		while (regFileIter->hasMoreElements())
		{ 
			if ((regFileIter->currentElement()).isValidID(regID))
			{ 
				retReg = &(regFileIter->currentElement());
				delete regFileIter;
				return (*retReg);
			}
			regFileIter->nextElement();
		}
		delete regFileIter;
		
		// error condition.
		cout << "Panic: getRegFile recieved an invalid  register ID" << endl;
		assert(0);
		return *retReg;
	}
	
	RegisterFile & operator [] (const int index)
	{
		try
		{
			return _regFileList->elementAt(index);
		}
		catch (ArrayOutOfBoundsException r)
		{
			cout << "Panic: Invalid Register File Index" << endl;
			cout << r;
		}
		catch (NoSuchElementException r)
		{
			cout << "Panic: No such Register File exists";
			cout << r;
			cout << endl;
		}
		assert(0);
		return _regFileList->elementAt(0);
	}
	
	static int totNumRegs()
	{
		return _totNumOfRegisters;
	}
	
	ArrayIterator<RegisterFile> *getRegFileListIter()
	{
		if (_regFileList == _NULL_) return NULL;
		
		return _regFileList->elements();
	}
	
	void print(ostream & out, int regID)
	{
		ArrayIterator<RegisterFile> *regFileIter;
		regFileIter = _regFileList->elements();
		
		int actRegNum;
		
		while (regFileIter->hasMoreElements())
		{
			if ((regFileIter->currentElement()).isValidID(regID))
			{
				actRegNum = regID - (regFileIter->currentElement()).getID();
				(regFileIter->currentElement()).print(out);
				out << actRegNum << " ";
				delete regFileIter;
				return;
			}
			regFileIter->nextElement();
		}  
		delete regFileIter; 
	}
	

	// Functions for generic and target regFile

	// get all the generic register files
	void getAllGenericRFs(RegFileVectorType &RegFileVector)
	{
		ArrayIterator<RegisterFile> *regFileIter;
		regFileIter = _regFileList->elements();
		
		while (regFileIter->hasMoreElements())
		{ 
			RegisterFile currRegFile = regFileIter->currentElement();
			if (currRegFile.isGenericOrTarget() == _GENERIC_)
			{ 
				// add to the vector RegFileVector
				RegFileVector.push_back(currRegFile);
			}
			regFileIter->nextElement();
		}
		delete regFileIter;
		return;
	}
	
	// get all target register files
	void getAllTargetRFs(RegFileVectorType &RegFileVector)
	{
		ArrayIterator<RegisterFile> *regFileIter;
		regFileIter = _regFileList->elements();
		
		while (regFileIter->hasMoreElements())
		{ 
			RegisterFile currRegFile = regFileIter->currentElement();
			if (currRegFile.isGenericOrTarget() == _TARGET_)
			{ 
				// add to the vector RegFileVector
				RegFileVector.push_back(currRegFile);
			}
			regFileIter->nextElement();
		}
		delete regFileIter;
		return;
	}
};

extern RegisterFileList *globalRegFileList;

#endif


