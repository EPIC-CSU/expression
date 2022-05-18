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
 *file  : Unit.h                                             *
 *authors : ashok, vgee			                             *
 *last modified :                                            *
 *          29th april,98.vgee. clean-up.                    *
 *                                                           *
 *contents:contains class definitions for Unit class & other *
 *         classes derived from it.                          *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/
#ifndef _UNIT_H_
#define _UNIT_H_

#include "BaseSimObject.h"
#include "ArchGlobalIncludes.h"
#include "ArchClassIncludes.h"
#include "Latch.h"
#include "Port.h"
#include "Connection.h"
#include "OpCodeList.h"
#include "BaseOpExecSemantics.h"
#include "StatList.h"

/*************************************************************
 *class : Unit:-this class describes the various units of an *
 *        Architecture like the fetch,decode etc. each unit  *
 *        talks to other units through latches(storage) and  *
 *        /or through signals(connections).                  * 
 *        Each Unit has an "OpCodeList" which is a list of op*
 *        codes that the unit can support.                   * 
 *Methods:                                                   *
 *        copy()  :- creates a copy of this object           *
 *        is()    :- returns true if type of object is passed*
 *                   is the same as that of this class.      *
 *        ==      :- yesss, does exactly what u r thinking   *
 *        print() :- prints private data member values.      *
 *        isEq()  :- returns _NO_                            *
 *        init()  :- initializes Unit. NULL here.            *
 *        addUnit :- adds a Unit. NULL here.                 *
 *        addLatch:- adds a Latch. NULL here.                *
 *        addConnection():- adds a connection. NULL here.    *
 *        addOpCode() :- adds an opcode to the _opList.      *
 *        getOpList() :- returns a reference _opList(not a   *
 *                       good member function)               *         
 *        doStep():- executes the Unit through one m/c cycle *
 *                   the other methods are self-explanatory  *
 *        most of the virtual functions are valid for the    *
 *        derived classes like SimpleUnit etc. also.         *
 *************************************************************/
class Unit : public BaseSimObject
{
 protected:
   OpCodeList _opList;//list of opcodes supported by this Unit
   bool _busyBit;
   
public:
  Unit();
  Unit(const Unit &);
  ~Unit();

  virtual Unit * copy() = 0; 
  virtual int is(ClassType ctype);
  friend  int operator ==(const Unit &a, const Unit & b);
  virtual void print(ostream &out);
  virtual void name(ostream & out);
  virtual int isEq(Unit * ptr);
  virtual void flushBuffer() {}

  virtual void init();
  virtual void reset();

  virtual void addUnit(Unit *u);
  virtual void addLatch(LatchType,Latch *l);
  virtual LatchList * getLatchList(LatchType);
  virtual void addConnection(Connection *c);
  virtual void addPort(Port *p);
  virtual void addOpCode(char *opName, BaseOpExecSemantics & execOp);
  virtual void addOpCode(char *opName, BaseOpExecSemantics & execOp, int timing);
  virtual OpCodeList & getOpList();
   
  bool getBusyBit();
  void setBusyBit(bool);

  virtual void doStep() = 0;
  virtual void preprocess();
};

typedef Vector<Unit *> UnitList;
typedef Vector<Connection *> ConnectionList;
typedef Vector<Storage *> StorageList;
typedef Vector<UnitList> ListUnitList;

typedef Vector<Unit *>::iterator UnitListIterator;
typedef Vector<Connection *>::iterator ConnectionListIterator;
typedef Vector<Storage *>::iterator StorageListIterator;
typedef Vector<UnitList>::iterator ListUnitListIterator;

/*************************************************************
 *class : SimpleUnit:- Class which is a single Unit.         *
 *Methods:                                                   *
 *        doStep():- executes the Unit through one m/c cycle *
 *                   the other methods are self-explanatory  *
 *        In any case, For explanation on other methods see  *
 *        comments for Unit class.                           *
 *************************************************************/
class SimpleUnit : public Unit {
  protected :
    LatchList _inputLatches;         //List of Latches connected to the input of this Unit.
    LatchList _outputLatches;        //List of Latches connected to the output of this Unit.
	LatchList _otherLatches;	     //List of other Latches connected to this Unit.
    ConnectionList _connections;     //List of Connections to Storage elements.
	PortList _ports;

  public :
    SimpleUnit();
    SimpleUnit(const SimpleUnit&);
    ~SimpleUnit();

    virtual Unit * copy();
    virtual int is(ClassType ctype);
    friend int operator ==(const SimpleUnit &a, const SimpleUnit &b);
    virtual int isEq(Unit * ptr);
   
    virtual void init();
    virtual void addLatch(LatchType, Latch *);
	virtual LatchList * getLatchList(LatchType);
    virtual void addConnection(Connection *);
	virtual void addPort(Port *);
    virtual void print(ostream & );
    virtual void doStep();
};

/*************************************************************
 *class :CompoundUnit:-Class to describe units which can have*
 *       multiple units.                                     *
 *Methods:                                                   *
 *        doStep():- executes the Unit through one m/c cycle *
 *                   the other methods are self-explanatory  *
 *        In any case, For explanation on other methods see  *
 *        comments for Unit class.                           *
 *************************************************************/
class CompoundUnit : public Unit {
  protected :
    UnitList _units;            //List of Units which make this Unit.
    LatchList _inputLatches;         //List of Latches connected to the input of this Unit.
    LatchList _outputLatches;        //List of Latches connected to the output of this Unit.
	LatchList _otherLatches;	     //List of other Latches connected to this Unit.
    ConnectionList _connections;//List of Connections to Storage elements.
	PortList _ports;

    public :
    CompoundUnit();
    CompoundUnit(const CompoundUnit&);
    ~CompoundUnit();

    virtual Unit * copy();
    virtual int is(ClassType ctype);
    friend int operator ==(const CompoundUnit &a, const CompoundUnit &b);
    virtual int isEq(Unit * ptr);

    virtual void init();
    virtual void addUnit(Unit *);
    virtual void addLatch(LatchType, Latch *);
	virtual void addPort(Port *);
    virtual void addConnection(Connection *);
    virtual void print(ostream &);
  
    virtual void doStep();
};
#endif








