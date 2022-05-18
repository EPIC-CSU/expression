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
// $Id: VirtualRegArg.cc,v 1.3 1998/01/22 23:46:40 pgrun Exp $
//
// File:  		VirtualRegArg.cc
// Created:		Wed Nov 20, 96
// Last modified: 	Wed Nov 20, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARGUMENT

#include "stdafx.h"

#include "VirtualRegArg.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#ifdef COUNT_VIRTUALREGS
   int VirtualRegArg::_numVirtualRegs = 0;	
#endif

BaseArgument *VirtualRegArg::copyBody()
{
   BaseArgument *retPtr;
   retPtr = new VirtualRegArg(_symVal, _regText);
   
   // Currently, we do not copy any properties of the argument.
/*
   MyLinkedList<PropertyNames> propList;
   propList.appendElement(<insert_property_name_here>
   
   (retPtr->getPropertyList()).makeACopyOf(_propertyPtr, propList);
*/

  return retPtr;
}

   
BaseArgument *VirtualRegArg::copy()
 {
	 BaseArgument *retPtr;
	 
	 retPtr = new VirtualRegArg;
	 //
	 // Initialize member data here.
  
	 return retPtr;
 }
