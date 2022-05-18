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
 *file  : ArchStats.h                                        *
 *authors : ashok				                             *
 *created : Wed Jan 27, 99                                   *
 *last modified : Wed Jan 27, 99                             *
 *                                                           *
 *compiled with : VC++                                       *
 *for more info :                                            *
 *************************************************************/

#ifndef _ARCHSTATS_H_
#define _ARCHSTATS_H_

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "StatList.h"
#include "Unit.h"
#include "Latch.h"
#include "Storage.h"
#include "Port.h"
#include "Connection.h"

extern BaseStat *getNewStat(StatType st);

class ArchStatsDB
{
public:
	StatsCollector<Unit> unitStatsDB;
	StatsCollector<Latch> latchStatsDB;
	StatsCollector<Storage> storageStatsDB;
	StatsCollector<Port> portStatsDB;
	StatsCollector<Connection> connectionStatsDB;

	StatsCollector<Unit> trackUnitStatsDB;
	StatsCollector<Latch> trackLatchStatsDB;
	StatsCollector<Storage> trackStorageStatsDB;
	StatsCollector<Port> trackPortStatsDB;
	StatsCollector<Connection> trackConnectionStatsDB;

	BaseStat *addStat(Unit *key, StatType st, GUI_IDType guiObj = NULL)
	{
		BaseStat *newStat = getNewStat(st);
		newStat->setGUIID(guiObj);

		if (newStat->isCumulativeStat())
			unitStatsDB.addStat(key, newStat);
		else
			trackUnitStatsDB.addStat(key, newStat);

		return newStat;
	}

	BaseStat *addStat(Latch *key, StatType st, GUI_IDType guiObj = 0)
	{
		BaseStat *newStat = getNewStat(st);
		newStat->setGUIID(guiObj);

		if (newStat->isCumulativeStat())
			latchStatsDB.addStat(key, newStat);
		else
			trackLatchStatsDB.addStat(key, newStat);

		return newStat;
	}

	BaseStat *addStat(Storage *key, StatType st, GUI_IDType guiObj = 0)
	{
		BaseStat *newStat = getNewStat(st);
		newStat->setGUIID(guiObj);

		if (newStat->isCumulativeStat())
			storageStatsDB.addStat(key, newStat);
		else
			trackStorageStatsDB.addStat(key, newStat);

		return newStat;
	}

	BaseStat *addStat(Port *key, StatType st, GUI_IDType guiObj = 0)
	{
		BaseStat *newStat = getNewStat(st);
		newStat->setGUIID(guiObj);

		if (newStat->isCumulativeStat())
			portStatsDB.addStat(key, newStat);
		else
			trackPortStatsDB.addStat(key, newStat);

		return newStat;
	}

	BaseStat *addStat(Connection *key, StatType st, GUI_IDType guiObj = 0)
	{
		BaseStat *newStat = getNewStat(st);
		newStat->setGUIID(guiObj);

		if (newStat->isCumulativeStat())
			connectionStatsDB.addStat(key, newStat);
		else
			trackConnectionStatsDB.addStat(key, newStat);

		return newStat;
	}

	BaseStat *addStat(BaseSimObject *key, StatType st, GUI_IDType guiObj = 0)
	{
		BaseStat *newStat = getNewStat(st);
		newStat->setGUIID(guiObj);

		if (newStat->isCumulativeStat())
		{
			if (key->is(_UNIT_)) unitStatsDB.addStat((Unit *)key, newStat);
			else if (key->is(_LATCH_)) latchStatsDB.addStat((Latch *)key, newStat);
			else if (key->is(_STORAGE_)) storageStatsDB.addStat((Storage *)key, newStat);
			else if (key->is(_PORT_)) portStatsDB.addStat((Port *)key, newStat);
			else if (key->is(_CONNECTION_)) connectionStatsDB.addStat((Connection *)key, newStat);
		}
		else
		{
			if (key->is(_UNIT_)) trackUnitStatsDB.addStat((Unit *)key, newStat);
			else if (key->is(_LATCH_)) trackLatchStatsDB.addStat((Latch *)key, newStat);
			else if (key->is(_STORAGE_)) trackStorageStatsDB.addStat((Storage *)key, newStat);
			else if (key->is(_PORT_)) trackPortStatsDB.addStat((Port *)key, newStat);
			else if (key->is(_CONNECTION_)) trackConnectionStatsDB.addStat((Connection *)key, newStat);
		}

		return newStat;
	}


	BaseStat *getStat(BaseSimObject *key, StatType st)
	{
		if (key->is(_UNIT_)) return unitStatsDB.getStat((Unit *)key, st);
		else if (key->is(_LATCH_)) return latchStatsDB.getStat((Latch *)key, st);
		else if (key->is(_STORAGE_)) return storageStatsDB.getStat((Storage *)key, st);
		else if (key->is(_PORT_)) return portStatsDB.getStat((Port *)key, st);
		else if (key->is(_CONNECTION_)) return connectionStatsDB.getStat((Connection *)key, st);

		return NULL;
	}
	
	StatsRow & getStatList(BaseSimObject *key)
	{
		if (key->is(_UNIT_)) return unitStatsDB.getStatList((Unit *)key);
		else if (key->is(_LATCH_)) return latchStatsDB.getStatList((Latch *)key);
		else if (key->is(_STORAGE_)) return storageStatsDB.getStatList((Storage *)key);
		else if (key->is(_PORT_)) return portStatsDB.getStatList((Port *)key);
		else if (key->is(_CONNECTION_)) return connectionStatsDB.getStatList((Connection *)key);

		cout << "Panic: Recieved an invalid component type" << endl;

		assert(0);
		return unitStatsDB.getStatList((Unit *)key);
	}

	StatsRow & getTrackStatList(BaseSimObject *key)
	{
		if (key->is(_UNIT_)) return trackUnitStatsDB.getStatList((Unit *)key);
		else if (key->is(_LATCH_)) return trackLatchStatsDB.getStatList((Latch *)key);
		else if (key->is(_STORAGE_)) return trackStorageStatsDB.getStatList((Storage *)key);
		else if (key->is(_PORT_)) return trackPortStatsDB.getStatList((Port *)key);
		else if (key->is(_CONNECTION_)) return trackConnectionStatsDB.getStatList((Connection *)key);

		cout << "Panic: Recieved an invalid component type" << endl;

		assert(0);
		return unitStatsDB.getStatList((Unit *)key);
	}

	void execStat(BaseSimObject *key, StatType st)
	{
		if (key->is(_UNIT_)) unitStatsDB.execStat((Unit *)key, st);
		else if (key->is(_LATCH_)) latchStatsDB.execStat((Latch *)key, st);
		else if (key->is(_STORAGE_)) storageStatsDB.execStat((Storage *)key, st);
		else if (key->is(_PORT_)) portStatsDB.execStat((Port *)key, st);
		else if (key->is(_CONNECTION_)) connectionStatsDB.execStat((Connection *)key, st);
	}

	void execStat(BaseSimObject *key, StatType st, ParamList parm)
	{
		if (key->is(_UNIT_)) unitStatsDB.execStat((Unit *)key, st, parm);
		else if (key->is(_LATCH_)) latchStatsDB.execStat((Latch *)key, st, parm);
		else if (key->is(_STORAGE_)) storageStatsDB.execStat((Storage *)key, st, parm);
		else if (key->is(_PORT_)) portStatsDB.execStat((Port *)key, st, parm);
		else if (key->is(_CONNECTION_)) connectionStatsDB.execStat((Connection *)key, st, parm);
	}

	void execTrackStat(BaseSimObject *key, StatType st)
	{
		if (key->is(_UNIT_)) trackUnitStatsDB.execStat((Unit *)key, st);
		else if (key->is(_LATCH_)) trackLatchStatsDB.execStat((Latch *)key, st);
		else if (key->is(_STORAGE_)) trackStorageStatsDB.execStat((Storage *)key, st);
		else if (key->is(_PORT_)) trackPortStatsDB.execStat((Port *)key, st);
		else if (key->is(_CONNECTION_)) trackConnectionStatsDB.execStat((Connection *)key, st);
	}

	void execTrackStat(BaseSimObject *key, StatType st, ParamList parm)
	{
		if (key->is(_UNIT_)) trackUnitStatsDB.execStat((Unit *)key, st, parm);
		else if (key->is(_LATCH_)) trackLatchStatsDB.execStat((Latch *)key, st, parm);
		else if (key->is(_STORAGE_)) trackStorageStatsDB.execStat((Storage *)key, st, parm);
		else if (key->is(_PORT_)) trackPortStatsDB.execStat((Port *)key, st, parm);
		else if (key->is(_CONNECTION_)) trackConnectionStatsDB.execStat((Connection *)key, st, parm);
	}

	void execAllTrackStats()
	{
		trackUnitStatsDB.execAllStats();
		trackLatchStatsDB.execAllStats();
		trackStorageStatsDB.execAllStats();
		trackPortStatsDB.execAllStats();
		trackConnectionStatsDB.execAllStats();
	}

	void resetAllStats()
	{
		unitStatsDB.resetAllStats();
		latchStatsDB.resetAllStats();
		storageStatsDB.resetAllStats();
		portStatsDB.resetAllStats();
		connectionStatsDB.resetAllStats();
		
		trackUnitStatsDB.resetAllStats();
		trackLatchStatsDB.resetAllStats();
		trackStorageStatsDB.resetAllStats();
		trackPortStatsDB.resetAllStats();
		trackConnectionStatsDB.resetAllStats();
	}

	void printUnitStats(ostream & out)
	{
		unitStatsDB.print(out);
	}

	void printLatchStats(ostream & out)
	{
		latchStatsDB.print(out);
	}

	void printStorageStats(ostream & out)
	{
		storageStatsDB.print(out);
	}
	void printPortStats(ostream & out)
	{
		portStatsDB.print(out);
	}

	void printConnectionStats(ostream & out)
	{
		connectionStatsDB.print(out);
	}
};

#endif