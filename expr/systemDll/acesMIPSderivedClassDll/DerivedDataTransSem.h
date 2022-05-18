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
 *file  : DerivedDataTransSem.h                              *
 *authors : ahalambi	                                     *
 *last modified :			                                 *
 *                                                           *
 *contents:												     *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/
#ifndef __DERIVEDDATATRANSSEM_H__
#define __DERIVEDDATATRANSSEM_H__

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <map>

#include "BaseDataTransSemantics.h"
#include "DerivedClassIncludes.h"
#include "DerivedAddrData.h"
#include "Connection.h"
#include "Storage.h"
#include "DataConversion.h"

#include "../../pcProDll/DebugSwitches.h"

//define ALL_SIGNED in order to have all the loads produce signed results
//#define ALL_SIGNED 1


/*
**** SAH: Wed, Apr 19, 2000
****
**** Changed this to reflect the fact that we now model the transfer of data over the bus, by using an array of bytes
****
****

// This class is for IVSTORE
//
class DoIntegerStore : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn) // a = value to be stored, b = memory location 
	{
		SignalClass lat;

		// if a is a byte, need to store it as an integer, becos IVSTORE is supposed to
		// store 4 bytes. The following piece of code makes it clear why storing it as a
		// byte could cause a problem
		//
		// char j = 'a';
		// A[i] = (int)j;
		//
		// However, the .s file looks like this:
		//
		// IADD tmp Abase i
		// IVSTORE tmp 0 j

		Val tmpa = a;
		if (a._valType == _CHAR)
		{
			tmpa.changeValue(a._as.integer);
		}
		conn->write(tmpa, b, lat);

		return lat;
	}
};
*/

// This class is for IVSTORE
//
class DoIntegerStore : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn) // a = value to be stored, b = memory location 
	{
		SignalClass lat;
		int numBytes;

		// TBD: Error checking.
		// Check to see if the value inside a is acceptable (for instance, it is not a label, or a float, double)

		// First convert a to 8 bytes.
		//
		unsigned char byteList[MAX_NUM_BYTES];
		DataToArray(a, byteList, numBytes);

		Val tmpa(byteList);
		// then store the value as an integer, i.e. do a write to memory of the 1st four bytes.
		//
		conn->write(tmpa, b, lat, 4);

		return lat;
	}
};

// This class is for DVSTORE
//
class DoDoubleStore : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn) // a = value to be stored, b = memory location 
	{
		SignalClass lat;
		int numBytes;

		// TBD: Error checking.
		// Check to see if the value inside a is acceptable (for instance, it is not a label, or an int)

		// first convert a to 8 bytes.
		//
		unsigned char byteList[MAX_NUM_BYTES];
		DataToArray(a, byteList, numBytes);

		Val tmpa(byteList);
		// then store the value as a double, i.e. do a write to memory of the 1st eight bytes.
		//
		conn->write(tmpa, b, lat, 8);

		return lat;
	}
};

// This class is for FVSTORE
//
class DoFloatStore : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn) // a = value to be stored, b = memory location 
	{
		SignalClass lat;
		int numBytes;

		// TBD: Error checking.
		// Check to see if the value inside a is acceptable (for instance, it is not a label, or an int)

		// first convert a to 8 bytes.
		//
		unsigned char byteList[MAX_NUM_BYTES];
		DataToArray(a, byteList, numBytes);

		Val tmpa(byteList);
		// then store the value as a float, i.e. do a write to memory of the 1st four bytes.
		//
		conn->write(tmpa, b, lat, 4);

		return lat;
	}
};

/*
**** SAH: Wed, Apr 19, 2000
****
**** Changed this to reflect the fact that we now model the transfer of data over the bus, by using an array of bytes
****
****

// This class is for QIVSTORE
//
class DoCharStore : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn) // a = value to be stored, b = memory location
	{
		SignalClass lat;

		// If a is an int store it as a byte, becos QIVSTORE is supposed to store a byte.
		//
		Val tmpa = a;
		if (a._valType != _CHAR)
		{
			tmpa._valType = _CHAR; // Forcibly convert it to a byte.
		}
		conn->write(tmpa, b, lat);
		
		return lat;
	}
};
*/

// This class is for QIVSTORE
//
class DoCharStore : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn) // a = value to be stored, b = memory location
	{
		SignalClass lat;

		// If a is an int store it as a byte, becos QIVSTORE is supposed to store a byte.
		//
		Val tmpa = a;
		if (a._valType != _CHAR)
		{
			tmpa._valType = _CHAR; // Forcibly convert it to a byte.
		}

		int numBytes;

		// TBD: Error checking.
		// Check to see if the value inside a is acceptable (for instance, it is not a label, or an int)

		// first convert a to 8 bytes.
		//
		unsigned char byteList[MAX_NUM_BYTES];
		DataToArray(tmpa, byteList, numBytes);

		Val tmpb(byteList);
		// then store the value as a char, i.e. do a write to memory of the 1st byte.
		//
		conn->write(tmpb, b, lat, 1);
		return lat;
	}
};

// This class is for QIVSTOREU
//
class DoUCharStore : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn) // a = value to be stored, b = memory location
	{
		SignalClass lat;

		// If a is an int store it as an unsigned byte, becos QIVSTOREU is supposed to store a byte.
		//
		Val tmpa = a;
		if (a._valType != _UCHAR)
		{
			tmpa._valType = _UCHAR; // Forcibly convert it to a byte.
		}

		int numBytes;

		// TBD: Error checking.
		// Check to see if the value inside a is acceptable (for instance, it is not a label, or an int)

		// first convert a to 8 bytes.
		//
		unsigned char byteList[MAX_NUM_BYTES];
		DataToArray(tmpa, byteList, numBytes);

		Val tmpb(byteList);
		// then store the value as a uchar, i.e. do a write to memory of the 1st byte.
		//
		conn->write(tmpb, b, lat, 1);
		return lat;
	}
};

void ShortToTwoBytes(__int16 a, __int8 & t1, __int8 & t2, int littleendian);
void UShortToTwoUBytes(unsigned short a, unsigned char & t1, unsigned char & t2, int littleendian);

/*
**** SAH: Wed, Apr 19, 2000
****
**** Changed this to reflect the fact that we now model the transfer of data over the bus, by using an array of bytes
****
****

// This class is for HIVSTORE
//
class DoShortAs2CharStore : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn) // a = value to be stored, b = memory location
	{
		SignalClass lat;

		// first convert a into 2 bytes
		//
		Val tmpa = a;
		if (tmpa._valType != _SHORT)
		{
			// print warning.
			//
			cout << "Warning: STORE expected short, went long :-(" << endl;

			tmpa._valType = _SHORT; // Forcibly convert it to a short
		}
		
		__int8 t1, t2;
		ShortToTwoBytes(a._as.short_int, t1, t2, 1);

		Val v1(t1);
		Val v2(t2);

		// now write each 
		conn->write(v1, b, lat);

		AddrOperand tmpAddr(_DIRECTMEMORY_, b.getAddrOperand() + 1);
		conn->write(v2, tmpAddr, lat);

		return lat;
	}
};
*/

// This class is for HIVSTORE
//
class DoShortAs2CharStore : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn) // a = value to be stored, b = memory location
	{
		SignalClass lat;

		// first convert a into 2 bytes
		//
		Val tmpa = a;
		if (tmpa._valType != _SHORT)
		{
			// print warning.
			//
			//cout << "Warning: STORE expected short, went long :-(" << endl;

			tmpa._valType = _SHORT; // Forcibly convert it to a short
		}

		int numBytes;

		// TBD: Error checking.
		// Check to see if the value inside a is acceptable (for instance, it is not a label, or an int)

		// first convert a to 8 bytes.
		//
		unsigned char byteList[MAX_NUM_BYTES];
		DataToArray(tmpa, byteList, numBytes);

		Val tmpb(byteList);
		// then store the value as a short, i.e. do a write to memory of the 1st two bytes.
		//
		conn->write(tmpb, b, lat, 2);
		return lat;
	}
};

/*
**** SAH: Wed, Apr 19, 2000
****
**** Changed this to reflect the fact that we now model the transfer of data over the bus, by using an array of bytes
****
****

// This class is for HIVSTOREU
//
class DoUShortAs2UCharStore : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn) // a = value to be stored, b = memory location
	{
		SignalClass lat;

		// first convert a into 2 bytes
		//
		Val tmpa = a;
		if (tmpa._valType != _SHORT)
		{
			// print warning.
			//
			cout << "Warning: STORE expected short, went long :-(" << endl;

			tmpa._valType = _SHORT; // Forcibly convert it to a short
		}

		unsigned char t1, t2;
		UShortToTwoUBytes((unsigned short)a._as.short_int, t1, t2, 1);

		Val v1(t1);
		Val v2(t2);

		// now write each 
		conn->write(v1, b, lat);

		AddrOperand tmpAddr(_DIRECTMEMORY_, b.getAddrOperand() + 1);
		conn->write(v2, tmpAddr, lat);

		return lat;
	}
};
*/

// This class is for HIVSTOREU
//
class DoUShortAs2UCharStore : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn) // a = value to be stored, b = memory location
	{
		SignalClass lat;

		// first convert a into 2 bytes
		//
		Val tmpa = a;
		if (tmpa._valType != _USHORT)
		{
			// print warning.
			//
//			cout << "Warning: STORE expected ushort, went long :-(" << endl;

			tmpa._valType = _USHORT; // Forcibly convert it to a short
		}

		int numBytes;

		// TBD: Error checking.
		// Check to see if the value inside a is acceptable (for instance, it is not a label, or an int)

		// first convert a to 8 bytes.
		//
		unsigned char byteList[MAX_NUM_BYTES];
		DataToArray(tmpa, byteList, numBytes);

		Val tmpb(byteList);
		// then store the value as a ushort, i.e. do a write to memory of the 1st two bytes.
		//
		conn->write(tmpb, b, lat, 2);
		return lat;
	}
};

class DoDefaultStore : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn) // a = value to be stored, b = memory location
	{
	   SignalClass lat;
	   
	   conn->write(a, b, lat);

	   return lat;
	}
};

class DoDefaultLoad : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn)
	{
		SignalClass lat;

		conn->read(a, b, lat);

		return lat;
	}
};

// This class is for IVLOAD
//
class DoIntegerLoad : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn)
	{
		SignalClass lat;

		// TBD: Error checking.
		// Check to see if the value inside a is acceptable (for instance, it is not a label, or a float, double)

		// First read 4 bytes from memory.
		//
		Val tmpa;
		conn->read(tmpa, b, lat, 4);

		// Then, load the value as an integer
		//
		unsigned char byteList[MAX_NUM_BYTES];
		tmpa.getValue(byteList);

		ArrayToData(byteList, 4, _INT, a);

		return lat;
	}
};

// This class is for DVLOAD
//
class DoDoubleLoad : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn)
	{
		SignalClass lat;

		// TBD: Error checking.
		// Check to see if the value inside a is acceptable (for instance, it is not a label, or an int)

		// First read 8 bytes from memory.
		//
		Val tmpa;
		conn->read(tmpa, b, lat, 8);

		// Then, load the value as a double
		//
		unsigned char byteList[MAX_NUM_BYTES];
		tmpa.getValue(byteList);

		ArrayToData(byteList, 8, _DBL, a);

		return lat;
	}
};

// This class is for FVLOAD
//
class DoFloatLoad : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn)
	{
		SignalClass lat;

		// TBD: Error checking.
		// Check to see if the value inside a is acceptable (for instance, it is not a label, or an int)

		// First read 4 bytes from memory.
		//
		Val tmpa;
		conn->read(tmpa, b, lat, 4);

		// Then, load the value as a float
		//
		unsigned char byteList[MAX_NUM_BYTES];
		tmpa.getValue(byteList);

		ArrayToData(byteList, 4, _FLT, a);

		return lat;
	}
};

/*
**** SAH: Wed, Apr 19, 2000
****
**** Changed this to reflect the fact that we now model the transfer of data over the bus, by using an array of bytes
****
****

// This class is for QIVLOAD
//
class DoIntLoad : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn)
	{
		SignalClass lat;

		conn->read(a, b, lat);

		if (a._valType == _CHAR)
		{
			// i.e. if the value is a byte, then we need to convert this into an int.
			//
			a.cast_to(_INT);
		}
		
		// if the retval is a string, then take the first character and convert it into
		// the corresponding ascii number.
		//
		else if (a._valType == _INTERNAL_LABEL)
		{
			char *str = getStringValue(a);
			if (str != NULL)
			{
				char buf = str[0];
				int t1 = buf;
				a.changeValue(t1);
			}
		}
		else
			_ASSERT(0);

		return lat;
	}
};
*/

// This class is for QIVLOAD
//
class DoCharLoad : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn)
	{
		SignalClass lat;

		// TBD: Error checking.
		// Check to see if the value inside a is acceptable (for instance, it is not a label, or a float)

		// First read 1 bytes from memory.
		//
		Val tmpa;
		conn->read(tmpa, b, lat, 1);

		// Then, load the value as an int
		//
		unsigned char byteList[MAX_NUM_BYTES];
		tmpa.getValue(byteList);

		ArrayToData(byteList, 1, _INT, a);

		return lat;
	}
};

/*
**** SAH: Wed, Apr 19, 2000
****
**** Changed this to reflect the fact that we now model the transfer of data over the bus, by using an array of bytes
****
****

// This class is for QIVLOADU
//
class DoUCharAsUIntLoad : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn)
	{
		SignalClass lat;
		
		conn->read(a, b, lat);
		
		if (a._valType == _CHAR)
		{
			// i.e. if the value is a byte, then we need to convert this into an unsigned int.
			//
			unsigned char tmp = a._as.byte;
			unsigned int t2 = tmp;
			a.changeValue(t2);
		}
		
		// if the retval is a string, then take the first character and convert it into
		// the corresponding ascii number.
		//
		else if (a._valType == _INTERNAL_LABEL)
		{
			char *str = getStringValue(a);
			if (str != NULL)
			{
				unsigned char buf = str[0];
				unsigned int t1 = buf;
				a.changeValue(t1);
			}
		}
		else
			_ASSERT(0);
		return lat;
	}
};
*/

// This class is for QIVLOADU
//
class DoUCharLoad : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn)
	{
		SignalClass lat;

		// TBD: Error checking.
		// Check to see if the value inside a is acceptable (for instance, it is not a label, or a float)

		// First read 1 byte from memory.
		//
		Val tmpa;
		conn->read(tmpa, b, lat, 1);

		// Then, load the value as an int
		//
		unsigned char byteList[MAX_NUM_BYTES];
		tmpa.getValue(byteList);

/***** SAH : Tue, May 09 2000 : (ALL_SIGNED) Changes to make the interpretation of words inside the processor uniform */

#ifdef ALL_SIGNED
		ArrayToData(byteList, 1, _INT, a); // was previously _UINT
#else 
		ArrayToData(byteList, 1, _UINT, a); // was previously _UINT
#endif

/***** End change *****************************************************************************************************/
		return lat;
	}
};

__int16 TwoBytesToWord(__int8 b1,__int8 b2,int littleendian);
unsigned short TwoUBytesToUWord(unsigned char b1,unsigned char b2,int littleendian);

/*
**** SAH: Wed, Apr 19, 2000
****
**** Changed this to reflect the fact that we now model the transfer of data over the bus, by using an array of bytes
****
****

// This class is for HIVLOAD
//
class Do2CharAsShortLoad : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn)
	{
		SignalClass lat;

		conn->read(a, b, lat);

		if (a._valType == _CHAR)
		{
			// if it is a byte, read the next byte too.
			//
			Val tmpVal;
			AddrOperand tmpAddr(_DIRECTMEMORY_, b.getAddrOperand() + 1);
			conn->read(tmpVal, tmpAddr, lat);

			// compose the two bytes.
			//
			a.changeValue(TwoBytesToWord(a._as.byte, tmpVal._as.byte, 1));

		}
		else if (a._valType == _INTERNAL_LABEL) // take the first two chars and convert it into short
		{
			if (strlen(a._as.label) < 2) // error condition
			{
				assert(0);
			}
			a.changeValue(TwoBytesToWord((a._as.label)[0], (a._as.label)[1], 1));
		}
		else
			_ASSERT(0);

		return lat;
	}
};
*/

// This class is for HIVLOAD
//
class Do2CharAsShortLoad : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn)
	{
		SignalClass lat;

		// TBD: Error checking.
		// Check to see if the value inside a is acceptable (for instance, it is not a label, or a float)

		// First read 2 byte from memory.
		//
		Val tmpa;
		conn->read(tmpa, b, lat, 2);

		// Then, load the value as a short
		//
		unsigned char byteList[MAX_NUM_BYTES];
		tmpa.getValue(byteList);

/***** SAH : Tue, May 09 2000 : (ALL_SIGNED) Changes to make the interpretation of words inside the processor uniform */
#ifdef ALL_SIGNED
		ArrayToData(byteList, 2, _INT, a); // was previously _SHORT
#else
		ArrayToData(byteList, 2, _SHORT, a); // was previously _SHORT
#endif

/***** End change *****************************************************************************************************/


		return lat;
	}
};

/*
**** SAH: Wed, Apr 19, 2000
****
**** Changed this to reflect the fact that we now model the transfer of data over the bus, by using an array of bytes
****
****

// This class is for HIVLOADU
//
// Note: Ideally, it should take two chars (as unsigned) and make an unsigned short out of it.
//
class Do2UCharAsUShortLoad : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn)
	{
		SignalClass lat;

		conn->read(a, b, lat);

		if (a._valType == _CHAR)
		{
			// if it is a byte, read the next byte too.
			//
			Val tmpVal;
			AddrOperand tmpAddr(_DIRECTMEMORY_, b.getAddrOperand() + 1);
			conn->read(tmpVal, tmpAddr, lat);

			// compose the two bytes.
			//
			unsigned char t1, t2;
			t1 = a._as.byte;
			t2 = tmpVal._as.byte;
			a.changeValue(TwoUBytesToUWord(t1, t2, 1));

		}
		else if (a._valType == _INTERNAL_LABEL) // take the first two chars and convert it into short
		{
			if (strlen(a._as.label) < 2) // error condition
			{
				assert(0);
			}
			unsigned char t1, t2;
			t1 = (a._as.label)[0];
			t2 = (a._as.label)[1];
			a.changeValue(TwoUBytesToUWord(t1, t2, 1));
		}
		else
			_ASSERT(0);

		return lat;
	}
};
*/

// This class is for HIVLOADU
//
// Note: Ideally, it should take two chars (as unsigned) and make an unsigned short out of it.
//
class Do2UCharAsUShortLoad : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn)
	{
		SignalClass lat;

		// TBD: Error checking.
		// Check to see if the value inside a is acceptable (for instance, it is not a label, or a float)

		// First read 2 byte from memory.
		//
		Val tmpa;
		conn->read(tmpa, b, lat, 2);

		// Then, load the value as an unsigned short
		//
		unsigned char byteList[MAX_NUM_BYTES];
		tmpa.getValue(byteList);

/***** SAH : Tue, May 09 2000 : (ALL_SIGNED) Changes to make the interpretation of words inside the processor uniform */

#ifdef ALL_SIGNED
		ArrayToData(byteList, 2, _INT, a); // was previously _USHORT
#else
		ArrayToData(byteList, 2, _USHORT, a); // was previously _USHORT
#endif

/***** End change *****************************************************************************************************/

		return lat;
	}
};

class DoRegRead : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn)
	{
		SignalClass lat;

		conn->read(a, b, lat);

		return lat;
	}
};

class DoRegWrite : public BaseDataTransSemantics
{
	SignalClass operator()(Val & a, AddrOperand & b, Connection *conn)
	{
		SignalClass lat;

		conn->write(a, b, lat);

		return lat;
	}
};

extern DoIntegerStore integerStoreFunc;
extern DoDoubleStore doubleStoreFunc;
extern DoFloatStore floatStoreFunc;
extern DoCharStore charStoreFunc;
extern DoUCharStore ucharStoreFunc;
extern DoShortAs2CharStore shortAs2CharStoreFunc;
extern DoUShortAs2UCharStore ushortAs2UCharStoreFunc;

extern DoIntegerLoad integerLoadFunc;
extern DoDoubleLoad doubleLoadFunc;
extern DoFloatLoad floatLoadFunc;
extern DoCharLoad charLoadFunc;
extern DoUCharLoad ucharLoadFunc;
extern Do2CharAsShortLoad twoCharAsShortLoadFunc;
extern Do2UCharAsUShortLoad twoUCharAsUShortLoadFunc;

extern DoDefaultStore defaultStoreFunc;
extern DoDefaultLoad defaultLoadFunc;
extern DoRegWrite regWriteFunc;
extern DoRegRead regReadFunc;

extern void writeBytes(char *bytes, int numBytes, AddrOperand & b, Connection *conn);
extern void readBytes(char *bytes, int numBytes, AddrOperand & b, Connection *conn);
extern void writeString(char *str, AddrOperand & b, Connection *conn);
extern void readString(char *str, AddrOperand & b, Connection *conn);
extern void initializeString(char *str, AddrOperand & b, Storage *mem);
extern void initMemory(Val & a, AddrOperand & b, Storage *mem);

#endif
