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
// pcProGUIDoc.cpp : implementation of the pcProGUIDoc class
//

#include "stdafx.h"
#include "pcProGUI.h"

#include "pcProGUIDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// pcProGUIDoc

IMPLEMENT_DYNCREATE(pcProGUIDoc, CDocument)

BEGIN_MESSAGE_MAP(pcProGUIDoc, CDocument)
//{{AFX_MSG_MAP(pcProGUIDoc)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// pcProGUIDoc construction/destruction

int pcProGUIDoc::storeVersion = 100; //means x.xx
int pcProGUIDoc::loadVersion = 000; //not set

pcProGUIDoc::pcProGUIDoc()
{
	// TODO: add one-time construction code here
}

pcProGUIDoc::~pcProGUIDoc()
{
 //walk the list and delete all objects
 for(POSITION i=m_Files.GetHeadPosition();i!=NULL;m_Files.GetNext(i))
 {
  delete m_Files.GetAt(i);
 }  
 m_Files.RemoveAll(); 
}

BOOL pcProGUIDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
 
	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
 
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// pcProGUIDoc serialization

void pcProGUIDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
  ProjectFile* pf;
  
  loadVersion=storeVersion; //make sure we're storing with the latest format
  ar << loadVersion;
  ar << m_Files.GetCount();
  for(POSITION i=m_Files.GetHeadPosition();i!=NULL;m_Files.GetNext(i))
  {
   pf=m_Files.GetAt(i);
   ar << pf->name 
      << pf->selected
      << pf->afterTbz
      << pf->doMut
      << pf->doTbz
      << pf->printCFG
      << pf->printHTG
      << pf->printIList
      << pf->simHTG
      << pf->simNoSSA
      << pf->simSSA
      << pf->simISel
      << pf->simTbz
      << pf->doISel
      << pf->printMC
      << pf->pipelinedTbz
      << pf->doUnroll;
  }  
	}
	else
	{
		// TODO: add loading code here
  int count;
  
  m_Files.RemoveAll(); //free nodes?
  ar >> loadVersion;
  ar >> count;
  if (count>0)
  {
   ProjectFile* pf;
   for(int i=0;i<count;i++)
   {
    pf = new ProjectFile;
    //ar >> pf->name >> pf->selected;
    ar >> pf->name 
       >> pf->selected
//NOSTIP comment the stream ops below to load an old format .PCP file then save it with the new format
       >> pf->afterTbz
       >> pf->doMut
       >> pf->doTbz
       >> pf->printCFG
       >> pf->printHTG
       >> pf->printIList
       >> pf->simHTG
       >> pf->simNoSSA
       >> pf->simSSA
       >> pf->simISel
       >> pf->simTbz
       >> pf->doISel
       >> pf->printMC
       >> pf->pipelinedTbz
       >> pf->doUnroll;

    m_Files.AddTail(pf);
   }  
  }
	}
}

/////////////////////////////////////////////////////////////////////////////
// pcProGUIDoc diagnostics

#ifdef _DEBUG
void pcProGUIDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void pcProGUIDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// pcProGUIDoc commands
