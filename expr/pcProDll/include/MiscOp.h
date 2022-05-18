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
// $Id: MiscOp.h,v 1.4 1998/01/22 23:59:07 pgrun Exp $
//
// File:  		MiscOp.h
// Created:		Fri Nov 22, 96
// Last modified: 	Fri Nov 22, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION

#ifndef __MISCOP_H__
#define __MISCOP_H__

#include "NormalOp.h"

class MiscOp : public NormalOp 
{
 private:

#ifdef COUNT_MISCOPS
   static int _numMiscOps;	// See comment 1 below.
#endif

 protected:
 public:
   MiscOp()
      {
#ifdef COUNT_MISCOPS
	 _numMiscOps++;	
#endif
      }
   
   virtual ~MiscOp()
      {
#ifdef COUNT_MISCOPS
	 _numMiscOps--;
#endif
      } 

#ifdef COUNT_MISCOPS
   static int numMiscOps()
      {
	 return _numMiscOps;
      }
#endif

   int isMiscOp()
      {
	 return _YES_;
      }

   virtual BaseOperation *copyBody(bool copyProp=false)
      {
	 // Not yet implemented.
	    cout << "Panic: CopyBody for MiscOp has not been implemented" << endl;
	 exit(1);
	 return NULL;
      }
   
   friend int operator == (const MiscOp & a, const MiscOp & b)
      {
	 return _YES_;
      }
   
   friend ostream& operator << (ostream & out, const MiscOp & s)
      {
	 return out;
      }
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
// _numMiscOps is used to keep a count of the number of MiscOp objects
// in existence at any point. COUNT_MISCOPS can be either defined or
// undefined in the file "OperationIncludes.h". The count information
// will be used for debugging (and/or statistical) purposes. It might
// be a good idea to allow the user to specify whether he needs this
// info. 
// Since this is an abstract class (see comment 0 above), no MiscOp
// object will be created explicitly. But, whenever an object of any
// class derived from it is created (destroyed), the MiscOp
// constructor (destructor) will be called (which will
// increment/decrement the count).
//
