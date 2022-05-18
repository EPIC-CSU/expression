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
// $Id: FlowOp.h,v 1.8 1998/01/22 23:58:39 pgrun Exp $
//
// File:  		FlowOp.h
// Created:		Mon Oct 28, 96
// Last modified: 	Thu Oct 31, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION

#ifndef __FLOWOP_H__
#define __FLOWOP_H__

#include "NormalOp.h"

class FlowOp : public NormalOp 
{
 private:

#ifdef COUNT_FLOWOPS
   static int _numFlowOps;	// See comment 1 below.
#endif

 protected:
   int _targetMask;

 public:

	 // 1. Constructor
   FlowOp()
      {
#ifdef COUNT_FLOWOPS
	 _numFlowOps++;	
#endif
      clearTargetMask();
      }
   



   // 2. Destructor
   virtual ~FlowOp()
      {
#ifdef COUNT_FLOWOPS
	 _numFlowOps--;
#endif
      } 




    
   // 3. Target Mask 
    void clearTargetMask() 
    {
       _targetMask = 0;
    }

    void setTargetMask(int mask)
    {
       _targetMask = mask;
    }
 
    int getTargetMask()
    {
      return _targetMask;
    }





	// 4. counter
#ifdef COUNT_FLOWOPS
   static int numFlowOps()
      {
	 return _numFlowOps;
      }
#endif






   // 5. Identity
   int isFlowOp()
      {
	 return _YES_;
      }






   // 6. operator ==
   friend int operator == (const FlowOp & a, const FlowOp & b)
      {
	 return _YES_;
      }
   




   // 7. operator <<
   friend ostream & operator << (ostream & out, const FlowOp & s)
      {
	 return out;
      }

	  
	  



 
 
   // 8. Some specific functions
   virtual void setTargetInstr(BaseInstruction *someInstr) = 0;
   
   virtual BaseInstruction *getTargetInstr() = 0;


};

//Comment 0:
//
// This will also be an abstract class. (because we will not be
//  redefining the initializeID method)

#endif

// Comment 1:
//
// _numFlowOps is used to keep a count of the number of FlowOp objects
// in existence at any point. COUNT_FLOWOPS can be either defined or
// undefined in the file "OperationIncludes.h". The count information
// will be used for debugging (and/or statistical) purposes. It might
// be a good idea to allow the user to specify whether he needs this
// info. 
// Since this is an abstract class (see comment 0 above), no FlowOp
// object will be created explicitly. But, whenever an object of any
// class derived from it is created (destroyed), the FlowOp
// constructor (destructor) will be called (which will
// increment/decrement the count).
//
