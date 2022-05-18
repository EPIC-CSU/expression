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
 *file  : IssueUnit.h										 *
 *created : 3 May, 2000.                                     *
 *authors : Prabhat, Srikanth                                *
 *last modified :                                            *
 *                                                           *
 *contents:contains class methods for Unit class & other     *
 *         classes derived from it.                          *
 *compiled with : VC++                                       *
 *for more info :                                            *
 *************************************************************/

#ifndef _ISSUE_UNIT_H_
#define _ISSUE_UNIT_H_

#include "DerivedClassIncludes.h"
#include "Unit.h"
#include "Storage.h"
#include "DerivedLatchData.h"

const int OPER_ISSUED = 999; // if the operation has already been issued
const int OPER_STALLED = 998; // if the operation has been stalled due to some reason
const int OPER_INIT = 997; // operation has this value when it is first stored in the issue buffer

/**********************************************************************************************/
/**********************************************************************************************/
// IMPORTANT #DEFINE FOLLOWS! :-)

// Comment out this #define if you want to use the old version of the issue unit.
// The old version does not support feedback path exploration.

// #define ISSUE_FOR_PIPELINE_EXPLORATION

/**********************************************************************************************/
/**********************************************************************************************/

typedef Vector<int> ListOfFUIndicesDS;
typedef Vector<int>::iterator ListOfFUIndicesDSIter;

typedef Vector<int> ListOfCompIndicesDS;
typedef Vector<int>::iterator ListOfCompIndicesDSIter;

typedef struct
{
	AddrOperand _addr;
	MyBusyElem _elem;
} BusyRegsElem;

typedef Vector<BusyRegsElem> BusyRegsList;
typedef Vector<BusyRegsElem>::iterator BusyRegsListIter;

/* FUandRFConnectivityTable : Connectivity between (and among) FUs and RFs. */
/* Note: This connectivity is only in terms of reading sources */

typedef struct 
{
	int _hasPath; // an entry of 1 indicates that the unit1 (e.g. row index) has a path to unit2 (column index)
} FUandRFConnTableEntry;

typedef Vector<FUandRFConnTableEntry> FUandRFConnRow;

// In the FUandRFConnTable the convention we follow is:
//
// If an entry in the table has _hasPath = 1, then
//		there exists a path from the column component to the row component.
//		i.e. there is either a feedbackpath or a read regfile connection from the column component to
//		the row component.

class FUandRFConnTable
{
private:

	Vector<FUandRFConnRow> _fuAndRFConnTable;

	Map<Unit *, int> _fuToIntMap; // This allows us to index into the table, given a FU.
	Map<Storage *, int> _rfToIntMap; // This allows us to index into the table, given a RF.

	int _tempIndex; //This is a temporary variable that is used to set the unit and storage indices.

public:
	FUandRFConnTable() : _fuAndRFConnTable(), _fuToIntMap(), _rfToIntMap(), _tempIndex(0) { }
	~FUandRFConnTable() { _fuAndRFConnTable.clear(); _fuToIntMap.clear(); _rfToIntMap.clear(); }

	// The next two methods are used to initialize the maps that determine the indices for the FUs and RFs.
	// These methods are to be used in the buildSystem.

	void addFUToMap(Unit *unit);
	void addRFToMap(Storage *stor);

	void createConnTable(); // This method assumes that the _fuToIntMap and _rfToIntMap have been initialized.

	void addPath(int fromIndex, int toIndex);
	void addPath(Unit *fromU, Unit *toU);
	void addPath(Unit *fromU, Storage *toS);
	void addPath(Storage *fromS, Unit *toU);
	void addPath(Storage *fromS, Storage *toS);

	int hasPath(int fromIndex, int toIndex);
	int hasPath(Unit *fromU, Unit *toU);
	int hasPath(Unit *fromU, Storage *toS);
	int hasPath(Storage *fromS, Unit *toU);
	int hasPath(Storage *fromS, Storage *toS);

	int getIndex(Unit *unit) { return ((unit == NULL) ? _INVALID_ : _fuToIntMap[unit]); }
	int getIndex(Storage *stor) { return ((stor == NULL) ? _INVALID_ : _rfToIntMap[stor]); }
};

extern FUandRFConnTable globalFUandRFConnTable;

#endif



