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
// $Id: IBSLoopProperty.cc,v 1.1 1998/03/21 01:16:15 ahalambi Exp $
//
// File:  		IBSLoopProperty.cc
// Created:		Wed Mar 11, 98
// Last modified: 	Wed Mar 11, 98
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: SUPPORT

#include "stdafx.h"

#include "IBSLoopProperty.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

IBSLoopProperty::IBSLoopProperty()
{
 _indexList = _NULL_;
 _startBoundsPtr = _NULL_;
 _endBoundsPtr = _NULL_;
 _stepPtr = _NULL_;
}

IBSLoopProperty::IBSLoopProperty(const IBSLoopProperty & a) :
_indexList(a._indexList), _startBoundsPtr(a._startBoundsPtr),
_endBoundsPtr(a._endBoundsPtr), _stepPtr(a._stepPtr) 
{
}

IBSLoopProperty::~IBSLoopProperty()
{
}

void IBSLoopProperty::print(ostream& out)
{
	// First print the index.
	//
	out << "Index Variables: ";


	if (_indexList != NULL)
	{
		MyLinkedListIterator<BaseArgument *> *iter;
		iter = _indexList->elementsForward();

		while (iter->hasMoreElements())
		{
			if (iter->currentElement() != NULL)
			{
				out << "\t"; (iter->currentElement())->print(out);
			}
			iter->nextElement();
		}
		delete iter;
	}

	out << endl;
	
	// Next print the start and end bounds.
	//
	out << "Loop Bounds : (* ";

	if (_startBoundsPtr != NULL)
	{
		_startBoundsPtr->print(out);
	}

	out << "\t";

	if (_endBoundsPtr != NULL)
	{
		_endBoundsPtr->print(out);
	}

	out << " *)" << endl;

	// Lastly, print the loop step.
	//
	out << "Loop Step: ";

	if (_stepPtr != NULL)
	{
		_stepPtr->print(out);
	}

	out << endl;
}

const int operator == (const IBSLoopProperty & a,
                       const IBSLoopProperty & b)
{
 if ((a._indexList == b._indexList) && 
  (a._startBoundsPtr == b._startBoundsPtr) &&
  (a._endBoundsPtr == b._endBoundsPtr) &&
  (a._stepPtr == b._stepPtr))
  return _YES_;
 
 return _NO_;
}

ostream & operator << (ostream & out, const IBSLoopProperty & s)
{
 return out;
}


BaseLoopProperty *IBSLoopProperty::copy()
{
 BaseLoopProperty *retPtr;
 
 retPtr = new IBSLoopProperty(*this);
 
 return retPtr;
}
