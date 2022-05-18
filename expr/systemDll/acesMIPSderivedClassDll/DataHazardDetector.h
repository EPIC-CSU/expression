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
/*************************************************************
 *file  : DataHazardDetector.h                               *
 *authors : ashok   				                         *
 *last modified :                                            *
 *                                                           *
 *contents : contains the storage class definitions  and     *
 *           associated classes.                             *
 *compiled with : g++                                        *
 *for more info :                                            *
 *caution : always grep for TODO to check for incompleteness *
 *************************************************************/
#ifndef _DATAHAZARDDETECTOR_H_
#define _DATAHAZARDDETECTOR_H_

#include "GlobalIncludes.h"
#include "DerivedClassIncludes.h"

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <algorithm>

class DataUnit
{
public:
	AddrOperand _data;
	int	_timeStamp;

	DataUnit(AddrOperand & a, int tStamp) : _data(a), _timeStamp(tStamp) { }
	
	DataUnit(const DataUnit &du){
		_data=du._data;
		_timeStamp=du._timeStamp;
	}

	bool operator == (DataUnit & b)
	{
		if (_data == b._data) return true;
		return false;
	}

	bool operator == (AddrOperand & a)
	{
		if (_data == a) return true;
		return false;
	}

	friend bool operator == (DataUnit a, DataUnit b)
	{
		if (a._data == b._data) return true;
		return false;
	}

	bool operator != (DataUnit & b)
	{
		if (_data == b._data) return false;
		return true;
	}

};

#include <vector>

class DataHazardDetector
{
private:
	static int _clockCount;
	vector<DataUnit> _listOfDests;

	typedef vector<DataUnit>::iterator DataUnitListIterator;

public:

	void addDest(AddrOperand & a)
	{
		DataUnit *du;
		DataUnit b(a, _clockCount);
		_listOfDests.push_back(b); 
		du=&(_listOfDests.back());
	}

	int hazardLength(AddrDataUnion & a)
	{
		if (a.getType() != _ADDRTYPE_) return -1;
		AddrOperand b = a.getAddr();

		DataUnit c(b, -1);
		DataUnitListIterator i1, i2, i3, prev;

//		cout << "List of Dests: " << _listOfDests.size() << endl;

		i2 = _listOfDests.begin();
		i3 = _listOfDests.end();
		prev = i3;

		while (i2 != i3)
		{
			i1 = std::find(i2, i3, c);
			if ((i1 == i3))
				break;
				
			if ((*i1)._timeStamp == _clockCount)
				break;

			i2 = i1;
			i2++;
			prev = i1;
		}
		if (prev == _listOfDests.end()) return -1;
/*		int timeStamp = (*i1)._timeStamp;
		if (_clockCount == timeStamp) // i.e. the dest was just added to the list.
			return -1;
			*/
		int retVal;
		retVal = _clockCount - (*prev)._timeStamp;
		return (retVal);
	}
	
	void deleteDest(AddrOperand & a)
	{
		DataUnit b(a, -1);

		DataUnitListIterator i1;
		i1 = std::find(_listOfDests.begin(), _listOfDests.end(), b);
		if (i1 != _listOfDests.end())
		{
			_listOfDests.erase(i1);
		}
		else // i.e. did not find the destination in the list.
		{
			cout << "Warning: Could not find dest in the data hazard detection unit" << endl;
		}
	}

	void updateClockCount()
	{
		_clockCount++;
	}

	void reset()
	{
		_listOfDests.clear();
	}

	void print(void){
		DataUnitListIterator i2, i3, temp;
		i2 = _listOfDests.begin();
		i3 = _listOfDests.end();
		DataUnit *du;
		AddrOperand *ao;

		cout << "#***************************************************#" << endl;
		for(temp=i2;temp!=i3;temp++){
			du=&(*temp);
			ao=&(du->_data);
			ao->print();
		}
		cout << "#***************************************************#" << endl;
	}
};

#endif
