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
 *file  : Connection.cpp                                      *
 *authors : ashok,vgee                                       *
 *last modified :                                            *
 *                                                           *
 *contents:contains the class definition for the connections *
 *comments:-                                                 *
 *         the connection is modeled as follows. every conn. *
 *         has a 'master(s)' and 'slave(s)'. the master has  *
 *         ptr to the 'connection' and the 'connection' has  *
 *         ptr to the 'slave'. so for example to do a read   *
 *         operation on memory connected to the fetch unit   *
 *         thru' a connection, the fetch unit will call the  *
 *         read method of the connection which in turn will  *
 *         call the read method of the 'storage(memory)'.    *
 *         we are not modelling the 'data' and 'address' bus *
 *         'per se' since it would mean that memory has got  *
 *         an active unit in terms of the memory management  *
 *         Unit. we don't have such a unit in our model right*
 *         now.                                              *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/
#include "stdafx.h"

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

/************************************************************
 *Connection Class.                                         *
 ************************************************************/
Connection::Connection(){}
Connection::Connection(const Connection &a):_listOfPorts(a._listOfPorts) {}
Connection::~Connection(){}

Connection * Connection::copy()
{
  Connection *retPtr;
  retPtr = new Connection(*this);
  return retPtr;
} 

int Connection::is(ConnectionType ctype)
{
  return ((ctype == _CONNECTION_) ? _YES_ : _NO_);
}

int operator ==(const Connection &a, const Connection &b)
{
  return _NO_;
}

void Connection::print(ostream &out){}

void Connection::name(ostream &out) { printName(out, this); }

int Connection::isEq(Connection * ptr)
{
  return _NO_;
}

Connection& Connection::operator=(const Connection &a)
{
  _listOfPorts = a._listOfPorts;
  return *this;
}

void Connection::addPort(Port * port)
{
  _listOfPorts.push_back(port);
}

void Connection::read(BaseType &)
{
	// Error: this function should never be called.
	cout << "Error: Base class method (Connection::read(BaseType)) should not be invoked" << endl;
	return ;
}
/*
void Connection::read(BaseType &,BaseType &)
{
	// Error: this function should never be called.
	cout << "Error: Base class method (Connection::read(BaseType, BaseType)) should not be invoked" << endl;
	return ;
}
*/

void Connection::read(BaseType &, BaseType &, SignalClass & latency, unsigned int portNum)
{
	// Error: this function should never be called.
	cout << "Error: Base class method (Connection::read(BaseType, BaseType, SignalClass)) should not be invoked" << endl;
	return ;
}

void Connection::read(BaseType &, int memType, SignalClass & latency, BaseType &,unsigned int portNum)
{
	// Error: this function should never be called.
	cout << "Error: Base class method (Connection::read(BaseType, BaseType, SignalClass)) should not be invoked" << endl;
	return ;
}

void Connection::read(BaseType & value, BaseType & addr, unsigned int portNum)
{
  if(_READ_ == (_listOfPorts[portNum])->getType() || 
     _READWRITE_ == (_listOfPorts[portNum])->getType()) 
     (_listOfPorts[portNum])->read(value,addr);
  else
	  // Error condition.
	  cout << "Error: Not a read port" << endl;
  return;
}

void Connection::write(BaseType &){}
void Connection::write(BaseType &, BaseType &, SignalClass & latency, unsigned int portNum){}
void Connection::write(BaseType & data, BaseType & addr, unsigned int portNum)
{
  if(_READWRITE_ == (_listOfPorts[portNum])->getType() || 
     _WRITE_ == (_listOfPorts[portNum])->getType())
     (_listOfPorts[portNum])->write(data, addr);
}
    
/************************************************************
 *Register Connection Class.                                *
 ************************************************************/
RegisterConnection::RegisterConnection(){}
RegisterConnection::RegisterConnection(Port * b)
{
  _listOfPorts.push_back(b);
}
RegisterConnection::RegisterConnection(const RegisterConnection & a) 
{
  _listOfPorts = a._listOfPorts;
}
RegisterConnection::~RegisterConnection(){}
 
Connection * RegisterConnection::copy()
{
   Connection *retPtr;
   retPtr = new RegisterConnection(*this);
   return retPtr;
}

RegisterConnection & 
RegisterConnection::operator=(const RegisterConnection & a)
{
 _listOfPorts = a._listOfPorts; 
 return *this;
}

void RegisterConnection::print(ostream &out){}
void RegisterConnection::read(BaseType & value,BaseType & addr,unsigned int portNum)
{
   if(_READ_ == (_listOfPorts[portNum])->getType() || 
      _READWRITE_ == (_listOfPorts[portNum])->getType()) 
      (_listOfPorts[portNum])->read(value,addr);
   else
	  // Error condition.
	  cout << "Error: Not a read port" << endl;
   return;
}

void RegisterConnection::write(BaseType & data,BaseType & addr,
			       unsigned int portNum)
{
   if(_READWRITE_ == (_listOfPorts[portNum])->getType() || 
      _WRITE_ == (_listOfPorts[portNum])->getType())
     (_listOfPorts[portNum])->write(data, addr);
}

void RegisterConnection::read(BaseType & value,BaseType & addr,
							  SignalClass & latency, unsigned int portNum)
{
	latency = 0;
   if(_READ_ == (_listOfPorts[portNum])->getType() || 
      _READWRITE_ == (_listOfPorts[portNum])->getType()) 
      (_listOfPorts[portNum])->read(value,addr);
   else
	  // Error condition.
	  cout << "Error: Not a read port" << endl;
   return;
}

void RegisterConnection::write(BaseType & data,BaseType & addr,
							   SignalClass & latency, unsigned int portNum)
{
	latency = 0;
   if(_READWRITE_ == (_listOfPorts[portNum])->getType() || 
      _WRITE_ == (_listOfPorts[portNum])->getType())
     (_listOfPorts[portNum])->write(data, addr);
}

Port * RegisterConnection::getPort(unsigned int p)
{
  return _listOfPorts[p];
}

/************************************************************
 *Memory Connection Class.                                  *
 ************************************************************/
MemoryConnection::MemoryConnection(){}
MemoryConnection::MemoryConnection(Port * b)
{
  _listOfPorts.push_back(b);
}
MemoryConnection::MemoryConnection(const MemoryConnection & a) 
{
  _listOfPorts = a._listOfPorts;
}
MemoryConnection::~MemoryConnection(){}

Connection * MemoryConnection::copy()
{
  Connection *retPtr;
  retPtr = new MemoryConnection(*this);
  return retPtr;
}

MemoryConnection& MemoryConnection::operator=(const MemoryConnection& a)
{
  _listOfPorts = a._listOfPorts; 
  return *this;
}

void MemoryConnection::print(ostream &out){} 
Port * MemoryConnection::getPort(unsigned int portNum)
{ 
  return _listOfPorts[portNum];
}

void MemoryConnection::read(BaseType & value,BaseType & addr, unsigned int portNum)
{
  if(_READ_ == (_listOfPorts[portNum])->getType() || 
     _READWRITE_ == (_listOfPorts[portNum])->getType()) 
     (_listOfPorts[portNum])->read(value,addr);
   else
	  // Error condition.
	  cout << "Error: Not a read port" << endl;
   return;
}

void MemoryConnection::write(BaseType & data, BaseType & addr, 
			     unsigned int portNum)
{
  if(_READWRITE_ == (_listOfPorts[portNum])->getType() || 
     _WRITE_ == (_listOfPorts[portNum])->getType())
     (_listOfPorts[portNum])->write(data, addr);
}
 
void MemoryConnection::read(BaseType & value,BaseType & addr, SignalClass & latency, 
				 unsigned int portNum)
{
    (_listOfPorts[portNum])->read(value,addr,latency);
}

void MemoryConnection::read(BaseType & value, int memType, SignalClass & latency, 
				 BaseType & addr, unsigned int portNum)
{
    (_listOfPorts[portNum])->read(value,addr,latency, memType);
}

void MemoryConnection::write(BaseType & data, BaseType & addr, 
			     SignalClass & latency, unsigned int portNum)
{ 
    (_listOfPorts[portNum])->write(data,addr,latency);
}

// Each component has a name. Need to store that in a map.

Map<Connection *, MyString> globalConnectionNameList;
