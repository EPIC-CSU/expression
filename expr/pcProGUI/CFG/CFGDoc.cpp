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
// CFGDoc.cpp : implementation file
//

#include "stdafx.h"
#include "../pcprogui.h"
#include "CFGDoc.h"

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
// CFGDoc

IMPLEMENT_DYNCREATE(CFGDoc, CDocument)

CFGDoc::CFGDoc()
{
 m_routine=NULL;
 SetModifiedFlag();
 scaleFactor=0;
	xBound=-1;
	yBound=-1;
}

BOOL CFGDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CFGDoc::~CFGDoc()
{
 //CleanMaps();
}


BEGIN_MESSAGE_MAP(CFGDoc, CDocument)
//{{AFX_MSG_MAP(CFGDoc)
// NOTE - the ClassWizard will add and remove mapping macros here.
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFGDoc diagnostics

#ifdef _DEBUG
void CFGDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFGDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFGDoc serialization

void CFGDoc::Serialize(CArchive& ar)
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
// CFGDoc commands

__declspec (dllimport) void setCurrRoutine(Routine* routine);

void CFGDoc::GenerateGraphDoc()//Routine* _routine)
{
 char temp[2000];

 SetModifiedFlag(false);
 setCurrRoutine(m_routine);

 ASSERT(m_routine);
 //m_routine=_routine;

	m_graphText="digraph routine\n";
 m_graphText+="{\n";
 m_graphText+="nodesep = 0.1;\n";
 m_graphText+="ranksep = 0.3;\n";
 //m_graphText+="node [height = 0.25, width = 0.8, shape = box ];\n"; 
 sprintf(temp,"node [height = %f, width = %f, shape = box ];\n",BOX_HEIGHT_STEP * ((float)1)+0.1,BOX_WIDTH);
 m_graphText+=temp;
 m_graphText+="edge [color = ";
 itoa(EDGE_COLOR_FLOW,&temp[0],10);
 m_graphText+=temp;
 m_graphText+=" ];\n"; 
 
 //output node definitions
 LinkedListIterator<BasicBlockNode> *iter = m_routine->getCFG().createBBIterForw();
 while (iter->hasMoreElements()) 
 {
  iter->currentElement().DOTText(GNT_CFG,m_graphText);
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

 ParseGraphDoc(GNT_CFG,output,xBound,yBound,scaleFactor);

	fclose(input);
	fclose(output);
}


void CFGDoc::ParseGraphDoc(GRAPH_NODE_TYPE nodeType, FILE * output, int& xBound, int& yBound, float& scaleFactor)
{
 //parse the output file and update node and edge info with what DOT generated

 int res;

 /*
	res = fseek(output,0,SEEK_SET);
 ASSERT_TRACE(res == 0,"Where is the start of this file, Scooby?");

 //dump output to cerr
	char cx = fgetc(output);
 DWORD dummy;
 while (cx != EOF) 
 {
  WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), &cx,1,&dummy,NULL); 
		cx = fgetc(output);
	}
	*/

 res = fseek(output,0,SEEK_SET);
 ASSERT_TRACE(res == 0,"Where is the start of this file, Scooby?");

	float tmp_x,tmp_y;
	res = fscanf(output,"graph %f %f %f",&scaleFactor,&tmp_x,&tmp_y);
 ASSERT_TRACE(res == 3,"Need scale, width, and height.");
 
	xBound = (int)(scaleFactor*POINT_POS*tmp_x);
	yBound = (int)(scaleFactor*POINT_POS*tmp_y);
 
	char c = getc(output);
	while ((c != 'n') && 
        (c != 'e') && 
        (c != 's') && 
        (c != 'c') && 
        (c != EOF)) 
 {
		c = getc(output);
	}
 
 ASSERT_TRACE(c!=EOF,"First node or edge not found in DOT output");

	while (true) 
 {
		if (c == 'n') //NODE
  {
			char name[32];
			float x,y,xSize,ySize;
   char label[32];
			char style[32];
			char shape[32];
			char color[32];
   
			//res = fscanf(output,"ode %s %f %f %f %f _%X %s %s %s",name,&x,&y,&xSize,&ySize,&label,style,shape,color);
			res = fscanf(output,"ode %s %f %f %f %f %s %s %s %s",name,&x,&y,&xSize,&ySize,label,style,shape,color);
   ASSERT_TRACE(res == 9,"Not all entries read in for a DOT node from DOT output");
   
   switch (name[0])
   {
   case 'b': //basic block node
   case 'i': //instruction node
   case 'h': //htg node
    {
     GenericNode* gn;
     sscanf(&name[5],"%X",&gn);
     GraphNode* node=gn->GetGraphNode(nodeType);
     
     node->rect.left=(int)(scaleFactor*(xBound-(int)POINT_POS*(x+xSize/2)));
     node->rect.top=(int)(scaleFactor*(yBound-(int)POINT_POS*(y+ySize/2)));
     node->rect.right=node->rect.left+(int)(scaleFactor*POINT_SIZE*xSize);
     node->rect.bottom=node->rect.top+(int)(scaleFactor*POINT_SIZE*ySize);
     node->rect.OffsetRect(POINT_OFFSET,POINT_OFFSET);
     node->style=style;
     node->shape=shape;
     //node->borderColor=atoi(&color[0]);
    }
    break;

   /*
   case 'i': //instruction node
    {
     BaseInstruction* bi;
     sscanf(&name[5],"%X",&bi);
     GraphNode* node=bi->GetGraphNode(nodeType);

     node->rect.left=(int)(scaleFactor*(xBound-(int)POINT_POS*(x+xSize/2)));
     node->rect.top=(int)(scaleFactor*(yBound-(int)POINT_POS*(y+ySize/2)));
     node->rect.right=node->rect.left+(int)(scaleFactor*POINT_SIZE*xSize);
     node->rect.bottom=node->rect.top+(int)(scaleFactor*POINT_SIZE*ySize);
     node->rect.OffsetRect(POINT_OFFSET,POINT_OFFSET);
     node->style=style;
     node->shape=shape;
     //node->borderColor=atoi(&color[0]);
    }
    
   case 'h': //htg node
    {
     BaseHTGNode* bn;
     sscanf(&name[5],"%X",&bn);
     GraphNode* node=bn->GetGraphNode(nodeType);

     node->rect.left=(int)(scaleFactor*(xBound-(int)POINT_POS*(x+xSize/2)));
     node->rect.top=(int)(scaleFactor*(yBound-(int)POINT_POS*(y+ySize/2)));
     node->rect.right=node->rect.left+(int)(scaleFactor*POINT_SIZE*xSize);
     node->rect.bottom=node->rect.top+(int)(scaleFactor*POINT_SIZE*ySize);
     node->rect.OffsetRect(POINT_OFFSET,POINT_OFFSET);
     node->style=style;
     node->shape=shape;
     //node->borderColor=atoi(&color[0]);
    }
    break;
   */

   default:
    ASSERT_TRACE(0,"What?");
    break;
   } 
  }
  else if (c == 'e') //EDGE
  {
			char tail_name[32];
			char head_name[32];
   //int label;
			int num;
   
			//res = fscanf(output,"dge %s %s _%X %d",head_name,tail_name,&label,&num);
			res = fscanf(output,"dge %s %s %d",head_name,tail_name,&num);
  	ASSERT_TRACE(res == 3,"Not all entries read in for a DOT edge from DOT output");
   
   switch (head_name[0])
   {
/*
   case 'b'://edge from node to whatever
    {
     BasicBlockNode* bb;
     GraphNode* node;
     sscanf(&head_name[5],"%X",&bb);
     node=bb->GetGraphNode(nodeType);

     BezierEdge* edge;
     int dest;
     sscanf(&tail_name[5],"%X",&dest);
     node->edges.Lookup(dest,edge);
     ASSERT_TRACE(res,"Can't find edge.");

     edge->isSelfLoop=(strcmp(head_name,tail_name)==0);
     if (edge->points.GetSize()!=0) //clean up previous points
     {
      edge->points.RemoveAt(0,edge->points.GetSize());
     }
     
     for (int i = 0; i < num;i++) 
     {
      POINT p;
      float x,y;
      res = fscanf(output,"%f %f",&x,&y);
      ASSERT_TRACE(res == 2,"Need points for a Bezier spline coordinates from DOT");
      p.x=(int)(scaleFactor*(xBound-(int)POINT_POS*x))+POINT_OFFSET;
      p.y=(int)(scaleFactor*(yBound-(int)POINT_POS*y))+POINT_OFFSET;
      edge->points.Add(p);
     }
     
     char style[32];
     char color[32];
     
     res = fscanf(output,"%s %s",style,color);
     ASSERT_TRACE(res == 2,"Need style and color fields for edge from DOT");
     edge->style=style;
     edge->lineColor=atoi(&color[0]);
     
     //now read the coords for the end/start point of the bezier curve
     float x,y;
     res = fscanf(output,"%f %f",&x,&y);
     ASSERT_TRACE(res == 2,"Need start/end point for a Bezier from DOT");
     edge->arrow.x=(int)(scaleFactor*(xBound-(int)POINT_POS*x))+POINT_OFFSET;
     edge->arrow.y=(int)(scaleFactor*(yBound-(int)POINT_POS*y))+POINT_OFFSET;
    }
    break;
*/

   case 'b'://edge from node to whatever
   case 'h'://edge from HTG node to whatever
   case 'i'://edge from instruction to whatever
    {
     GenericNode* from;
     GenericNode* to;
     GraphNode* node;

     sscanf(&head_name[5],"%X",&from);
     sscanf(&tail_name[5],"%X",&to);

     node=from->GetGraphNode(nodeType);

     //find edge for this target in edge map
     BezierEdge* edge;
     int res=node->edges.Lookup((int)to,edge);
     ASSERT_TRACE(res,"Can't find edge");

     edge->isSelfLoop=(strcmp(head_name,tail_name)==0);
     if (edge->points.GetSize()!=0) //clean up previous points
     {
      edge->points.RemoveAt(0,edge->points.GetSize());
     }
     
     for (int i = 0; i < num;i++) 
     {
      POINT p;
      float x,y;
      res = fscanf(output,"%f %f",&x,&y);
      ASSERT_TRACE(res == 2,"Need points for a Bezier spline coordinates from DOT");
      p.x=(int)(scaleFactor*(xBound-(int)POINT_POS*x))+POINT_OFFSET;
      p.y=(int)(scaleFactor*(yBound-(int)POINT_POS*y))+POINT_OFFSET;
      edge->points.Add(p);
     }
     
     char style[32];
     char color[32];
     
     res = fscanf(output,"%s %s",style,color);
     ASSERT_TRACE(res == 2,"Need style and color fields for edge from DOT");
     edge->style=style;
     edge->lineColor=atoi(&color[0]);
     
     //now read the coords for the end/start point of the bezier curve
     float x,y;
     res = fscanf(output,"%f %f",&x,&y);
     ASSERT_TRACE(res == 2,"Need start/end point for a Bezier from DOT");
     edge->arrow.x=(int)(scaleFactor*(xBound-(int)POINT_POS*x))+POINT_OFFSET;
     edge->arrow.y=(int)(scaleFactor*(yBound-(int)POINT_POS*y))+POINT_OFFSET;
    }
    break;

/*
   case 'i'://edge from instruction to whatever
    {
     BaseInstruction* from_instr;
     BaseInstruction* to_instr;
     GraphNode* node;

     sscanf(&head_name[5],"%X",&from_instr);
     sscanf(&tail_name[5],"%X",&to_instr);

     node=from_instr->GetGraphNode(nodeType);

     //find edge for this target in instr edge map
     BezierEdge* edge;
     int res=node->edges.Lookup((int)to_instr,edge);
     ASSERT_TRACE(res,"Can't find edge");

     edge->isSelfLoop=(strcmp(head_name,tail_name)==0);
     if (edge->points.GetSize()!=0) //clean up previous points
     {
      edge->points.RemoveAt(0,edge->points.GetSize());
     }
     
     for (int i = 0; i < num;i++) 
     {
      POINT p;
      float x,y;
      res = fscanf(output,"%f %f",&x,&y);
      ASSERT_TRACE(res == 2,"Need points for a Bezier spline coordinates from DOT");
      p.x=(int)(scaleFactor*(xBound-(int)POINT_POS*x))+POINT_OFFSET;
      p.y=(int)(scaleFactor*(yBound-(int)POINT_POS*y))+POINT_OFFSET;
      edge->points.Add(p);
     }
     
     char style[32];
     char color[32];
     
     res = fscanf(output,"%s %s",style,color);
     ASSERT_TRACE(res == 2,"Need style and color fields for edge from DOT");
     edge->style=style;
     edge->lineColor=atoi(&color[0]);
     
     //now read the coords for the end/start point of the bezier curve
     float x,y;
     res = fscanf(output,"%f %f",&x,&y);
     ASSERT_TRACE(res == 2,"Need start/end point for a Bezier from DOT");
     edge->arrow.x=(int)(scaleFactor*(xBound-(int)POINT_POS*x))+POINT_OFFSET;
     edge->arrow.y=(int)(scaleFactor*(yBound-(int)POINT_POS*y))+POINT_OFFSET;
    }
    break;
*/

   default://edge from ?
    ASSERT_TRACE(0,"I feel kinda edgy!");
    break;
   }
		} 
  else if (c == 's') //final "stop", ignore
  {
			break;
		} 
  else if (c == 'c') //cluster
  {
   //cluster cluster_498990 0.000 3.667 2.319 3.667 2.319 6.861 0.000 6.861 _498990
			BasicBlockNode* bb;
   float ulx,uly,urx,ury,lrx,lry,llx,lly;
   char label[50];
   
			res = fscanf(output,"luster cluster_%X %f %f %f %f %f %f %f %f %s",
                       &bb,
                       &ulx, &uly,
                       &urx, &ury,
                       &lrx, &lry,
                       &llx, &lly,
                       label);
  	ASSERT_TRACE(res == 10,"Not all entries read in for a DOT edge from DOT output");

   GraphNode* node=&bb->cfgNode;

   node->style="solid";
   node->shape="box";
   //node->point.x=(int)(scaleFactor*(xBound+(float)POINT_POS*ulx)+.5-(urx-ulx)/2.0);
   //node->point.y=(int)(scaleFactor*(yBound+(float)POINT_POS*uly)+1.0+(lly-uly)/2.0);
   //node->size.cx=(int)(scaleFactor*(float)POINT_POS*(urx-ulx));
   //node->size.cy=(int)(scaleFactor*(float)POINT_POS*(lly-uly));
   //node->point.x=(int)(scaleFactor*(xBound+node->size.cx-(float)POINT_POS*(urx+.5)));
   //node->point.y=(int)(scaleFactor*(yBound+node->size.cy/2-(float)POINT_POS*lry));
   node->rect.left=0;//(int)(scaleFactor*(xBound-(int)POINT_POS*x));
   node->rect.top=0;//(int)(scaleFactor*(yBound-(int)POINT_POS*y));
   node->rect.right=0;//node->rect.left+(int)(scaleFactor*POINT_SIZE*xSize);
   node->rect.bottom=0;//node->rect.top+(int)(scaleFactor*POINT_SIZE*ySize);
   node->rect.OffsetRect(POINT_OFFSET,POINT_OFFSET);

			break;
		} 
  else 
  {
			ASSERT_TRACE(0,"Unknown construct in DOT output");
		}

		c = getc(output);
		while ((c != 'n') && 
         (c != 'e') && 
         (c != 's') && 
         (c != 'c') && 
         (c != EOF)) 
  {
			c = getc(output);
		}
	}
}

MouseHitInfo* CFGDoc::getHit(CPoint & point)
{
 //walk all the basic blocks in the routine and see if there's a hit
 LinkedListIterator<BasicBlockNode> *iter = m_routine->getCFG().createBBIterForw();
 while (iter->hasMoreElements()) 
 {
  //RECT tRect;
  
  //tRect.left=iter->currentElement().cfgNode.point.x-iter->currentElement().cfgNode.size.cx/2;
  //tRect.top=iter->currentElement().cfgNode.point.y-iter->currentElement().cfgNode.size.cy/2;
  //tRect.right=iter->currentElement().cfgNode.point.x+iter->currentElement().cfgNode.size.cx/2;
  //tRect.bottom=iter->currentElement().cfgNode.point.y+iter->currentElement().cfgNode.size.cy/2;

  if (iter->currentElement().cfgNode.rect.PtInRect(point))
  {
   if (iter->currentElement().cfgNode.expanded)
   {
    MOUSEHITINFO* hit=iter->currentElement().getHit(GNT_CFG, point);
    if (hit!=NULL) //the hit is on an instr in the block
    {
     //hitCFG=new MouseHitInfo(_hit->type,_hit->node);
     //delete _hit;
     //return hit;
     hitCFG.hitType=hit->type;
     hitCFG.node=hit->node;
     delete hit;
     return &hitCFG;
    }
   }
   //we have our hit still in the basic block
   //MouseHitInfo* hit=new MouseHitInfo(MHI_BASICBLOCK,(void*)&(iter->currentElement()));
   //return hit;
   hitCFG.hitType=MHI_BASICBLOCK;
   hitCFG.node=(void*)&(iter->currentElement());
   return &hitCFG;
  }

  iter->nextElement();
 }
 return NULL;
}
