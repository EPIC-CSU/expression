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
 *file  : Storage.h                                          *
 *authors : vgee,ashok				                         *
 *last modified :                                            *
 *                                                           *
 *contents : contains the storage class definitions  and     *
 *           associated classes.                             *
 *compiled with : g++                                        *
 *for more info :                                            *
 *caution : always grep for TODO to check for incompleteness *
 *************************************************************/
#ifndef _STORAGE_H_
#define _STORAGE_H_

#include "BaseSimObject.h"
#include "ArchGlobalIncludes.h"
#include "ArchClassIncludes.h"
#include "AddrData.h"
#include "Port.h"


#define BUSYLATENCY _INVALID_
class Port;

/*************************************************************
 *class : Storage.                                           *
 *description : this class defines the storage in a processor*
 *              the latches, memory, and register are derived*
 *              from this class.                             *
 *                                                           *
 *Methods :      doUpdate() updates the old values with new  *
 *Data Members :                                             *
 *ToDo List : isEq() has to be redefined. confirm if portlist*
 *            is needed for storage.                         *
 *************************************************************/
class Storage : public BaseSimObject
{
protected:
	Vector<Port *> _ports;
	
public:
	Storage();
	Storage(const Storage &);
	~Storage();
	
	virtual Storage * copy();
	virtual int is(StorageClassType ctype);
	friend  int operator ==(const Storage &a, const Storage & b);
	virtual void print(ostream &out) = 0;
	virtual void name(ostream &out);
	virtual void print(ostream & out, BaseType & addr){}
	
	virtual int isEq(Storage * ptr);
	Storage& operator=(const Storage &a);
	
	virtual void addPort(Port *);
	
	virtual void read();
	virtual void read(BaseType &);
	virtual void read(BaseType &,BaseType &);
	virtual void read(BaseType &, BaseType &, SignalClass & latency);
	virtual void read(BaseType &, BaseType &, SignalClass & latency, int memType);
	
	virtual void write(BaseType &);
	virtual void write(BaseType &, BaseType &); 
	virtual void write(BaseType &, BaseType &, SignalClass & latency);  
	virtual void doUpdate() = 0;

    virtual void reset() {}

	// The following methods are asynchronous writes that are used to initialize
    // memories (typically for static variables)
    //
    // The first method: initialize all address with the value specified.
    //
    // The second method: initialize the address with the data specified.
    //
    virtual void initialize(BaseType &);
    virtual void initialize(BaseType &, BaseType &); 

	
	// The next set of functions are only defined in certain derived classes.
	virtual void setProperty(int, int){}
	virtual int getProperty(int)
	{
		cout << "Panic: Base class method should never be called" << endl;
		assert(0);
		return -1;
	}
};


/*************************************************************
*class : RegFile                                            *
*description : This class defines the register file of the  *
*              processor. the type stored in the regfile is *
*              an AbstractDataType which is a collection of *
*              (array, list etc.) "Type". ensure that Type  *
*              derived from "BaseType". the SizeOfRegFile   *
*              gives you the number of elements in the      *
*              AbstractDataType.                            *
*The new changes are stored in a list,_newData and this list*
* is used in the end to update all the members of _data.    *
*note to consumers of this class:                           *
*              class Q should be of type AbstractDataType   *
*              class R should be of type BaseType           *
*                                                           *
*Methods :      doUpdate() updates the old values with new  *
*Data Members :                                             *
*ToDo List : isEq() has to be redefined. confirm if portlist*
*            is needed for storage.                         *
*            firstly,we have to create a derived class with *
*the following props: 1. ability to decide whether it is    *
*virtual or real. 2. add regs dynamically if virtual        *

*************************************************************/
template
<class DS, class R, class S>
class RegFile : public Storage {
protected :
	DS _regData;       // Actually DS<R,S> ; R is address type, S is datatype
	
	struct NewData{
		R regaddress;
		S regdata;
	};
	vector<NewData> _newDataList;
	vector<NewData>::iterator _newDataListIterBegin, _newDataListIterEnd;
	/*PortList  * _port; TODO. should we have a portlist in the
	storage??*/
	
	public :
		
		RegFile() {}
		~RegFile() {}
		RegFile(const RegFile<DS,R,S> & a) 
		{
			_regData = a._regData;

			vector<NewData>::const_iterator _newDataListIterBegin = a._newDataList.begin();
			vector<NewData>::const_iterator _newDataListIterEnd = a._newDataList.end();
			
			while (_newDataListIterBegin != _newDataListIterEnd)
			{
				_newDataList.push_back(*_newDataListIterBegin);
				_newDataListIterBegin++;
			}
		}
		RegFile(int size):_regData(size){}
		
		virtual Storage * copy()
		{
			Storage * retPtr;  
			retPtr = new RegFile<DS,R,S>(*this);
			return retPtr;
		}
		virtual int is(StorageClassType ctype) {return ((ctype == _REGISTER_) ? _YES_ :(Storage :: is(ctype)));}
		friend int operator ==(const RegFile<DS,R,S> &a, const RegFile<DS,R,S> &b) {return _NO_;}
		virtual void print(ostream & out)
		{
			out << "Printing register file" << endl;
			_regData.print(out);
			out << "End printing register file" << endl;
			
		}
		virtual void print(ostream & out, BaseType & addr)
		{
			S value;
			out << "Addr: ";
			addr.print(out);
			_regData.read(value,(R&)addr);
			out << " Data: ";
			value.print(out);
		}
		
		virtual int isEq(Storage * ptr) {return _NO_;}
		RegFile<DS,R,S>& operator=(const RegFile<DS,R,S> &a) {  _regData = a._regData; return *this;}
		
		virtual void read(BaseType & value, BaseType & addr) 
		{
			_regData.read((S&)value,(R&)addr);
		}
		
		virtual void write(BaseType & value, BaseType & addr) 
		{  
			NewData a;  
			a.regaddress = (R&)addr;
			a.regdata = (S&)value;         // a.regdata is of the type S , so a typecast
			
			/*******************
			if (debugSIMULATOR) {
				cout << "Printing write values: " << endl;
				cout << "Addr: " ;
				a.regaddress.print(cout);
				cout << endl;
				cout << "Data: ";
				a.regdata.print(cout);
				cout << endl;
			}
			*********************/
						
			_newDataList.push_back(a);
		}

		virtual void initialize(BaseType & value, BaseType & addr)
		{
			_regData.write((S&)value, (R&)addr);
		}
		
		virtual void doUpdate()
		{
			_newDataListIterBegin = _newDataList.begin();
			_newDataListIterEnd = _newDataList.end();
			
			while (_newDataListIterBegin != _newDataListIterEnd)
			{
				_regData.write((*_newDataListIterBegin).regdata,(*_newDataListIterBegin).regaddress);
				//         cout << "Data: ";
				//		 ((*_newDataListIterBegin).regdata).print(cout);
				//		 cout << "  Addr: ";
				//		 ((*_newDataListIterBegin).regaddress).print(cout);
				//		 cout << endl;
				_newDataListIterBegin++;
			}
			_newDataList.clear();
		}

		virtual void reset() { _regData.reset(); _newDataList.clear(); }
		
};

/*************************************************************
*class : Memory                                             *
*description : This class defines the Memory of a processor.*
*              Data is stored in the memory in an Abstract- *
*              DataType(class DS) which could be one of the *
*              following : Array, Vector, List, Queue ..... *
*              The Class DS can in turn be templated by     *
*              "Type" (class R,class S).                    *
*              Class R is the addressType (e.g. long), and  *
*              Class S is the data type (e.g. byte, int etc)*
*              Caution : Ensure that class R,S are derived  *
*              from class "BaseType".(AddrData.h)           *
*              The Size of Class DS is defined using the    *
*              constructor of the class Memory.             *
*              latency is a signal sent by memory to unit   *
*              which wants to read it. the memory may have  *
*              a latency associated with or some such attr  *
*              can be transfered to the unit trying to acces*
*note to consumers of this class:                           *
*              class DS should be of type Array/Vector/Map..*
*              class R,S should be derived from BaseType    *
*              DS,R and S need to provide member functions  *
*              mentioned below.                             *
*                                                           *
*Methods :      doUpdate() updates the old values with new  *
*Data Members :                                             *
*ToDo List : isEq() has to be redefined. confirm if portlist*
*            is needed for storage.                         *
*************************************************************/

/*************************************************************
* Following member functions must be supported by class DS: *
* void read(S & value, R & addr);                           *
* void write(S & value, R & addr);                          *
*                                                           *
* Following member functions must be supported by  class S: *
* void compose(Vector<S> & listOfData);                     *
*  -- compose the required 'data' from the 'units' stored in*
*     memory for e.g make int from bytes by putting together*
*     4 bytes                                               *
* void decompose(Vector<S> & listOfData);                   *
*  -- decompose a 'data' into the constituent 'units' for   *
*     storage in memory. e.g convert int into 4 bytes and   *
*     provide these bytes to the memory class for storage.  *
* void getListOfAddr(BaseType & addr,Vector<R> & listOfAddr)*
*  -- Given a starting address, this function fills the     *
*     listOfAddr with addresses where the constituent'units'*
*     (e.g bytes) for the requested data (e.g word, double) *
*     reside in the memory. e.g - addr,addr+1,addr+2,addr+3 *
*************************************************************/

template
<class DS,class R,class S>
class Memory : public Storage {
  protected :
	  DS _memory;        // Actually DS<R,S> ; R is address type; S is Data type
	  Vector<int> _properties;
	  bool _busyBit;
	  int _size;
#define  _readCycleLatency  _properties[0] //reserved. cant recast
#define  _writeCycleLatency _properties[1] //reserved. cant recast
	  
	  /*PortList  * _port; TODO. should we have a portlist in the
	  storage??*/
	  
  public :
	  Memory() {_busyBit = false;}
	  Memory(const Memory<DS,R,S>& a) 
	  {
		  _memory = a._memory; 
		  _readCycleLatency = a._readCycleLatency;
		  _writeCycleLatency = a._writeCycleLatency;
		  _busyBit = a._busyBit;
		  _size = a._size;
	  }
	  
	  Memory<DS,R,S>(int size): _memory(size) {_size = size; _busyBit = false;}
	  Memory<DS,R,S>(int size, Val & value): _memory(size, value) {_size = size; _busyBit = false;}
	  ~Memory() {}
	  
	  virtual Storage * copy()
	  {
		  Storage * retPtr;  
		  retPtr = new Memory<DS,R,S>(*this);
		  return retPtr;
	  }
	  
	  virtual int is(StorageClassType ctype){return ((ctype == _MEM_) ? _YES_ : (Storage :: is(ctype)));}
	  friend int operator ==(const Memory<DS,R,S> &a, const Memory<DS,R,S> &b) {return _NO_;}
	  virtual void print(ostream & out)
	  {
		  out << "Printing memory" << endl;
		  _memory.print(out);
		  out << "End printing memory" << endl;
		  
	  }
	  virtual void print(ostream & out, BaseType & addr)
	  {
		  out << "Addr: ";
		  addr.print(out);
		  
		  Vector<R> listOfAddr;
		  Vector<R>::iterator addrIterBegin, addrIterEnd;
		  Vector<S> listOfData;
		  
		  /* getListOfAddr function fills the vector listOfAddr 
		  with addresses in memory where the constituent 'units' 
		  of the data element are stored in the memory 
		  */
		  
		  S value;
		  value.getListOfAddr(addr,listOfAddr);
		  
		  addrIterBegin = listOfAddr.begin();
		  addrIterEnd = listOfAddr.end();
		  
		  
		  /* get the constituent 'units' from memory into a vector
		  listOfData 
		  */
		  
		  while(addrIterBegin != addrIterEnd) {
			  S a;
			  listOfData.push_back(a);
			  
			  _memory.read(listOfData.back(),*addrIterBegin);
			  addrIterBegin++;
		  }
		  
		  /* compose the data element from the data units */
		  
		  value.compose(listOfData);
		  
		  out << " Data: ";
		  value.print(out);
	  }
	  
	  Memory<DS,R,S>& operator=(const Memory<DS,R,S> &a) 
	  {  
		  _memory = a._memory;
		  Vector<int>::const_iterator i1;
		  i1 = (a._properties).begin();
		  while (i1 != (a._properties).end())
		  {
			  _properties.push_back(*i1);
			  i1++;
		  }
		  _busyBit = a._busyBit;
		  _size = a._size ;		  
		  return *this;
	  }
	  virtual int getProperty(int pos) {return _properties[pos];}
	  virtual void setProperty(int pos, int value)
	  {
		  int i = _properties.size();
		  
		  while (i <= pos)
		  {
			  _properties.push_back(-1);
			  i++;
		  }
		  _properties[pos] = value;
	  }
	  
	  bool getBusyBit() {return _busyBit;}
	  void setBusyBit(bool value) {_busyBit = value;}
	  virtual void read(BaseType & value, BaseType & addr, SignalClass & latency)
	  {
		  if(!_busyBit)
		  {
			  latency = _readCycleLatency;
			  
			  Vector<R> listOfAddr;
			  Vector<R>::iterator addrIterBegin, addrIterEnd;
			  Vector<S> listOfData;
			  
			  /* getListOfAddr function fills the vector listOfAddr 
			  with addresses in memory where the constituent 'units' 
			  of the data element are stored in the memory 
			  */
			  
			  ((S&)value).getListOfAddr(addr,listOfAddr);
			  
			  addrIterBegin = listOfAddr.begin();
			  addrIterEnd = listOfAddr.end();
			  
			  
			  /* get the constituent 'units' from memory into a vector
			  listOfData 
			  */
			  while(addrIterBegin != addrIterEnd) {
				  S a;
				  a = (Val&)value;	// so that the info coming through this parameter can be sent to the memory
				  listOfData.push_back(a);
				  
				  _memory.read(listOfData.back(),*addrIterBegin);
				  addrIterBegin++;
			  }
			  
			  /* compose the data element from the data units */
			  
			  ((S&)value).compose(listOfData);
			  return ;
		  }
		  else
		  {
			  latency = BUSYLATENCY;
			  return ;
		  }
	  }
	  
	  virtual void write(BaseType & value, BaseType & addr, SignalClass & latency)
	  {
		  if(!_busyBit)
		  {
			  latency = _writeCycleLatency;
			  
			  Vector<R> listOfAddr;
			  Vector<R>::iterator addrIterBegin, addrIterEnd;
			  Vector<S> listOfData;
			  Vector<S>::iterator dataIter;
			  
			  /* getListOfAddr function fills the vector listOfAddr 
			  with addresses in memory where the constituent 'units' 
			  of the data element are to be stored in the memory 
			  */
			  
			  ((S&)value).getListOfAddr(addr,listOfAddr);
			  
			  addrIterBegin = listOfAddr.begin();
			  addrIterEnd = listOfAddr.end();
			  
			  /* decompose the data element into its constituent units
			  for storage in the memory
			  */
			  
			  ((S&)value).decompose(listOfData);
			  dataIter = listOfData.begin();
			  
			  while(addrIterBegin != addrIterEnd) {
				  //_memory.write(S & value, R & Addr)
				  _memory.write(*dataIter, *addrIterBegin);
				  addrIterBegin++; dataIter++;
			  }
			  return ;
		  }
		  else
		  {
			  latency = BUSYLATENCY;
			  return ;
		  }
	  }

	  virtual void initialize(BaseType & value, BaseType & addr)
	  {
		  Vector<R> listOfAddr;
		  Vector<R>::iterator addrIterBegin, addrIterEnd;
		  Vector<S> listOfData;
		  Vector<S>::iterator dataIter;
		  
		  /* getListOfAddr function fills the vector listOfAddr 
		  with addresses in memory where the constituent 'units' 
		  of the data element are to be stored in the memory 
		  */
		  
		  ((S&)value).getListOfAddr(addr,listOfAddr);
		  
		  addrIterBegin = listOfAddr.begin();
		  addrIterEnd = listOfAddr.end();
		  
		  /* decompose the data element into its constituent units
		  for storage in the memory
		  */
		  
		  ((S&)value).decompose(listOfData);
		  dataIter = listOfData.begin();
		  
		  while(addrIterBegin != addrIterEnd) {
			  _memory.write(*dataIter, *addrIterBegin);
			  addrIterBegin++; dataIter++;
		  }
		  return ;
	  }


	  virtual void doUpdate() {}
	 
	  virtual void reset() { _memory.reset(); }

};
#endif