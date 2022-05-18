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
#include "stdafx.h"

#include "DUProperty.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


BaseArgProperty* DUChainProperty::copy() 
{
 BaseArgProperty* prop;
 prop = new DUChainProperty(*this);
 return prop;
}


void DUChainProperty::print() { 
	if (_useList.isEmpty()) return;

	cout << "Use: "; 
	MyLinkedListIterator<BaseOperation*>* iter = useListIteratorForw();
	while ( iter->hasMoreElements() ) {
		BaseOperation* op = iter->currentElement();
		//cout << op->idValue() << "; ";
		op->shortPrint(cout);
		cout << "; ";
		//op->print();
		iter->nextElement();
	}
	delete iter;
}
void DUChainProperty::print(ostream& out) {
	if (_useList.isEmpty()) return;

	out << "{<Use:"; 
	MyLinkedListIterator<BaseOperation*>* iter = useListIteratorForw();

	// becos cout has a problem, we need to insert an endl after some
	// shortprints.
	//
	int i = 0;
	while ( iter->hasMoreElements() ) {
		BaseOperation* op = iter->currentElement();
		if (op != NULL)
		{
			out << "  ";
			op->shortPrint(out);
		}
		//op->print(out);
		i++;
		if ((i % 2) == 0)
			out << endl;
		iter->nextElement();
	}
	out << " >}";
	// if (i >= 2) out << endl;
	delete iter;
}