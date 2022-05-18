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
 *file  : DerivedStorage.h                                   *
 *authors : ashok			                                 *
 *created : Thu Oct 8, 98                                    *
 *last modified : Mon Oct 12, 98                             *
 *                                                           *
 *contents: contains Data Structures for storage objects     *
 *compiled with : VC++                                       *
 *for more info :                                            *
 *************************************************************/

#ifndef _DERIVEDSTORAGE_H_
#define _DERIVEDSTORAGE_H_

#include "DerivedClassIncludes.h"
#include "..\..\pcProDll\DebugSwitches.h"
#include "CompName.h"
//#include "DerivedAddrData.h"
#include "MyString.h"
#include "Cache.h"

 /*
#include "RegisterFileList.h"
#include "RegisterFile.h"
#include "SymbolTable.h"
*/
/*
extern RegisterFileList *globalRegFileList;
extern SymbolTable *globalSymbolTable;
*/

class MyDeque{
private:
	
	struct Memstruct
	{
		AddrOperand _addr;
		Val _val;
		
		Memstruct(){}
		~Memstruct(){}
		Memstruct(const Memstruct & a)
		{
			_addr = a._addr;
			_val = a._val;
			
		}
		
		void operator=(const Memstruct & a)
		{
			_addr = a._addr;
			_val = a._val;
		}
	};
	
	deque<Memstruct> _myqueue;
	
	public:
		
	MyDeque() {}
	MyDeque(const MyDeque & a) {_myqueue = a._myqueue;}
	~MyDeque() {}
	void read(Val & value, AddrOperand & addr)
	{
		deque<Memstruct>::iterator i1;
		i1 = _myqueue.begin();
		while (i1 != _myqueue.end())
		{
			if ((*i1)._addr == addr)
			{
				break;
			}
			i1++;
		}

		Val dataToBeRead ;

		//int address = addr.convertAddrMechanism();
				
		if (i1 != _myqueue.end())
		{
			dataToBeRead = i1->_val;	
			// Do register file write to transfer data from mem to regfile for  this load operation
			if (value.getType() == _INTERNAL_LABEL)
			{
				int dstRegAddress = value._as.integer;
				AddrOperand tempAddr(_REG_,dstRegAddress);

				//int RegFile = getRegFileType(dstRegAddress);				
				FieldType fType = addr.getFieldType();

				MyString WrIRFportname("MemWrPort1");
				MyString WrFRFportname("MemWrPort2");
				
				if (fType == _ADDR_INT_)
				{
					Port * port = (Port*)getComp(WrIRFportname);
					port->write(dataToBeRead,tempAddr);
				}
				else if ((fType == _ADDR_FLT_)||(fType == _ADDR_DBL_))
				{
					Port * port = (Port*)getComp(WrFRFportname);
					port->write(dataToBeRead,tempAddr);
				}
				else
				{
					cout << "Eror: MyDeque.read() - Invalid register File Name" << endl;
					assert(0);
				}
			}
		}
		
		value = dataToBeRead;
		
		//setTypeOfVal(value, addr);
		//
		// Note: the type of the value should preferably be set in the Memory class
		// and not in the data-structure MyMap.
		//
		// This is a hack, cos as of now, we do not have a separate Memory class for 
		// virtual memories.
		
		//_mymap[address]._valType = value._valType;
		
		if (debugSIMULATOR)
		{
			cout << "Printing DEQUE READ values: " << endl;
			cout << "Addr: " ;
			addr.print(cout);
			//cout << endl;
			cout << "Data: ";
			value.print(cout);
			cout << endl; 
		}
		
	}
	
	void write(Val & value, AddrOperand & addr){
		
		Val dataToBeStored;
		// Do register file read to get the data for  this store operation
		if (value.getType() == _INTERNAL_LABEL)
		{
			int srcRegAddress = value._as.integer;
			AddrOperand tempAddr(_REG_,srcRegAddress);
			
			//int RegFile = getRegFileType(srcRegAddress);
			FieldType fType = addr.getFieldType();
			tempAddr.setFieldType(fType);
			
			MyString RdIRFportname("MemRdPort1");
			MyString RdFRFportname("MemRdPort2");
			
			//if (RegFile == _INT_REG_FILE)
			if (fType == _ADDR_INT_)
			{
				Port * port = (Port*)getComp(RdIRFportname);
				port->read(dataToBeStored,tempAddr);
			}
			else if ((fType == _ADDR_FLT_) || (fType == _ADDR_DBL_))
			{
				Port * port = (Port*)getComp(RdFRFportname);
				port->read(dataToBeStored,tempAddr);
			}
			else
			{
				cout << "Eror: MyDeque.read() - Invalid register File Name" << endl;
				assert(0);
			}
		}
		else
			dataToBeStored = (Val &)value;
		
		Memstruct tempdata;
		tempdata._val = dataToBeStored;
		tempdata._addr = addr;
		_myqueue.push_back(tempdata);
		
		if (debugSIMULATOR)
		{
			cout << "Printing DEQUE WRITE values: " << endl;
			cout << "Addr: " ;
			addr.print(cout);
			//cout << endl;
			cout << "Data: ";
			dataToBeStored.print(cout);
			cout << endl; 
		}   
	}

	void deleteHead(Val & value, AddrOperand & addr)
	{
		if (!_myqueue.empty())
		{
			Memstruct tempdata = _myqueue.front();
			value = tempdata._val;
			addr = tempdata._addr;
			_myqueue.pop_front();
		}
	}

	void print(ostream & out)
	{
	}

	void print(ostream & out, BaseType & addr)
	{
	}

	void clearBuffer()
	{
		_myqueue.clear();
	}

	void reset()
	{
		_myqueue.clear();
	}
};

/*************************************************************
*class : BufferedMemory                                     *
*description : This class defines the Buffered Memory of a  *
*              processor, used for modeling memory with     *
*              speculative loads and stores                 *
*                                                           *
*              Writes go to the FIFO buffer. Reads first    *
*              read from the buffer and if data is not      *
*              present then read from the memory array      *
*              doUpdate moves data from the buffer to the   *
*              memory array                                 *
*                                                           *
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
*                                                           *
*note to consumers of this class:                           *
*              class DS should be of type Array/Vector/Map..*
*              class R,S should be derived from BaseType    *
*              DS,R and S need to provide member functions  *
*              mentioned below.                             *
*                                                           *
*Methods :      doUpdate() updates the old values with new  *
*Data Members :                                             *
*ToDo List :                                                *
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



typedef vector<int> Nways; // Captures which datacahce is least recently used

template
<class DS, class DSB, class R,class S>
class BufferedMemory : public Storage {
  protected :
	  Memory<DS,R,S> _dataCache0; // Actually DS<R,S> ; R is address type; S is Data type
	  Memory<DS,R,S> _dataCache1; // Actually DS<R,S> ; R is address type; S is Data type
	  Memory<DS,R,S> _tagCache0;  // Actually DS<R,S> ; R is address type; S is Data type
	  Memory<DS,R,S> _tagCache1;  // Actually DS<R,S> ; R is address type; S is Data type

	  DSB _buffer;                // Actually DS<R,S> ; R is address type; S is Data type

	  map <int, Nways> LRUArray;  // Maintains a table of LRU informations for all the blocks
	                              // in the datacache. Key is tagAddr. Content is Nways, which
	                              // maintains the most recently accessed dataCache info.
	                              // Set the accessed dataCache to 1 and other to 0. So in case
	                              // of replacement, replace the the block marked as 0.
	  
#define  _readLatency  0 //reserved. cant recast
#define  _writeLatency 1 //reserved. cant recast 
	  


  public :
	  BufferedMemory() {}
	  BufferedMemory(const BufferedMemory<DS,DSB,R,S>& a) 
	  {
		  _dataCache0 = a._dataCache0; 
		  _dataCache1 = a._dataCache1;
		  _tagCache0 = a._tagCache0;
		  _tagCache1 = a._tagCache1;

		  _buffer = a._buffer; 
	  }
	  
	  BufferedMemory<DS,DSB,R,S>(int datasize, int tagsize)
	  {
		  // Initialize with '_ANY' which indicates invalid data for both tag as well as cache
		  Val value;
		  value.setType(_ANY);

		  _dataCache0 = Memory<DS, R, S>(datasize, value);
		  _dataCache1 = Memory<DS, R, S>(datasize, value);
		  _tagCache0 = Memory<DS, R, S>(tagsize, value);
		  _tagCache1 = Memory<DS, R, S>(tagsize, value);
	  }

	  ~BufferedMemory() {}
	  
	  virtual Storage * copy()
	  {
		  Storage * retPtr;  
		  retPtr = new BufferedMemory<DS,DSB,R,S>(*this);
		  return retPtr;
	  }
	  
	  //virtual int is(StorageClassType ctype){return ((ctype == _MEM_) ? _YES_ : (Storage :: is(ctype)));}
	  friend int operator ==(const BufferedMemory<DS,DSB,R,S> &a, const BufferedMemory<DS,DSB,R,S> &b) {return _NO_;}
	  virtual void print(ostream & out)
	  {
		  out << "Printing Data Cahe 0" << endl;
		  _dataCache0.print(out);
		  out << "Printing Data Cahe 1" << endl;
		  _dataCache1.print(out);
		  out << "Printing Tag Cahe 0" << endl;
		  _tagCache0.print(out);
		  out << "Printing Tag Cahe 1" << endl;
		  _tagCache1.print(out);
		  out << "Printing buffer" << endl;
		  _buffer.print(out);
		  out << "End printing memory" << endl;
		  
	  }

	  virtual void print(ostream & out, BaseType & addr)
	  {
		  _dataCache0.print(out,addr);
		  _dataCache1.print(out,addr);
          _tagCache0.print(out,addr);
		  _tagCache1.print(out,addr);
		  _buffer.print(out,addr);
	  }
	  
	  BufferedMemory<DS,DSB,R,S>& operator=(const BufferedMemory<DS,DSB,R,S> &a) 
	  {  
		  _dataCache0 = a._dataCache0; 
		  _dataCache1 = a._dataCache1;
		  _tagCache0 = a._tagCache0;
		  _tagCache1 = a._tagCache1;

		  _buffer = a._buffer;

		  return *this;
	  }

	 
	  virtual int getProperty(int pos) {return _dataCache0.getProperty(pos);}

	  virtual void setProperty(int pos, int value)
	  {
		  _dataCache0.setProperty(pos,value);
		  _dataCache1.setProperty(pos,value);
		  _tagCache0.setProperty(pos,value);
		  _tagCache1.setProperty(pos,value);
	  }
	  
	  
	  //bool getBusyBit() {return _buffmem.getBusyBit();}
	  //void setBusyBit(bool value) {_buffmem.setBusyBit(value);}
	  virtual void read(BaseType & value, BaseType & addr, SignalClass & latency)
	  {
		  //Only reads from buffer if available.
		  
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
			  a = (S&)value;	// so that the info coming through this parameter can be sent to the memory
			  listOfData.push_back(a);
			  
			  _buffer.read(listOfData.back(),*addrIterBegin);
			  addrIterBegin++;
		  }
		  
		  /* compose the data element from the data units */
		  
		  ((S&)value).compose(listOfData);
		 
		  // Latency = Primary Cache read time
		  latency = _dataCache0.getProperty(_readLatency);		   
	  }


	  virtual void read(BaseType & value, BaseType & addr, SignalClass & latency, int memType)
	  {
		  // If it is refill then INITIATE CACHE REFILL and return.
		  if (memType == 999)
		  {
			  int address = ((R &)addr).getAddrOperand();
			  RefillPrimaryCacheBlock(address, latency);

			  // Latency = Primary Cache refill time + Primary Cache Read time
			  latency += _dataCache0.getProperty(_readLatency);
			  return;
		  }

		  //first read from buffer. If not present in buffer, then read from memory
		  
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
			  a = (S&)value;	// so that the info coming through this parameter can be sent to the memory
			  listOfData.push_back(a);
			  
			  _buffer.read(listOfData.back(),*addrIterBegin);
			  addrIterBegin++;
		  }
		  
		  /* compose the data element from the data units */
		
		  ((S&)value).compose(listOfData);
		  
		  latency = _dataCache0.getProperty(_readLatency);

		  S temp;
		  if (((S&)value).getType() == temp.getType())
		  {
			  // TODO: how to get hold of address field. I have split .......
			  // Virtual Address is 64 bits, 0-10 is used to address dataCache, 2-10 is
			  // is used to address tagCache, 11-39 is used as the content of the
			  // tagCache. Data Cahe size is 2048, tagCache size is 512.
			  
			  S tagValue;
			  R tagAddr;
			  R dataAddr;
			  
			  extractTagValueAndTagAddrFromVirtualAddr((R &)addr, dataAddr, tagValue, tagAddr);
			 
			  int index = tagAddr.getAddrOperand();

			  switch(memType)
			  {
			    case 0:
				{
					// Read the data.
					_dataCache0.read(value,dataAddr,latency);
				
					// If valid data, mark LRUArray to account for this access.					
					if (((S&)value).getType() != _ANY)
					{
						LRUArray[index][0] = 1; // Mark dataCache0 as MRU/read
						LRUArray[index][1] = 0; // UnMark dataCache1 as LRU
					}
					break;
				}

				case 1: 
				{
					// Read the data
					_dataCache1.read(value,dataAddr,latency);

					// If valid data, mark LRUArray to account for this access.					
					if (((S&)value).getType() != _ANY)
					{					
						LRUArray[index][1] = 1; // Mark dataCache1 as MRU/read
						LRUArray[index][0] = 0; // UnMark dataCache0 as LRU
					}
					break;
					break;
				}

			    case 2:  
				{
					_tagCache0.read(value,tagAddr,latency);
					break;
				}

			    case 3:  
				{
					_tagCache1.read(value,tagAddr,latency);
					break;
				}
			  }
		  }
	  }
	  

	  virtual void write(BaseType & value, BaseType & addr, SignalClass & latency)
	  {			  
		  // always write to buffer
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
			  _buffer.write(*dataIter, *addrIterBegin);
			  addrIterBegin++; dataIter++;
		  }

		  // Latency = Primary Cache write time
		  latency = _dataCache0.getProperty(_writeLatency);
	  }
	  

	  virtual void doUpdate() 
	  {
		  // move data from the buffer to the main array
		  S value; R addr; SignalClass latency;
		  _buffer.deleteHead(value, addr);
		  // check if valid data has been received from the buffer
		  R temp;
		  if (temp == addr)
			  ;
		  else
		  {
			  // TODO: how to get hold of address field. I have split .......
			  // Virtual Address is 64 bits, 0-10 is used to address dataCache, 2-10 is
			  // is used to address tagCache, 11-39 is used as the content of the
			  // tagCache. Data Cahe size is 2048, tagCache size is 512.
			  
			  S tagValue;
			  R tagAddr;
			  R dataAddr;
			  
			  extractTagValueAndTagAddrFromVirtualAddr(addr, dataAddr, tagValue, tagAddr);
			  
			  // We have 'dataAddr' and 'value' for the dataCache and 
			  // 'tagAddr' and 'tagValue' for the tagCache. 
			  // Check if the entry is free to be written for _dataCache0 by checking type = _ANY
			  Val prevData1Val;
			  Val prevTag1Val;
			  
			  _dataCache0.read(prevData1Val, dataAddr, latency);
			  _tagCache0.read(prevTag1Val, tagAddr, latency);
			  
			  // If it's free (_ANY) fine. If not free, then the address for the existing data is
			  // same as the address of the current data to be written.
			  if (prevTag1Val.getType() == _ANY)
			  {
				  // First time writing into DataCache0
				  _dataCache0.write(value,dataAddr,latency);
				  _tagCache0.write(tagValue,tagAddr,latency);
				  return;
			  }
			  else if (prevTag1Val == tagValue)
			  {
				  // Not the first time. May be new entry or modifying an existing one.
				  _dataCache0.write(value,dataAddr,latency);
				  return;
			  }
			  
			  
			  Val prevData2Val;
			  Val prevTag2Val;
			  
			  // _dataCache0 is busy. Check _dataCahe1
			  _dataCache1.read(prevData2Val, dataAddr, latency);
			  _tagCache1.read(prevTag2Val, tagAddr, latency);
			  
			  // If it's free (_ANY) fine. If not free, then the address for the existing data is
			  // same as the address of the current data to be written.
			  if (prevTag2Val.getType() == _ANY)
			  {
				  // First time writing into DataCache1
				  _dataCache1.write(value,dataAddr,latency);
				  _tagCache1.write(tagValue,tagAddr,latency);
				  return;
			  }
			  else if (prevTag2Val == tagValue)
			  {
				  // Not the first time. May be new entry or modifying an existing one.
				  _dataCache1.write(value,dataAddr,latency);
				  return;
			  }
			  
			  // Both the caches has valid data. Replace the Least Recently Used (LRU) block
			  // and then write the data there. This involves maintainance of a dynamic
			  // list for LRU. TODO: LRU implementation/replacement/write and adjust latency.
			  
			  int replaceBlock = ReplacePrimaryCacheBlock(addr.getAddrOperand(), latency);
			  
			  // write now.
			  if (replaceBlock == 0)
			  {
				  // dataCache0
				  _dataCache0.write(value,dataAddr,latency);
				  _tagCache0.write(tagValue,tagAddr,latency);
			  }
			  else 
			  {
				  // dataCache1
				  _dataCache1.write(value,dataAddr,latency);
				  _tagCache1.write(tagValue,tagAddr,latency);
			  }
		  }
	  }

	  

	  void clearBuffer()
	  {
		  _buffer.clearBuffer();
	  }

	  void reset()
	  {
		  _dataCache0.reset();
		  _dataCache1.reset();
		  _tagCache0.reset();
		  _tagCache1.reset();
		  _buffer.reset();
	  }

	  void extractTagValueAndTagAddrFromVirtualAddr(AddrOperand addr, AddrOperand & dataAddr, Val & tagVal, AddrOperand & tagAddr)
	  {
		  long address = addr.getAddrOperand();

		  long dataAddress = address;
		  dataAddress = dataAddress & _DATA_ADDR_MASK_;
		  dataAddr.setAddrOperand(_DIRECTMEMORY_, dataAddress);
		  dataAddr.setFieldType(addr.getFieldType());

		  long tagAddress = dataAddress >> _ADDR_SHIFT_FOR_DATA_TAG_;
		  tagAddr.setAddrOperand(_DIRECTMEMORY_, tagAddress);
		  tagAddr.setFieldType(_ADDR_INT_);

		  long tagValue = address;
		  tagValue = tagValue >> _DATA_CACHE_ADDR_SIZE_;
		  tagValue = tagValue & _TAG_VALUE_MASK_;
		  tagVal._as.integer = tagValue;
		  tagVal.setType(_INT);


		  //cout << "Address: " << address << "  dataAddr: " << dataAddress << "  tagAddr: " << tagAddress << "  Tag: " << tagValue << endl;
	  }


	  int ReplacePrimaryCacheBlock(int physicalAddress, SignalClass & latency)
	  {
		  S tagValue;
		  R dataAddr;
		  int dataAddress;
		  int tagAddress;
		  
	  	  R tagAddr;
		  tagAddress = (physicalAddress >> _ADDR_SHIFT_FOR_DATA_TAG_) & _TAG_ADDR_MASK_;
		  tagAddr.setAddrOperand(_DIRECTMEMORY_, tagAddress);

		  // Replace the least recently used(LRU) block.
		  int replace = 1;
		  
		  if (LRUArray[tagAddress][0] == 0)
			  replace = 0; // dataCache0 is LRU.

		  // Read tagValue;
		  if (replace == 0)
			  _tagCache0.read(tagValue, tagAddr, latency);		  
		  else
			  _tagCache1.read(tagValue, tagAddr, latency);
		  
		  // Return if invalid tagValue, nothing to replace
		  if (tagValue.getType() == _ANY)
			  return replace;
		 
		  // |<------------ Physical Address -------------------------->|
		  // |<--- tagValue ---->|<---- tagAddr ----->|<--- ...... ---->|
		  //                     |<-------- data address -------------->|
		  //                     
		  int tempAddress = (tagValue._as.integer << _DATA_CACHE_ADDR_SIZE_);    // tagValue
		  tempAddress = tempAddress | (tagAddress << _ADDR_SHIFT_FOR_DATA_TAG_); // tagAddr
		  
		  if (debugSIMULATOR)
			cout << "REPLACE :: Primary Cache -> Secondary Cache" << endl;

		  AddrOperand physicalAddr;
		  int entries;
		  for (entries = 0; entries < _DATA_CACHE_BLOCK_SIZE_ * 2; entries++) // Since address increment by 4 bytes
		  {
			  physicalAddress = tempAddress;       
			  physicalAddr.setAddrOperand(_DIRECTMEMORY_, physicalAddress);
			   
			  dataAddress = physicalAddress & _DATA_ADDR_MASK_;
			  dataAddr.setAddrOperand(_DIRECTMEMORY_, dataAddress);
			  
			  S dataValue;
			  
			  if (replace == 0)
				  _dataCache0.read(dataValue, dataAddr, latency);
			  else
				  _dataCache1.read(dataValue, dataAddr, latency);
			  
			  // if Valid data, write it in secondary memory and reset the current location
			  if (dataValue.getType() != _ANY)
			  {
				  PORT_ELEM(4)->write(dataValue, physicalAddr, latency); // write to secondary memory

				  if (debugSIMULATOR)
				  {
					  cout << "       Address: " << physicalAddress << "      Data: ";
					  dataValue.print(cout);
					  cout << "      Cache: " << replace << endl;
				  }
				  
				  S temp;
				  
				  if (replace == 0)
					  _dataCache0.write(temp, dataAddr, latency); // reset to _ANY
				  else
					  _dataCache1.write(temp, dataAddr, latency); // reset to _ANY
			  }

			  tempAddress = tempAddress + 4;  // For byte address the alignment
		  }
		  
		  return replace;
	  }
	  	  
	  
	  void RefillPrimaryCacheBlock(int physicalAddress, SignalClass & latency)
	  {
		  S tagValue;
		  R dataAddr;
		  R tagAddr;
		  int dataAddress;
		  int tagAddress;

		  // Replace the least recently used(LRU) block 
		  int refillBlock = ReplacePrimaryCacheBlock(physicalAddress, latency);
		  
		  // From the tag addesss generate dataAddress. First generate the cache data address
		  // for the block and then add tagValue to generate physical address.
		 
		  R physicalAddr;
		  int tempAddress = physicalAddress & _DATA_TAG_SHIFT_MASK_;
		  tagAddress = (physicalAddress >> _ADDR_SHIFT_FOR_DATA_TAG_) & _TAG_ADDR_MASK_;
		  tagAddr.setAddrOperand(_DIRECTMEMORY_, tagAddress);

		  tagValue._as.integer = physicalAddress >> _DATA_CACHE_ADDR_SIZE_;
		  tagValue.setType(_INT);

		  if (refillBlock == 0)
			  _tagCache0.write(tagValue, tagAddr, latency);
		  else
			  _tagCache1.write(tagValue, tagAddr, latency);


		  int entries;

		  if (debugSIMULATOR)
			cout << "REFILLL :: Secondary Cache -> Primary Cache" << endl;
		  
		  int Latency = _dataCache0.getProperty(_readLatency);

		  for (entries = 0; entries < _DATA_CACHE_BLOCK_SIZE_ * 2; entries++) // Since address increment is 4 bytes
		  {
			  physicalAddress = tempAddress;
			  physicalAddr.setAddrOperand(_DIRECTMEMORY_, physicalAddress);
			  			  
			  S dataValue;
			  
			  // Read from secondary cache
			  PORT_ELEM(4)->read(dataValue, physicalAddr, latency);

			  if (latency > Latency)
				  Latency = latency;

			  if (dataValue.getType() != _ANY)
			  {
				  if (debugSIMULATOR)
				  {
					  cout << "       Address: " << physicalAddress << "      Data: ";
					  dataValue.print(cout);
					  cout << "      Cache: " << refillBlock << endl;
				  }
				  
				  dataAddress = physicalAddress & _DATA_ADDR_MASK_;
				  dataAddr.setAddrOperand(_DIRECTMEMORY_, dataAddress);
				  
				  // Refill the entry and update the LRU information
				  if (refillBlock == 0)
				  {
					  _dataCache0.write(dataValue, dataAddr, latency);
					  LRUArray[tagAddress][0] = 1;
					  LRUArray[tagAddress][1] = 0;
					  
				  }
				  else
				  {
					  _dataCache1.write(dataValue, dataAddr, latency);
					  LRUArray[tagAddress][0] = 0;
					  LRUArray[tagAddress][1] = 1;
				  }
			  }

			  tempAddress = tempAddress + 4; // For byte address memory alignment
		  }
		  latency = Latency;

	  }

	  
};



/************************************************************
*class : SecondaryMemory                                    *
*description : This class defines the Secondary Memory of a *
*              processor, used for modeling cache hierarchy *
*                                                           *
*                                                           *                            
*                                                           *
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
*                                                           *
*note to consumers of this class:                           *
*              class DS should be of type Array/Vector/Map..*
*              class R,S should be derived from BaseType    *
*              DS,R and S need to provide member functions  *
*              mentioned below.                             *
*                                                           *
*Methods :      doUpdate() updates the old values with new  *
*Data Members :                                             *
*ToDo List :                                                *
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

extern int _secondaryCacheAccess_;
extern int _secondaryCacheMiss_;

template
<class DS, class DSB, class R,class S>
class SecondaryMemory : public Storage {
  protected :
	  Memory<DS,R,S> _dataCache0; // Actually DS<R,S> ; R is address type; S is Data type
	  Memory<DS,R,S> _dataCache1; // Actually DS<R,S> ; R is address type; S is Data type
	  Memory<DS,R,S> _tagCache0;  // Actually DS<R,S> ; R is address type; S is Data type
	  Memory<DS,R,S> _tagCache1;  // Actually DS<R,S> ; R is address type; S is Data type
 
	  map <int, Nways> LRUArray;  // Maintains a table of LRU informations for all the blocks
	                              // in the datacache. Key is tagAddr. Content is Nways, which
	                              // maintains the most recently accessed dataCache info.
	                              // Set the accessed dataCache to 1 and other to 0. So in case
	                              // of replacement, replace the the block marked as 0.
	  
#define  _readLatency  0 //reserved. cant recast
#define  _writeLatency 1 //reserved. cant recast 
	  


  public :
	  SecondaryMemory() {}
	  SecondaryMemory(const SecondaryMemory<DS,DSB,R,S>& a) 
	  {
		  _dataCache0 = a._dataCache0; 
		  _dataCache1 = a._dataCache1;
		  _tagCache0 = a._tagCache0;
		  _tagCache1 = a._tagCache1;
	  }
	  
	  SecondaryMemory<DS,DSB,R,S>(int datasize, int tagsize)
	  {
		  // Initialize with '_ANY' which indicates invalid data for both tag as well as cache
		  Val value;
		  value.setType(_ANY);

		  _dataCache0 = Memory<DS, R, S>(datasize, value);
		  _dataCache1 = Memory<DS, R, S>(datasize, value);
		  _tagCache0 = Memory<DS, R, S>(tagsize, value);
		  _tagCache1 = Memory<DS, R, S>(tagsize, value);
	  }

	  ~SecondaryMemory() {}
	  
	  virtual Storage * copy()
	  {
		  Storage * retPtr;  
		  retPtr = new SecondaryMemory<DS,DSB,R,S>(*this);
		  return retPtr;
	  }
	  
	  //virtual int is(StorageClassType ctype){return ((ctype == _MEM_) ? _YES_ : (Storage :: is(ctype)));}
	  friend int operator ==(const SecondaryMemory<DS,DSB,R,S> &a, const SecondaryMemory<DS,DSB,R,S> &b) {return _NO_;}
	  virtual void print(ostream & out)
	  {
		  out << "Printing Data Cahe 0" << endl;
		  _dataCache0.print(out);
		  out << "Printing Data Cahe 1" << endl;
		  _dataCache1.print(out);
		  out << "Printing Tag Cahe 0" << endl;
		  _tagCache0.print(out);
		  out << "Printing Tag Cahe 1" << endl;
		  _tagCache1.print(out);
		  out << "Printing buffer" << endl;
	  }

	  virtual void print(ostream & out, BaseType & addr)
	  {
		  _dataCache0.print(out,addr);
		  _dataCache1.print(out,addr);
          _tagCache0.print(out,addr);
		  _tagCache1.print(out,addr);
	  }
	  
	  SecondaryMemory<DS,DSB,R,S>& operator=(const SecondaryMemory<DS,DSB,R,S> &a) 
	  {  
		  _dataCache0 = a._dataCache0; 
		  _dataCache1 = a._dataCache1;
		  _tagCache0 = a._tagCache0;
		  _tagCache1 = a._tagCache1;
 
		  return *this;
	  }

	 
	  virtual int getProperty(int pos) {return _dataCache0.getProperty(pos);}

	  virtual void setProperty(int pos, int value)
	  {
		  _dataCache0.setProperty(pos,value);
		  _dataCache1.setProperty(pos,value);
		  _tagCache0.setProperty(pos,value);
		  _tagCache1.setProperty(pos,value);
	  }
	  
	  
	  //bool getBusyBit() {return _buffmem.getBusyBit();}
	  //void setBusyBit(bool value) {_buffmem.setBusyBit(value);}

	  virtual void read(BaseType & value, BaseType & addr, SignalClass & latency)
	  {
		   S tagValue;
		   R tagAddr;
		   R dataAddr;
			  
		   extractTagValueAndTagAddrFromVirtualAddr((R &)addr, dataAddr, tagValue, tagAddr);
			 
		   int index = tagAddr.getAddrOperand();

		   // Read data from both dataCache and decide the correct data based on
		   // tagCache value

		   S dataValue0;
		   S dataValue1;
		   
		   _dataCache0.read(dataValue0,dataAddr,latency);
		   _dataCache1.read(dataValue1,dataAddr,latency);


		   S tagValue0;
		   S tagValue1;

		   _tagCache0.read(tagValue0,tagAddr,latency);
		   _tagCache1.read(tagValue1,tagAddr,latency);

		   // Increment the access count
		   _secondaryCacheAccess_++;

		   // Latency = Secondary Cache read time
		   latency = _dataCache0.getProperty(_readLatency);

		   // Decide the hit way
		   if ((tagValue0.getType() != _ANY) && 
			   (tagValue0._as.integer == tagValue._as.integer)) 
		   {
			   // Hit way 0
			   (S &)value = dataValue0; 
			   
			   // If valid data, mark LRUArray to account for this access.					
			   if (((S&)value).getType() != _ANY)
			   {
				   LRUArray[index][0] = 1; // Mark dataCache0 as MRU/read
				   LRUArray[index][1] = 0; // UnMark dataCache1 as LRU
			   }
		   }
		   else if ((tagValue1.getType() != _ANY) && 
			        (tagValue1._as.integer == tagValue._as.integer))
		   {
			   // Hit way 1
			   (S &)value = dataValue1;
			   
			   // If valid data, mark LRUArray to account for this access.					
			   if (((S&)value).getType() != _ANY)
			   {
				   LRUArray[index][0] = 0; // Mark dataCache0 as MRU/read
				   LRUArray[index][1] = 1; // UnMark dataCache1 as LRU
			   }
		   }
		   /****************
		   else	if (tagValue0.getType() == _ANY)
		   {
			   // This memory location was not written earlier but read now.

			   // Hit way 0
			   (S &)value = dataValue0; 
		   }
		   else	if (tagValue1.getType() == _ANY)
		   {
			   // This memory location was not written earlier but read now.

			   // Hit way 1
			   (S &)value = dataValue1; 
		   }
		   **********/
		   else
		   {
			   // MISS

			   // Increment miss count
			   _secondaryCacheMiss_++;

			   // Initiate Secondary Cache Refill. Main Memory -> secondary Cache

			   int address = ((R &)addr).getAddrOperand();
			   int refillBlock = RefillSecondaryCacheBlock(address, latency);

			   // Latency = Secondary Cache refill time + Secondary Cache read time
			   latency = latency + _dataCache0.getProperty(_readLatency);

			   int dummy;

			   // Read now
			   if (refillBlock == 0)
			   {
				   // Data Cache 0
				   _dataCache0.read(value,dataAddr,dummy);

			   }
			   else
			   {
				   // Data Cache 1
				   _dataCache1.read(value,dataAddr,dummy);
			   }

		   }		   
	  }
	  

	  virtual void write(BaseType & value, BaseType & addr, SignalClass & latency)
	  {		  
		  S tagValue;
		  R tagAddr;
		  R dataAddr;
		  
		  extractTagValueAndTagAddrFromVirtualAddr((R &)addr, dataAddr, tagValue, tagAddr);
		  
		  // We have 'dataAddr' and 'value' for the dataCache and 
		  // 'tagAddr' and 'tagValue' for the tagCache. 
		  // Check if the entry is free to be written for _dataCache0 by checking type = _ANY
		  Val prevData1Val;
		  Val prevTag1Val;
		  
		  _dataCache0.read(prevData1Val, dataAddr, latency);
		  _tagCache0.read(prevTag1Val, tagAddr, latency);

		  // Latency = Secondary Cache write time
		  latency = _dataCache0.getProperty(_writeLatency);
		  
		  // If it's free (_ANY) fine. If not free, then the address for the existing data is
		  // same as the address of the current data to be written.
		  if (prevTag1Val.getType() == _ANY)
		  {
			  // First time writing into DataCache0
			  _dataCache0.write(value,dataAddr,latency);
			  _tagCache0.write(tagValue,tagAddr,latency);
			  return;
		  }
		  else if (prevTag1Val == tagValue)
		  {
			  // Not the first time. May be new entry or modifying an existing one.
			  _dataCache0.write(value,dataAddr,latency);
			  return;
		  }
		  
		  
		  Val prevData2Val;
		  Val prevTag2Val;
		  
		  // _dataCache0 is busy. Check _dataCahe1
		  _dataCache1.read(prevData2Val, dataAddr, latency);
		  _tagCache1.read(prevTag2Val, tagAddr, latency);
		  
		  // If it's free (_ANY) fine. If not free, then the address for the existing data is
		  // same as the address of the current data to be written.
		  if (prevTag2Val.getType() == _ANY)
		  {
			  // First time writing into DataCache1
			  _dataCache1.write(value,dataAddr,latency);
			  _tagCache1.write(tagValue,tagAddr,latency);
			  return;
		  }
		  else if (prevTag2Val == tagValue)
		  {
			  // Not the first time. May be new entry or modifying an existing one.
			  _dataCache1.write(value,dataAddr,latency);
			  return;
		  }
		  
		  // Both the caches has valid data. Replace the Least Recently Used (LRU) block
		  // and then write the data there. This involves maintainance of a dynamic
		  // list for LRU. TODO: LRU implementation/replacement/write and adjust latency.
		  
		  int replaceBlock = ReplaceSecondaryCacheBlock(((R &)addr).getAddrOperand(), latency);
		
		  // write now.
		  if (replaceBlock == 0)
		  {
			  // dataCache0
			  _dataCache0.write(value,dataAddr,latency);
			  _tagCache0.write(tagValue,tagAddr,latency);
		  }
		  else 
		  {
			  // dataCache1
			  _dataCache1.write(value,dataAddr,latency);
			  _tagCache1.write(tagValue,tagAddr,latency);
		  }
		  
	  }	  

	  void clearBuffer()
	  {
		  _buffer.clearBuffer();
	  }

	  void reset()
	  {
		  _dataCache0.reset();
		  _dataCache1.reset();
		  _tagCache0.reset();
		  _tagCache1.reset();
	  }

	  void extractTagValueAndTagAddrFromVirtualAddr(AddrOperand addr, AddrOperand & dataAddr, Val & tagVal, AddrOperand & tagAddr)
	  {
		  long address = addr.getAddrOperand();

		  long dataAddress = address;
		  dataAddress = dataAddress & _SEC_DATA_ADDR_MASK_;
		  dataAddr.setAddrOperand(_DIRECTMEMORY_, dataAddress);
		  dataAddr.setFieldType(addr.getFieldType());

		  long tagAddress = dataAddress >> _SEC_ADDR_SHIFT_FOR_DATA_TAG_;
		  tagAddr.setAddrOperand(_DIRECTMEMORY_, tagAddress);
		  tagAddr.setFieldType(_ADDR_INT_);

		  long tagValue = address;
		  tagValue = tagValue >> _SEC_DATA_CACHE_ADDR_SIZE_;
		  tagValue = tagValue & _SEC_TAG_VALUE_MASK_;
		  tagVal._as.integer = tagValue;
		  tagVal.setType(_INT);

		  //cout << "Address: " << address << "  dataAddr: " << dataAddress << "  tagAddr: " << tagAddress << "  Tag: " << tagValue << endl;
	  }


	  int ReplaceSecondaryCacheBlock(int physicalAddress, SignalClass & latency)
	  {
		  S tagValue;
		  R dataAddr;
		  int dataAddress;
		  int tagAddress;
		  
	  	  R tagAddr;
		  tagAddress = (physicalAddress >> _SEC_ADDR_SHIFT_FOR_DATA_TAG_) & _SEC_TAG_ADDR_MASK_;
		  tagAddr.setAddrOperand(_DIRECTMEMORY_, tagAddress);

		  // Replace the least recently used(LRU) block.
		  int replace = 1;
		  
		  if (LRUArray[tagAddress][0] == 0)
			  replace = 0; // dataCache0 is LRU.

		  // Read tagValue;
		  if (replace == 0)
			  _tagCache0.read(tagValue, tagAddr, latency);		  
		  else
			  _tagCache1.read(tagValue, tagAddr, latency);
		  
		  // Return if invalid tagValue, nothing to replace
		  if (tagValue.getType() == _ANY)
			  return replace;
		 
		  // |<------------ Physical Address ---------------------------|
		  // |<--- tagValue ---->|<---- tagAddr ----->|<--- ...... ---->|
		  //                     |<-------- data address -------------->|
		  //                     
		  int tempAddress = (tagValue._as.integer << _SEC_DATA_CACHE_ADDR_SIZE_);    // tagValue
		  tempAddress = tempAddress | (tagAddress << _SEC_ADDR_SHIFT_FOR_DATA_TAG_); // tagAddr
		  
		  if (debugSIMULATOR)
			cout << "REPLACE :: Secondary Cache -> Main Memory" << endl;

		  AddrOperand physicalAddr;
		  int entries;
		  for (entries = 0; entries < _SEC_DATA_CACHE_BLOCK_SIZE_ * 2; entries++) // Since address increment is 4 bytes
		  {
			  physicalAddress = tempAddress;
			  physicalAddr.setAddrOperand(_DIRECTMEMORY_, physicalAddress);
			   
			  dataAddress = physicalAddress & _SEC_DATA_ADDR_MASK_;
			  dataAddr.setAddrOperand(_DIRECTMEMORY_, dataAddress);
			  
			  S dataValue;
			  
			  if (replace == 0)
				  _dataCache0.read(dataValue, dataAddr, latency);
			  else
				  _dataCache1.read(dataValue, dataAddr, latency);
			  
			  // if Valid data, write it in Main memory and reset the current location
			  if (dataValue.getType() != _ANY)
			  {
				  PORT_ELEM(0)->write(dataValue, physicalAddr, latency); // write to main memory

				  if (debugSIMULATOR)
				  {
					  cout << "       Address: " << physicalAddress << "      Data: ";
					  dataValue.print(cout);
					  cout << "      Cache: " << replace << endl;
				  }
				  
				  S temp;
				  
				  if (replace == 0)
					  _dataCache0.write(temp, dataAddr, latency); // reset to _ANY
				  else
					  _dataCache1.write(temp, dataAddr, latency); // reset to _ANY
			  }

			  tempAddress = tempAddress + 4; // For byte address memory alignment
		  }
		  
		  return replace;
	  }
	  	  
	  
	  int RefillSecondaryCacheBlock(int physicalAddress, SignalClass & latency)
	  {
		  S tagValue;
		  R dataAddr;
		  R tagAddr;
		  int dataAddress;
		  int tagAddress;

		  // Replace the least recently used(LRU) block 
		  int refillBlock = ReplaceSecondaryCacheBlock(physicalAddress, latency);
		  
		  // From the tag addesss generate dataAddress. First generate the cache data address
		  // for the block and then add tagValue to generate physical address.
		 
		  R physicalAddr;
		  int tempAddress = physicalAddress & _SEC_DATA_TAG_SHIFT_MASK_;
		  tagAddress = (physicalAddress >> _SEC_ADDR_SHIFT_FOR_DATA_TAG_) & _SEC_TAG_ADDR_MASK_;
		  tagAddr.setAddrOperand(_DIRECTMEMORY_, tagAddress);

		  tagValue._as.integer = physicalAddress >> _SEC_DATA_CACHE_ADDR_SIZE_;
		  tagValue.setType(_INT);

		  if (refillBlock == 0)
			  _tagCache0.write(tagValue, tagAddr, latency);
		  else
			  _tagCache1.write(tagValue, tagAddr, latency);


		  int entries;

		  if (debugSIMULATOR)
			cout << "REFILLL :: Main Memory -> Secondary Cache" << endl;
		  
		  for (entries = 0; entries < _SEC_DATA_CACHE_BLOCK_SIZE_ * 2; entries++) // Since address increment is 4 bytes
		  {
			  physicalAddress = tempAddress;
			  physicalAddr.setAddrOperand(_DIRECTMEMORY_, physicalAddress);
			  			  
			  S dataValue;
			  
			  // Read from main memory
			  PORT_ELEM(0)->read(dataValue, physicalAddr, latency);

			  if (dataValue.getType() != _ANY)
			  {
				  if (debugSIMULATOR)
				  {
					  cout << "       Address: " << physicalAddress << "      Data: ";
					  dataValue.print(cout);
					  cout << "      Cache: " << refillBlock << endl;
				  }
				  
				  dataAddress = physicalAddress & _SEC_DATA_ADDR_MASK_;
				  dataAddr.setAddrOperand(_DIRECTMEMORY_, dataAddress);
				  
				  // Refill the entry and update the LRU information
				  if (refillBlock == 0)
				  {
					  _dataCache0.write(dataValue, dataAddr, latency);
					  LRUArray[tagAddress][0] = 1;
					  LRUArray[tagAddress][1] = 0;
					  
				  }
				  else
				  {
					  _dataCache1.write(dataValue, dataAddr, latency);
					  LRUArray[tagAddress][0] = 0;
					  LRUArray[tagAddress][1] = 1;
				  }
			  }

			  tempAddress = tempAddress + 4; // For byte address memory alignment
		  }

		  return refillBlock;
	  }

	  void doUpdate()
	  {
		  // TODO: if read/write happens in same cycle, some buffering mechanism
		  // needs to implemented. Read in the first half of the cycle and write 
		  // in the second half. 
	  }
};





template
<class DS, class DSB, class R,class S>
class MainMemory : public Storage {
  protected :
	  Memory<DS,R,S> _mainMemory;  // Actually DS<R,S> ; R is address type; S is Data type
	  
#define  _readLatency  0 //reserved. cant recast
#define  _writeLatency 1 //reserved. cant recast 
	  

  public :
	  MainMemory() {}
	  MainMemory(const MainMemory<DS,DSB,R,S>& a) 
	  {
		  _mainMemory = a._mainMemory; 
	  }
	  
	  MainMemory<DS,DSB,R,S>(int size)
	  {
		  // Initialize with '_ANY' which indicates invalid data.
		  Val value;
		  value.setType(_ANY);

		  _mainMemory = Memory<DS, R, S>(size, value);
	  }

	  ~MainMemory() {}
	  
	  virtual Storage * copy()
	  {
		  Storage * retPtr;  
		  retPtr = new MainMemory<DS,DSB,R,S>(*this);
		  return retPtr;
	  }
	  
	  //virtual int is(StorageClassType ctype){return ((ctype == _MEM_) ? _YES_ : (Storage :: is(ctype)));}
	  friend int operator ==(const MainMemory<DS,DSB,R,S> &a, const MainMemory<DS,DSB,R,S> &b) {return _NO_;}
	  virtual void print(ostream & out)
	  {
		  out << "Printing Main Memory" << endl;
		  _mainMemory.print(out);		  
	  }

	  virtual void print(ostream & out, BaseType & addr)
	  {
		  _mainMemory.print(out,addr);
	  }
	  
	  MainMemory<DS,DSB,R,S>& operator=(const MainMemory<DS,DSB,R,S> &a) 
	  {  
		  _mainMemory = a._mainMemory; 
 
		  return *this;
	  }

	 
	  virtual int getProperty(int pos) {return _mainMemory.getProperty(pos);}

	  virtual void setProperty(int pos, int value)
	  {
		  _mainMemory.setProperty(pos,value);
	  }
	  
	  
	  //bool getBusyBit() {return _mainMemory.getBusyBit();}
	  //void setBusyBit(bool value) {_mainMemory.setBusyBit(value);}
	   

	  virtual void read(BaseType & value, BaseType & addr, SignalClass & latency)
	  {
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
			  a = (S&)value;	// so that the info coming through this parameter can be sent to the memory
			  listOfData.push_back(a);
			  
			  _mainMemory.read(listOfData.back(),*addrIterBegin, latency);
			  addrIterBegin++;
		  }
		  
		  /* compose the data element from the data units */
		  ((S&)value).compose(listOfData);
		  
		  // Latency = Main Memory read latency
		  latency = _mainMemory.getProperty(_readLatency);
	  }
	  

	  virtual void write(BaseType & value, BaseType & addr, SignalClass & latency)
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
			  //_memory.write(S & value, R & Addr)
			  _mainMemory.write(*dataIter, *addrIterBegin, latency);
			  addrIterBegin++; dataIter++;
		  }

		  // Latency = Main Memory write latency
		  latency = _mainMemory.getProperty(_writeLatency);
	  }

	  void reset()
	  {
		  _mainMemory.reset();
	  }

	  void doUpdate()
	  {
		  // TODO: if read/write happens in same cycle, some buffering mechanism
		  // needs to implemented. Read in the first half of the cycle and write 
		  // in the second half. 
	  }
};




class MyMapRegFile{
private:
	inline void setTypeOfVal(Val & value, AddrOperand & address)
	{
		if (value._valType != _ANY) return;
		
		FieldType fType = address.getFieldType();
		
		if (fType == _ADDR_INT_)
		{
			value._valType = _INT;
			value._as.integer = 0;
		}
		
		if (fType == _ADDR_FLT_)
		{
			value._valType = _FLT;
			value._as.flt = 0.0;
		}
		
		if (fType == _ADDR_DBL_)
		{
			value._valType = _DBL;
			value._as.dbl = 0.0;
		}

		// SAH 05/21/2000 : Added _ADDR_MISC_
		//
		if (fType == _ADDR_MISC_)
		{
			value._valType = _INT;
			value._as.integer = 0;
		}
	}
	
public:
	map<int,Val> _int_mymap;
	map<int,Val> _flt_mymap;
	map<int,Val> _misc_mymap;
	
	/*TODO: uncomment this only if iterator is needed for myVector class
	   
		 vector<int>::iterator _iter;
		 
		   MyVector(int size):_vector(){_iter = _vector.begin();};
		   typedef vector<int>::iterator MyVectIterator;
		   
			 MyVectIterator & begin(){ return _vector.begin();}
			 MyVectIterator & end() { return _vector.end();}
			 MyVectIterator & operator ++(){ _iter++; return _iter;}
	*/
	
	MyMapRegFile() {}
	MyMapRegFile(const MyMapRegFile & a) 
	{
		_int_mymap = a._int_mymap;
		_flt_mymap = a._flt_mymap;
		_misc_mymap = a._misc_mymap;
	}

	~MyMapRegFile() {}
	MyMapRegFile(int size):_int_mymap(){};
	void read(Val & value, AddrOperand & addr){
		int address = addr.convertAddrMechanism();
		
		FieldType fType = addr.getFieldType();

		switch(fType)
		{
			case _ADDR_INT_:
			{
				value = _int_mymap[address];
				setTypeOfVal(value, addr);
				//_int_mymap[address]._valType = value._valType;
			}
			break;

			case _ADDR_FLT_:
			case _ADDR_DBL_:
			{
				value = _flt_mymap[address];
				setTypeOfVal(value, addr);
				//_flt_mymap[address]._valType = value._valType;
			}
			break;

			case _ADDR_MISC_:
			{
				value = _misc_mymap[address];
				setTypeOfVal(value, addr);
				//_misc_mymap[address]._valType = value._valType;
			}
			break;
		}
	}
	
	
	void write(Val & value, AddrOperand & addr){
		int address = addr.convertAddrMechanism();
		FieldType fType = addr.getFieldType();

		switch(fType)
		{
			case _ADDR_INT_:
			{
				_int_mymap[address] = (Val &) value;
			}
			break;

			case _ADDR_FLT_:
			case _ADDR_DBL_:
			{
				_flt_mymap[address] = (Val &) value;
			}
			break;

			case _ADDR_MISC_:
			{
				_misc_mymap[address] = (Val &) value;
			}
			break;
		} 
	}
	
	void dump()
	{
	/*
	   map<int, Val>::iterator i;
	   char *regStr;
	   
		 for (i=_mymap.begin(); i != _mymap.end(); i++)
		 {
		 regStr = globalSimSymTab.getName((*i).first);
		 if (regStr == NULL) continue;
		 //printf("Reg: %s ; ", regStr);
		 regFile_doc->Append("Reg: ");
		 regFile_doc->Append(regStr);
		 regFile_doc->Append(" ; ");
		 ((*i).second).print();
		 }
		*/

	}
	
	void print(ostream & out)
	{
		map<int, Val>::iterator i;
		for (i=_int_mymap.begin(); i!= _int_mymap.end(); i++)
		{
			out << "REG " << (*i).first << " : " ;
			((*i).second).print(cout);
			out << endl;
		}
		for (i=_flt_mymap.begin(); i!= _flt_mymap.end(); i++)
		{
			out << "REG " << (*i).first << " : " ;
			((*i).second).print(cout);
			out << endl;
		}
		for (i=_misc_mymap.begin(); i!= _misc_mymap.end(); i++)
		{
			out << "REG " << (*i).first << " : " ;
			((*i).second).print(cout);
			out << endl;
		}
	}

   void reset()
   {
	   _int_mymap.clear();
	   _flt_mymap.clear();
	   _misc_mymap.clear();
   }

};

class MyMapMemory{
private:
	inline void setTypeOfVal(Val & value, AddrOperand & address)
	{
		if (value._valType != _ANY) return;
		assert(0);
		
		FieldType fType = address.getFieldType();
		
		value._valType = _INT;
		
		if (fType == _ADDR_INT_)
			value._valType = _INT;
		
		if (fType == _ADDR_FLT_)
			value._valType = _FLT;
		
		if (fType == _ADDR_DBL_)
			value._valType = _DBL;

		// SAH 05/21/2000 : Added _ADDR_MISC_
		//
		if (fType == _ADDR_MISC_)
			value._valType = _INT;
	}
	
public:
	map<int,Val> _mymap;
	
	MyMapMemory() {}
	MyMapMemory(const MyMapMemory & a) {_mymap = a._mymap;}
	~MyMapMemory() {}
	MyMapMemory(int size):_mymap(){}
	MyMapMemory(int size, Val & value)
	{
		// Initialize the chunk of memory 0 to 'size-1' by 'value'.
	//	int i;
	//	for (i = 0; i < size; i++)
	//	{
	//		_mymap[i] = value;
	//	}
	}

	void read(Val & value, AddrOperand & addr)
	{
		int address = addr.convertAddrMechanism();		

		Val dataToBeRead = _mymap[address];	

	    /************************
		// Do register file write to transfer data from mem to regfile for  this load operation
		if (value.getType() == _INTERNAL_LABEL)
		{
			int dstRegAddress = value._as.integer;
			AddrOperand tempAddr(_REG_,dstRegAddress);

			//int RegFile = getRegFileType(dstRegAddress);
			FieldType fType = addr.getFieldType();
			
			MyString WrIRFportname("MemWrPort1");
			MyString WrFRFportname("MemWrPort2");
			
			if (fType == _ADDR_INT_)
			{
				Port * port = (Port*)getComp(WrIRFportname);
				port->write(dataToBeRead,tempAddr);
			}
			else if ((fType == _ADDR_FLT_) || (fType == _ADDR_DBL_))
			{
				Port * port = (Port*)getComp(WrFRFportname);
				port->write(dataToBeRead,tempAddr);
			}
			else
			{
				cout << "Eror: MyMapMemory.read() - Invalid register File Name" << endl;
				assert(0);
			}
		}
		***************************/
		
		value = dataToBeRead;
		
		// setTypeOfVal(value, addr);
		
		// _mymap[address]._valType = value._valType;


		//
		// Note: the type of the value should preferably be set in the Memory class
		// and not in the data-structure MyMap.
		//
		// This is a hack, cos as of now, we do not have a separate Memory class for 
		// virtual memories.
		
		
		
		if (debugSIMULATOR)
		{
			cout << "Printing MEMORY READ values: " << endl;
			cout << "Addr: " ;
			addr.print(cout);
			//cout << endl;
			cout << "Data: ";
			value.print(cout);
			cout << endl; 
		}		
	}
	
	void write(Val & value, AddrOperand & addr){
		
		Val dataToBeStored;
		// Do register file read to get the data for  this store operation
		if (value.getType() == _INTERNAL_LABEL)
		{
			int srcRegAddress = value._as.integer;
			AddrOperand tempAddr(_REG_,srcRegAddress);
			
			FieldType fType = addr.getFieldType();
			//int RegFile = getRegFileType(srcRegAddress);
			
			MyString RdIRFportname("MemRdPort1");
			MyString RdFRFportname("MemRdPort2");
			
			//if (RegFile == _INT_REG_FILE)
			if (fType == _ADDR_INT_)
			{
				Port * port = (Port*)getComp(RdIRFportname);
				port->read(dataToBeStored,tempAddr);
			}
			else if ((fType == _ADDR_FLT_) || (fType == _ADDR_DBL_))
			{
				Port * port = (Port*)getComp(RdFRFportname);
				port->read(dataToBeStored,tempAddr);
			}
			else
			{
				cout << "Eror: MyMapMemory.read() - Invalid register File Name" << endl;
				assert(0);
			}
		}
		else
			dataToBeStored = (Val &)value;
		
		int address = addr.convertAddrMechanism();
		_mymap[address] = dataToBeStored;
		
		if (debugSIMULATOR)
		{
			cout << "Printing MEMORY WRITE values: " << endl;
			cout << "Addr: " ;
			addr.print(cout);
			//cout << endl;
			cout << "Data: ";
			dataToBeStored.print(cout);
			cout << endl; 
		}   
	}
	
	void print(ostream & out)
	{
	}

	void reset()
	{
	   _mymap.clear();
	}
};


typedef RegFile<MyMapRegFile,AddrOperand,Val> VirtualRegFile;
typedef Memory<MyMapMemory,AddrOperand,Val> VirtualMemory;
typedef BufferedMemory<MyMapMemory,MyDeque,AddrOperand,Val> BufferedVirtualMemory;
typedef SecondaryMemory<MyMapMemory,MyDeque,AddrOperand,Val> SecondaryVirtualMemory;
typedef MainMemory<MyMapMemory,MyDeque,AddrOperand,Val> VirtualMainMemory;
#endif