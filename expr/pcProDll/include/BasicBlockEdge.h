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
// $Id: BasicBlockEdge.h,v 1.3 1998/01/22 23:58:08 pgrun Exp $
//
// File:  		BasicBlockEdge.h
// Created:		Mon Jan 20, 97
// Last modified: 	Mon Jan 20, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: include

#ifndef __BASICBLOCKEDGE_H__
#define __BASICBLOCKEDGE_H__


#ifdef WIN32
#include <afxtempl.h>
typedef CArray<POINT,POINT&> PointArray;
#endif

#include "GlobalIncludes.h"
// #include "BasicBlockNode.h"
#include "BasicBlockIncludes.h"

class BasicBlockNode;

class BasicBlockEdge
{
private:
protected:
 
 BasicBlockNode *_next;
 int _arcType;
 
public:
 BasicBlockEdge()
 {
	 _next = NULL;
	 _arcType = _INVALID_;
 }
 
 BasicBlockEdge(const BasicBlockEdge & a)
 {
	 _next = a._next;
	 _arcType = a._arcType;
 }
 
 BasicBlockEdge(BasicBlockNode *bb, int arcType = _TREE_ARC_):_next(bb), _arcType(arcType)
 {
 }
 
 ~BasicBlockEdge()
 {
 }
 
 void setNext(BasicBlockNode *nextBB)
 {
	 _next = nextBB;
 }
 
 BasicBlockNode *getNext()
 {
	 return _next;
 }
 
 void setToTreeArc()
 {
	 _arcType = _TREE_ARC_;
 }
 
 void setToBackArc()
 {
	 _arcType = _BACK_ARC_;
 }
 
 void setToForwardArc()
 {
	 _arcType = _FORWARD_ARC_;
 }
 
 void setToCrossArc()
 {
	 _arcType = _CROSS_ARC_;
 }
 
 int isTreeArc()
 {
	 if (_arcType == _TREE_ARC_)
   return _YES_;
	 return _NO_;
 }
 
 int isBackArc()
 {
	 if (_arcType == _BACK_ARC_)
   return _YES_;
	 return _NO_;
 }
 
 int isForwardArc()
 {
	 if (_arcType == _FORWARD_ARC_)
   return _YES_;
	 return _NO_;
 }
 
 int isCrossArc()
 {
	 if (_arcType == _CROSS_ARC_)
   return _YES_;
	 return _NO_;
 }
 
 BasicBlockEdge *copy();
 
 // The next two functions are provided for completeness sake. Using
 // them is not recommended.
 //
 void setArcType(int arcNum)
 {
	 _arcType = arcNum;
 }
 
 int getArcType()
 {
	 return _arcType;
 }
 
 void print()
 {
	 cout << _next << " ARC: " << _arcType;
 }
 
};

#endif
