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
 *file  : connection.h                                       *
 *authors : Ashok Halambi,vgee                               *
 *last modified :                                            *
 *							                                 *
 *                                                           *
 *contents:contains the class definition for the connections *
 *comments:-                                     *
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
#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "ArchClassIncludes.h"
#include "AddrData.h"
#include "Storage.h"
#include "Latch.h"
#include "Port.h"

/*************************************************************
 *class : Connection:please see comments above.              *
 *Methods:                                                   *
 *        copy()  :- creates a copy of this object           *
 *        is()    :- returns true if type of object is passed*
 *                   is the same as that of this class.      *
 *        ==      :- yesss, does exactly what u r thinking   *
 *        print() :- prints private data member values.      *
 *        isEq()  :- returns _NO_                            *
 *        addPort :-                                         *
 *                   adds a port to the list of ports that   *
 *                   Connections has.                        *
 *************************************************************/
class Connection : public BaseSimObject
{
 protected :
   PortList _listOfPorts;//list of ports connected to the connection.
 public:
   Connection();
   Connection(const Connection &a);
   ~Connection();

   virtual Connection * copy();
   virtual int is(ConnectionType ctype);
   friend  int operator ==(const Connection &a, const Connection &b);
   virtual void print(ostream &out);
   virtual void name(ostream &out);

   virtual int isEq(Connection * ptr);
   Connection& operator=(const Connection &a);
   
   virtual void addPort(Port * port);
   virtual void read(BaseType & value,BaseType & addr, unsigned int portNum = 0);
   virtual void read(BaseType & value);
//   virtual void read(BaseType &,BaseType &);
   virtual void read(BaseType &,BaseType &, SignalClass & latency, unsigned int portNum = 0);
   virtual void read(BaseType &,int memType, SignalClass & latency, BaseType &, unsigned int portNum = 0);
   virtual void write(BaseType & data, BaseType & addr, unsigned int portNum = 0);
   virtual void write(BaseType &);
   virtual void write(BaseType &, BaseType &, SignalClass & latency, unsigned int portNum = 0);    
};

/*************************************************************
 *class : RegisterConnection:A connection from the Unit to   *
 *        register file.                                     *
 *Methods:                                                   *
 *        copy()  :- creates a copy of this object           *
 *        is()    :- returns true if type of object is passed*
 *                   is the same as that of this class.      *
 *        ==      :- yesss, does exactly what u r thinking   *
 *        print() :- prints private data member values.      *
 *        isEq()  :- returns _NO_                            *
 *        addPort :-                                         *
 *                   adds a port to the list of ports that   *
 *                   Connections has.                        *
 *************************************************************/
class RegisterConnection : public Connection {
  public :
  RegisterConnection();
  RegisterConnection(Port * b);
  RegisterConnection(const RegisterConnection & a);
  ~RegisterConnection();

  virtual Connection *copy();
  friend  int operator ==(const Connection &a, const Connection &b);
  RegisterConnection & operator=(const RegisterConnection & a);
  virtual void print(ostream &out);

  virtual void read(BaseType & value,BaseType & addr, unsigned int portNum = 0);
  virtual void write(BaseType & data, BaseType & addr, unsigned int portNum = 0);
  
  virtual void read(BaseType & value,BaseType & addr, 
			 SignalClass & latency, unsigned int portNum = 0);
  virtual void write(BaseType & data, BaseType & addr, 
		     SignalClass & latency, unsigned int portNum = 0);
  Port * getPort(unsigned int portNum);
};

class MemoryConnection : public Connection {
  public :
  MemoryConnection();
  MemoryConnection(Port * b);
  MemoryConnection(const MemoryConnection & a);
  ~MemoryConnection();

  virtual Connection *copy();
  friend  int operator ==(const Connection &a, const Connection &b);
  MemoryConnection& operator=(const MemoryConnection& a);

  virtual void print(ostream &out);
  Port * getPort(unsigned int portNum);
  virtual void read(BaseType & value, BaseType & addr, unsigned int portNum = 0);
  virtual void write(BaseType & data, BaseType & addr, unsigned int portNum = 0);
  virtual void read(BaseType & value,BaseType & addr, 
			 SignalClass & latency, unsigned int portNum = 0);
  virtual void read(BaseType & value, int memType,
			 SignalClass & latency, BaseType & addr, unsigned int portNum = 0);
  virtual void write(BaseType & data, BaseType & addr, 
		     SignalClass & latency, unsigned int portNum = 0);
};
#endif

