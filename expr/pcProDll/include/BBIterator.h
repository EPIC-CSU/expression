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
// $Id: BBIterator.h,v 1.7 1998/04/11 00:35:34 xji Exp $
//
// File:  		BBIterator.h
// Created:		Tue Oct 28 , 97
// Last modified: 	Tue Oct 28 , 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: CONTROL_FLOW_GRAPH

#ifndef __BBITERATOR_H__
#define __BBITERATOR_H__

#include "BasicBlockNode.h"
#include "NormalInstr.h"

class BBForwIterator
{
 private:
   BasicBlockNode *_bb;
   BaseInstruction *_curItem;
   BaseInstruction *_lastItem;
   int _valid;

 public:
   
   BBForwIterator(BasicBlockNode *bb = NULL) : _bb(bb)
      {
	 if (_bb != NULL)
	    {
		 // need check to see if it is a dummy block.
		 //
		 if ((_bb->getFirstInstr() == NULL) || (_bb->getLastInstr() == NULL))
		 {
			 _curItem = _lastItem = NULL;
			 _valid = _NO_;
		 }
		 else
		 {
	       _curItem = _bb->getFirstInstr();
	       _lastItem = _bb->getLastInstr();
	       _valid = _YES_;
		 }
	    }
	 else
	    {
	       _curItem = NULL;
	       _lastItem = NULL;
	       _valid = _NO_;
	    }
      }

   void setBB(BasicBlockNode *bb)
      {
	 _bb = bb;
	 
	 if (_bb != NULL)
	    {
		 // need check to see if it is a dummy block.
		 //
		 if ((_bb->getFirstInstr() == NULL) || (_bb->getLastInstr() == NULL))
		 {
			 _curItem = _lastItem = NULL;
			 _valid = _NO_;
		 }
		 else
		 {
	       _curItem = _bb->getFirstInstr();
	       _lastItem = _bb->getLastInstr();
	       _valid = _YES_;
		 }
	    }
	 else
	    {
	       _curItem = NULL;
	       _lastItem = NULL;
	       _valid = _NO_;
	    }
      }
	      
   BasicBlockNode *getBB()
      {
	 return _bb;
      }
   
   int hasMoreElements()
      {
	 return _valid;
      }
   
   void nextElement()
      {
	 BaseInstruction *_prevItem;

	 int TEMP_DEBUG = 0;

	 if ((_curItem != NULL) && (_curItem == _lastItem))
	    _valid = _NO_;
	 else
	    {
			  if (TEMP_DEBUG)
			  {
				  cout << endl << "------------------------" << endl;
				  _lastItem->shortPrint(cout);
				  _curItem->shortPrint(cout);
				  cout << endl << "------------------------" << endl;
			  }

		 _prevItem = _curItem;
		 
		 // SAH: Thu July 22 99
		 //
		 // Changed this piece of code to call only getDefaultTargetInstr().
		 //
		 /*
		 if(!_curItem->hasMultipleSuccs())
		 _curItem = _curItem->getFirstSucc();
		 else 
		 _curItem = ((NormalInstr*)_curItem)->getDefaultTargetInstr();
		 */
		 _curItem = ((NormalInstr*)_curItem)->getDefaultTargetInstr();
	    }
      }

   BaseInstruction *currentElement()
      {
	 if (_curItem != NULL)
	    return _curItem;
	 
	 throw (Exception *)
	    new NoSuchElementException("BBIterator::currentElement()");

	 //NOS
	 ASSERT_TRACE(0,"Forced EXIT!");
	 return NULL;

      }
   
   void reset()
      {
	 if (_bb != NULL)
	    {
		 // need check to see if it is a dummy block.
		 //
		 if ((_bb->getFirstInstr() == NULL) || (_bb->getLastInstr() == NULL))
		 {
			 _curItem = _lastItem = NULL;
			 _valid = _NO_;
		 }
		 else
		 {
	       _curItem = _bb->getFirstInstr();
	       _lastItem = _bb->getLastInstr();
	       _valid = _YES_;
		 }
	    }
	 else
	    {
	       _curItem = NULL;
	       _lastItem = NULL;
	       _valid = _NO_;
	    }
      }

      BBForwIterator&  operator=(BBForwIterator& iter) {
	_bb = iter.getBB();
	_curItem = iter.currentElement();
	_lastItem = _bb->getLastInstr();
	_valid = iter.hasMoreElements();

	return (*this);
      }
};



class BBBackIterator
{
 private:
   BasicBlockNode *_bb;
   BaseInstruction *_curItem;
   BaseInstruction *_firstItem;
   int _valid;

 public:
   
   BBBackIterator(BasicBlockNode *bb = NULL) : _bb(bb)
      {
	 if (_bb != NULL)
	    {
		 // need check to see if it is a dummy block.
		 //
		 if ((_bb->getFirstInstr() == NULL) || (_bb->getLastInstr() == NULL))
		 {
			 _curItem = _firstItem = NULL;
			 _valid = _NO_;
		 }
		 else
		 {
	       _curItem = _bb->getLastInstr();
	       _firstItem = _bb->getFirstInstr();
	       _valid = _YES_;
		 }
	    }
	 else
	    {
	       _curItem = NULL;
	       _firstItem = NULL;
	       _valid = _NO_;
	    }
      }

   void setBB(BasicBlockNode *bb)
      {
	 _bb = bb;
	 
	 if (_bb != NULL)
	    {
		 // need check to see if it is a dummy block.
		 //
		 if ((_bb->getFirstInstr() == NULL) || (_bb->getLastInstr() == NULL))
		 {
			 _curItem = _firstItem = NULL;
			 _valid = _NO_;
		 }
		 else
		 {
	       _curItem = _bb->getLastInstr();
	       _firstItem = _bb->getFirstInstr();
	       _valid = _YES_;
		 }
	    }
	 else
	    {
	       _curItem = NULL;
	       _firstItem = NULL;
	       _valid = _NO_;
	    }
      }
	      
   BasicBlockNode *getBB()
      {
	 return _bb;
      }
   
   int hasMoreElements()
      {
	 return _valid;
      }
   
   void prevElement()
      {
	 BaseInstruction *_prevItem, *p, *q;
	 
	 int TEMP_DEBUG = 0;
	 if ((_curItem != NULL) && (_curItem == _firstItem))
	    _valid = _NO_;
	 else
	    {
	      for(p=q=_firstItem;p!=NULL && p!=_curItem; ){
			  if (TEMP_DEBUG)
			  {
				  cout << endl << "------------------------" << endl;
				  _firstItem->shortPrint(cout);
				  _curItem->shortPrint(cout);
				  p->shortPrint(cout);
				  cout << endl << "------------------------" << endl;
			  }

		q=p;

		// SAH: Wed. Jul 21, 99
		//
		// Note: Do not get first Succ. In the case of a call op, this
		// will not work as the first succ (i.e. true branch) will point
		// to some other instruction.
		// Instead get the default target instr.
		//
		// Note that default target instr is the same as first succ
		// for data ops.
		//
		// This was the original piece of code:
		//
		// p=p->getFirstSucc();
		//
		p = ((NormalInstr *)p)->getDefaultTargetInstr();
	      }
	      _prevItem = _curItem;
	      _curItem = q;
	      //_prevItem->deleteSuccIter();
	    }
	 
      }

   BaseInstruction *currentElement()
      {
	 if (_curItem != NULL)
	    return _curItem;
	 
	 throw (Exception *)
	    new NoSuchElementException("BBIterator::currentElement()");

	 //NOS
	 return NULL;
      }
   
   void reset()
      {
	 if (_bb != NULL)
	    {
		 // need check to see if it is a dummy block.
		 //
		 if ((_bb->getFirstInstr() == NULL) || (_bb->getLastInstr() == NULL))
		 {
			 _curItem = _firstItem = NULL;
			 _valid = _NO_;
		 }
		 else
		 {
	       _curItem = _bb->getLastInstr();
	       _firstItem = _bb->getFirstInstr();
	       _valid = _YES_;
		 }
	    }
	 else
	    {
	       _curItem = NULL;
	       _firstItem = NULL;
	       _valid = _NO_;
	    }
      }
};

#endif

   
   
	 
