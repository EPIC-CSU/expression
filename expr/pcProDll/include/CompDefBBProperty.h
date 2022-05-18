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
//==========================================================================
// File:			CompDefBBProperty.h
// Created:			Oct. 21, 97
// Author:			Xiaomei Ji
//
//=========================================================================

#ifndef __COMPDEFBBPROPERTY_H__
#define __COMPDEFBBPROPERTY_H__

#ifndef WIN32
#include <iostream.h>
#endif

#include "PropertyIncludes.h"
#include "MyLinkedList.h"
#include "BaseArgument.h"
#include "BaseBBProperty.h"


//=========================================================================
// the compute definition property of basic block,
// which records all the defined variables in the basic block
// please note that: all variables refer to Register Argument in this file
//=========================================================================

class CompDefBBProperty : public BaseBBProperty {
private:
	MyLinkedList<BaseArgument*> _defList;
public:
	CompDefBBProperty() { }
	virtual ~CompDefBBProperty() { }
 // because of non-const in class MyLinkedListIterator
 // omit const for the following argument
	CompDefBBProperty(CompDefBBProperty& cdProp) {
  _defList.removeAll();
  MyLinkedListIterator<BaseArgument*>* iter=cdProp.defListIterForw();	
  while ( iter->hasMoreElements() ) {
   this->appendDefinition(iter->currentElement());
   iter->nextElement();
  }
 }
 /***************      no == in MyLinkedList
 
   CompDefBBProperty& operator = (const CompDefBBProperty& prop) {
   if ( *this == prop )
   return;
   _defList.removeAll();
   MyLinkedListIterator<BaseArgument*>* iter=prop.defListIterForw();	
   while ( iter->hasMoreElements() ) {
   this->addDefinition(iter->currentElement());
   iter->nextElement();
   }
   return *this;
   }
 ******/
 
	MyLinkedList<BaseArgument*>& getDefList() { return _defList; }
	
 void appendDefinition(BaseArgument* def) { _defList.append(def); }
	
 MyLinkedListIterator<BaseArgument*>* defListIterForw() {
		return _defList.elementsForward();
	}
	
 MyLinkedListIterator<BaseArgument*>* defListIterBack() {
		return _defList.elementsBackward();
 }
	int is(PropertyNames prop){if (prop==_COMPDEF_) return _YES_;return _NO_;} 
 
	BaseBBProperty* copy();
 
	// the second parameter should be const,  
	// but now without because of the << of MyLinkedList
	friend ostream& operator<< (ostream& out, CompDefBBProperty& prop) {
  out << "CompDefBBProperty: " << endl;
  return out << prop._defList;
 }
	void print() {
  cout << "CompDefBBProperty: " << _defList << endl;
  /***************
  cout << "CompDefBBProperty: " << endl;
  MyLinkedListIterator<BaseArgument*>* iter=defListIterForw();
  while ( iter->hasMoreElements() ) {
  BaseArgument* arg = iter->currentElement();
  cout << arg << ": ";
  arg->print();
  iter->nextElement();
  }
  cout << "end of CompDefBBProperty " << endl;
  *****************/
	}
	void print(ostream& out) {
  out << "CompDefBBProperty: " << _defList << endl;
  /*************
  out << "CompDefBBProperty: " << endl;
  MyLinkedListIterator<BaseArgument*>* iter=defListIterForw();
  while ( iter->hasMoreElements() ) {
  BaseArgument* arg = iter->currentElement();
  out << arg << ": ";
  arg->print(out);
  iter->nextElement();
  }
  out << "end of CompDefBBProperty " << endl;
  ***************/
	}
 
	int isEq(BaseBBProperty* a) {
  if ( a->is(_COMPDEF_) ) {
   MyLinkedList<BaseArgument*>& temp_list = ((CompDefBBProperty*)a)->getDefList();
   if ( temp_list.numItems() == _defList.numItems() ) {
    MyLinkedListIterator<BaseArgument*>* iter=defListIterForw();
    while ( iter->hasMoreElements() ) {
     if ( temp_list.contains(iter->currentElement()) == _NO_ ) {
      delete iter;
      return _NO_;
     }
     iter->nextElement();
    }
    delete iter;
    return _YES_;
   }
  }
  return _NO_;
 }
};

#endif

