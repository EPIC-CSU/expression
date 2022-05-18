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
 *file  : MemData.cpp                                        *
 *created : Feb 18, 2000                                     *
 *authors : Prabhat Mishra                                   *
 *last modified :                                            *
 *                                                           *
 *contents:contains definitios of MemData class              *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

 
#include "MemData.h"

 
MemData :: MemData()
{
	_address = -1;
	_latency = -1;
	_state = _WAITING_;
}

MemData :: MemData(long address, Val & value, int latency, int opcode, MemAccessState state)
{
	_address = address;
	_value = value;
	_latency = latency;
	_opcode = opcode;
	_state = state;
}

MemData :: MemData(const MemData & mData)
{
	_address = mData._address;
	_value = mData._value;
	_latency = mData._latency;
	_opcode = mData._opcode;
	_state = mData._state;
}

MemData& MemData::operator=(const MemData& mData)
{
	_address = mData._address;
	_value = mData._value;
	_latency = mData._latency;
	_opcode = mData._opcode;
	_state = mData._state;

	return (*this);
}

MemData :: ~MemData()
{
}

void MemData :: setAddress(long address)
{
	_address = address;
}

void MemData :: setValue(Val & value)
{
	_value = value;
}

void MemData :: setLatency(int latency)
{
	_latency = latency;
}

void MemData :: setOpcode(int opcode)
{
	_opcode = opcode;
}

void MemData :: setState(MemAccessState state)
{
	_state = state;
}

long MemData :: getAddress()
{
	return _address;
}

Val & MemData :: getValue()
{
	return _value;
}

int MemData :: getLatency()
{
	return _latency;
}

int MemData :: getOpcode()
{
	return _opcode;
}

MemAccessState MemData :: getState()
{
	return _state;
}
