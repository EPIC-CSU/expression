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
 *file  : FUandOpcodeDS.h                                    *
 *created : 11 Jan,99                                        *
 *authors : Ashok Halambi                                    *
 *last modified :                                            *
 *                                                           *
 *compiled with : VC++                                       *
 *for more info :                                            *
 *************************************************************/

#ifndef __FUandOpcodeDS_H__
#define __FUandOpcodeDS_H__

#include "ArchSTLIncludes.h"
#include "../../pcProDll/include/MyString.h"

#include "../../pcProDll/include/OpCodeTable.h"

#define _DEFAULT_ 5
#define _INVALID_ -1

extern OpCodeTable globalOpCodeTable;

/*
	For a given functional unit (_fuName), this class contains a map of
	opcode and the latency.
	Example: _fuName : ALU_EX
		_opcodeLatencyMap :	ADD -> 1
							SUB -> 1
							MULT -> 3
							DIV -> 5

*/
class FUandOpcode
{
private:
	MyString _fuName;
	Map<int, int> _opcodeLatencyMap;

	inline int _getLatency(int a)
	{
		Map<int, int>::iterator i = _opcodeLatencyMap.find(a);
		if (i != _opcodeLatencyMap.end()) return (*i).second;

		// error condition.
		//
		// cout << "Warning: Tried to get Latency for a non-existant Opcode" << endl;
		return _DEFAULT_;
	}

	inline int _getOpIndex(char *opName)
	{
		int a = _INVALID_;
		try
		{
			a = globalOpCodeTable.getIndex(opName);
		}
		catch (NoSuchElementException *r)
		{
			cout << "Panic: Opcode: " << opName << "is not present in the opcode table" << endl;
			
			cout << *r;
			assert(0);
		}
		return a;
	}

public:
	FUandOpcode() {}
	FUandOpcode(char *fu) : _fuName(fu) {} 
	FUandOpcode(const FUandOpcode & a) : _fuName(a._fuName), _opcodeLatencyMap(a._opcodeLatencyMap) {}
	~FUandOpcode() {}

	void setFU(char *fu) { _fuName = fu; }
	char *getFU() { return _fuName.getStr(); }

	int isFU(char *fu)
	{
		if (strcmp(_fuName.getStr(), fu) == 0) return _YES_;
		return _NO_;
	}

	void addOpcode(char *opName, int latency = _DEFAULT_)
	{
		int a = _getOpIndex(opName);
		_opcodeLatencyMap[a] = latency;
	}

	int getLatency(int index)
	{
		return _getLatency(index);
	}

	int getLatency(char *opName)
	{
		int a = _getOpIndex(opName);
		return _getLatency(a);
	}

	void print(ostream & out)
	{
		out << "FU: ";
		_fuName.print(out);
		out << endl;

		Map<int, int>::iterator i;
		for (i = _opcodeLatencyMap.begin(); i != _opcodeLatencyMap.end(); i++)
		{
			out << "\t" << (*i).first << "\t" << (*i).second << endl;
		}
	}
};

/*
	This class is a list of all the opcodeLatencyMaps for
	all the functional units.
	Example : 
				IF	-> FUandOpcode info about IF.
				ID	-> FUandOpcode info about ID
				ALU1RD	-> FUandOpcode info about ALU1_RD
				LDST_EX	-> FUandOpcode info about LDST_EX
*/

/*
	This class is a list of all the opcodeLatencyMaps for
	all the functional units.
	Example : 
				IF	-> FUandOpcode info about IF.
				ID	-> FUandOpcode info about ID
				ALU1RD	-> FUandOpcode info about ALU1_RD
				LDST_EX	-> FUandOpcode info about LDST_EX
*/

class FUandOpcodeDS
{
private:
	Vector<FUandOpcode> _fuList;

	inline int _getFUIndex(char *fuName)
	{
		for (int i = 0; i < _fuList.size(); i++)
		{
			if (_fuList[i].isFU(fuName) == _YES_)
				return i;
		}

		// error condition.
		//
		cout << "The FU: " << fuName << " is not present in the DS" << endl;
		assert(0);
		return _INVALID_;
	}

	inline int _getOpIndex(char *opName)
	{
		int a = _INVALID_;
		try
		{
			a = globalOpCodeTable.getIndex(opName);
		}
		catch (NoSuchElementException *r)
		{
			cout << "Panic: Opcode: " << opName << "is not present in the opcode table" << endl;
			
			cout << *r;
			assert(0);
		}
		return a;
	}

	inline void _addOpcodeLatency(int index, char *opName, int latency)
	{
		if (index >= _fuList.size())
		{
			// error condition.
			//
			cout << "Panic: Invalid opcode index (in FUandOpcodeDS)" << endl;
			assert(0);
		}
		_fuList[index].addOpcode(opName, latency);
	}

	inline int _getLatency(int fuIndex, int opIndex)
	{
		if (fuIndex >= _fuList.size())
		{
			// error condition.
			//
//			cout << "Warning: Invalid opcode index (in FUandOpcodeDS)" << endl;
			return _DEFAULT_;
		}
		return (_fuList[fuIndex].getLatency(opIndex));
	}

public:
	FUandOpcodeDS() {}
	~FUandOpcodeDS() {}

	int addFU(char *fu)
	{
		FUandOpcode a(fu);
		_fuList.push_back(a);

		return (_fuList.size() - 1);
	}

	void addOpcode(int index, char *opName)
	{
		_addOpcodeLatency(index, opName, _DEFAULT_);
	}

	void addOpcodeAndLatency(int index, char *opName, int latency)
	{
		_addOpcodeLatency(index, opName, latency);
	}

	void addOpcode(char *fuName, char *opName)
	{
		int index = _getFUIndex(fuName);
		_addOpcodeLatency(index, opName, _DEFAULT_);
	}

	void addOpcodeAndLatency(char *fuName, char *opName, int latency)
	{
		int index = _getFUIndex(fuName);
		_addOpcodeLatency(index, opName, latency);
	}

	int getLatency(int fuIndex, int opIndex)
	{
		return (_getLatency(fuIndex, opIndex));
	}

	int getLatency(int fuIndex, char *opName)
	{
		int opIndex = _getOpIndex(opName);
		return (_getLatency(fuIndex, opIndex));
	}

	int getLatency(char *fuName, char *opName)
	{
		int fuIndex = _getFUIndex(fuName);
		int opIndex = _getOpIndex(opName);
		return (_getLatency(fuIndex, opIndex));
	}

	void print(ostream & out)
	{
		Vector<FUandOpcode>::iterator i;
		for (i = _fuList.begin(); i != _fuList.end(); i++)
		{
			(*i).print(out);
			out << endl;
		}
	}
};

#endif
