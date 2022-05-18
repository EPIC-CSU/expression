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
// $Id$
//
// File:  		STLIncludes.h
// Created:		Wed Dec 02, 98
// Last modified: 	Wed Dec 02, 98
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: SUPPORT DS

#ifndef _ARCHSTLINCLUDES_H_
#define _ARCHSTLINCLUDES_H_

#ifndef WIN32
#include <stl.h>
#include <multimap.h>
#include <algo.h>
#include <strstream.h>
#else //WIN32
#pragma warning(disable:4786)
#include <map>
#include <set>
#include <deque>
#include <vector>
#include <algorithm>
#include <strstrea.h>
#include <string.h>
#include <ctype.h>

#include "../../pcProDll/include/MyString.h"

#define set_intersection std::set_intersection
#define lower_bound std::lower_bound
// #define find std::find
#define insert_iterator std::insert_iterator
#define sort std::sort
#define pair std::pair

#define set std::set
#define less std::less
#define deque std::deque
#define multimap std::multimap

#ifdef map
#undef map
#endif
#define map std::map

#ifdef vector
#undef vector
#endif 
#define vector std::vector
#endif

#define Vector vector
#define Map map

#endif
