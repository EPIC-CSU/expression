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
 *file  : Unit.cpp                                           *
 *authors : ashok,vgee		                                 *
 *last modified :                                            *
 *                                                           *
 *contents:contains class methods for Unit class & other     *
 *         classes derived from it.                          *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/
#include "Unit.h"
#include "CompName.h"

/*************************************************************
 *class : Unit                                               *
 *************************************************************/
Unit::Unit(){_busyBit = false;}
Unit::Unit(const Unit &){_busyBit = false;}
Unit::~Unit(){}

int Unit::is(ClassType ctype){return ((ctype == _UNIT_) ? _YES_ : _NO_);}
int operator ==(const Unit &a, const Unit &b){return _NO_;}
void Unit::print(ostream &out){ printName(out, this); }
void Unit::name(ostream &out){ printName(out, this); }

int Unit::isEq(Unit * ptr){return _NO_;}

void Unit::init(){}
void Unit::reset() {}

void Unit::addUnit(Unit *u){}
void Unit::addLatch(LatchType,Latch *l){}
LatchList * Unit::getLatchList(LatchType t)
{
	cout << "Unit::getLatchList() - BaseClass function should not be called" << endl;
	assert(0);
	return 0;
}

void Unit::addConnection(Connection *c){}
void Unit::addPort(Port * p){}
void Unit::addOpCode(char *opName, BaseOpExecSemantics & execOp)
{
   _opList.addOpCode(opName, execOp, 1);
}
void Unit::addOpCode(char *opName, BaseOpExecSemantics & execOp, int timing)
{
   _opList.addOpCode(opName, execOp, timing);
}

OpCodeList & Unit::getOpList(){return _opList;}

void Unit::setBusyBit(bool bit){_busyBit = bit;}
bool Unit::getBusyBit(){return _busyBit;}
void Unit::preprocess(){}

/*************************************************************
 *class : SimpleUnit                                         *
 *************************************************************/
SimpleUnit::SimpleUnit():_inputLatches(),_outputLatches(),_otherLatches(),_connections(){}
SimpleUnit::SimpleUnit(const SimpleUnit& a):
              _inputLatches(a._inputLatches),_outputLatches(a._outputLatches),
				  _otherLatches(a._otherLatches),_connections(a._connections){}
SimpleUnit::~SimpleUnit(){}

Unit * SimpleUnit::copy()
{
  Unit * retPtr;
  retPtr = new SimpleUnit(* this);
  return retPtr;
}

void SimpleUnit::doStep(void)
{
	assert(0);
}

int SimpleUnit::is(ClassType ctype)
{
  return((ctype == _SIMPLE_UNIT_)?_YES_:(Unit::is(ctype)));
}

int operator ==(const SimpleUnit &a, const SimpleUnit &b)
{
  return _NO_;
}

int SimpleUnit::isEq(Unit * ptr)
{
  return _NO_;
}

void SimpleUnit::init()
{
  Unit::init();
}

void SimpleUnit::addLatch(LatchType ltype,Latch * latch)
{
  if (ltype == _IN_L_)
	  _inputLatches.push_back(latch);
  else if (ltype == _OUT_L_)
	  _outputLatches.push_back(latch);
  else
	  _otherLatches.push_back(latch);
}

LatchList * SimpleUnit::getLatchList(LatchType ltype)
{
  if (ltype == _IN_L_)
	  return &_inputLatches;
  else if (ltype == _OUT_L_)
	  return &_outputLatches;
  else
	  return &_otherLatches;
}

void SimpleUnit::addConnection(Connection * connection)
{
  _connections.push_back(connection);
}

void SimpleUnit::addPort(Port * port)
{
  _ports.push_back(port);
}

void SimpleUnit::print(ostream & out)
{
   LatchListIterator latchIter;
   ConnectionListIterator connectIter;

   out << endl << "Printing Input Latches: " << endl;
   CREATE_ITER(latchIter, _inputLatches);
   while(HAS_MORE_ELEMENTS(latchIter, _inputLatches))
      {
	CURRENT_ELEMENT(latchIter)->print(out);
	NEXT_ELEMENT(latchIter);
      }

   out << endl << "#+-+-+-+-+-+-+#" << endl;
   out << endl << "Printing Output Latches: " << endl;
   CREATE_ITER(latchIter, _outputLatches);
   while(HAS_MORE_ELEMENTS(latchIter, _outputLatches))
      {
	CURRENT_ELEMENT(latchIter)->print(out);
	NEXT_ELEMENT(latchIter);
      }
   
   out << endl << "#+-+-+-+-+-+-+#" << endl;
   out << endl << "Printing Other Latches: " << endl;
   CREATE_ITER(latchIter, _otherLatches);
   while(HAS_MORE_ELEMENTS(latchIter, _otherLatches))
      {
	CURRENT_ELEMENT(latchIter)->print(out);
	NEXT_ELEMENT(latchIter);
      }

   out << endl << "#+-+-+-+-+-+-+#" << endl;
   out << endl << "Printing Connections: " << endl;

   CREATE_ITER(connectIter, _connections);
   while(HAS_MORE_ELEMENTS(connectIter, _connections))
      {
	 CURRENT_ELEMENT(connectIter)->print(out);
	 NEXT_ELEMENT(connectIter);
      }
   out << endl << "#+-+-+-+-+-+-+#" << endl;
}

/*************************************************************
 *class : CompoundUnit                                       *
 *************************************************************/

CompoundUnit::CompoundUnit():_units(),_inputLatches(),_outputLatches(),
				_otherLatches(),_connections(){} 

CompoundUnit::CompoundUnit(const CompoundUnit& a):
      _units(a._units),_inputLatches(a._inputLatches),_outputLatches(a._outputLatches),
		  _otherLatches(a._otherLatches),_connections(a._connections) {} 

CompoundUnit::~CompoundUnit(){}

void CompoundUnit::init()
{
 Unit::init();
}

Unit * CompoundUnit::copy()
{
  Unit * retPtr;
  retPtr = new CompoundUnit(* this);
  return retPtr;
}

int CompoundUnit::is(ClassType ctype)
{
   return((ctype == _COMPOUND_UNIT_) ? _YES_ : (Unit :: is(ctype)));
   //return((ctype == _COMPOUND_UNIT_) ? _YES_ : _NO_);
   //TODO.vgee.
}

int operator ==(const CompoundUnit &a, const CompoundUnit &b)
{
  return _NO_;
}
    
int CompoundUnit::isEq(Unit * ptr)
{
 return _NO_;
}

void CompoundUnit::addUnit(Unit * unit)
{
  _units.push_back(unit);
}

void CompoundUnit::addLatch(LatchType ltype,Latch * latch)
{
  if (ltype == _IN_L_)
	  _inputLatches.push_back(latch);
  else if (ltype == _OUT_L_)
	  _outputLatches.push_back(latch);
  else
	  _otherLatches.push_back(latch);
}

void CompoundUnit::addConnection(Connection * connection)
{
  _connections.push_back(connection);
}

void CompoundUnit::addPort(Port * port)
{
  _ports.push_back(port);
}

void CompoundUnit::print(ostream & out)
{
   UnitListIterator unitIter;
   LatchListIterator latchIter;
   ConnectionListIterator connectIter;

   out << endl << "Printing Units: " << endl;
   
   CREATE_ITER(unitIter, _units);
   while (HAS_MORE_ELEMENTS(unitIter, _units))
      {
	 CURRENT_ELEMENT(unitIter)->print(out);
	 NEXT_ELEMENT(unitIter);
      }

   out << endl << "#+-+-+-+-+-+-+#" << endl;
   out << endl << "Printing Input Latches: " << endl;
   CREATE_ITER(latchIter, _inputLatches);
   while(HAS_MORE_ELEMENTS(latchIter, _inputLatches))
      {
	CURRENT_ELEMENT(latchIter)->print(out);
	NEXT_ELEMENT(latchIter);
      }

   out << endl << "#+-+-+-+-+-+-+#" << endl;
   out << endl << "Printing Output Latches: " << endl;
   CREATE_ITER(latchIter, _outputLatches);
   while(HAS_MORE_ELEMENTS(latchIter, _outputLatches))
      {
	CURRENT_ELEMENT(latchIter)->print(out);
	NEXT_ELEMENT(latchIter);
      }
   
   out << endl << "#+-+-+-+-+-+-+#" << endl;
   out << endl << "Printing Other Latches: " << endl;
   CREATE_ITER(latchIter, _otherLatches);
   while(HAS_MORE_ELEMENTS(latchIter, _otherLatches))
      {
	CURRENT_ELEMENT(latchIter)->print(out);
	NEXT_ELEMENT(latchIter);
      }

   out << endl << "#+-+-+-+-+-+-+#" << endl;
   out << endl << "Printing Connections: " << endl;
   
   CREATE_ITER(connectIter, _connections);
   while (HAS_MORE_ELEMENTS(connectIter, _connections))
      {
	 CURRENT_ELEMENT(connectIter)->print(out);
	 NEXT_ELEMENT(connectIter);
      }

   out << endl << "#+-+-+-+-+-+-+#" << endl;
}

// Each component has a name. Need to store that in a map.

Map<Unit *, MyString> globalUnitNameList;
