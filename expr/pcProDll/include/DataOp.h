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
// $Id: DataOp.h,v 1.8 1998/01/22 23:58:30 pgrun Exp $
//
// File:  		DataOp.h
// Created:		Mon Oct 28, 96
// Last modified: 	Thu Oct 31, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION

#ifndef __DATAOP_H__
#define __DATAOP_H__

#include "NormalOp.h"

class Routine;

class DataOp : public NormalOp 
{
private:
	
#ifdef COUNT_DATAOPS
	static int _numDataOps;	// See comment 1 below.
#endif
	
protected:
public:
	
	// 1. Constructor
	DataOp()
	{
#ifdef COUNT_DATAOPS
		_numDataOps++;	
#endif
	}
	
	
	
	// 2. Destructor
	virtual ~DataOp()
	{
#ifdef COUNT_DATAOPS
		_numDataOps--;
#endif
	} 
	
	
	
	
	// 3. counter
#ifdef COUNT_DATAOPS
	static int numDataOps()
	{
		return _numDataOps;
	}
#endif
	
	
	
	
	
	// 4. Identity
	int isDataOp()
	{
		return _YES_;
	}
	
	
	
	// 5. operator ==
	friend int operator == (const DataOp & a, const DataOp & b)
	{
		return _YES_;
	}
	
	
	// 6. operator <<
	friend ostream & operator << (ostream & out, const DataOp & s)
	{
		return out << (*((NormalOp*) &s));
		
	}
	
	
	// 7. print
	virtual void print()
	{
	}
	
	
	
	// 8. shortPrint
	virtual void shortPrint()
	{
	}
	
	
	
	// 9. some specific functions
	
	virtual int percMoveOP(Routine* routine);
	
	virtual int percMigrate(Routine* routine);
	
	//
	// Rewrite the operators == and << later.
	
};

// Comment 0:
//
// This will also be an abstract class. (because we will not be
//  redefining the initializeID method)

#endif

// Comment 1:
//
// _numDataOps is used to keep a count of the number of DataOp objects
// in existence at any point. COUNT_DATAOPS can be either defined or
// undefined in the file "OperationIncludes.h". The count information
// will be used for debugging (and/or statistical) purposes. It might
// be a good idea to allow the user to specify whether he needs this
// info. 
// Since this is an abstract class (see comment 0 above), no DataOp
// object will be created explicitly. But, whenever an object of any
// class derived from it is created (destroyed), the DataOp
// constructor (destructor) will be called (which will
// increment/decrement the count).
//
