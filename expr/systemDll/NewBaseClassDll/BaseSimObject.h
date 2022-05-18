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
// BaseSimObject.h: interface for the BaseSimObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASESIMOBJECT_H__4010017E_B031_11D2_BBB6_00C04FB17504__INCLUDED_)
#define AFX_BASESIMOBJECT_H__4010017E_B031_11D2_BBB6_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//class SIMGraphObj;
#include "stdafx.h"
#include "../../pcProDll/win32/GraphNode.h"
#include "../../pcProDll/win32/MouseHitInfoStruct.h"

#include "../../pcProGUI\Resource.h"
#include "../../pcProGUI\Prop\BasePropPage.h"

//class BaseSimObject;

//#include "../../pcProGUI/SIM/SIMGraphObj.h"

#include "StatList.h"
#include "ArchClassIncludes.h"

class BaseSimObject  
{

public:
	BaseSimObject();
	virtual ~BaseSimObject();
	
	virtual int is(ClassType ctype){ return _NO_; }
	virtual int is(StorageClassType ctype){ return _NO_; }
	virtual int is(PortType ctype){ return _NO_; }
	virtual int is(ConnectionType ctype){ return _NO_; }

/******** Statistics Collection **********************************/

protected:
	StatList _statList; // list of stats maintained by the SimObject.

public:
	virtual void addStat(StatType stat, StatParams parmPtr = NULL);
	virtual StatList & getStatList();

protected:
	
	/**************************************/
	/**** Stat related methods ************/
	
	virtual void resetAllStats();
	virtual void resetStat(StatType a);

	virtual void doStat(StatType a);
	virtual void doStat(StatType a, ParamList parm);

	// The next method calls the default stat update method for 
	// each stat in the statList.
	//
	// Note: The default stat update is the operator () without
	// any parameters (i.e. a vector<int> a.k.a. ParamList)
	//
	virtual void doStats();

	/**************************************/

/******** Statistics Collection **********************************/

};

#endif // !defined(AFX_BASESIMOBJECT_H__4010017E_B031_11D2_BBB6_00C04FB17504__INCLUDED_)
