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
// OpPredicate.h: interface for the OpPredicate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPPREDICATE_H__9BA47F58_9D15_448E_9880_FF1C903F0B69__INCLUDED_)
#define AFX_OPPREDICATE_H__9BA47F58_9D15_448E_9880_FF1C903F0B69__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifdef WIN32
#pragma warning(disable:4786)
#endif

//#include <vector>

class RealRegArg;
class BaseArgument;
class BaseOperation;
#include "PropertyIncludes.h"
#include "BaseOpProperty.h"

class PredicateArg
{
public:
 BaseOperation* op;
 RealRegArg* arg;
 bool negated;
 PredicateArg();
 virtual ~PredicateArg();
 virtual PredicateArg* copy() const;
 void print(ostream& out);
};

typedef vector<PredicateArg*> PAPList;
typedef vector<PredicateArg*>::iterator PAPListIter;

class OpPredicate : public BaseOpProperty  
{
public:
 //temp
 char* text;
	OpPredicate(char* txt = NULL);
 virtual BaseOpProperty* copy() const;
	virtual ~OpPredicate();
	int is(PropertyNames name);
 int isEq(BaseOpProperty* a);
	void print(ostream& out);
 void addPredicateArg(BaseOperation* op, BaseArgument* arg, bool isNegated);
 PAPList ccList;
};

#endif // !defined(AFX_OPPREDICATE_H__9BA47F58_9D15_448E_9880_FF1C903F0B69__INCLUDED_)
