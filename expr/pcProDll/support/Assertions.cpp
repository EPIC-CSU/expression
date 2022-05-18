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

#ifdef WIN32
#pragma warning(disable:4786)
#include "crtdbg.h"
#endif

#include "stdafx.h"


#include "Assertions.h"
#include "BaseOperation.h"
#include "InstrTypeProperty.h"

#include <string.h>

#include <map>


//the map of assertions for each operation
//map<BaseOperation *,Assertion *> assertions;

BaseArgProperty *createHLInfoAssertion(char *name, char *value)
{
	BaseArgProperty *retptr;
	retptr = new HLInfoArgAssertion(name, value);
	
	return retptr;
}


//creates a new assertion node with the given name and value
Assertion * create_new_assertion(char *name,char *value){
	Assertion *a;

	a=new Assertion();
	a->name=strdup(name);
	a->value=strdup(value);

	return a;
}

/*
//adds an assertion to an operation 
void add_assertion_to_stmt(Assertion *assert,BaseOperation *oper){
	InstrTypeProperty * prop;
	_ASSERT(oper);
	_ASSERT(assert);

	if(strcmp(assert->name,"INSTRTYPE")==0){
		//the INSTRTYPE assertion
		prop = new InstrTypeProperty(assert->value);
		oper->addProperty(*prop);
	}

	//assertions[oper]=assert;
}
*/

//adds an assertion to an operation 
void add_assertion_to_stmt(BaseArgProperty *arg,BaseOperation *oper){
	InstrTypeProperty * prop;
	HLInfoArgAssertion * assert;
	_ASSERT(oper);
	_ASSERT(assert);

	if((assert=dynamic_cast<HLInfoArgAssertion*>(arg))!=NULL){
		if(strcmp(assert->getName().getStr(),"INSTRTYPE")==0){
			//the INSTRTYPE assertion
			prop = new InstrTypeProperty(assert->getValue().getStr());
			oper->addProperty(*prop);
		}
	}

	//assertions[oper]=assert;
}