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
 *file  : StatList.h                                         *
 *authors : ashok				                             *
 *created : Thu Oct 8, 98                                    *
 *last modified : Fri Oct9, 98                               *
 *                                                           *
 *compiled with : VC++                                       *
 *for more info :                                            *
 *************************************************************/
#ifndef _STATLIST_H_
#define _STATLIST_H_

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "ArchGlobalIncludes.h"
#include "ArchClassIncludes.h"

//#define PCPRO_GUI

#ifdef WIN32_GUI
#include "../../pcProGUI/stat/GUIBaseStat.h"
#endif

enum StatType {_NUMTIMES_, _HAZARDSTALL_, _TRACKSTAT_, _TRACKNUMTIMES_, _PERCENTUSAGE_, _ALLSTAT_, _NOSTAT_};
enum StatParam {STAT_OPCODE, STAT_DEST, STAT_SRC1, STAT_SRC2, STAT_CYC};

extern StatType getStatType(char *str);

typedef Vector<StatType> StatTypeList;
typedef Vector<StatParam> *StatParams;

typedef Vector<int> ParamList;

extern unsigned long clock_count;

class StatList
{
private:
	Map<StatType, StatParams> _statArray;
	typedef Map<StatType, StatParams>::iterator _StatArrayIter;

public:
	StatList() : _statArray() {}
	~StatList() {}

	void addStat(StatType aStat, StatParams parms = NULL)
	{
		_statArray[aStat] = parms;
	}

	StatParams getStatParams(StatType a);

	StatParams justGetStatParams(StatType a)
	{
		return _statArray[a];
	}

	void getStatTypeList(StatTypeList& arr);

	void reset(StatType a = _ALLSTAT_);
};

#ifdef WIN32_GUI
typedef GUIBaseStat *GUI_IDType;
#else
typedef int GUI_IDType;
#endif

class BaseStat
{
protected:
	GUI_IDType _guiId; // To notify the GUI whenever necessary.

	virtual void _updateGUIObject() { }

	virtual void _notify() 
	{
#ifdef WIN32_GUI
		if (_guiId != 0) _guiId->Notify();
/*
		else
		{
			// error condition.
			//
			cout << "Warning: Stat does not have a corresponding GUIStat" << endl;
		}
*/
#endif
	}

public:
	BaseStat() : _guiId(0) {}

	virtual int is(StatType a) = 0;
	virtual StatType getType() = 0;

	virtual BaseStat *copy() = 0;
	virtual void operator ()(void) {}
	virtual void operator ()(ParamList parm) {}

	virtual void reset() {}
	virtual void print(ostream & out) {}

	virtual int isCumulativeStat() { return _NO_; }
	virtual int isTrackingStat() { return _NO_; }

	virtual void setGUIID(GUI_IDType id) { _guiId = id; _updateGUIObject(); _notify(); }
};
	
class NumTimesStat : public BaseStat
{
protected:
	int _numTimes;

public:
	NumTimesStat() : _numTimes(0) {}
	NumTimesStat(const NumTimesStat & a) : _numTimes(a._numTimes) {}

	~NumTimesStat() {}

	virtual int is(StatType a)
	{
		if (a == _NUMTIMES_) return 1;
		return 0;
	}

	virtual StatType getType() { return _NUMTIMES_; }

	virtual int isCumulativeStat() { return _YES_; }

	virtual BaseStat *copy()
	{
		BaseStat *retPtr;
		retPtr = new NumTimesStat(*this);

		return retPtr;
	}

	virtual void reset() { _numTimes = 0; }

	void inc(int times = 1)
	{
		_numTimes += times;
	}

	virtual void operator ()(void)
	{
		_numTimes += 1;

		// Note: We also update the GUI object after every cycle.
		//
		// However, we could also make it such that this stat is shown
		// only at the end of simulation.
		// In that case _updateGUIObject would need to be called only once
		// at the end of simulation.
		//
		_updateGUIObject(); 
	}

	int getNumTimes() { return _numTimes; }

	virtual void print(ostream & out)
	{
		out << " Usage: " << _numTimes << " " << endl;
	}

	virtual void _updateGUIObject() 
 { 
#ifdef WIN32_GUI
  if (_guiId!=0) _guiId->updateThisInt=_numTimes;
  _notify();
#endif
 }

};

class HazardStallStat : public BaseStat
{
private:
	int _numStalls;

public:
	HazardStallStat() : _numStalls(0) {}
	HazardStallStat(const HazardStallStat & a) : _numStalls(a._numStalls) {}

	~HazardStallStat() {}

	virtual int is(StatType a)
	{
		if (a == _HAZARDSTALL_) return 1;
		return 0;
	}

	virtual StatType getType() { return _HAZARDSTALL_; }

	virtual int isCumulativeStat() { return _YES_; }

	virtual BaseStat *copy()
	{
		BaseStat *retPtr;
		retPtr = new HazardStallStat(*this);

		return retPtr;
	}

	virtual void reset() { _numStalls = 0; }

	void inc(int times = 1)
	{
		_numStalls += times;
	}

	virtual void operator ()(void)
	{
		_numStalls += 1;

		// Note: We also update the GUI object after every cycle.
		//
		// However, we could also make it such that this stat is shown
		// only at the end of simulation.
		// In that case _updateGUIObject would need to be called only once
		// at the end of simulation.
		//
		_updateGUIObject(); 
	}

	int getNumStalls() { return _numStalls; }

	virtual void print(ostream & out)
	{
		out << " Stall Cycles: " << _numStalls << " " << endl;
	}

	virtual void _updateGUIObject() 
 { 
#ifdef WIN32_GUI
  if (_guiId!=0) _guiId->updateThisInt=_numStalls;
  _notify();
#endif
 }
};


class TrackStat : public BaseStat
{
protected:

	int _trackInterval; // = num cycles over which the stat is averaged.

	BaseStat *_statPtr; // Pointer to stat that it is tracking.

	virtual void _endTracking() { }

public:
	TrackStat() : _trackInterval(1), _statPtr(NULL) { }
	TrackStat(const TrackStat & a) : _trackInterval(a._trackInterval),
		_statPtr(a._statPtr) { }

	virtual int is(StatType a)
	{
		if (a == _TRACKSTAT_) return 1;
		return 0;
	}

	virtual StatType getType() { return _TRACKSTAT_; }

	virtual int isTrackingStat() { return _YES_; }

	virtual BaseStat *copy()
	{
		BaseStat *retPtr;
		retPtr = new TrackStat(*this);

		return retPtr;
	}

	virtual void reset()
	{
		_guiId = 0;
		_trackInterval = 1;
		_statPtr = NULL;
	}

	virtual void setTrackInterval(int num) { _trackInterval = num; }
	virtual void setStatToTrack(BaseStat *ptr) { _statPtr = ptr; }

	virtual void operator ()(void)
	{
		if (clock_count % _trackInterval == 0)
		{
			// i.e. reached the end of the tracking interval.
			_updateGUIObject();
			
			_endTracking();
		}
	}
};

class TrackNumTimesStat : public TrackStat
{
protected:
	int _trackNumTimes;
	int _prevNumTimes;

	virtual void _endTracking()
	{
		_trackNumTimes = 0;
	}

 virtual void _updateGUIObject()
 {
#ifdef WIN32_GUI
  if (_guiId!=0) _guiId->updateThisInt=_trackNumTimes;
  _notify();
#endif
 }

public:
	TrackNumTimesStat() : _trackNumTimes(0), _prevNumTimes(0) {}
	TrackNumTimesStat(const TrackNumTimesStat & a) : _trackNumTimes(a._trackNumTimes),
		_prevNumTimes(a._prevNumTimes) { _statPtr = a._statPtr; _trackInterval = a._trackInterval;}

	~TrackNumTimesStat() {}

	virtual int is(StatType a)
	{
		if (a == _TRACKNUMTIMES_) return 1;
		if (TrackStat::is(a)) return 1;
		return 0;
	}

	virtual StatType getType() { return _TRACKNUMTIMES_; }

	virtual BaseStat *copy()
	{
		BaseStat *retPtr;
		retPtr = new TrackNumTimesStat(*this);

		return retPtr;
	}

	virtual void reset() 
	{
		_trackNumTimes = 0;
		
		// Note: Calling TrackStat::reset will make both statPtr and guiId NULL.
		// Don't want that to happen.
		//
		// TrackStat::reset(); 
	}

	void inc(int times = 1)
	{
		_trackNumTimes += times;
	}

	virtual void operator ()(void)
	{
		int currNumTimes = ((NumTimesStat *)_statPtr)->getNumTimes(); 
		_trackNumTimes += (currNumTimes - _prevNumTimes);
		_prevNumTimes = currNumTimes;

		TrackStat::operator ()();
	}

	int getNumTimes() { return _trackNumTimes; }

	virtual void print(ostream & out)
	{
		out << " Track Val: " << _trackNumTimes << " " << endl;
	}
};

class PercentUsageStat : public TrackStat
{
protected:
	int _usage;

	virtual void _endTracking()
	{
		_usage = 0;
	}

 virtual void _updateGUIObject()
 {
#ifdef WIN32_GUI
  if (_guiId!=0) _guiId->updateThisInt=_usage;
  _notify();
#endif
 }


public:
	PercentUsageStat() : _usage(0) {}
	PercentUsageStat(const PercentUsageStat & a) : _usage(a._usage) 
	{_statPtr = a._statPtr; _trackInterval = a._trackInterval;}

	~PercentUsageStat() {}

	virtual int is(StatType a)
	{
		if (a == _PERCENTUSAGE_) return 1;
		if (TrackStat::is(a)) return 1;
		return 0;
	}

	virtual StatType getType() { return _PERCENTUSAGE_; }

	virtual BaseStat *copy()
	{
		BaseStat *retPtr;
		retPtr = new PercentUsageStat(*this);

		return retPtr;
	}

	virtual void reset() 
    { 
        _usage = 0; 

        // Do not call base class reset, because it resets
        // the stat ptr and the gui id.
        // TrackStat::reset();
    }

	virtual void operator ()(void)
	{
		int currNumTimes = ((NumTimesStat *)_statPtr)->getNumTimes(); 

		_usage = (currNumTimes * 100)/clock_count;

		TrackStat::operator ()();
	}

	int getPercentUsage() { return _usage; }

	virtual void print(ostream & out)
	{
		out << "Percent Usage: " << _usage << " " << endl;
	}
};

typedef Map<StatType, BaseStat *> StatsRow;
typedef Map<StatType, BaseStat *>::iterator StatsRowIter;

template
<class T>
class StatsCollector
{
private:
	Map<T *, StatsRow> _statTable;
	typedef Map<T *, StatsRow>::iterator _StatTableIter;
	
public:
	StatsCollector() : _statTable() {}
	~StatsCollector() {}
	
	void addKey(T *key)
	{
		StatsRow tmp;
		_statTable[key] = tmp;
	}
	
	void addStat(T *key, BaseStat *stat)
	{
		if (stat == NULL) return;
		StatType st;
		
		st = stat->getType();
		(_statTable[key])[st] = stat->copy();
	}
	
	BaseStat *getStat(T *key, StatType stat)
	{
		return (_statTable[key])[stat];
	}
	
	StatsRow & getStatList(T *key)
	{
		return (_statTable[key]);
	}
	
	void execStat(T *key, StatType stat)
	{
		BaseStat *temp = (_statTable[key])[stat];
		if (temp != NULL)
			(*temp)();
	}
	
	void execStat(T *key, StatType stat, ParamList param)
	{
		BaseStat *temp = (_statTable[key])[stat];
		if (temp != NULL)
			(*temp)(param);
	}
	
	void execAllStats()
	{
		_StatTableIter i;
		StatsRowIter j;
		
		for (i = _statTable.begin(); i != _statTable.end(); i++)
		{
			for (j = ((*i).second).begin(); j != ((*i).second).end(); j++)
			{
				(*((*j).second))();
			}
		}
	}
	
	void resetStat(T * key, StatType stat)
	{
		BaseStat *temp = (_statTable[key])[stat];
		if (temp != NULL)
			temp->reset();
	}
	
	void resetStats(T *key)
	{
		StatsRowIter i;
		for (i = (_statTable[key]).begin(); i != (_statTable[key]).end(); i++)
			resetStat(key, (*i).first);	
	}
	
	void resetAllStats()
	{
		_StatTableIter i;
		for (i = _statTable.begin(); i != _statTable.end(); i++)
			resetStats((*i).first);
	}
	
	void print(ostream & out, T *key, StatType stat)
	{
		BaseStat *temp = (_statTable[key])[stat];
		if (temp != NULL)
		{
			key->name(out);
			temp->print(out);
			out << endl;
		}
	}
	
	void print(ostream & out, StatType stat)
	{
		_StatTableIter i;
		for (i = _statTable.begin(); i != _statTable.end(); i++)
			print(out, (*i).first, stat);
	}
	
	void print(ostream & out, T *key)
	{
		StatsRowIter i;
		for (i = (_statTable[key]).begin(); i != (_statTable[key]).end(); i++)
			print(out, key, (*i).first);
	}
	
	void print(ostream & out)
	{
		_StatTableIter i;
		for (i = _statTable.begin(); i != _statTable.end(); i++)
			print(out, (*i).first);
	}
};

#endif