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
// HTGDoc.cpp : implementation file
//

#include "stdafx.h"
#include "../pcProGUI.h"

#include "HTGDoc.h"
#include "../CFG/CFGDoc.h"

#include "../ColorCodes.h"

#include "Routine.h"

#include "../DOTGraph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

__declspec( dllimport ) int do_dot(FILE* iFile,FILE* oFile);

#define POINT_POS 72
#define POINT_SIZE 72

/////////////////////////////////////////////////////////////////////////////
// HTGDoc

IMPLEMENT_DYNCREATE(HTGDoc, CDocument)

HTGDoc::HTGDoc()
{
 m_routine=NULL;
 SetModifiedFlag(true);
}

BOOL HTGDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

HTGDoc::~HTGDoc()
{
}


BEGIN_MESSAGE_MAP(HTGDoc, CDocument)
	//{{AFX_MSG_MAP(HTGDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// HTGDoc diagnostics

#ifdef _DEBUG
void HTGDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void HTGDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// HTGDoc serialization

void HTGDoc::Serialize(CArchive& ar)
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
// HTGDoc commands

__declspec (dllimport) void setCurrRoutine(Routine* routine);

void HTGDoc::GenerateGraphDoc()//Routine* _routine)
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
 m_graphText+=" ];\n"; 
 
 //output node definitions
 MyLinkedListIterator<BaseHTGNode *> *iter = m_routine->getHTG()->elementsForward();
 while (iter->hasMoreElements()) 
 {
  iter->currentElement()->DOTText(GNT_HTG,m_graphText);
  iter->nextElement();
 }

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

 CFGDoc::ParseGraphDoc(GNT_HTG,output,xBound,yBound,scaleFactor);

	fclose(input);
	fclose(output);
}


MouseHitInfo* HTGDoc::getHit(CPoint & point)
{
 //walk all the nodes in the routine and see if there's a hit
 MyLinkedListIterator<BaseHTGNode *> *iter = m_routine->getHTG()->elementsForward();
 while (iter->hasMoreElements()) 
 {
  BaseHTGNode* node=iter->currentElement();
  if (node->htgNode.rect.PtInRect(point))
  {
   if (node->htgNode.expanded)
   {
    MOUSEHITINFO* hit=iter->currentElement()->getHit(GNT_HTG, point);
    if (hit!=NULL) //the hit is on an enclosed block
    {
     //MouseHitInfo* hit=new MouseHitInfo(_hit->type,_hit->node);
     //delete _hit;
     //return hit;
     hitHTG.hitType=hit->type;
     hitHTG.node=hit->node;
     delete hit;
     return &hitHTG;
    }
   }
   //we have our hit still in the main node
   MHI_HIT_TYPE type=MHI_INVALID;
   if ((BaseHTGNode*)iter->currentElement()->isSimpleNode()) type=MHI_SIMPLE_HTG_NODE;
   if ((BaseHTGNode*)iter->currentElement()->isIfNode()) type=MHI_IF_HTG_NODE;
   if ((BaseHTGNode*)iter->currentElement()->isLoopNode()) type=MHI_LOOP_HTG_NODE;
   //if ((BaseHTGNode*))iter->currentElement()->isCompundNode() type=MHI_COMPOUND_HTG_NODE;
   
   //MouseHitInfo* hit=new MouseHitInfo(type,(void*)node);
   //return hit;
   hitHTG.hitType=type;
   hitHTG.node=(void*)node;
   return &hitHTG;
  }

  iter->nextElement();
 }
 return NULL;
}
