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
 *file  : Port.cpp                                           *
 *created : 16th dec,97.                                     *
 *authors : vgee,ashok				                         *
 *last modified :                                            *
 *                                                           *
 *contents : contains the storage class definitions          *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/
#include "stdafx.h"

#include "Port.h"
#include "Connection.h"
#include "CompName.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

Port * Port :: copy()
{
  Port * retPtr;
  retPtr = new Port(* this);
  return retPtr;
}

void Port :: addConnection(Connection * connection)
{
  _connections.push_back(connection);
}

void Port :: addPtrToStorage(Storage * s)
{
  _ptrToStorage = s;
}

void Port :: print(ostream & out)
{
   ConnectionListIterator connectIter;

   out << endl << "#+-+-+-+-+-+-+#" << endl;
   out << endl << "Printing Connections: " << endl;
   
   if (!(_connections.empty()))
      {
	 CREATE_ITER(connectIter, _connections);
	 while (HAS_MORE_ELEMENTS(connectIter,_connections))
	    {
	       CURRENT_ELEMENT(connectIter)->print(out);
	       NEXT_ELEMENT(connectIter);
	    }
      }
   out << endl << "#+-+-+-+-+-+-+#" << endl;
}

void Port :: name(ostream & out) { printName(out, this); }

void Port :: read(BaseType & a,BaseType & b)
{ 
	 if (_typeOfPort == _WRITE_){
		 cout << "Panic: READ Operation cannot be done through a WRITE port" << endl;
		 assert(0);
	 }
	 else 
         _ptrToStorage->read(a,b);
}

void Port :: write(BaseType& a , BaseType & b)
{
	 if (_typeOfPort == _READ_){
		 cout << "Panic: WRITE Operation cannot be done through a READ port" << endl;
		 assert(0);
	 }
	 else 
         _ptrToStorage->write(a,b);
}  
 
void Port :: read(BaseType& value, BaseType & addr, SignalClass & latency)
{
	 if (_typeOfPort == _WRITE_){
		 cout << "Panic: READ Operation cannot be done through a WRITE port" << endl;
		 assert(0);
	 }
	 else 
         _ptrToStorage->read(value,addr,latency);
}

void Port :: read(BaseType& value, BaseType & addr, SignalClass & latency, int memType)
{
	 if (_typeOfPort == _WRITE_){
		 cout << "Panic: READ Operation cannot be done through a WRITE port" << endl;
		 assert(0);
	 }
	 else 
         _ptrToStorage->read(value,addr,latency, memType);
}
  
void Port :: write(BaseType & data, BaseType & addr, SignalClass & latency)
{ 
	 if (_typeOfPort == _READ_){
		 cout << "Panic: WRITE Operation cannot be done through a READ port" << endl;
		 assert(0);
	 }
	 else 
         _ptrToStorage->write(data,addr,latency);
}

// Each component has a name. Need to store that in a map.

Map<Port *, MyString> globalPortNameList;