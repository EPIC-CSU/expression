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
 *file  : StatList.cpp                                       *
 *authors : ashok				                             *
 *created : Thu Oct 8, 98                                    *
 *last modified : Wed Jun 15, 98                             *
 *                                                           *
 *compiled with : VC++                                       *
 *for more info :                                            *
 *************************************************************/

#include "StatList.h"

StatParams StatList::getStatParams(StatType a)
{
	_StatArrayIter i;
	if ((i = _statArray.find(a)) != _statArray.end())
	{
		return ((*i).second);
	}
	return NULL;
}

void StatList::getStatTypeList(StatTypeList& arr)
{
	_StatArrayIter i;

	for (i = _statArray.begin(); i != _statArray.end(); i++)
	{
		arr.push_back((*i).first);
	}
}

void StatList::reset(StatType a)
{
	_StatArrayIter i;
	if (a == _ALLSTAT_)
	{
		for (i = _statArray.begin(); i != _statArray.end(); i++)
		{
			if ((*i).second != NULL)
			{
				delete (*i).second;
				(*i).second = NULL;
			}
		}
	}
	else
	{
		if ((i = _statArray.find(a)) != _statArray.end())
		{
			if ((*i).second != NULL)
			{
				delete (*i).second;
				(*i).second = NULL;
			}
		}
		else
		{
			// error condition
			//
			cout << "Warning: Tried to reset non-existant stat" << endl;
		}
	}
}

static BaseStat *getNewStat(StatType st)
{
	BaseStat *retPtr = NULL;
	if (st == _NUMTIMES_) retPtr = new NumTimesStat();
	if (st == _HAZARDSTALL_) retPtr = new HazardStallStat();
	if (st == _TRACKNUMTIMES_) retPtr = new TrackNumTimesStat();

	return retPtr;
}

