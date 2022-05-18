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
// ILGDoc.cpp : implementation file
//

#include "stdafx.h"
#include "..\pcprogui.h"

#include "ILGDoc.h"
#include "../CFG/CFGDoc.h"

#include "MyLinkedListIterator.h"

#include "../ColorCodes.h"

#include "Routine.h"

#include "../DOTGraph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

__declspec( dllimport ) int do_dot(FILE* iFile,FILE* oFile);

/////////////////////////////////////////////////////////////////////////////
// ILGDoc

IMPLEMENT_DYNCREATE(ILGDoc, CDocument)

ILGDoc::ILGDoc()
{
 m_routine=NULL;
 SetModifiedFlag(true);
}

BOOL ILGDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

ILGDoc::~ILGDoc()
{
}


BEGIN_MESSAGE_MAP(ILGDoc, CDocument)
	//{{AFX_MSG_MAP(ILGDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ILGDoc diagnostics

#ifdef _DEBUG
void ILGDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void ILGDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// ILGDoc serialization

void ILGDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// ILGDoc commands

__declspec (dllimport) void setCurrRoutine(Routine* routine);

void ILGDoc::GenerateGraphDoc()//Routine * _routine)
{
 char temp[2000];

 SetModifiedFlag(false);
 setCurrRoutine(m_routine);
 
 ASSERT(m_routine);//=_routine;

	m_graphText="digraph routine\n";
 m_graphText+="{\n";
 m_graphText+="nodesep = 0.1;\n";
 m_graphText+="ranksep = 0.3;\n";
 //m_graphText+="node [height = 0.25, width = 0.75, shape = box ];\n";
 sprintf(temp,"node [height = %f, width = %f, shape = box ];\n",BOX_HEIGHT_STEP * ((float)1)+0.1,BOX_WIDTH);
 m_graphText+=temp;
 m_graphText+="edge [color = ";
 itoa(EDGE_COLOR_FLOW,&temp[0],10);
 m_graphText+=temp;
 m_graphText+="];\n"; 
 
 //output node definitions
 MyLinkedListIterator<BaseInstruction *> *iter=	m_routine->instrListIteratorForw();
 while (iter->hasMoreElements()) 
 {
  iter->currentElement()->DOTText(GNT_ILG,m_graphText);
  iter->nextElement();
 }
 
 delete iter;

 //wrap things up
	m_graphText+="}\n"; 
 
	// create 2 temporary files for DOT
	FILE* input = tmpfile();
	FILE* output = tmpfile();
 
	// write graph data to input file
	fwrite(LPCSTR(m_graphText),1,m_graphText.GetLength()+1,input); 
 
	/*
 //dump output to cerr
 {
  DWORD dummy;
  WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), LPCSTR(m_graphText),m_graphText.GetLength(),&dummy,NULL); 
 }
 */
 
	// Call dot to do the layout.
 int res = fseek(input,0,SEEK_SET);
 ASSERT_TRACE(res==0,"Where is the start of this file, Scooby?");
	do_dot(input,output);
 
 CFGDoc::ParseGraphDoc(GNT_ILG,output,xBound,yBound,scaleFactor);

	fclose(input);
	fclose(output);
}


MouseHitInfo* ILGDoc::getHit(CPoint& point)
{
 //walk all the instructions in the instruction list and see if there's a hit
 MyLinkedListIterator<BaseInstruction *> *iter=	m_routine->instrListIteratorForw();
 while (iter->hasMoreElements()) 
 {
  //RECT tRect;
  //BaseInstruction* instr=iter->currentElement();
  
  //tRect.left=instr->ilgNode.point.x-instr->ilgNode.size.cx/2;
  //tRect.top=instr->ilgNode.point.y-instr->ilgNode.size.cy/2;
  //tRect.right=instr->ilgNode.point.x+instr->ilgNode.size.cx/2;
  //tRect.bottom=instr->ilgNode.point.y+instr->ilgNode.size.cy/2;

  if (iter->currentElement()->ilgNode.rect.PtInRect(point))
  {
   //MouseHitInfo* hit=new MouseHitInfo(MHI_INSTR,(void*)iter->currentElement());
   //return hit;
   hitILG.hitType=MHI_INSTR;
   hitILG.node=(void*)iter->currentElement();
   return &hitILG;
  }

  iter->nextElement();
 }
 return NULL;
}
