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
// $Id: CouldNotOpenFileException.h,v 1.4 1998/03/25 01:50:26 wtang Exp $
//
// File:  		CouldNotOpenFileException.h
// Created:		Mon Nov 04, 96
// Last modified: 	Tue Nov 05, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: include

// This file contains the definition of the CouldNotOpenFileException
// class . This clas defines the exception object that will be thrown
// by the FilePointer class object whenever fopen returns an error
// value.
// This exception also has a method which can be called to display
// error messages.
// See FilePointer.h for an example on how to cath this exception.
// I have designed the FilePointer class so that the procedure for
// opening any file is hidden. I do not expect that this exception
// will be thrown by any other class. However, I have designed this
// class with global scope (as opposed to defining it within the
// FilePointer class), so that anyone foolish enuff to use fopen to
// open files will still be able to redeem himself by using this
// exception class to throw any error conditions. :-)

// Tue Nov 05, 96 :
// Another thing:
// This comment does not really belong here., but I have to mention it
// somewhere : I'd really love to have all the Exception files in a
// separate module (called EXCEPTION), and for the Exceptions.h file
// to act as the global include file for that module. The way
// Exceptions.h is used now is not the way it should be.
//

#ifndef __COULDNOTOPENFILEEXCEPTION_H__
#define __COULDNOTOPENFILEEXCEPTION_H__

#include "Exception.h"

#ifdef __UNIX__
#define ASSERT assert
#endif

// Tue Nov, 05 96 :
// As of now, this class does not have any member data. Maybe, it
// should have the filename stored in it.
// The message data member of the Exception class is being used to
// store the file name as of now. (see the second constructor below)

class CouldNotOpenFileException : public Exception
{
 private:
 public:
   CouldNotOpenFileException() : Exception()
      {
      }

   CouldNotOpenFileException(const char *m) : Exception(m)
      {
	print(cout);
 	ASSERT(0);
      }
   
   ~CouldNotOpenFileException()
      {
      }
   
   virtual void print(ostream &out = cout)
      {
	 out << *this;
      }
   
   friend ostream & operator<< (ostream &out, CouldNotOpenFileException &e)
      {
	 out << "CouldNotOpenFileException: ";
	 if (e.message)
	    out << e.message << " " << endl;
	 else
	    out << "Unknown" << endl;
	 return out;
      }
};

#endif
