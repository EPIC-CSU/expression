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
 *file  : port.h                                             *
 *authors : S. Ashok Halambi,vgee                            *
 *last modified :                                            *
 *                                                           *
 *contents:contains the class definition for the ports       *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/
#ifndef _PORT_H_
#define _PORT_H_

#include "BaseSimObject.h"
#include "ArchClassIncludes.h"
#include "AddrData.h"
#include "Storage.h"
//#include "Latch.h"

class Connection;
typedef Vector<Connection *> ConnectionList;
typedef Vector<Connection *>::iterator ConnectionListIterator;

/*the read and write accept types of 'BaseType' */
/*************************************************************
 *class : Port                                               *
 *description : This class is the entry to a Storage. When a *
 *              wants to access a storage it invokes a conn- *
 *              ection which in turn invokes a port to read  *
 *              storage. this complies entirely with the hard*
 *              ware though this scheme may have degrading   *
 *              effect from efficiency point of view.        *
 *Methods :                                                  *
 *Data Members :                                             *
 *ToDo List :   //get the implementation verified            *               
 *************************************************************/
class Storage;

class Port : public BaseSimObject
{
 protected:
 Storage * _ptrToStorage; //TODO. should it be a single storage??
 PortType _typeOfPort;
 ConnectionList _connections;

 public:
 Port() {}
 Port(Storage * b, PortType a) : _ptrToStorage(b), _typeOfPort(a){}
 Port(PortType a) : _ptrToStorage(_NULL_), _typeOfPort(a){}
 ~Port(){}
 Port(const Port &a) : _ptrToStorage(a._ptrToStorage), _typeOfPort(a._typeOfPort)
 {}

 virtual Port * copy();
 virtual int is(PortType ctype)
 { return ((ctype == _PORT_) ? _YES_ : _NO_);}
 friend  int operator ==(const Port &a, const Port &b){return _NO_;}
 virtual void print(ostream &out);
 virtual void name(ostream &out);
 virtual int isEq(Port * ptr){return _NO_;}
 Port& operator=(const Port& a)
 {
  _ptrToStorage = a._ptrToStorage; 
  _typeOfPort = _typeOfPort;
  return *this;
 }
 virtual void addConnection(Connection *);
 virtual void addPtrToStorage(Storage *);

 virtual void setType(PortType a){_typeOfPort = a;}
 virtual PortType getType(){return _typeOfPort;}
 virtual void read(BaseType & a,BaseType & b);
 virtual void write(BaseType& a , BaseType & b);
 virtual void read(BaseType& value, BaseType & addr, SignalClass & latency);
 virtual void read(BaseType& value, BaseType & addr, SignalClass & latency, int memType);
 virtual void write(BaseType & data, BaseType & addr, SignalClass & latency);
};

typedef Vector<Port *> PortList;
typedef Vector<Port *>::iterator PortListIterator;
#endif

