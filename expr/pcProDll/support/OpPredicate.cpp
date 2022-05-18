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
// OpPredicate.cpp: implementation of the OpPredicate class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OpPredicate.h"
#include "RealRegArg.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


PredicateArg::PredicateArg()
{
 arg=NULL;
 negated=false;
}

PredicateArg::~PredicateArg()
{
 if (arg!=NULL) delete arg;
}

PredicateArg* PredicateArg::copy() const
{
 PredicateArg* pa=new PredicateArg();
 
 //copy the negate
 pa->negated=negated;
 //copy the argument
 pa->arg=(RealRegArg*)(arg->copy());
 //copy the op
 pa->op=op;

 return pa;
}


void PredicateArg::print(ostream& out) 
{
 if (negated==true) out << "!";
 arg->print(out);
}


OpPredicate::OpPredicate(char* txt)
{
 text=txt; 
}

OpPredicate::~OpPredicate()
{
 if (text!=NULL) delete text;
 for (PAPList::iterator i=ccList.begin();i!=ccList.end();i++)
 {
  delete *i;
 }
}

int OpPredicate::is(PropertyNames name)
{
 if (name==_PREDICATE_) return _YES_;
 return _NO_;
}

int OpPredicate::isEq(BaseOpProperty* a) 
{
 return _NO_; //for now
}

void OpPredicate::print(ostream& out) 
{
 if (text!=NULL) 
  out << " PRED: " << text;
 else
  out << " PRED: NULL";

// //now print the same thing based on the args in ccList
// out << "PRED: ";
// for (PAPList::iterator i=(PredicateArg**)ccList.begin();i!=ccList.end();i++)
// {
//  if (i!=ccList.begin()) out << " & ";
//  (*i)->print(out);
// }

 out << endl;
}

BaseOpProperty* OpPredicate::copy() const
{
 OpPredicate* opPred=new OpPredicate();
 
 //copy the text
 opPred->text=strdup(text);

 //also copy all the items in the ccList
 for (PAPList::iterator i=(PredicateArg**)ccList.begin();i!=ccList.end();i++)
 {
  opPred->ccList.push_back((*i)->copy());
 }

 return opPred;
}

void OpPredicate::addPredicateArg(BaseOperation* op, BaseArgument* arg, bool isNegated)
{
 PredicateArg* pa=new PredicateArg(); 
 //copy the argument
 pa->op=op;
 pa->arg=(RealRegArg*)arg;
 pa->negated=isNegated;
 ccList.push_back(pa);
}

