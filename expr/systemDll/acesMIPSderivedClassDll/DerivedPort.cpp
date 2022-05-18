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
 *created : 26 Aug. 98                                       *
 *authors : Asheesh Khare		                             *
 *last modified :                                            *
 *                                                           *
 *contents : contains the storage class definitions          *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/



#include "DerivedClassIncludes.h"
#include "DerivedAddrData.h"
#include "Connection.h"
#include "DerivedPort.h"


/*UnitPort * UnitPort :: copy()
{
  UnitPort * retPtr;
  retPtr = new UnitPort(* this);
  return retPtr;
}
*/
void UnitPort :: addConnection(Connection * connection)
{  
  _connections.push_back(connection);
}

void UnitPort :: print(ostream & out)
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

void UnitPort :: read (BaseType & value, BaseType & addr, SignalClass & latency, int memType)
{
	AddrOperand & tempaddr = (AddrOperand &)addr;

	if (_typeOfPort == _WRITE_){
	   cout << "Panic: READ Operation cannot be done through a WRITE port" << endl;
       assert(0);
	}	    
	else if (tempaddr.getType() == _NONE_)
	{
		cout << "Panic: Need to send (reg/memory) address through UnitPort" << endl;
		assert(0);
	}
	// TODO: commented because we assume one Virtual Register File instead of 2
	// register files as in the c62x.

	//else if (tempaddr.getAddrOperand() < NumOfRegInRF1)
	else
		CONNECT_ELEM(0)->read(value,memType,latency,addr);

	//else if (tempaddr.getAddrOperand() < NumOfRegInRF1 + NumOfRegInRF2)
	//	CONNECT_ELEM(1)->read(value,addr,latency);
}


void UnitPort :: read (BaseType & value, BaseType & addr, SignalClass & latency)
{
	AddrOperand & tempaddr = (AddrOperand &)addr;

	if (_typeOfPort == _WRITE_){
	   cout << "Panic: READ Operation cannot be done through a WRITE port" << endl;
       assert(0);
	}	    
	else if (tempaddr.getType() == _NONE_)
	{
		cout << "Panic: Need to send (reg/memory) address through UnitPort" << endl;
		assert(0);
	}
	// TODO: commented because we assume one Virtual Register File instead of 2
	// register files as in the c62x.

	//else if (tempaddr.getAddrOperand() < NumOfRegInRF1)
	else
		CONNECT_ELEM(0)->read(value,addr,latency);

	//else if (tempaddr.getAddrOperand() < NumOfRegInRF1 + NumOfRegInRF2)
	//	CONNECT_ELEM(1)->read(value,addr,latency);
}


void UnitPort :: read (BaseType & value, BaseType & addr)
{
	AddrOperand & tempaddr = (AddrOperand &)addr;

	if (_typeOfPort == _WRITE_){
	   cout << "Panic: READ Operation cannot be done through a WRITE port" << endl;
       assert(0);
	}	    
	else if (tempaddr.getType() == _NONE_)
	{
		cout << "Panic: Need to send (reg/memory) address through UnitPort" << endl;
		assert(0);
	}
	// TODO: commented because we assume one Virtual Register File instead of 2
	// register files as in the c62x.

	//else if (tempaddr.getAddrOperand() < NumOfRegInRF1)
	else
		CONNECT_ELEM(0)->read(value,addr);

	//else if (tempaddr.getAddrOperand() < NumOfRegInRF1 + NumOfRegInRF2)
	//	CONNECT_ELEM(1)->read(value,addr);
}

void UnitPort :: write (BaseType & value, BaseType & addr, SignalClass & latency)
{
	AddrOperand & tempaddr = (AddrOperand &)addr;

	if (_typeOfPort == _READ_){
	   cout << "Panic: WRITE Operation cannot be done through a READ port" << endl;
       assert(0);
	}	    
	else if (tempaddr.getType() == _NONE_)
	{
		cout << "Panic: Need to send (reg/memory) address through UnitPort" << endl;
		assert(0);
	}
	// TODO: commented because we assume one Virtual Register File instead of 2
	// register files as in the c62x.

	//else if (tempaddr.getAddrOperand() < NumOfRegInRF1)
	else
		CONNECT_ELEM(0)->write(value,addr,latency);

	//else if (tempaddr.getAddrOperand() < NumOfRegInRF1 + NumOfRegInRF2)
	//	CONNECT_ELEM(1)->write(value,addr,latency);
}

void UnitPort :: write (BaseType & value, BaseType & addr)
{
	AddrOperand & tempaddr = (AddrOperand &)addr;

	if (_typeOfPort == _READ_){
	   cout << "Panic: WRITE Operation cannot be done through a READ port" << endl;
       assert(0);
	}	    
	else if (tempaddr.getType() == _NONE_)
	{
		cout << "Panic: Need to send (reg/memory) address through UnitPort" << endl;
		assert(0);
	}
	// TODO: commented because we assume one Virtual Register File instead of 2
	// register files as in the c62x.

	//else if (tempaddr.getAddrOperand() < NumOfRegInRF1)
	else
		CONNECT_ELEM(0)->write(value,addr);

	//else if (tempaddr.getAddrOperand() < NumOfRegInRF1 + NumOfRegInRF2)
	//	CONNECT_ELEM(1)->write(value,addr);
}
