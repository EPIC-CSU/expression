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
// $Id: PropertyList.h,v 1.15 1998/03/25 01:50:35 wtang Exp $
//
// File:  		PropertyList.h
// Created:		Thu Nov 21, 96
// Last modified: 	Thu Nov 21, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: PROPERTY

#ifndef __PROPERTYLIST_H__
#define __PROPERTYLIST_H__

#ifndef WIN32
#include <assert.h>
#endif

#include "PropertyIncludes.h"
#include "LinkedList.h"
#include "LinkedListIterator.h"

#include "MyLinkedList.h"

#define FULL_DIAGNOSTICS

template <class T> class PropertyList
{
private:
#ifdef LEAKY
	//NOS: used for memory leaks
	char tag[MAX_PATH];
#endif
	
protected:
	LinkedList<T> _propList;
public:
	PropertyList()
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
	
	virtual ~PropertyList()
	{
		
		//_propList.~LinkedList();
	}
	
	void addProperty(T *someProperty)
	{
		if (someProperty != NULL)
			_propList.append(*someProperty);
	}
	
	void addProperty(T & someProperty)
	{
		_propList.append(someProperty);
	}
	
	void removeProperty(T & someProperty)
	{
		_propList.removeHier(someProperty);
	}
	
	void addPropertyBefore(T & someProperty, PropertyNames name)
	{
		// If we know the order in which the properties are accessed,
		// we can do a good job of storing them to minimize access
		// time. 
	}
	
	void addPropertyAfter(T & someProperty, PropertyNames name)
	{
		// If we know the order in which the properties are accessed,
		// we can do a good job of storing them to minimize access
		// time. 
	}	 
	
	T *getPtrToProperty(PropertyNames name)
	{
		if (T::isValidProperty(name) == _NO_)
		{
			// cout << "Warning: Not a valid property" << endl;
			// _ASSERT(0);
			//When its obviously an error, put assert(0), not return NULL
			return NULL;
		}
		
		LinkedListIterator<T> *propListIterator;
		
		propListIterator = _propList.elementsForward();
		
		while (propListIterator->hasMoreElements())
		{
			try
			{
				if ((propListIterator->currentElement()).is(name) == _YES_){
					T *retPtr = &(propListIterator->currentElement());
					delete propListIterator;
					return (retPtr);
				}
			}
			catch (NoSuchElementException r)
			{
#ifdef FULL_DIAGNOSTICS
				cout << "Property does not exist" << endl;
				cout << r;
#endif
				delete propListIterator;
				return _NULL_;
			}
			propListIterator->nextElement();
		}
		
		delete propListIterator;
		return _NULL_;
	}
	
	
	T & getProperty(PropertyNames name)
	{
		if (T::isValidProperty(name) == _NO_)
		{
			cout << "Panic: Not a vaild property" << endl;
			assert(0);
		}
		
		LinkedListIterator<T> *propListIterator;
		
		propListIterator = _propList.elementsForward();
		
		while (propListIterator->hasMoreElements())
		{
			try
			{
				if ((propListIterator->currentElement()).is(name) == _YES_)		     {
					T& p= propListIterator->currentElement();
					delete propListIterator;
					return p;
				}
			}
			catch (NoSuchElementException r)
			{
#ifdef FULL_DIAGNOSTICS
				cout << "Property does not exist" << endl;
				cout << r;
#endif
   }
   propListIterator->nextElement();
  }
	 
	 delete propListIterator;
  
	 //NOS
	 _ASSERT(0);
	 //don't use exit(-1) on errors. It is hard to find why a program exits
	 //exit (-1);
	 T & VCplusplusret = propListIterator->currentElement();
	 return VCplusplusret;
 }
 
 void removeProperty(PropertyNames name)
 {
	 if (T::isValidProperty(name) == _NO_) return;

	 T *tptr = getPtrToProperty(name);

	 if (tptr != NULL)
		 removeProperty(*tptr);
 }

 void makeACopyOf(PropertyList<T> & a, MyLinkedList<PropertyNames> & propList)
 {
	 // Assumption:
	 // All the elements in propList are unique.
	 
	 LinkedListIterator<T> *copyIter = a.elementsForward();

	 if (!copyIter->hasMoreElements()) 
	 {
		 delete copyIter;
		 return;
	 }

	 MyLinkedListIterator<PropertyNames> *propNameIter = propList.elementsForward();

	 if (!propNameIter->hasMoreElements()) 
	 {
		 delete propNameIter;
		 return;
	 }

	 T & currProp = copyIter->currentElement();
	 
	 while (copyIter->hasMoreElements())
	 {
		 currProp = copyIter->currentElement();
		 
		 propNameIter->reset();
		 while (propNameIter->hasMoreElements())
		 {
			 if (currProp.is(propNameIter->currentElement()))
			 {
				 addProperty(currProp);
				 break;
			 }
			 propNameIter->nextElement();
		 }
		 copyIter->nextElement();
	 }
	 delete copyIter;
	 delete propNameIter;
 }
 
 void makeACopyOf(PropertyList<T> *a, MyLinkedList<PropertyNames> & propList)
 {
	 if (a == NULL) return;
	 makeACopyOf(*a, propList);
 }

 int isEmpty()
 {
	 return (_propList.isEmpty());
 }

 void print()
 {
	 LinkedListIterator<T> *printIter = _propList.elementsForward();
	 
	 cout << "Properties: " << endl;
	 while (printIter->hasMoreElements())
  {
   //       cout << " " << printIter->currentElement() << endl;
   printIter->currentElement().print(cout);
   cout << endl; 
   printIter->nextElement();
  }
	 delete printIter;
 }
 
 void print(ostream& out)
 {
	 LinkedListIterator<T> *printIter = _propList.elementsForward();
	 
	 out << "Properties: " << endl;
	 while (printIter->hasMoreElements())
  {
   printIter->currentElement().print(out);
   out << endl;
   printIter->nextElement();
  }
	 delete printIter;
 }
 
 T& getTail() {
  return _propList.getTail();
 }
 
 LinkedListIterator<T>* elementsForward() {
  return _propList.elementsForward();
 }
};

#endif


