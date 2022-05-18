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
 *file  : DerivedOpExecSemantics.cpp                         *
 *created : 3 May, 2000.                                     *
 *authors : Prabhat, Srikanth                                *
 *last modified :                                            *
 *                                                           *
 *contents:contains class methods for functionality of each  *
 *         supported by Elf.                                 *
 *compiled with : VC++                                       *
 *for more info :                                            *
 *************************************************************/

#ifndef __DERIVEDOPEXECSEM_H__
#define __DERIVEDOPEXECSEM_H__

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <map>

#include "BaseOpExecSemantics.h"
#include "DerivedClassIncludes.h"
#include "DerivedAddrData.h"
#include "Connection.h"
#include "..\..\acesMIPSdll\DebugSwitches.h"
#include "DataConversion.h"

class DoNOP : public BaseOpExecSemantics
{
   Val operator()(Val & a, Val & b)
      {
	 return a;
      }
   Val operator()(Val & a)
      {
	 return a;
      }
   Val operator()()
   {
	   Val a;
	   return a;
   }

   Val operator()(Val & a, Val & b, Connection *memC)
      {
	 return a;
      }
};

class DoASSERT : public BaseOpExecSemantics
{
	Val operator()(Val & a, Val & b)
	{
		assert(0);
		return a;
	}
	Val operator()(Val & a)
	{
		assert(0);
		return a;
	}
	Val operator()()
	{
		assert(0);
		Val a;
		return a;
	}
	
	Val operator()(Val & a, Val & b, Connection *memC)
	{
		assert(0);
		return a;
	}
};

// CONSTANT opcodes.

class DoICONSTANT : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
      {
	   if (a._valType != _INT)
	   {
		   if (debugSIMULATOR)
		   {
			   // Error condition.
			   cout << "Warning: ICONSTANT recieved a non-int value" << endl;
		   }
	   }
//	   a.cast_to(_INT);
	 return (a);
      }

   Val operator()(Val & a, Val & b)
      {
	   if ((a._valType != _INT) || (b._valType != _INT))
	   {
		   if (debugSIMULATOR)
		   {
			   // Error condition.
			   cout << "Warning: ICONSTANT recieved a non-int value" << endl;
		   }
	   }
//	   a.cast_to(_INT);
	 return (a + b);
      }

};

class DoDCONSTANT : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
      {
	   if (a._valType != _DBL)
	   {
		   if (debugSIMULATOR)
		   {
			   // Error condition.
			   cout << "Warning: DCONSTANT recieved a non-dbl value" << endl;
		   }
	   }
	   a.cast_to(_DBL);
	 return (a);
      }
};

class DoFCONSTANT : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
      {
	   if (a._valType != _FLT)
	   {
		   if (debugSIMULATOR)
		   {
			   // Error condition.
			   cout << "Warning: FCONSTANT recieved a non-float value" << endl;
		   }
	   }
	   a.cast_to(_FLT);
	 return (a);
      }
};

// ASSIGN opcodes

class DoIASSIGN : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
      {
	   if (a._valType != _INT)
	   {
		   if (debugSIMULATOR)
		   {
			   // Error condition.
			   cout << "Warning: IASSIGN recieved a non-int value" << endl;
		   }
	   }
//	   a.cast_to(_INT);
	 return (a);
      }
};

class DoASSIGN : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
      {
	   return (a);
      }
};

class DoFASSIGN : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
      {
	   if (a._valType != _FLT)
	   {
		   if (debugSIMULATOR)
		   {
			   // Error condition.
			   cout << "Warning: FASSIGN recieved a non-float value" << endl;
		   }
	   }
	   a.cast_to(_FLT);
	 return (a);
      }
};

class DoDASSIGN : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
      {
	   if (a._valType != _DBL)
	   {
		   if (debugSIMULATOR)
		   {
			   // Error condition.
			   cout << "Warning: DASSIGN recieved a non-dbl value" << endl;
		   }
	   }
	   a.cast_to(_DBL);
	 return (a);
      }
};

// ADD opcodes

class DoIADD : public BaseOpExecSemantics
{
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of INT type.
	    
	    /* This is a hack. (The cast_to part). This should be transparently */
	    /* handled inside Val (or in some other fashion). */
	    
	 return (a.cast_to(_INT) + b.cast_to(_INT));
      }
};

class DoDADD : public BaseOpExecSemantics
{
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of INT type.
	    
	    /* This is a hack. (The cast_to part). This should be transparently */
	    /* handled inside Val (or in some other fashion). */
	    
	 return (a.cast_to(_DBL) + b.cast_to(_DBL));
      }
};

class DoFADD : public BaseOpExecSemantics
{
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of INT type.
	    
	    /* This is a hack. (The cast_to part). This should be transparently */
	    /* handled inside Val (or in some other fashion). */
	    
	 return (a.cast_to(_FLT) + b.cast_to(_FLT));
      }
};

// SUB opcodes

class DoISUB : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of INT type.
      
	 return (a - b);
      }
};

class DoDSUB : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of DOUBLE type.

	 return (a - b);
      }
};

class DoFSUB : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of FLOAT type.

	 return (a - b);
      }
};

// MUL opcodes
//
// ToDo, TBD
class DoIMUL : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of INT type.

	 return (a * b);
      }
};

// MIPS uses 2 32-bit registers to store the result of the multiply.
// Perform multiplication and store the result in Val as EIGHT_BYTES type.
//
//
// ToDo, TBD
class DoIMUL_MIPS : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	   return (mult64(a, b));
    }
};

class DoMAC : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b, Val & c)
      {
	   return ((a * b) + c);
    }
};

// ToDo, TBD
class DoIMULU : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of INT type.

	 return (a * b);
      }
};

// Use this only if you are using DoIMUL_MIPS
//
// The result of a multiply is stored in HILO as an _EIGHT_BYTE value.
// word[0-3] == LO, and word[4-7] = HI.
//
class DoMFHI : public BaseOpExecSemantics
{
 public:
	 Val operator()(Val & a)
	 {
		 if (a._valType != _EIGHT_BYTES) // error condition
		 {
			 cout << "MFHI recieved an invalid value to convert" << endl;
			 assert(0);
		 }
		 Val retVal;
		 unsigned char array1[MAX_NUM_BYTES];
		 //initialize array1
		 for (int i = 0; i < 4; i++)
			 array1[i] = a._as.word[i+4];
		 for(i=4;i<MAX_NUM_BYTES;i++)
			 array1[i]=0;
		 
		 ArrayToData(array1, 4, _INT, retVal);
		 
		 return retVal;
	 }
};

// Use this only if you are using DoIMUL_MIPS
//
// The result of a multiply is stored in HILO as an _EIGHT_BYTE value.
// word[0-3] == LO, and word[4-7] = HI.
//
class DoMFLO : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
   {
	   if (a._valType != _EIGHT_BYTES) // error condition
	   {
		   cout << "MFHI recieved an invalid value to convert" << endl;
		   assert(0);
	   }
	   Val retVal;
	   unsigned char array1[MAX_NUM_BYTES];
		 //initialize array1
		 for (int i = 0; i < 4; i++)
			 array1[i] = a._as.word[i];
		 for(i=4;i<MAX_NUM_BYTES;i++)
			 array1[i]=0;

	   ArrayToData(array1, 4, _INT, retVal);
	   
	   return retVal;
      }
};

class DoDMUL : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of FLOAT, DOUBLE type.

	 return (a * b);
      }
};

class DoFMUL : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of FLOAT type.

	 return (a * b);
      }
};

// DIV opcodes

// ToDo, TBD
class DoIDIV : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of INT type.

	 return (a / b);
      }
};

// MIPS uses 2 32-bit registers to store the result/remainder of the division.
// Perform division and store the values in Val as EIGHT_BYTES type.
//
//
// ToDo, TBD
// ToDo, TBD
class DoIDIV_MIPS : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of INT type.

	 return (div64(a, b));
      }
};

// ToDo, TBD
class DoIDIVU : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of INT type.

	 return (a / b);
      }
};

class DoDDIV : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of DBL type.

	 return (a / b);
      }
};

class DoFDIV : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of FLT type.

	 return (a / b);
      }
};

// REM opcodes

class DoIREM : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of INT type.
	    
	 return (a % b);
      }
};

// EQ opcodes

class DoIEQ : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of INT type.
	    
	 return Val(a == b);
      }
};

class DoDEQ : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of DBL type.
	    
	 return Val(a == b);
      }
};

class DoFEQ : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of FLT type.
	    
	 return Val(a == b);
      }
};

// NE opcodes

class DoINE : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of INT type.
	    
	 return Val(a != b);
      }
};

class DoDNE : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of DBL type.
	    
	 return Val(a != b);
      }
};

class DoFNE : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of DBL type.
	    
	 return Val(a != b);
      }
};

// LE opcodes

class DoILE : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of INT type.

	 return Val(a <= b);
      }
};

class DoDLE : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of DBL type.

	 return Val(a <= b);
      }
};

class DoFLE : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of DBL type.

	 return Val(a <= b);
      }
};

// GE opcodes

class DoIGE : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of INT type.

	 return Val(a >= b);
      }
};

class DoDGE : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of DBL type.

	 return Val(a >= b);
      }
};

class DoFGE : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of DBL type.

	 return Val(a >= b);
      }
};

// LT opcodes

class DoILT : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of INT type.
	    
	 return Val(a < b);
      }
};

class DoDLT : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of DLT type.
	    
	 return Val(a < b);
      }
};

class DoFLT : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of DLT type.
	    
	 return Val(a < b);
      }
};

// GT opcodes

class DoIGT : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of INT type.
	    
	 return Val(a > b);
      }
};

class DoDGT : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of DBL type.
	    
	 return Val(a > b);
      }
};

class DoFGT : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 // may need to check to see if a, b are of DBL type.
	    
	 return Val(a > b);
      }
};

// LSH opcodes

class DoILSH : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
   {
	 // check to see if a & b are of INT type.
	   if (a.isFractionalType() || b.isFractionalType())
	   {
		   cout << "Warning: ILSH recieved a FractionalType source argument" << endl;
		   return (a);
	   }
	   int shftval = b._as.integer;

	   if (shftval >= 0) return (a << b);
	   else 
	   {
		   Val c(-shftval);
		   return (a >> c);
	   }
   }
};

// ASH opcodes

class DoIASH : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val& b)
      {
	 // check to see if a & b are of INT type.
	   if (a.isFractionalType() || b.isFractionalType())
	   {
		   cout << "Warning: IASH recieved a FractionalType source argument" << endl;
		   return (a);
	   }
	   int shftval = b._as.integer;

	   if (shftval >= 0) return (a << b);
	   else 
	   {
		   Val c(-shftval);
		   return (a >> c);
	   }
      }
};

// AND, OR, NOR, NAND, XOR opcodes.

class DoIAND : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val& b)
      {
	 // check to see if a & b are of INT type.
	   if (a.isFractionalType() || b.isFractionalType())
	   {
		   cout << "Warning: IAND recieved a FractionalType source argument" << endl;
		   return (a);
	   }
	   return (a & b);
      }
};

class DoIOR : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val& b)
      {
	 // check to see if a & b are of INT type.
	   if (a.isFractionalType() || b.isFractionalType())
	   {
		   cout << "Warning: IOR recieved a FractionalType source argument" << endl;
		   return (a);
	   }
	   return (a | b);
      }
};

class DoINOR : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val& b)
      {
	 // check to see if a & b are of INT type.
	   if (a.isFractionalType() || b.isFractionalType())
	   {
		   cout << "Warning: INOR recieved a FractionalType source argument" << endl;
		   return (a);
	   }
	   return (~(a | b));
      }
};

class DoIXOR : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val& b)
      {
	 // check to see if a & b are of INT type.
	   if (a.isFractionalType() || b.isFractionalType())
	   {
		   cout << "Warning: IXOR recieved a FractionalType source argument" << endl;
		   return (a);
	   }
	   return (a ^ b);
      }
};

// LOAD opcodes

class DoIVLOAD : public BaseOpExecSemantics
{
 public:
	 //
	 // ********************************************
	 // NOTE: THIS METHOD IS NO LONGER BEING USED.
	 // ********************************************
	 //
/*
   Val operator()(Val & a, Val & b, Connection *memC)
      {
	   SignalClass lat;

	 int memAddr;
	 memAddr = (a + b)._as.integer;
	 
	 AddrOperand readaddr(_DIRECTMEMORY_, memAddr);

	 Val retVal ;
	 memC->read(retVal,readaddr, lat);
	 return retVal;
      }
*/

   Val operator()(Val & a, Val & b)
      {
		// compute the address
		//
		int addr = getIntegerValue(a + b);

		return Val(addr);
      }
};

class DoDVLOAD : public BaseOpExecSemantics
{
 public:
	 //
	 // ********************************************
	 // NOTE: THIS METHOD IS NO LONGER BEING USED.
	 // ********************************************
	 //
/*
	 Val operator()(Val & a, Val & b, Connection *memC)
      {
		SignalClass lat;

	 int memAddr;
	 memAddr = (a + b)._as.integer;
	 
	 AddrOperand readaddr(_DIRECTMEMORY_, memAddr);

	 Val retVal ;
	 memC->read(retVal,readaddr, lat);
	 return retVal;
      }
*/

   Val operator()(Val & a, Val & b)
      {
		// compute the address
		//
		int addr = getIntegerValue(a + b);

		return Val(addr);
      }
};

class DoFVLOAD : public BaseOpExecSemantics
{
 public:
	 //
	 // ********************************************
	 // NOTE: THIS METHOD IS NO LONGER BEING USED.
	 // ********************************************
	 //
/*
   Val operator()(Val & a, Val & b, Connection *memC)
      {
		SignalClass lat;

	 int memAddr;
	 memAddr = (a + b)._as.integer;
	 
	 AddrOperand readaddr(_DIRECTMEMORY_, memAddr);

	 Val retVal ;
	 memC->read(retVal,readaddr, lat);
	 return retVal;
      }
*/

   Val operator()(Val & a, Val & b)
      {
		// compute the address
		//
		int addr = getIntegerValue(a + b);

		return Val(addr);
      }
};

class DoQIVLOAD : public BaseOpExecSemantics
{
 public:
	 //
	 // ********************************************
	 // NOTE: THIS METHOD IS NO LONGER BEING USED.
	 // ********************************************
	 //
/*
	 Val operator()(Val & a, Val & b, Connection *memC)
	 {
		 SignalClass lat;
		 
		 int memAddr;
		 memAddr = (a + b)._as.integer;
		 
		 AddrOperand readaddr(_DIRECTMEMORY_, memAddr);
		 
		 Val retVal ;
		 memC->read(retVal,readaddr, lat);
		 
		 if (retVal._valType == _CHAR)
		 {
			 // i.e. if the value is a byte, then we need to convert this into an int.
			 //
			 retVal.cast_to(_INT);
		 }

		 // if the retval is a string, then take the first character and convert it into
		 // the corresponding ascii number.
		 //
		 if (retVal._valType == _INTERNAL_LABEL)
		 {
			 char *str = getStringValue(retVal);
			 if (str != NULL)
			 {
				 char a = str[0];
				 int b = a;
				 retVal.changeValue(b);
			 }
		 }
		 return retVal;
	 }
*/

   Val operator()(Val & a, Val & b)
      {
		// compute the address
		//
		int addr = getIntegerValue(a + b);

		return Val(addr);
      }
};

// QIVLOADU stands for loading an unsigned char as an unsigned int.
//
class DoQIVLOADU : public BaseOpExecSemantics
{
 public:
	 //
	 // ********************************************
	 // NOTE: THIS METHOD IS NO LONGER BEING USED.
	 // ********************************************
	 //
/*
	 Val operator()(Val & a, Val & b, Connection *memC)
	 {
		 SignalClass lat;
		 
		 int memAddr;
		 memAddr = (a + b)._as.integer;
		 
		 AddrOperand readaddr(_DIRECTMEMORY_, memAddr);
		 
		 Val retVal ;
		 memC->read(retVal,readaddr, lat);
		 
		 if (retVal._valType == _CHAR)
		 {
			 // i.e. if the value is a byte, then we need to convert this into an unsigned int.
			 //
			 unsigned char tmp = retVal._as.byte;
			 unsigned int t2 = tmp;
			 retVal.changeValue(t2);
		 }

		 // if the retval is a string, then take the first character and convert it into
		 // the corresponding ascii number.
		 //
		 if (retVal._valType == _INTERNAL_LABEL)
		 {
			 char *str = getStringValue(retVal);
			 if (str != NULL)
			 {
				 unsigned char a = str[0];
				 unsigned int b = a;
				 retVal.changeValue(b);
			 }
		 }
		 return retVal;
	 }
*/

	 Val operator()(Val & a, Val & b)
      {
		// compute the address
		//
		int addr = getIntegerValue(a + b);

		return Val(addr);
      }
};

class DoHIVLOAD : public BaseOpExecSemantics
{
 public:
	 //
	 // ********************************************
	 // NOTE: THIS METHOD IS NO LONGER BEING USED.
	 // ********************************************
	 //
/*
   Val operator()(Val & a, Val & b, Connection *memC)
      {
		SignalClass lat;

	 int memAddr;
	 memAddr = (a + b)._as.integer;
	 
	 AddrOperand readaddr(_DIRECTMEMORY_, memAddr);

	 Val retVal ;
	 memC->read(retVal,readaddr, lat);
	 return retVal;
      }
*/

   Val operator()(Val & a, Val & b)
      {
		// compute the address
		//
		int addr = getIntegerValue(a + b);

		return Val(addr);
      }
};

// STORE opcodes

class DoIVSTORE : public BaseOpExecSemantics
{
 public:

	 //
	 // ********************************************
	 // NOTE: THIS METHOD IS NO LONGER BEING USED.
	 // ********************************************
	 //
/*
   Val operator()(AddrOperand & dst, Val & a, Val & b, Connection *memC)
      {
	   SignalClass lat;

		AddrOperand addr;
		addr.setAddrOperand(_DIRECTMEMORY_, (dst.getAddrOperand() + b._as.integer));

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
		memC->write(tmpa, addr, lat);

		return (Val(0));
      }
*/

   Val operator()(AddrOperand & dst, Val & a, Val & b)
   {
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

		if (b._valType == _CHAR)
		{
			b.changeValue(b._as.integer);
		}

		// compute the address
		//
		int addr = dst.getAddrOperand() + getIntegerValue(a);

		return Val(addr);
   }

   Val operator()(Val & base, Val & offset, Val & source)
   {
		// if source is a byte, need to store it as an integer, becos IVSTORE is supposed to
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

		if (source._valType == _CHAR)
		{
			source.changeValue(source._as.integer);
		}

		// compute the address
		//
		int addr = getIntegerValue(base) + getIntegerValue(offset);

		return Val(addr);
   }

};

class DoDVSTORE : public BaseOpExecSemantics
{
 public:
	 //
	 // ********************************************
	 // NOTE: THIS METHOD IS NO LONGER BEING USED.
	 // ********************************************
	 //
/*
   Val operator()(AddrOperand & dst, Val & a, Val & b, Connection *memC)
      {

	   SignalClass lat;
		
		AddrOperand addr;
		addr.setAddrOperand(_DIRECTMEMORY_, (dst.getAddrOperand() + b._as.integer));
		memC->write(a, addr, lat);

		return (Val(0));
   }
*/

   Val operator()(AddrOperand & dst, Val & a, Val & b)
   {
		// compute the address
		//
		int addr = dst.getAddrOperand() + getIntegerValue(a);

		return Val(addr);
   }

   Val operator()(Val & base, Val & offset, Val & source)
   {
		// compute the address
		//
		int addr = getIntegerValue(base) + getIntegerValue(offset);

		return Val(addr);
   }
};

class DoFVSTORE : public BaseOpExecSemantics
{
 public:
	 //
	 // ********************************************
	 // NOTE: THIS METHOD IS NO LONGER BEING USED.
	 // ********************************************
	 //
/*
   Val operator()(AddrOperand & dst, Val & a, Val & b, Connection *memC)
      {

	   SignalClass lat;
		
		AddrOperand addr;
		addr.setAddrOperand(_DIRECTMEMORY_, (dst.getAddrOperand() + b._as.integer));
		memC->write(a, addr, lat);

		return (Val(0));
   }
*/

   Val operator()(AddrOperand & dst, Val & a, Val & b)
   {
		// compute the address
		//
		int addr = dst.getAddrOperand() + getIntegerValue(a);

		return Val(addr);
   }

   Val operator()(Val & base, Val & offset, Val & source)
   {
		// compute the address
		//
		int addr = getIntegerValue(base) + getIntegerValue(offset);

		return Val(addr);
   }
};

class DoQIVSTORE : public BaseOpExecSemantics
{
 public:
	 //
	 // ********************************************
	 // NOTE: THIS METHOD IS NO LONGER BEING USED.
	 // ********************************************
	 //
/*
   Val operator()(AddrOperand & dst, Val & a, Val & b, Connection *memC)
      {

	   SignalClass lat;
		
		AddrOperand addr;
		addr.setAddrOperand(_DIRECTMEMORY_, (dst.getAddrOperand() + b._as.integer));
		memC->write(a, addr, lat);

		return (Val(0));
   }
*/

   Val operator()(AddrOperand & dst, Val & a, Val & b)
   {
		// compute the address
		//
		int addr = dst.getAddrOperand() + getIntegerValue(a);

		return Val(addr);
   }

   Val operator()(Val & base, Val & offset, Val & source)
   {
		// compute the address
		//
		int addr = getIntegerValue(base) + getIntegerValue(offset);

		return Val(addr);
   }
};

class DoHIVSTORE : public BaseOpExecSemantics
{
 public:
 	 //
	 // ********************************************
	 // NOTE: THIS METHOD IS NO LONGER BEING USED.
	 // ********************************************
	 //
/*
   Val operator()(AddrOperand & dst, Val & a, Val & b, Connection *memC)
      {

	   SignalClass lat;
		
		AddrOperand addr;
		addr.setAddrOperand(_DIRECTMEMORY_, (dst.getAddrOperand() + b._as.integer));
		memC->write(a, addr, lat);

		return (Val(0));
   }
*/

   Val operator()(AddrOperand & dst, Val & a, Val & b)
   {
		// compute the address
		//
		int addr = dst.getAddrOperand() + getIntegerValue(a);

		return Val(addr);
   }

   Val operator()(Val & base, Val & offset, Val & source)
   {
		// compute the address
		//
		int addr = getIntegerValue(base) + getIntegerValue(offset);

		return Val(addr);
   }
};

// IF opcodes

class DoIF : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
      {
	 Val b(0);
	 
	 return Val(a != b);
      }
};

// GOTO opcodes

class DoGOTO : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
      {
	 return (a);
      }
};

class DoIGOTO : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
      {
	 return (a);
      }
};

// CALL, RET opcodes

class DoCALL : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
   {
	 return (a);
   }
   Val operator()()
   {
	   return (Val(-1));
   }
};

class DoRETURN : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
   {
	 return (a);
   }
   Val operator()()
   {
	   return (Val(-1));
   }
};

// CVT opcodes

class DoCVTDI : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
   {
	   if ((a._valType != _INT) && (a._valType != _UINT) && (a._valType != _SHORT))
	   {
		   if (debugSIMULATOR)
		   {
			   // error condition.
			   cout << "Warning: CVTDI recieved a non int/uint/short source operand." << endl;
		   }
	   }

	   Val ret = a;
	   ret.cast_to(_DBL);
	   return (ret);
   } 
};

class DoCVTSI : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
   {
	   if ((a._valType != _INT) && (a._valType != _UINT) && (a._valType != _SHORT))
	   {
		   if (debugSIMULATOR)
		   {
			   // error condition.
			   cout << "Warning: CVTSI recieved a non int/uint/short source operand." << endl;
			   
		   }
		   // Forcibly cast it to int.
		   a._valType = _INT;
	   }

	   Val ret = a;
	   ret.cast_to(_FLT);
	   return (ret);
   } 
};

class DoTRUNCID : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
   {
	   if ((a._valType != _FLT) && (a._valType != _DBL))
	   {
		   if (debugSIMULATOR)
		   {
			   // error condition.
			   cout << "Warning: TRUNCID recieved a non float/dbl source operand." << endl;
		   }
	   }

	   Val ret = a;
	   ret.cast_to(_INT);
	   return (ret);
   } 

   Val operator()(Val & a, Val & b)
   {
	   if ((a._valType != _FLT) && (a._valType != _DBL))
	   {
		   if (debugSIMULATOR)
		   {
			   // error condition.
			   cout << "Warning: TRUNCID recieved a non float/dbl source operand." << endl;
		   }
	   }

	   Val ret = a;
	   ret.cast_to(_INT);
	   return (ret);
   } 
};

class DoTRUNCIS : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
   {
	   if ((a._valType != _FLT) && (a._valType != _DBL))
	   {
		   if (debugSIMULATOR)
		   {
			   // error condition.
			   cout << "Warning: TRUNCIS recieved a non float/dbl source operand." << endl;
		   }
	   }

	   Val ret = a;
	   ret.cast_to(_INT);
	   return (ret);
   } 

   Val operator()(Val & a, Val & b)
   {
	   if ((a._valType != _FLT) && (a._valType != _DBL))
	   {
		   if (debugSIMULATOR)
		   {
			   // error condition.
			   cout << "Warning: TRUNCIS recieved a non float/dbl source operand." << endl;
		   }
	   }

	   Val ret = a;
	   ret.cast_to(_INT);
	   return (ret);
   } 
};

// MTC opcodes

class DoMTC : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
   {
	   if ((a._valType != _INT) && (a._valType != _UINT) && (a._valType != _SHORT))
	   {
		   if (debugSIMULATOR)
		   {
			   // error condition.
			   cout << "Warning: MTC recieved a non int/uint/short source operand." << endl;
		   }
	   }

	   Val ret = a;
//	   ret.cast_to(_FLT);
	   return (ret);
   }
};
   
class DoDMTC : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
      {
	   if ((a._valType != _INT) && (a._valType != _UINT) && (a._valType != _SHORT))
	   {
		   if (debugSIMULATOR)
		   {
			   // error condition.
			   cout << "Warning: DMTC recieved a non int/uint/short source operand." << endl;
		   }
	   }

	   Val ret = a;
//	   ret.cast_to(_DBL);
	   return (ret);
      }
};

// MFC opcodes

class DoMFC : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
   {
	   if ((a._valType != _FLT) && (a._valType != _DBL))
	   {
		   if (debugSIMULATOR)
		   {
			   // error condition.
			   cout << "Warning: MFC recieved a non float/dbl source operand." << endl;
		   }
	   }

	   Val ret = a;
//	   ret.cast_to(_INT);
	   return (ret);
   }
};

class DoDMFC : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
   {
	   if ((a._valType != _FLT) && (a._valType != _DBL))
	   {
		   if (debugSIMULATOR)
		   {
			   // error condition.
			   cout << "Warning: DMFC recieved a non float/dbl source operand." << endl;
		   }
	   }

	   Val ret = a;
//	   ret.cast_to(_INT);
	   return (ret);
   }
};

// Special opcodes:

// Radu - May 21, 2000
/*
class DoPRINTF : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
   {
	   return a;
   }
};

*/
class DoSQRT : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
   {
	   return (a.squareroot());
   }
};

class DoEXP : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
   {
	   return (a.exponential());
   }
};
  

/*
// DUMP function.
// Note: this is not an actual opcode.
// However, the dump function can be used as a normal opcode,
// Instead of a computation, the opcode prints out the contents:

class DoDUMP : public BaseOpExecSemantics
{
public:
	Val operator()(Val & a)
	{
		cout << a;
		return (a);
	}

	Val operator()(Val & a, AddrOperand & b)
	{
		cout << b << "\t" << a;
	}
}
*/

class DoFNEG : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
      {
	   if (a._valType != _FLT)
	   {
		   if (debugSIMULATOR)
		   {
			   // Error condition.
			   cout << "Warning: FNEG recieved a non-float value" << endl;
		   }
	   }
	 return (-a);
      }
};

class DoDNEG : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
      {
	   if (a._valType != _DBL)
	   {
		   if (debugSIMULATOR)
		   {
			   // Error condition.
			   cout << "Warning: DNEG recieved a non-double value" << endl;
		   }
	   }
	 return (-a);
      }
};

// ABS opcodes

class DoIABS : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
      {
	   if (a._valType != _INT)
	   {
		   if (debugSIMULATOR)
		   {
			   // Error condition.
			   cout << "Warning: DASSIGN recieved a non-dbl value" << endl;
		   }
	   }
	   a.cast_to(_INT);

	   int temp = a._as.integer;
	   if (temp < 0) 
	   {
		   a._as.integer = -temp;
	   }

	 return (a);
      }
};

class DoDABS : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
      {
	   if (a._valType != _DBL)
	   {
		   if (debugSIMULATOR)
		   {
			   // Error condition.
			   cout << "Warning: DASSIGN recieved a non-dbl value" << endl;
		   }
	   }
	   a.cast_to(_DBL);

	   double temp = a._as.dbl;
	   if (temp < 0.0) 
	   {
		   a._as.dbl = -temp;
	   }

	 return (a);
      }
};

// ILAND, ILOR opcodes

class DoILAND : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 return Val(a & b);
      }
};

class DoILOR : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
      {
	 return Val(a | b);
      }
};

class DoCVTSD : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
      {
	   if (a._valType != _DBL)
	   {
		   if (debugSIMULATOR)
		   {
			   // error condition.
			   cout << "Warning: CVTSD recieved a non dbl source operand." << endl;
		   }
	   }

	   Val ret = a;
	   ret.cast_to(_FLT);
	   return (ret);
      }
};

class DoCVTDS : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a)
      {
	   if (a._valType != _FLT)
	   {
		   if (debugSIMULATOR)
		   {
			   // error condition.
			   cout << "Warning: CVTSD recieved a non flt source operand." << endl;
		   }
	   }

	   Val ret = a;
	   ret.cast_to(_DBL);
	   return (ret);
      }
};

// OPCODES due to the latest version of GCC/MIPS.

// Logical Shift opcodes
//
class DoILLSH : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
   {
	 // check to see if a & b are of INT type.
	   if (a.isFractionalType() || b.isFractionalType())
	   {
		   cout << "Warning: ILSH recieved a FractionalType source argument" << endl;
		   return (a);
	   }

	   return (a << b);
   }
};

class DoIRLSH : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val & b)
   {
	 // check to see if a & b are of INT type.
	   if (a.isFractionalType() || b.isFractionalType())
	   {
		   cout << "Warning: ILSH recieved a FractionalType source argument" << endl;
		   return (a);
	   }

	   return (a >> b);
   }
};

// Arithmetic shift opcodes
//
class DoILASH : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val& b)
      {
	 // check to see if a & b are of INT type.
	   if (a.isFractionalType() || b.isFractionalType())
	   {
		   cout << "Warning: IASH recieved a FractionalType source argument" << endl;
		   return (a);
	   }

	   return (a << b);
	  }
};

class DoIRASH : public BaseOpExecSemantics
{
 public:
   Val operator()(Val & a, Val& b)
      {
	 // check to see if a & b are of INT type.
	   if (a.isFractionalType() || b.isFractionalType())
	   {
		   cout << "Warning: IASH recieved a FractionalType source argument" << endl;
		   return (a);
	   }

	   return (a >> b);
	  }
};

/**************************************************************/
/**************************************************************/

extern DoNOP nopFunc;

extern DoICONSTANT iconstantFunc;
extern DoDCONSTANT dconstantFunc;
extern DoFCONSTANT fconstantFunc;
extern DoASSIGN assignFunc;
extern DoIASSIGN iassignFunc;
extern DoFASSIGN fassignFunc;
extern DoDASSIGN dassignFunc;

extern DoIADD iaddFunc;
extern DoDADD daddFunc;
extern DoFADD faddFunc;
extern DoISUB isubFunc;
extern DoDSUB dsubFunc;
extern DoFSUB fsubFunc;
extern DoIMUL imulFunc;
extern DoMAC macFunc;
extern DoIMULU imuluFunc;
extern DoDMUL dmulFunc;
extern DoFMUL fmulFunc;
extern DoIDIV idivFunc;
extern DoIDIVU idivuFunc;
extern DoDDIV ddivFunc;
extern DoFDIV fdivFunc;
extern DoIREM iremFunc;

extern DoIEQ ieqFunc;
extern DoDEQ deqFunc;
extern DoFEQ feqFunc;
extern DoINE ineFunc;
extern DoDNE dneFunc;
extern DoFNE fneFunc;
extern DoIGE igeFunc;
extern DoIGE igeuFunc;
extern DoDGE dgeFunc;
extern DoFGE fgeFunc;
extern DoILE ileFunc;
extern DoILE ileuFunc;
extern DoDLE dleFunc;
extern DoFLE fleFunc;
extern DoILT iltFunc;
extern DoILT iltuFunc;
extern DoDLT dltFunc;
extern DoFLT fltFunc;
extern DoIGT igtFunc;
extern DoIGT igtuFunc;
extern DoDGT dgtFunc;
extern DoFGT fgtFunc;

extern DoILSH ilshFunc;
extern DoIASH iashFunc;
extern DoILAND ilandFunc;
extern DoILOR ilorFunc;

extern DoIAND iandFunc;
extern DoIOR iorFunc;
extern DoINOR inorFunc;
extern DoIXOR ixorFunc;

extern DoIVLOAD ivloadFunc;
extern DoDVLOAD dvloadFunc;
extern DoFVLOAD fvloadFunc;
extern DoHIVLOAD hivloadFunc;
extern DoHIVLOAD hivloaduFunc;
extern DoQIVLOAD qivloadFunc;
extern DoQIVLOADU qivloaduFunc;
extern DoIVSTORE ivstoreFunc;
extern DoDVSTORE dvstoreFunc;
extern DoFVSTORE fvstoreFunc;
extern DoHIVSTORE hivstoreFunc;
extern DoHIVSTORE hivstoreuFunc;
extern DoQIVSTORE qivstoreFunc;
extern DoQIVSTORE qivstoreuFunc;

extern DoIF ifFunc;
extern DoIF ifftFunc;
extern DoGOTO gotoFunc;
extern DoIGOTO igotoFunc;
extern DoCALL callFunc;
extern DoRETURN returnFunc;

// extern DoDUMP dumpFunc;

// Radu - May 21, 2000
/*
extern DoPRINTF printfFunc;
*/
extern DoSQRT sqrtFunc;
extern DoEXP expFunc;


extern DoCVTDI cvtdiFunc;
extern DoCVTSI cvtsiFunc;
extern DoCVTSD cvtsdFunc;
extern DoCVTDS cvtdsFunc;

extern DoTRUNCID truncidFunc;
extern DoTRUNCIS truncisFunc;

extern DoMTC mtc1Func;
extern DoDMTC dmtc1Func;
extern DoMFC mfc1Func;
extern DoDMFC dmfc1Func;

extern DoFNEG fnegFunc;
extern DoDNEG dnegFunc;

extern DoIABS iabsFunc;
extern DoDABS dabsFunc;

extern DoILLSH illshFunc;
extern DoIRLSH irlshFunc;
extern DoILASH ilashFunc;
extern DoIRASH irashFunc;

extern DoIMUL_MIPS imulMIPSFunc;
extern DoIDIV_MIPS idivMIPSFunc;
extern DoMFHI mfhiFunc;
extern DoMFLO mfloFunc;

extern DoASSERT assertFunc;

#endif

