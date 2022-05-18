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
// $Id$
//
// File:  		ValuesSET.h
// Created:		Thu Jul 23, 98
// Last modified: 	Thu Jul 23, 98
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: INSTRUCTION

#ifndef __VALUESSET_H__
#define __VALUESSET_H__

#include "GlobalIncludes.h"
#include <assert.h>
#include "PropertyIncludes.h"
#include "RegisterArg.h"
#include "RegisterFileList.h"
#include "STLIncludes.h"

#ifdef WIN32
	#define STLCopy std::copy
#else
	#define STLCopy copy
#endif

typedef map<int, int, less<int> > ValSET;
typedef ValSET::iterator ValSETIterator;

extern RegisterFileList *globalRegFileList;

#ifdef WIN32
#endif

class ValuesSET
{
private:
	ValSET _set;

	inline int getIndex(BaseArgument *arg)
	{
		int regNum = ((RegisterArg *)arg)->getVal();
		char *regText = ((RegisterArg *)arg)->getText();

		if (globalRegFileList == NULL) 
	    {
	       cout << "Panic: No global register file list." << endl;
	       assert(0);
	    }
	 
		int index = globalRegFileList->getID(regNum, regText);
		return index;
	}

	inline int getSSANum(BaseArgProperty *ssa)
	{
		int ssaNum = ((SSAArgProperty*)ssa)->getSSANum();
		return ssaNum;
	}

public:
	ValuesSET(){}
	~ValuesSET(){}

	void reset()
	{
		_set.erase(_set.begin(), _set.end());
	}

	void addEntry(BaseArgument *arg)
	{
		if (arg == NULL)
		{
			// error condition.
			cout << "Panic: recieved a null argument to add to ValuesSET" << endl;
			assert(0);
		}
		if (arg->isRegisterArg() == _NO_)
		{
			// error condition.
			cout << "Panic: recieved a non register argument to add to ValuesSET" << endl;
			assert(0);
		}
		int index = getIndex(arg);
		int ssaNum = getSSANum(arg->getPtrToProperty(_SSA_));

		_set[index] = ssaNum;
		//
		// Note this operator '[]' takes care of the situation when index was not previously
		// stored.
	}

	int getEntry(BaseArgument *arg)
	{
		if (arg == NULL)
		{
			// error condition.
			cout << "Panic: recieved a null argument to add to ValuesSET" << endl;
			assert(0);
		}
		if (arg->isRegisterArg() == _NO_)
		{
			// error condition.
			cout << "Panic: recieved a non register argument to getEntry in ValuesSET" << endl;
			assert(0);
		}
		int index = getIndex(arg);

		ValSETIterator iter;

		if ((iter = _set.find(index)) == _set.end())
		{
			// error. did not find the element in the set.
//			cout << "Warning: could not find the element corresponding to index in ValuesSet" << endl;
			return -1;
		}
		return iter->second;
	}

	void setAsUnion(ValuesSET & a, ValuesSET & b)
	{
		int index, ssaNum;

		_set = a._set;

		ValSETIterator i1 = b._set.begin();
		ValSETIterator i2 = b._set.end();
		ValSETIterator iter;

		while (i1 != i2)
		{
			index = i1->first;
			ssaNum = i1->second;

			if ((iter = _set.find(index)) == _set.end())
			{
				// did not find the element. add it to _set.
				_set.insert(ValSET::value_type(index, ssaNum));
			}
			else
			{
				// found a match. Do a check to see if the ssaNumbers are the same.
				// If they aren't then flag an error.

				if (ssaNum != iter->second)
				{
					// error condition.
					cout << "Panic: recieved two different ssaNumbers for the same index while trying to get union" << endl;
					assert(0);
				}
			}
			i1++;
		}
	}
	
	// See coment for next method.
	//
	void impose(ValSETIterator i)
	{
		int index, ssaNum;
		index = i->first;
		ssaNum = i->second;

		ValSETIterator iter;

		if ((iter = _set.find(index)) == _set.end())
		{
		 	// did not find the element. add it to _set.
			_set.insert(ValSET::value_type(index, ssaNum));
		}
		else
		{
			// found a match. Do a check to see if the ssaNumbers are the same.
			// If they aren't then impose it on _set.

			if (ssaNum != iter->second)
			{
				iter->second = ssaNum;
			}
		}
	}

	// the next method imposes "a" onto "this".
	// i.e. if "this" and "a" have the same index, but diff. data, then "this" gets set to a.data
	//
	void impose(ValuesSET & a)
	{
		ValSETIterator i1 = a._set.begin();
		ValSETIterator i2 = a._set.end();
		ValSETIterator iter;

		while (i1 != i2)
		{
			impose(i1);
			i1++;
		}
	}

	// fill "this" object with the indexes present in a and not present in b.
	//
	void setAsIndexDiff(ValuesSET & a, ValuesSET & b)
	{
		int index, ssaNum;

		ValSETIterator i1 = a._set.begin();
		ValSETIterator i2 = a._set.end();

		while (i1 != i2)
		{
			index = i1->first;
			ssaNum = i1->second;

			if (b._set.find(index) == b._set.end())
			{
				// did not find the element in b. add it to _set.
				_set.insert(ValSET::value_type(index, ssaNum));
			}
			i1++;
		}
	}

	// fill "this" object with entries s.t. indexes are present in a and b.
	// but the data (ssaNum) is different. Store a.index, a.data
	//
	void setAsDataDiff(ValuesSET & a, ValuesSET & b)
	{
		int index, ssaNum;

		ValSETIterator i1 = a._set.begin();
		ValSETIterator i2 = a._set.end();
		ValSETIterator iter;

		while (i1 != i2)
		{
			index = i1->first;
			ssaNum = i1->second;

			if ((iter = b._set.find(index)) == b._set.end())
			{
				// did not find the element in b. ignore it.
			}
			else 
			{
				// found the index in b. check to see if ssaNumbers match.
				if (iter->second != ssaNum)
				{
					// nums do not match. 
					_set.insert(ValSET::value_type(index, ssaNum));
				}
			}
			i1++;
		}
	}

	ValuesSET & operator = (ValuesSET & b)
	{
		reset();

		ValSETIterator i = _set.begin();

		insert_iterator< ValSET > ins(_set, i);

		STLCopy(b._set.begin(), b._set.end(), ins);
		return *this;
	}

	ValSETIterator begin()
	{
		return _set.begin();
	}

	ValSETIterator end()
	{
		return _set.end();
	}

	// SAH, Tue, Dec 02 98: find is #define to std::find. So, I changed find to my_find.
	//
	ValSETIterator find(int index)
	{
		return _set.find(index);
	}

	ostream & print(ostream & out)
	{
	//	out << " Printing contents of ValSet " << endl;
		ValSETIterator i = _set.begin();
		ValSETIterator j = _set.end();
		while (i != j)
		{
			out << "\t" << "Index = " << i->first << " \t" << " Data = " << i->second << endl;
			i++;
		}
		return out;
	}

	int isEmpty()
	{
		if (_set.empty())
			return _YES_;
		return _NO_;
	}
};
#endif
