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
 *file  : DynamicScheduler.cpp                               *
 *created : 5 May,1999                                       *
 *authors : Asheesh Khare                                    *
 *last modified :                                            *
 *                                                           *
 *contents:contains class definition and functions for       *
 *         objects that are needed for DynamicScheduling in  *
 *         R10000 processor                                  *
 *compiled with : VC++                                       *
 *for more info :                                            *
 *************************************************************/
#include "DynamicScheduler.h"
#include "ExecSemSupportFuncs.h"
#include "CompName.h"
#include "NormalInstr.h"
#include "DerivedAddrData.h"
#include "DerivedStorage.h"
#include "ControlTable.h"


/*************************************************************
 *class : BranchPrediction                                   *
 *************************************************************/
void BranchPrediction::read(BaseType & result, long addr)
{
	// THE STANDARD 2 BIT PREDICTOR; OUTPUTS PREDICTED_TAKEN/PREDICTED_NOT_TAKEN BASED ON THE STATE IT IS IN
	MyInt res;
	map<long , PREDICTION_STATE>::iterator i1;
	i1 = _branchHistoryMap.find(addr);
	if (i1 != _branchHistoryMap.end())
	{
		PREDICTION_STATE ps = _branchHistoryMap[addr];
		if ((ps == TAKEN1) || (ps == TAKEN2))
		{
			res._id = _PREDICTED_TAKEN_;
		}
		else
		{
			res._id = _PREDICTED_NOT_TAKEN_;
		}
	}
	else
	{
		_branchHistoryMap[addr] = TAKEN1;
		res._id = _PREDICTED_TAKEN_;
	}
	(MyInt &)result = res;
}

void BranchPrediction::write(BaseType & result, long addr)
{
	// THE STANDARD 2 BIT BRANCH PREDICTOR: STATES ARE TAKEN1, TAKEN2,NOTTAKEN1, NOTTAKEN2
	// inputs is _ASPREDICTED or _MISPREDICTED

	int res = ((MyInt&)result)._id;

	if ((res != _ASPREDICTED_) && (res != _MISPREDICTED_))
	{
		cout << "BranchPrediction::write() - Illegal value being stored" << endl;
		assert(0);
	}

	PREDICTION_STATE ps = _branchHistoryMap[addr];
	
	switch (ps)
	{
	case TAKEN1: 
		
		if (res == _MISPREDICTED_)
		{
			_branchHistoryMap[addr] = TAKEN2;
		}
		break;
		
	case TAKEN2: 
		
		if (res == _MISPREDICTED_)
		{
			_branchHistoryMap[addr] = NOTTAKEN1;
		}
		else
		{
			_branchHistoryMap[addr] = TAKEN1;
		}
		break;
		
	case NOTTAKEN1:

		if (res == _MISPREDICTED_)
		{
			_branchHistoryMap[addr] = NOTTAKEN2;
		}
		break;
		
	case NOTTAKEN2: 
		
		if (res == _MISPREDICTED_)
		{
			_branchHistoryMap[addr] = TAKEN1;
		}
		else
		{
			_branchHistoryMap[addr] = NOTTAKEN1;
		}
		break;
	default:;
	}
}

void BranchPrediction::doUpdate(){}
void BranchPrediction::print(ostream &){}

//BusyRegTable * IntBusyRegTable;
//BusyRegTable * TempIntBusyRegTable;
//BusyRegTable * FltBusyRegTable;
//BusyRegTable * TempFltBusyRegTable;
BranchPrediction BranchPredictor;
//vector<BusyRegTable *> ListOfIntBusyRegTables;
//vector<BusyRegTable *> ListOfFltBusyRegTables;

BusyRegTable * MiscBusyRegTable;
