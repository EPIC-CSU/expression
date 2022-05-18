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
 *file  : ControlTable.h                                     *
 *created : 3 May, 2000                                      *
 *authors : Prabhat, Srikanth                                *
 *last modified :                                            *
 *                                                           *
 *contents:contains class definition for classes ControlTable*
 *         ControlTAbleEntry, NetList & StallMechanism       *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

#ifndef _CONTROL_TABLE_H_
#define _CONTROL_TABLE_H_

#include <map>
#include <deque>
#include "ArchClassIncludes.h"
#include "DerivedClassIncludes.h"
#include "DerivedAddrData.h"
#include "DerivedUnit.h"

// for ControlTable
#define StallBit						    0   // execute portion will be stalled
#define ExecuteBit						    1
#define NextCycleExecuteBit				    2 
#define BusyBit                             3   // unit doing some useful stuff
#define PipeNumBit							4

#define setStallBit(row,col,val)		    globalControlTable.setProperty(row,col,StallBit,val)
#define setExecuteBit(row,col,val)			globalControlTable.setProperty(row,col,ExecuteBit,val)
#define setNextCycleExecuteBit(row,col,val) globalControlTable.setProperty(row,col,NextCycleExecuteBit,val)
//#define setBusyBit(row,col,val)		    	globalControlTable.setProperty(row,col,BusyBit,val)
#define setPipeNumBit(row,col,val)			globalControlTable.setProperty(row,col,PipeNumBit,val)

#define getStallBit(row,col)			    globalControlTable.getProperty(row,col,StallBit)
#define getExecuteBit(row,col)			    globalControlTable.getProperty(row,col,ExecuteBit)
//#define getBusyBit(row,col)				    globalControlTable.getProperty(row,col,BusyBit)
#define getPipeNumBit(row,col)	            globalControlTable.getProperty(row,col,PipeNumBit)

#define resetAllStallBits				    globalControlTable.setProperty(StallBit,0)
#define resetAllExecuteBits				    globalControlTable.setProperty(ExecuteBit,0)
#define resetAllNextCycleExecuteBits        globalControlTable.setProperty(NextCycleExecuteBit,0)
#define resetAllBusyBits				    globalControlTable.setProperty(BusyBit,0)
#define resetAllPipeNumBits					globalControlTable.setProperty(PipeNumBit,0)

class ControlTableEntry 
{
  Unit * _unit;
  Vector<int> _properties;
  Vector<BaseType *> _listOfDest;
  BaseType * _destPropagated;
  BaseType * _destWritten;

//  #define executeBit _properties[0];
//  #define stallBit _properties[1];
  typedef Vector<BaseType *>::iterator BasePtrListIterator;

public:
  ControlTableEntry(Unit * unit = 0);
  ~ControlTableEntry();
  ControlTableEntry(const ControlTableEntry &);
  ControlTableEntry * copy();

  void setUnit(Unit * unit);
  void setProperty(int pos, int value);
  void setDest(BaseType * dest);
  void setDestPropagated(BaseType * dest);
  void setDestWritten(BaseType * dest);
  int checkDest(BaseType * dest,Unit **);
  Unit * getUnit();
  int getProperty(int num);
  void getDest(Vector<BaseType*> & destList);
  BaseType * getDestPropagated();
  BaseType * getDestWritten();
  void deleteDest(BaseType * dest);
  void resetDest();


  void print(ostream &);

  ControlTableEntry & operator =(const ControlTableEntry & a);
};

typedef vector<ControlTableEntry> Row;
typedef vector<Row> RowList;
typedef Row::iterator RowIterator;
typedef RowList::iterator RowListIterator;

/*************************************************************
 *class : ControlTable.                                      *
 *description : this table contains control information      *
 *              (ControlTableEntry) about each unit in the   *
 *              processor. Each row of the table corresponds *
 *              to a stage in the pipelines of the processor *
 *              The table is a vector of such rows.          *
 *              Two copies of the table are maintained :     *
 *              _currentRowList and _newRowList              *
 *              Changes to the values in the table are always*
 *              made first in the _newRowList and then copied*
 *              later to the _currentRowList.                *
 *                                                           *
 *              functions of the friend class StallMechanism *
 *              can manipulate the stall bits globally using *
 *              the strategy specified in that class,e.g. the*
 *              strategy in c6x is: stall all predecessor    *
 *              units.                                       *
 *                                                           *
 *Methods :      update() updates the old values with new    *
 *               updateProperty() updates only the specified *
 *                     properties of the ControlTableEntry   *
 *                                                           *
 *Data Members :                                             *
 *ToDo List :                                                *
 *************************************************************/ 
class ControlTable {
  RowList _currentRowList;

public:
  ControlTable(int NumRows);
  ~ControlTable();

  int search(Unit *, int & rowNum, int & colNum);
  ControlTableEntry & getEntry(int rowNum, int colNum);
  ControlTableEntry & getEntry(Unit *);
  void addEntry(ControlTableEntry & entry, int rowNum, int colNum=0);
  void addUnitToTable(Unit *, int rowNum, int colNum=0);
  void setDest(Unit *, BaseType * dest);
  void setDest(int RowNum, int columnNum, BaseType * dest);
  void setDest(BaseType *dest);
  void setDestWritten(int row, int col, BaseType * dest);
  void setDestWritten(Unit *, BaseType * dest);
  void setDestPropagated(int row, int col, BaseType * dest);
  void setDestPropagated(Unit *, BaseType * dest);
  BaseType * getDestWritten(int row, int col);
  BaseType * getDestPropagated(int row, int col);
  void deleteDest(int row, int col, BaseType * dest);
  void resetDest(Unit *unit);
  void setProperty(Unit *, int pos, int value);
  void setProperty(int row, int col, int pos, int value);
  void setProperty(int pos, int value);
  void getDest(Unit *,Vector<BaseType*> & destList);
  int checkDest(Unit *,BaseType * dest,Unit **);
  int getProperty(Unit *, int pos);
  int getProperty(int row, int col, int pos);
  void getIthRow(int rowNum, vector<ControlTableEntry> & unitlist);

  void initGlobalFUList();
  void createFUandRFConnTable();

  void update();
  //void updateProperty(int pos);
  void print(ostream &);

  // This needs to be a friend because we need to read from and write to 
  // the same (_newRowList) list. Using member functions of ControlTable
  // (the alternative way of doing things) will not work because the member
  // functions only allow read from _currentRowList and write to _newRowList.

  friend class StallMechanism;   
};
  
/*************************************************************
 *class : NetList.                                           *
 *description : This is the netlist for the simulator. It is *
 *              a graph where the nodes are the units and    *
 *              the edges represent interconnections. The    *
 *              graph is represented using a map data str.   *
 *              indexed by the unit pointer. With each unit  *
 *              is associated a list of predecessors and a   *
 *              list of successors. This information is neeed*
 *              while propagating execute bits down the pipes*
 *              and propagating stallBits up the pipes       *
 *                                                           *
 *Methods :                                                  *
 *Data Members :                                             *
 *ToDo List :                                                *
 *************************************************************/ 

class NetList
{
	struct PredSucc{
		vector<Unit *> _predlist;
		vector<Unit *> _succlist;
	public:
		PredSucc(){}
		};

	Map<Unit *, struct PredSucc> _graph;
	typedef map<Unit *, struct PredSucc> :: iterator graphIterator;

public:
	NetList();
	~NetList(); //TODO: fix this

	void addUnit(Unit *unit, Unit * pred=0, Unit * succ=0);
	void setPredecessor(Unit *, Unit *);
	void setSuccessor(Unit *, Unit *);
	UnitList & getSuccessorList(Unit * unit);
	UnitList & getPredecessorList(Unit *unit);
	void print(ostream &);
};

/*************************************************************
 *class : StallMechanism                                     *
 *description : A global strategy for setting the stall bits *
 *         This is supplied by the user.The simplest strategy*
 *         (implemented for c6x is to stall all predecessors *
 *         of a unit that needs to be stalled for DataHazard *
 *         Another way could be that the coimpiler can figure*
 *         out the hazard and fill NOPS in the pipe where the*
 *         stalled unit lies. Then the stall needs to be     *
 *         propagated upwards only till the decodeunit since *
 *         the decodeunit can still decode operations & keep *
 *         sending operations to other pipes that can        *
 *         continue to work.                                 *
 *                                                           *
 *Methods :                                                  *
 *Data Members :                                             *
 *ToDo List :                                                *
 *************************************************************/ 

class StallMechanism
{
public:
	StallMechanism();
	~StallMechanism();
	void setStall();
};

const int HAS_BEEN_ISSUED_AN_OPER = 888;
const int INIT_FULISTELEM = 887;

typedef struct 
{
	Unit *_unit;
	int _flag; // flag can be one of : HAS_BEEN_ISSUED_AN_OPER, INIT_FULISTELEM
} FUListElem;

typedef vector<FUListElem> FUList;
typedef vector<FUListElem>::iterator FUListIter;

extern FUList globalFUList;

extern NetList globalNetList;

extern StallMechanism globalStallMechanism;

extern ControlTable globalControlTable;
 
extern void createForwardingTable(void);

extern void printForwardingTable(ostream&);

extern int binaryToDecimal(deque<int> & bitstr);

extern void decimalToBin(deque<int> & bitstr, int decimalValue);


#endif