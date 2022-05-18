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
// $Id: PhiOp.h,v 1.14 1998/03/25 01:50:33 wtang Exp $
//
// File:  		PhiOp.h
// Created:		Mon Oct 28, 96
// Last modified: 	Thu Oct 31, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION


// Note: Currently, we are working with real register arguments only.
// To be more general, the convert-to-SSA-form methods/functions
// should be capable of accepting virtual register arguments too.
    
#ifndef __PHIOP_H__
#define __PHIOP_H__

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#ifndef WIN32
#ifndef WIN32
#include <stl.h>
#endif
#include <map>
#include <assert.h>
#endif

#include "BasicBlockNode.h"
#include "MyLinkedList.h"
#include "BaseOperation.h"
#include "BaseArgument.h"
#include "RegisterArg.h"
#include "PredidArgProperty.h"
#include "RegisterFileList.h"
#include "SSAArgProperty.h"
#include <assert.h>

extern RegisterFileList *globalRegFileList;

class PhiOp : public BaseOperation 
{
 private:

#ifdef COUNT_PHIOPS
   static int _numPhiOps;	// See comment 1 below.
#endif

 protected:
  BaseArgument *_destOperand;

   MyLinkedList<BaseArgument *> _phiArgList;
   
 public:
   PhiOp() {};

   PhiOp(BasicBlockNode *curBB,int RegisterID)
      {
#ifdef COUNT_PHIOPS
	 _numPhiOps++;
#endif

  _destOperand = globalRegFileList->getRealReg(RegisterID);

  BasicBlockPredecessors& preds = curBB->getCFPred();
  MyLinkedListIterator<BasicBlockNode *> *preIter=preds.createPredListIterForw();
  BaseArgument *srcArg;
  PredidArgProperty tempProp;
	 
  while (preIter->hasMoreElements()){
    int dfsNum =  (preIter->currentElement())->getDFSNum();
    srcArg = globalRegFileList->getRealReg(RegisterID);
    tempProp.setPredID(dfsNum);
    srcArg->addProperty(tempProp);
    
    _phiArgList.append(srcArg);
    preIter->nextElement();
  }

  delete preIter;

   }
   
   PhiOp(const PhiOp & a)
      {
	 // Make a copy of destOperand and phiArgList

#ifdef COUNT_PHIOPS
	 _numPhiOps++;
#endif
      }
   
   ~PhiOp()
      {
#ifdef COUNT_PHIOPS
	 _numPhiOps--;
#endif
/* delete all Phi Argument and its SSAAProperty and PredidProperty*/
      }
   
// Redefine initializeID();

   void initializeID()
      {
	 _uniqueID = 0;
      }

#ifdef COUNT_PHIOPS
   static int numPhiOps()
      {
	 return _numPhiOps;
      }
#endif

   BaseOperation *copy()
      {
	 //BaseOperation *retPtr;
//I don't need copy for it
	 
	 // Have to initialize the member data.

	 //NOS
	 return NULL;

      }
   
   int isPhiOp()
      {
	 return _YES_;
      }

   void addDestOperand(BaseArgument *operand)
      {
	 _destOperand = operand->copy();
      }
   
   void addPhiArg(BaseArgument *arg)
      {
	 BaseArgument *newArg = arg->copy();
	 
	 _phiArgList.append(newArg);
      }
   
   BaseArgument *ptrToDestOperand()
      {
	 return _destOperand;
      }

   BaseArgument *ptrToPhiArg(int predBBID)
      {  
	PredidArgProperty *prop; 

	MyLinkedListIterator<BaseArgument *> *iter = _phiArgList.
	    elementsForward();
	while (iter->hasMoreElements()) {
	  RegisterArg *tmp = (RegisterArg *)iter->currentElement();
	  prop = (PredidArgProperty *) (tmp->getPtrToProperty(_PREDID_));
	  int id = prop->getPredID();
	  if (id==predBBID){
	    delete iter;
	    return tmp;
	  }
	  iter->nextElement();
	}
	delete iter;
	cout<<"Argument not found with the predecessor"<<endl;
	assert(0);

	//NOS
	return NULL;

      }
   
   MyLinkedListIterator<BaseArgument *> *phiArgIterForw()
      {
	 MyLinkedListIterator<BaseArgument *> *retPtr;
	 retPtr = _phiArgList.elementsForward();
	 
	 return retPtr;
      }
	 
   friend int operator == (const PhiOp & a, const PhiOp & b)
      {
	 return _YES_;
      }
   
   friend ostream & operator << (ostream & out, const PhiOp & s)
      {
	 return out;
      }
//
// Rewrite the operators == and << later.

   void print()
{
   //SSAArgProperty *ssaP;
   //int ssaNum;

   cout << "PhiOp: "<<endl;
   if (_destOperand != _NULL_){
      cout <<"    destOp ";
      _destOperand->print();
   }
   
   MyLinkedListIterator<BaseArgument *> *printIter;
   BaseArgument *curArg;
   
   printIter = _phiArgList.elementsForward();
   
   while (printIter->hasMoreElements())
      {
	 curArg = printIter->currentElement();
	 cout<<"    srcOp  ";
	 curArg->print();
	 printIter->nextElement();
      }
   cout << endl;
   delete printIter;
   
   printOpProperty(cout);
}

   void print(ostream& out);
   void shortPrint(ostream& out);

   MyLinkedList<BaseArgument*>& argList() { return _phiArgList; }

   BaseArgument* ptrToOperand(int index) {
	if ( index == _DEST_ )
		return  _destOperand;
	else {
		MyLinkedListIterator<BaseArgument*>* iter=_phiArgList.elementsForward();
		while ( iter->hasMoreElements() ) {
			if ( index == 2 ) {
				delete iter;
				return iter->currentElement();
			}
			iter->nextElement();
			index --;
		}
		delete iter;
#ifdef DEBUG
		assert(0>1);	
#endif
		return 0;
	}
    }

};

// PhiOp class is used to store information about the phi map. (This
// implementation is specific to SSA form representation of the source
// program). The initializeID method will have to be redefined here
// because I don't expect any classes to be derived from this. 

#endif

// Comment 1:
//
// _numPhiOps is used to keep a count of the number of PhiOp objects
// in existence at any point. COUNT_PHIOPS can be either defined or
// undefined in the file "OperationIncludes.h". The count information
// will be used for debugging (and/or statistical) purposes. It might
// be a good idea to allow the user to specify whether he needs this
// info. 
