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
// $Id: ControlOp.h,v 1.3 1998/01/22 23:58:25 pgrun Exp $
//
// File:  		ControlOp.h
// Created:		Mon Oct 28, 96
// Last modified: 	Thu Oct 31, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#ifndef __CONTROLOP_H__
#define __CONTROLOP_H__

#include "NormalOp.h"
#include "BaseArgument.h"


class ControlOp : public NormalOp 
{
 private:

#ifdef COUNT_CONTROLOPS
   static int _numControlOps;	// See comment 1 below.
#endif
      
/*      BaseArgument *_ctrlOperand;  */
   
 protected:
 public:

	 // 1. Constructor
   ControlOp()
      {
#ifdef COUNT_CONTROLOPS
	 _numControlOps++;	
#endif
      }





   // 2. Destructor
   virtual ~ControlOp()
      {
#ifdef COUNT_CONTROLOPS
	 _numControlOps--;
#endif
      } 





   // 3. counter
#ifdef COUNT_CONTROLOPS
   static int numControlOps()
      {
	 return _numControlOps;
      }
#endif

/*
   BaseArgument *getOperand()
    {
      return _ctrlOperand;
    }
*/




   // 4. Identity
   int isControlOp()
      {
	 return _YES_;
      }






   // 5. operator ==
   friend int operator == (const ControlOp & a, const ControlOp & b)
      {
	 return _YES_;
      }
   





   // 6. operator << 
   friend ostream & operator << (ostream & out, const ControlOp & s)
      {
	 return out;
      }




   // Some specific operations
  virtual int percMoveCJ(Routine* routine);

  virtual int percMigrate(Routine* routine);


  NODEPTR_TYPE BuildIburgTree(int rootOfTree, int essential);

};

//Comment 0:
//
// This will also be an abstract class. (because we will not be
//  redefining the initializeID method)

#endif

// Comment 1:
//
// _numControlOps is used to keep a count of the number of ControlOp objects
// in existence at any point. COUNT_CONTROLOPS can be either defined or
// undefined in the file "OperationIncludes.h". The count information
// will be used for debugging (and/or statistical) purposes. It might
// be a good idea to allow the user to specify whether he needs this
// info. 
// Since this is an abstract class (see comment 0 above), no ControlOps
// object will be created explicitly. But, whenever an object of any
// class derived from it is created (destroyed), the ControlOp
// constructor (destructor) will be called (which will
// increment/decrement the count).
//
