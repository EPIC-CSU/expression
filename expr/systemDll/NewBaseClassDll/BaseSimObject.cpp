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
// BaseSimObject.cpp: implementation of the BaseSimObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "BaseSimObject.h"
#include "ArchStats.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

extern ArchStatsDB globalArchStatsDB;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BaseSimObject::BaseSimObject()
{
 //simGraphObj=(SIMGraphObj*)0;
}

BaseSimObject::~BaseSimObject()
{

}

void BaseSimObject::addStat(StatType stat, StatParams parmPtr)
{
	_statList.addStat(stat, parmPtr);
}

StatList & BaseSimObject::getStatList(){return _statList;}

void BaseSimObject::doStats()
{
	StatTypeList arr;
	_statList.getStatTypeList(arr);
	
	StatTypeList::iterator i;
	
	for (i = arr.begin(); i != arr.end(); i++)
	{
		globalArchStatsDB.execStat(((BaseSimObject *)this), (*i));
	}
}

void BaseSimObject::doStat(StatType a)
{
	globalArchStatsDB.execStat(((BaseSimObject *)this), a);
}

void BaseSimObject::doStat(StatType a, ParamList parm)
{
	globalArchStatsDB.execStat(((BaseSimObject *)this), a, parm);
}

void BaseSimObject::resetAllStats()
{
	_statList.reset();
}

void BaseSimObject::resetStat(StatType a)
{
	_statList.reset(a);
}