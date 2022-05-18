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
// $Id: NoSuchElementException.h,v 1.4 1998/03/25 01:50:28 wtang Exp $

/*
** NoSuchElementException Class
** Throw this exception if an element does not exist in a Vector, LinkedList,
** etc.
*/

#ifndef _NO_SUCH_ELEMENT_EXCEPTION
#define _NO_SUCH_ELEMENT_EXCEPTION

// Includes
#ifdef __UNIX__
	#include <iostream.h>
	#define ASSERT assert
#endif

#include "Exception.h"

// Defines

// Class/(Module Function) declarations 

class NoSuchElementException : public Exception
{
  public:
    NoSuchElementException() : Exception()
    {
    }
    
    NoSuchElementException(const char *m) : Exception(m)
    {
	print(cout);
// ASSERT(0);
    }

	virtual void print(ostream &out = cout)
    {
    out << *this;
    }

    friend ostream & operator<< (ostream &out, NoSuchElementException &e)
    {
	out << "NoSuchElementException: ";
	if (e.message)
	    out << e.message << endl;
	else
	    out << "Unknown" << endl;
	return out;
    }
};

#endif
