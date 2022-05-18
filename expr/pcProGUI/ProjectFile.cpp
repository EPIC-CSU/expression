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
// ProjectFile.cpp: implementation of the ProjectFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "pcprogui.h"
#include "ProjectFile.h"

#include "Prop/PropPageList.h"
#include "Prop/FilePropPage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ProjectFile::ProjectFile()//CString* _name=NULL,_selected=0)
{
 //if (_name!=NULL)
 //{
 // name=*_name;
 //}
 selected=0;//_selected;

 //
  afterTbz = 0;
  doMut	   = 0;
  doUnroll = 0;
  doTbz	   = 0;

  printCFG = 0; 
  printHTG = 0; 
  printIList = 0;
  simHTG   = 0;

  simNoSSA = 0;
  simSSA   = 0;
  simISel  = 0;
  simTbz   = 0;
  doISel   = 0;
  printMC  = 0;

  pipelinedTbz=0;

  //linkEXPRESSION = 0;
  //dbEnabled=0;
}

ProjectFile::~ProjectFile()
{

}



//void ProjectFile::DisplayProperties()
//{
// FilePropPage* propPage=dynamic_cast<FilePropPage*>(theApp.propPageList->activePage);
// ASSERT(propPage!=NULL);
//
// propPage->attachedObj=(void*)this;
//
// //propPage->m_Name=name;
//	//propPage->m_AfterTBZ = afterTbz;
//	//propPage->m_DoMut = doMut;
//	//propPage->m_DoTbz = doTbz;
//	//propPage->m_DoTbzPipe = FALSE;
//	//propPage->m_PrintCFG = printCFG;
//	//propPage->m_PrintHTG = printHTG;
//	//propPage->m_PrintIL = printIList;
//	//propPage->m_SimHTG = simHTG;
//	//propPage->m_DoISel = doISel;
//	//propPage->m_SimISel = simISel;
//	//propPage->m_SimNoSSA = simNoSSA;
//	//propPage->m_SimSSA = simSSA;
//	//propPage->m_SimTbz = simTbz;
//
// propPage->UpdateData(false);
//}
