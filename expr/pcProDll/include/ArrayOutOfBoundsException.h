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
// $Id: ArrayOutOfBoundsException.h,v 1.4 1998/03/25 01:50:24 wtang Exp $

/*
** ArrayOutOfBoundsException Class
** Throw this exception if an array index is out of bounds
*/

#ifndef _ARRAY_OUT_OF_BOUNDS_EXCEPTION
#define _ARRAY_OUT_OF_BOUNDS_EXCEPTION

// Includes
#ifndef WIN32
#include <iostream.h>
#endif

#ifdef __UNIX__
#include <assert.h>
#define ASSERT assert
#endif
#include "Exception.h"

// Defines

// Class/(Module Function) declarations 

class ArrayOutOfBoundsException : public Exception
{
  private:
    const int index;

  public:
    ArrayOutOfBoundsException() : Exception(), index(0)
    {
    }
    
    ArrayOutOfBoundsException(const char *m, const int i) : 
	Exception(m), index(i)
    {
	print(cout);
 	ASSERT(0);
    }

    virtual void print(ostream &out = cout)
    {
    out << *this;
    }

    friend ostream & operator<< (ostream &out, ArrayOutOfBoundsException &e)
    {
	out << "ArrayOutOfBoundsException: ";
	if (e.message)
	    out << e.message << " " << e.index << endl;
	else
	    out << "Unknown" << endl;
	return out;
    }
};

#endif
