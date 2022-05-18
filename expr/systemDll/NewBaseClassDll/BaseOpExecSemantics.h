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
 *file  : BaseOpExecSemantics.h                              *
 *authors : ahalambi,vgee                                    *
 *last modified :			                                 *
 *                                                           *
 *contents:												     *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/
#ifndef __BASEOPEXECSEMANTICS_H__
#define __BASEOPEXECSEMANTICS_H__

#include "ArchClassIncludes.h"
#include "ArchSTLIncludes.h"
#include "AddrData.h"
#include "Connection.h"

class BaseOpExecSemantics
{
 public:
   virtual Val operator()()
      {
		// Error: this function should never be called.
		cout << "Error: Base class method (BaseOpExecSemantics::operator()) should not be called" << endl;
		return (Val(-1));
      }
   
   virtual Val operator()(Val & a)
      {
		// Error: this function should never be called.
		cout << "Error: Base class method (BaseOpExecSemantics::operator(Val)) should not be called" << endl;
		return (Val(-1));
       }
   
   virtual Val operator()(Val & a, Val & b)
      {
		// Error: this function should never be called.
		cout << "Error: Base class method (BaseOpExecSemantics::operator(Val, Val)) should not be called" << endl;
		return (Val(-1));
       }

   virtual Val operator()(Val & a, Connection *memC)
      {
		// Error: this function should never be called.
		cout << "Error: Base class method (BaseOpExecSemantics::operator(Val, Connection)) should not be called" << endl;
		return (Val(-1));
       }

   virtual Val operator()(Val & a, Val & b, Connection *memC)
      {
		// Error: this function should never be called.
		cout << "Error: Base class method (BaseOpExecSemantics::operator(Val, Val, Connection)) should not be called" << endl;
		return (Val(-1));
       }

   virtual Val operator()(Val & a, Val & b, Val & c)
      {
		// Error: this function should never be called.
		cout << "Error: Base class method (BaseOpExecSemantics::operator(Val, Val, Val)) should not be called" << endl;
		return (Val(-1));
       }

   virtual Val operator()(AddrOperand & dst, Val & a, Connection *memC)
      {
		// Error: this function should never be called.
		cout << "Error: Base class method (BaseOpExecSemantics::operator(AddrOperand, Val, Connection)) should not be called" << endl;
		return (Val(-1));
      }
   
   virtual Val operator()(AddrOperand & dst, Val & a, Val & b, Connection *memC)
      {
		// Error: this function should never be called.
		cout << "Error: Base class method (BaseOpExecSemantics::operator(AddrOperand, Val, Val, Connection)) should not be called" << endl;
		return (Val(-1));
      }
   
   virtual BaseOpExecSemantics & operator =(const BaseOpExecSemantics & a)
      {
		// Error: this function should never be called.
		cout << "Error: Base class method (BaseOpExecSemantics::operator =) should not be called" << endl;
		return (*this);
       }

   // Added by SAH & SS on 03/02/01
   //
   // Use this function to execute functions with variable number of source operands
   // or if the number of source operands is greater than 3.
   //
   virtual Val operator()(Vector<Val> & l)
      {
		// Error: this function should never be called.
		cout << "Error: Base class method (BaseOpExecSemantics::operator ()(Vector<Val>)) should not be called" << endl;
		return (Val(-1));
       }
};
#endif

