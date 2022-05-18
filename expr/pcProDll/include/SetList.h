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
#ifndef _DOUBLELIST_H
#define _DOUBLELIST_H

#include "MyLinkedList.h"
#include "GlobalIncludes.h"

template <class T> class SetList : public MyLinkedList<T> 
{
#ifdef LEAKY
	//NOS: used for memory leaks
	char tag[MAX_PATH];
#endif
	
public: 
	
	SetList()
	{
#ifdef LEAKY
		{
			//NOS
			char t[MAX_PATH];
			GetFileTitle(__FILE__,t,MAX_PATH); 
			sprintf(tag,"%d:%s",__LINE__,t);
		}
#endif
	}
	
	
	int set_union(SetList<T>& temp_list) {
		int change = 0;
		MyLinkedListIterator<T>* iter = temp_list.elementsForward();
		while ( iter->hasMoreElements() ) {
			if ( this->contains(iter->currentElement()) == _NO_ ) {
				this->append(iter->currentElement());
				change = 1;
			}
			iter->nextElement();
		}
		delete iter;

		return change;
	}
	
	void minus(MyLinkedList<T>& temp_list) {
		int change = 0;
		MyLinkedListIterator<T>* iter = temp_list.elementsForward();
		while ( iter->hasMoreElements() ) {
			if ( this->remove(iter->currentElement()) == _YES_ )
				change = 1;
			iter->nextElement();
		}
		delete iter;
	}
	
	SetList<T>& operator = (SetList<T>& temp_list) {
		this->removeAll();
		MyLinkedListIterator<T>* iter = temp_list.elementsForward();
		while ( iter->hasMoreElements() ) {
			this->append(iter->currentElement());
			iter->nextElement();
		}
		delete iter;
		return *this;
	}
	
	int operator == (SetList<T>& temp_list) {
		// assumption: an element only appears once in the doublelist
		//             the element set is a pure set
		if ( this->numItems() != temp_list.numItems() )
			return _NO_;
		MyLinkedListIterator<T>* iter = this->elementsForward();
		while ( iter->hasMoreElements() ) {
			if ( temp_list.contains(iter->currentElement()) == _NO_ )
				return _NO_;
			iter->nextElement();
		}
		delete iter;

		return _YES_;
	}
	
	int operator != (SetList<T>& temp_list ) {
		return ( ! this->operator==(temp_list) );
	}
	
	
	void intersection(SetList<T>& temp_list) {
		MyLinkedListIterator<T>* iter = this->elementsForward();
		while ( iter->hasMoreElements() ) {
			T& elem = iter->currentElement();
			iter->nextElement();
			if ( temp_list.contains(elem) == _NO_ )
				this->remove(elem);
		}
		delete iter; 
	}
};

#endif
