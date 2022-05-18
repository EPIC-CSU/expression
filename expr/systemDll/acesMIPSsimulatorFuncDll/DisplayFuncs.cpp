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
 *file  : DisplayFuncs.cpp                                   *
 *authors : ashok	                                         *
 *created : Thu Oct 8, 98                                    *
 *last modified : Mon Oct 12, 98                             *
 *                                                           *
 *contents : contains functions that implement simulator     *
 *			 functionality.                                  *
 *compiled with : VC++                                       *
 *for more info : contact Ashok at ahalambi@ics.uci.edu      *
 *caution : always grep for TODO to check for incompleteness *
 *************************************************************/

#ifndef __UNIX__
#include "StdAfx.h"
#else
#ifndef ASSERT_TRACE
#define ASSERT_TRACE(c,e) if (!(c)) { cerr << endl << "WOA, NELLIE! " << __FILE__ << "(" << __LINE__ << "): " << e << endl; assert(0); }
#endif
#endif

#include <fstream.h>
#include "Storage.h"
#include "StatList.h"
#include "Unit.h"
#include "ArchStats.h"
#include "MyString.h"
#include "CompName.h"

//#include "..\derivedClassDll\DerivedClassIncludes.h"
//#include "../derivedClassDll/DerivedStorage.h"

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
extern int getStallCycles();

//extern Storage *globalVirtualRegFile;

extern long cycleCount();

void dumpRegFile()
{
/*
	regFile_doc->Append("Printing Virtual Register File Dump\n");
	regFile_doc->Append("-----------------------------------\n");
	regFile_doc->Append("\n");
*/
//	globalVirtualRegFile->dump();
/*
	regFile_doc->Append("\n");
	regFile_doc->Append("-----------------------------------\n");
	regFile_doc->Append("\n");
*/
}


void dumpFUs()
{
}

int getNumTimes(Unit *unit)
{
/*	BaseStat *bStat = (unit->getStatList()).getStat(_NUMTIMES_);
	if (bStat->is(_NUMTIMES_))
	{
		cout << "Is NUMTIMES" << endl;
		int t = ((NumTimesStat *)bStat)->getNumTimes();
		return t;
	}
*/
	return 0;
}

void displayStats()
{
	//cout << endl << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
	//cout << "\tTotal Number of cycles: " << cycleCount() << endl;
	cout << endl << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
	cout << "\t Power Stats: " << endl;
	cout << endl << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
	globalArchStatsDB.printUnitStats(cout);

	fstream * fpDetailedPerf = new fstream("DetailedPerf.txt",ios::app);
	globalArchStatsDB.printUnitStats(*fpDetailedPerf);
	fpDetailedPerf->close();


	/*FILE * fp = fopen("Perf.txt","a");

	//MyString Decode("Control");
	//Unit * decode = (Unit *)getComp(Decode);

	//BaseStat *statPtr;
	//statPtr = (globalArchStatsDB.unitStatsDB).getStat(decode,_NUMTIMES_);
	//fprintf(fp," %9d ",statPtr->getNumTimes());

	fprintf(fp," %9d ",clock_count);

	//statPtr = (globalArchStatsDB.unitStatsDB).getStat(decode,_HAZARDSTALL_);
	//fprintf(fp," %9d\n ",((HazardStallStat*)statPtr)->getNumStalls());
	fprintf(fp," %9d ",getStallCycles());

	fclose(fp);
	*/

/*
	printf("\t\t IALU1 usage: %d\n", getNumTimes(IALU1));
	printf("\t\t IALU2 usage: %d\n", getNumTimes(IALU2));
	printf("\t\t  FALU usage: %d\n", getNumTimes(FALU));
	printf("\t\t   BR1 usage: %d\n", getNumTimes(BR1));
	printf("\t\t   BR2 usage: %d\n", getNumTimes(BR2));
	printf("\t\t  LDST usage: %d\n", getNumTimes(LDST));
	printf("\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
*/
}

void displayAtEndOfSimulation()
{
	displayStats();
}
