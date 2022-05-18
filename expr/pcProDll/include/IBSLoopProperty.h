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
// $Id: IBSLoopProperty.h,v 1.5 1998/03/21 01:18:50 ahalambi Exp $
//
// File:  	        IBSLoopProperty.h
// Created:		Fri Nov 22, 96
// Last modified: 	Fri Nov 22, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: LOOP, PROPERTY

// IBS : Index, Bounds, Step.


#ifndef __IBSLOOPPROPERTY_H__
#define __IBSLOOPPROPERTY_H__

#include "GlobalIncludes.h"
#include "BaseLoopProperty.h"
#include "MyLinkedList.h"
#include "BaseArgument.h"

#include "PropertyIncludes.h"

class IBSLoopProperty : public BaseLoopProperty
{
private:
protected:
 MyLinkedList<BaseArgument *> *_indexList;
 //
 
 BaseArgument *_startBoundsPtr;
 BaseArgument *_endBoundsPtr;
 //
 // Pointers to arguments which hold the Bounds value.
 
 BaseArgument *_stepPtr;
 //
 // Pointer to argument which holds the Step Value.
 
public:
 
 IBSLoopProperty();
 IBSLoopProperty(const IBSLoopProperty & a);
 virtual ~IBSLoopProperty();
 
 virtual BaseLoopProperty *copy();
 
 void setIndex(MyLinkedList<BaseArgument *> *aPtr)
 {
	 _indexList = aPtr;
 }
 
 void setStartBounds(BaseArgument *aPtr)
 {
	 _startBoundsPtr = aPtr;
 }
 
 void setEndBounds(BaseArgument *aPtr)
 {
	 _endBoundsPtr = aPtr;
 }
 
 void setStep(BaseArgument *aPtr)
 {
	 _stepPtr = aPtr;
 }
 
 MyLinkedList<BaseArgument *> *getIndex()
 {
	 return _indexList;
 }
 
 BaseArgument *getStartBounds()
 {
	 return _startBoundsPtr;
 }
 
 BaseArgument *getEndBounds()
 {
	 return _endBoundsPtr;
 }
 
 BaseArgument *getStep()
 {
	 return _stepPtr;
 }
 
 virtual void print(ostream& out);
 
 friend const int operator == (const IBSLoopProperty & a,
  const IBSLoopProperty & b);
 
 friend ostream & operator << (ostream & out, const IBSLoopProperty & s);
 
 virtual int is(PropertyNames aName)
 {
	 if (aName == _IBS_) return _YES_;
	 
	 return _NO_;
 }
};

#endif


