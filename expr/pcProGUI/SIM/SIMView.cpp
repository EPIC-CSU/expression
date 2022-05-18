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
// SIMView.cpp : implementation file
//

#include "stdafx.h"

#include <ostream.h>

#include "../pcProGUI.h"
#include "../MainFrm.h"

#include "SIMView.h"
#include "SIMDoc.h"

#include "../Stat/DefaultStatProp.h"
#include "../Stat/CompoundUnitStatPropPage.h"
#include "../Stat/UnitStatPropPage.h"
#include "../Stat/LatchStatPropPage.h"
#include "../Stat/StorageStatPropPage.h"
#include "../Stat/ConnectionStatPropPage.h"
#include "../Stat/PortStatPropPage.h"

#include "../Prop/PropPageList.h"
#include "../Prop/CompoundUnitPropPage.h"
#include "../Prop/UnitPropPage.h"
#include "../Prop/LatchPropPage.h"
#include "../Prop/StoragePropPage.h"
#include "../Prop/ConnectionPropPage.h"
#include "../Prop/BusPropPage.h"
#include "../Prop/PortPropPage.h"
#include "../Prop/PipelineStagePropPage.h"

#include "SIMGraphCompoundUnit.h"
#include "SIMGraphUnit.h"
#include "SIMGraphStorage.h"
#include "SIMGraphLatch.h"
#include "SIMGraphConnection.h"
#include "SIMGraphBus.h"
#include "SIMGraphPort.h"
#include "SIMGraphPipelineStage.h"

#include "../Ops/InstrDescrDialog.h"
#include "../Ops/OpMappingsDialog.h"
#include "../Ops/OpMappingsDialog1.h"
#include "../Ops/OpGroupsDialog.h"
#include "../Ops/VarGroupDialog.h"

#include "../MouseHitInfo.h"

#include "DataPathsDialog.h"

#include "../ColorCodes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

__declspec (dllimport) bool volatile lockPCPRO;
__declspec (dllimport) bool volatile stepPCPRO;
__declspec (dllimport) bool volatile donePCPRO;

extern int  expression_main(int argc, char **argv);

CString StorageDataPath;
CString newStorageDataPath;
//bool IsStorageDataPath=false;
int isStoragePathType = 0;

extern CString unit_inputlatch[50][50];
extern CString unit_outputlatch[50][50];
extern int unitnumber;
/////////////////////////////////////////////////////////////////////////////
// SIMView

IMPLEMENT_DYNCREATE(SIMView, CScrollView)

SIMView::SIMView()
{
 lockDraw=false;
 noDoc=true;
 dragging=false;
 buttonDown=false;
 bDatapath=false;
 viewSelection=NULL;

 bSimulatorActive=false;

 bSelectingStart=true;
 startObj=NULL;
 endObj=NULL;

 ownerObj=NULL;

 //set the view font
 viewFont=new CFont;
 //res = new_font.CreateFont(8,0,0,0,0,0,0,0,0,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"MS Sans Serif");
 //VERIFY(viewFont->CreateFont(12,0,0,0,0,0,0,0,0,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"Verdana"));
 VERIFY(viewFont->CreateFont(10,0,0,0,0,0,0,0,0,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"Small Fonts"));

 stepPCPRO=false;
 lockPCPRO=true;
 donePCPRO=false;
}

SIMView::~SIMView()
{
 //remove all prop pages needed for a SimView
 theApp.propPageList->RemovePage(IDD_STAT_SIM);
 //theApp.propPageList->RemovePage(IDD_STAT_COMP_UNIT);
 //theApp.propPageList->RemovePage(IDD_STAT_UNIT);
 //theApp.propPageList->RemovePage(IDD_STAT_LATCH);
 //theApp.propPageList->RemovePage(IDD_STAT_PORT);
 //theApp.propPageList->RemovePage(IDD_STAT_CONN);
 //theApp.propPageList->RemovePage(IDD_STAT_STOR);

 theApp.propPageList->RemovePage(IDD_PROP_COMP_UNIT);
 theApp.propPageList->RemovePage(IDD_PROP_UNIT);
 theApp.propPageList->RemovePage(IDD_PROP_LATCH);
 theApp.propPageList->RemovePage(IDD_PROP_PORT);
 theApp.propPageList->RemovePage(IDD_PROP_CONN);
 theApp.propPageList->RemovePage(IDD_PROP_STOR);
 theApp.propPageList->RemovePage(IDD_PROP_PIPELINESTAGE);

 delete viewFont;
}


BEGIN_MESSAGE_MAP(SIMView, CScrollView)
//{{AFX_MSG_MAP(SIMView)
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_COMMAND(ID_PBB_EXPAND, OnPopupExpand)
	ON_COMMAND(ID_PBB_SHOWPROPERTIES, OnPopupProperties)
	ON_COMMAND(ID_PBB_UDDU, OnPopupUDDU)
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_ADD_UNIT, OnAddUnit)
	ON_COMMAND(ID_ADD_STORAGE, OnAddStorage)
	ON_COMMAND(ID_ADD_PORT, OnAddPort)
	ON_COMMAND(ID_ADD_CONNECTION, OnAddConnection)
	ON_COMMAND(ID_SIM_RESET, OnSimReset)
	ON_COMMAND(ID_SIM_RUN, OnSimRun)
	ON_COMMAND(ID_SIM_STEP, OnSimStep)
	ON_COMMAND(ID_SIM_STOP, OnSimStop)
	ON_COMMAND(ID_DEFAULT, OnDefault)
	ON_COMMAND(ID_DELETE, OnDelete)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYUP()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_ADD_COMP_UNIT, OnAddCompUnit)
	ON_COMMAND(ID_FILE_SAVE_EXPR_DESCR, OnFileSaveExprDescr)
	ON_COMMAND(ID_FILE_LOAD_GRAPHICAL_MACHINE_DESCRIPTION, OnFileLoadGraphicalMachineDescription)
	ON_COMMAND(ID_FILE_SAVE_GRAPHICAL_MACHINE_DESCRIPTION, OnFileSaveGraphicalMachineDescription)
	ON_COMMAND(ID_ADD_LATCH, OnAddLatch)
	ON_COMMAND(ID_ADD_PIPELINESTAGE, OnAddPipelineStage)
	ON_WM_SIZE()
	ON_COMMAND(ID_ADD_DATAPATH, OnAddDatapath)
	ON_COMMAND(ID_EDIT_DATAPATHS, OnEditDatapaths)
	ON_COMMAND(ID_RESIZE, OnResizeObj)
	ON_COMMAND(ID_TEMP_SWITCHTOSTATISTICS, OnTempSwitchtostatistics)
	ON_COMMAND(ID_INSTRUCTIONSET_SETINSTRUCTIONDESCRIPTION, OnISSetInstructionDescription)
	ON_COMMAND(ID_INSTRUCTIONSET_SETOPMAPPINGS, OnISSetOpMappings)
	ON_COMMAND(ID_INSTRUCTIONSET_SETOPERANDMAPPING, OnISSetOperandMappings)
	ON_COMMAND(ID_INSTRUCTIONSET_SETOPGROUPS, OnISSetOpGroups)
	ON_COMMAND(ID_INSTRUCTIONSET_SETVARGROUPS, OnISSetVarGroups)
	ON_COMMAND(ID_SAVE_IS_DESCR, OnSaveISDescr)
	ON_COMMAND(ID_LOAD_IS_DESCR, OnLoadISDescr)
	ON_COMMAND(ID_ADD_BUS, OnAddBus)
	ON_WM_KILLFOCUS()
	ON_COMMAND(ID_FILE_SAVE_EXPR_DESCR_AS_C, OnFileSaveExprDescrAsC)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SIMView drawing

void SIMView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
 
	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
 
	pDoc->xBound=DIAG_WIDTH;
 pDoc->yBound=DIAG_HEIGHT;
}

void SIMView::OnDraw(CDC* pDC)
{
 if (lockDraw) return;
 //int res;
	RECT rect;
	CSize sizeTotal;
	SCROLLINFO scrollInfo;
	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
 
 //if (pDoc->m_DocIsDirty) 
 //{
 // CWaitCursor cwc;
 // pDoc->GenerateGraphDoc(GetCurrentRoutine());
 // noDoc=false;
 //}
 
 //setup the DC params
 
// //brush
// CBrush hollowBrush;
// hollowBrush.CreateStockObject(HOLLOW_BRUSH);
// CBrush* oldBrush=pDC->SelectObject(&hollowBrush);
 
	CFont* oldFont = pDC->SelectObject(viewFont);

 //text attributes
 pDC->SetTextColor(WHITE);
 pDC->SetBkMode(TRANSPARENT);

 //Get scroll bar positions.
	int xPos = GetScrollPos(SB_HORZ);
	int yPos = GetScrollPos(SB_VERT);
 
 //walk only the units and storage obj lists and draw them

 POSITION pos=pDoc->pipelineStages.GetHeadPosition();
 while (pos!=NULL)
 {
  pDoc->pipelineStages.GetNext(pos)->OnDraw(pDC);
 }

 pos=pDoc->simObjs.GetHeadPosition();
 while (pos!=NULL)
 {
  pDoc->simObjs.GetNext(pos)->OnDraw(pDC);
 }

 //update scrollbars
 //get window size.
	GetClientRect(&rect);
	//set the total size of this view
	sizeTotal.cx = pDoc->xBound + 2 * POINT_OFFSET;
 sizeTotal.cy = pDoc->yBound + 2 * POINT_OFFSET;
	SetScrollSizes(MM_TEXT, sizeTotal);
 //initialize the scroll page size for the vertical scroll bar
 scrollInfo.nPage = rect.bottom;
 scrollInfo.fMask = SIF_PAGE;
	VERIFY(SetScrollInfo(SB_VERT,&scrollInfo));
 //initialize the scroll page size for the horizontal scroll bar
 scrollInfo.nPage = rect.right;
 scrollInfo.fMask = SIF_PAGE;
	VERIFY(SetScrollInfo(SB_HORZ,&scrollInfo));
 
 // Replace original font and brush
	pDC->SelectObject(oldFont);
 //pDC->SelectObject(oldBrush);
}

/////////////////////////////////////////////////////////////////////////////
// SIMView diagnostics

#ifdef _DEBUG
void SIMView::AssertValid() const
{
	CScrollView::AssertValid();
}

void SIMView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// SIMView message handlers

void SIMView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
 SIMGraphObj::hScroll=GetScrollPos(SB_HORZ);
 RedrawWindow();
}

void SIMView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
 SIMGraphObj::vScroll=GetScrollPos(SB_VERT);
 RedrawWindow();
}

void SIMView::OnPopupExpand() 
{
 /*
	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
  
 ((GenericNode*)hit->node)->ToggleExpanded(GNT_SIM);
 pDoc->m_DocIsDirty=true;
 RedrawWindow();
 delete hit;
 */
}

void SIMView::OnPopupProperties() 
{
	//SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
	//ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
 // 
 //((GenericNode*)hit->node)->ToggleExpanded();
 //pDoc->m_DocIsDirty=true;
 //RedrawWindow();
 //delete hit;
}

void SIMView::OnPopupUDDU() 
{
 /*
	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
  
 ((GenericNode*)hit->node)->ToggleUDDU(GNT_SIM);
 pDoc->m_DocIsDirty=true;
 RedrawWindow();
 delete hit;
 */
}

void SIMView::OnSetFocus(CWnd* pOldWnd) 
{
	CScrollView::OnSetFocus(pOldWnd);

 if (viewSelection==NULL)
	 theApp.propPageList->ShowPage(bSimulatorActive ? IDD_STAT_SIM : IDD_PROP_BASE, NULL);
 else
 {
  switch(viewSelection->type)
  {
  case MHI_SIM_COMPOUND_UNIT:
   //theApp.propPageList->ShowPage(bSimulatorActive ? IDD_STAT_COMP_UNIT : IDD_PROP_COMP_UNIT, viewSelection);
   theApp.propPageList->ShowPage(bSimulatorActive ? IDD_STAT_SIM : IDD_PROP_COMP_UNIT, viewSelection);
   break;

  case MHI_SIM_UNIT:
   //theApp.propPageList->ShowPage(bSimulatorActive ? IDD_STAT_UNIT : IDD_PROP_UNIT, viewSelection);
   theApp.propPageList->ShowPage(bSimulatorActive ? IDD_STAT_SIM : IDD_PROP_UNIT, viewSelection);
   break;

  case MHI_SIM_LATCH:
   //theApp.propPageList->ShowPage(bSimulatorActive ? IDD_STAT_LATCH : IDD_PROP_LATCH, viewSelection);
   theApp.propPageList->ShowPage(bSimulatorActive ? IDD_STAT_SIM : IDD_PROP_LATCH, viewSelection);
   break;

  case MHI_SIM_PORT:
   //theApp.propPageList->ShowPage(bSimulatorActive ? IDD_STAT_PORT : IDD_PROP_PORT, viewSelection);
   theApp.propPageList->ShowPage(bSimulatorActive ? IDD_STAT_SIM : IDD_PROP_PORT, viewSelection);
   break;

  case MHI_SIM_CONNECTION:
   //theApp.propPageList->ShowPage(bSimulatorActive ? IDD_STAT_CONN : IDD_PROP_CONN, viewSelection);
   theApp.propPageList->ShowPage(bSimulatorActive ? IDD_STAT_SIM : IDD_PROP_CONN, viewSelection);
   break;

  case MHI_SIM_BUS:
   //theApp.propPageList->ShowPage(bSimulatorActive ? IDD_STAT_CONN : IDD_PROP_CONN, viewSelection);
   theApp.propPageList->ShowPage(bSimulatorActive ? IDD_STAT_SIM : IDD_PROP_BUS, viewSelection);
   break;

  case MHI_SIM_STORAGE:
   //theApp.propPageList->ShowPage(bSimulatorActive ? IDD_STAT_STOR : IDD_PROP_STOR, viewSelection);
   theApp.propPageList->ShowPage(bSimulatorActive ? IDD_STAT_SIM : IDD_PROP_STOR, viewSelection);
   break;

  case MHI_SIM_PIPELINESTAGE:
   theApp.propPageList->ShowPage(bSimulatorActive ? IDD_STAT_SIM : IDD_PROP_PIPELINESTAGE, viewSelection);
   break;

  default:
   ASSERT(0);
  }

 }
}


void SIMView::OnAddCompUnit() 
{
 bDatapath=false;
 theApp.pMainForm->UpdateExpressionToolbarAndMenus(ID_ADD_COMP_UNIT);
}

void SIMView::OnAddUnit() 
{
 bDatapath=false;
 theApp.pMainForm->UpdateExpressionToolbarAndMenus(ID_ADD_UNIT);
}

void SIMView::OnAddStorage() 
{
 bDatapath=false;
 theApp.pMainForm->UpdateExpressionToolbarAndMenus(ID_ADD_STORAGE);	
}

void SIMView::OnAddLatch() 
{
 bDatapath=false;
 theApp.pMainForm->UpdateExpressionToolbarAndMenus(ID_ADD_LATCH);	
}

void SIMView::OnAddPort() 
{
 bDatapath=false;
 theApp.pMainForm->UpdateExpressionToolbarAndMenus(ID_ADD_PORT);	
}

void SIMView::OnAddConnection() 
{
 bDatapath=false;
 theApp.pMainForm->UpdateExpressionToolbarAndMenus(ID_ADD_CONNECTION);	
}

void SIMView::OnAddBus() 
{
 bDatapath=false;
 theApp.pMainForm->UpdateExpressionToolbarAndMenus(ID_ADD_BUS);	
}

void SIMView::OnDefault() 
{
 bDatapath=false;
 theApp.pMainForm->UpdateExpressionToolbarAndMenus(ID_DEFAULT);	
 //completed datapath
 if (newDataPath!="")
 {
 	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
 	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
  pDoc->dataPaths.AddTail(newDataPath);
 }
}

void SIMView::OnAddPipelineStage() 
{
 bDatapath=false;
 theApp.pMainForm->UpdateExpressionToolbarAndMenus(ID_ADD_PIPELINESTAGE);	
}

void SIMView::OnAddDatapath() 
{
 bDatapath=true;
 theApp.pMainForm->UpdateExpressionToolbarAndMenus(ID_ADD_DATAPATH);	
 newDataPath="";
 newStorageDataPath = "";
}

void SIMView::OnEditDatapaths() 
{
	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
 bDatapath=false;
	DataPathsDialog dlg(this,&pDoc->dataPaths);
 dlg.DoModal();	
}

void SIMView::OnSimReset() 
{
	// TODO: Add your command handler code here	
}

//declarations for OnSimRun()
//class Program;
__declspec( dllimport ) void run_simulator_thread();

void SIMView::OnSimRun() 
{
 //if (clock_count==0)
 //{
  //switch to stats
  SwitchToStats();
 //}

 bSimulatorActive=true;

 //order is important
 stepPCPRO=false;
 lockPCPRO=false;
 donePCPRO=false;

 //run the simulator
 run_simulator_thread();

 //while (!donePCPRO);
 //((DefaultStatProp*)theApp.propPageList->activePage)->GetDataFromObject(); 
}

void SIMView::OnSimStep() 
{
 bSimulatorActive=true;
 //order is important
 ((DefaultStatProp*)theApp.propPageList->activePage)->GetDataFromObject();    
 stepPCPRO=true;
 lockPCPRO=false;
}

void SIMView::OnSimStop() 
{
 lockPCPRO=true;
}


void SIMView::OnLButtonUp(UINT nFlags, CPoint point) 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{


	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
 
 buttonDown=false;

 //adjust the point for scrolling
 CPoint	scrolledPoint(point.x+GetScrollPos(SB_HORZ),point.y+GetScrollPos(SB_VERT));
 CToolBarCtrl& toolbar=theApp.pMainForm->m_wndExpressionBar.GetToolBarCtrl();
 
 if (dragging) //dropping
 {
  VERIFY(ClipCursor(NULL)); 

  CPoint pt=scrolledPoint;
  pt-=dragStart;
  //dragObject->Offset(pt,dragObject->parent);
  if (viewSelection->type!=MHI_SIM_CONNECTION)
  {
   dragObject->Offset(pt,dragObject->parent);
  }
  else
  {
   SIMGraphConnection* conn=dynamic_cast<SIMGraphConnection*>(viewSelection);
   if (conn->startObj->rect.PtInRect(point))
   {
    dragObject->Offset(pt,conn->startObj);
   }
   else
   {
    dragObject->Offset(pt,conn->endObj);
   }
  }
  RedrawWindow();
  dragging=false;
 }
 else
 {  
  //go thru all objects and find if the user clicked on any
  SIMGraphObj* hit=pDoc->getHit(scrolledPoint);
  if (hit!=NULL)
  {
   //clicking in DATAPATH mode
   if (bDatapath) 
   {

    _CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "!", "Adding to datapath: %s\n", viewSelection->name);
//	if ((!(viewSelection->classname).CompareNoCase("SRAM"))||(!(viewSelection->classname).CompareNoCase("DRAM"))||(!(viewSelection->classname).CompareNoCase("ICACHE"))||(!(viewSelection->classname).CompareNoCase("DCACHE"))||(!(viewSelection->classname).CompareNoCase("REGFILE")))
	if (!(viewSelection->classname).CompareNoCase("Storage"))
	{
		isStoragePathType++;

		if (isStoragePathType == 2)
		{
			//newStorageDataPath+="(";
			//IsStorageDataPath=true;
			newDataPath=" ||STORAGE_PATH|| "+newDataPath;
			isStoragePathType=0;
		}
		//newStorageDataPath+=" "+viewSelection->name;
	}


	if (viewSelection->classname != "")
		newDataPath+=" " + viewSelection->name;


		return;
   }

   //if clicking on a bus and adding a port
   if ((hit->type==MHI_SIM_BUS) && toolbar.IsButtonChecked(ID_ADD_PORT))
   {
    AddSIMGraphObj(scrolledPoint);
   }

   //if clicking on a compound unit and adding a unit
   if ((hit->type==MHI_SIM_COMPOUND_UNIT) && (toolbar.IsButtonChecked(ID_ADD_UNIT) || toolbar.IsButtonChecked(ID_ADD_LATCH) || toolbar.IsButtonChecked(ID_ADD_COMP_UNIT)))
   {
    AddSIMGraphObj(scrolledPoint);
   }

   //if clicking on a unit and adding a latch
   if ((hit->type==MHI_SIM_UNIT) && toolbar.IsButtonChecked(ID_ADD_LATCH))
   {
    AddSIMGraphObj(scrolledPoint);
   }

   //if (theApp.pMainForm->m_wndSimulatorBar.GetToolBarCtrl().IsButtonChecked(ID_BREAK) && (viewSelection!=NULL))
   //{
   // viewSelection->ToggleBreakpoint();
   // RedrawWindow();
   //}
   
   if ((toolbar.IsButtonChecked(ID_ADD_CONNECTION)) && 
       (viewSelection->type!=MHI_SIM_COMPOUND_UNIT) && 
       (viewSelection->type!=MHI_SIM_CONNECTION))
   {
    if (bSelectingStart)
    {
     startObj=viewSelection;
     startObjPoint=scrolledPoint;
     bSelectingStart=!bSelectingStart;
    }
    else
    {
     if ((viewSelection==startObj) || startObj->IsAncestorOf(viewSelection)) //disable self loops
     {
      startObj=viewSelection;
      startObjPoint=scrolledPoint;
     }
     else 
     {
      endObj=viewSelection;
      endObjPoint=scrolledPoint;
      bSelectingStart=!bSelectingStart;
      AddSIMGraphObj(scrolledPoint);
     }
    }
   }
   
   if ( (toolbar.IsButtonChecked(ID_ADD_PORT)) && 
        (viewSelection->type!=MHI_SIM_PORT) && 
        (viewSelection->type!=MHI_SIM_CONNECTION) && 
        (viewSelection->type!=MHI_SIM_COMPOUND_UNIT) ) //can't add a port to any of these
   {
    AddSIMGraphObj(scrolledPoint);
   }

   //temp, use Draw() later
   RedrawWindow();
   //*/
  }
  else //no hit
  {
   CToolBarCtrl& toolbar=theApp.pMainForm->m_wndExpressionBar.GetToolBarCtrl();

   //if (viewSelection!=NULL)
   //{
   // viewSelection->selected=!viewSelection->selected;
   // viewSelection=NULL;
   // RedrawWindow();
   //}

   //if in CONNECTION mode reset the endpoints and exit
   if (toolbar.IsButtonChecked(ID_ADD_CONNECTION))
   {
    bSelectingStart=true;
    return;
   }

   //if in PORT (or LATCH mode exit)
   if (toolbar.IsButtonChecked(ID_ADD_PORT)) //|| (toolbar.IsButtonChecked(ID_ADD_LATCH)))
   {
    return;
   }

   if (!toolbar.IsButtonChecked(ID_DEFAULT) && !toolbar.IsButtonChecked(ID_ADD_DATAPATH))
   {
    AddSIMGraphObj(scrolledPoint);
    RedrawWindow();
   }
  }
 }
	CScrollView::OnLButtonUp(nFlags, point);
}

void SIMView::OnLButtonDown(UINT nFlags, CPoint point) 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
 
 //adjust the point for scrolling
 CPoint	scrolledPoint(point.x+GetScrollPos(SB_HORZ),point.y+GetScrollPos(SB_VERT));
 
 //go thru all objects and find if the user clicked on any
 SIMGraphObj* hit=pDoc->getHit(scrolledPoint);
 if (hit!=NULL)
 {
  MouseHitInfo mhi;
  mhi.hitType=hit->type;
  mhi.node=(GenericNode*)hit;
  mhi.ProcessLeftClick(GNT_SIM,(GenericNode**)&viewSelection, bSimulatorActive);

  //remember this for dragging
  dragObject=hit;
  buttonDown=true;
  dragStart=scrolledPoint;

  //temp, use Draw() later
  RedrawWindow();
 }
 else if (viewSelection!=NULL) //remove the current selection since user clicked outside of any obj
 {
  viewSelection->selected=false;
  CDC* pDC=GetDC();
  viewSelection->OnDraw(pDC);
  ReleaseDC(pDC);
  viewSelection=NULL;
	 //theApp.propPageList->ShowPage(bSimulatorActive ? IDD_STAT_SIM : IDD_PROP_BASE, NULL);
  if (bSimulatorActive)
  {
   theApp.propPageList->ShowPage(IDD_STAT_SIM, viewSelection);
   ((DefaultStatProp*)theApp.propPageList->activePage)->GetDataFromObject();    
  }
  else
  {
   theApp.propPageList->ShowPage(IDD_PROP_BASE, viewSelection);
   //((CompoundUnitPropPage*)theApp.propPageList->activePage)->GetDataFromObject();
  }

 }

	CScrollView::OnLButtonDown(nFlags, point);
}

void SIMView::OnRButtonUp(UINT nFlags, CPoint point) 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
  
 //adjust the point for scrolling
 CPoint	scrolledPoint(point.x+GetScrollPos(SB_HORZ),point.y+GetScrollPos(SB_VERT));
 
 //go thru all instructions and find if the user clicked on any
 SIMGraphObj* hit=pDoc->getHit(scrolledPoint);
 if (hit!=NULL)
 {
  ClientToScreen(&point);
  MouseHitInfo mhi;
  mhi.hitType=hit->type;
  mhi.node=(GenericNode*)hit;
  mhi.ProcessRightClick(GNT_SIM,point,this);
  //delete hit;
 }

 // added by Sudeep - on a right click the Datapath is added!
 //
  bDatapath=false;
 theApp.pMainForm->UpdateExpressionToolbarAndMenus(ID_DEFAULT);	
 //completed datapath
 if (newDataPath!="")
 {
 	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
 	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
    pDoc->dataPaths.AddTail(newDataPath);

	//newStorageDataPath+=") ";
	//StorageDataPath+= newStorageDataPath;
	newDataPath="";
	newStorageDataPath = "";
	isStoragePathType=0;
	//IsStorageDataPath=false;

 }
 // end of addition

	CScrollView::OnRButtonUp(nFlags, point);
}

void SIMView::OnMouseMove(UINT nFlags, CPoint point) 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
 static CPoint offsetPoint;

 //adjust the point for scrolling
 //CPoint	scrolledPoint(point.x+GetScrollPos(SB_HORZ),point.y+GetScrollPos(SB_VERT));
 CPoint	scrolledPoint(point.x,point.y);
 
 if (dragging)
 {
  CDC* pDC=GetDC();
  //dragObject->OnDragDraw(pDC,offsetPoint,dragObject->parent);
  if (viewSelection->type!=MHI_SIM_CONNECTION)
  {
   dragObject->OnDragDraw(pDC,offsetPoint,dragObject->parent);
  }
  else
  {
   SIMGraphConnection* conn=dynamic_cast<SIMGraphConnection*>(viewSelection);
   //if (viewSelection->parent==conn->startObj) //see which end are we dragging
   if (conn->startObj->rect.PtInRect(point))
   {
    dragObject->OnDragDraw(pDC,offsetPoint,conn->startObj);
   }
   else
   {
    dragObject->OnDragDraw(pDC,offsetPoint,conn->endObj);
   }
  }

  offsetPoint=scrolledPoint-dragStart;
  //dragObject->OnDragDraw(pDC,offsetPoint,dragObject->parent);
  if (viewSelection->type!=MHI_SIM_CONNECTION)
  {
   dragObject->OnDragDraw(pDC,offsetPoint,dragObject->parent);
  }
  else
  {
   SIMGraphConnection* conn=dynamic_cast<SIMGraphConnection*>(viewSelection);
   //if (viewSelection->parent==conn->startObj) //see which end are we dragging
   if (conn->startObj->rect.PtInRect(point))
   {
    dragObject->OnDragDraw(pDC,offsetPoint,conn->startObj);
   }
   else
   {
    dragObject->OnDragDraw(pDC,offsetPoint,conn->endObj);
   }
  }

  ReleaseDC(pDC);
  return;
 }

 //take care of dragging
 if (buttonDown && ((abs(dragStart.x-point.x-GetScrollPos(SB_HORZ))>2) || (abs(dragStart.y-point.y-GetScrollPos(SB_VERT))>2))) //verify that we moved more than 2 pixels :)  
 {
  CDC* pDC=GetDC();  
  dragging=true;
  if (viewSelection->parent!=NULL) //restrict dragging to parent
  {
   CRect r;
   if (viewSelection->type!=MHI_SIM_CONNECTION)
   {
    r=viewSelection->parent->rect;
    r.DeflateRect(point.x-viewSelection->rect.left+3,
                  point.y-viewSelection->rect.top+3,
                  viewSelection->rect.right-point.x+3,
                  viewSelection->rect.bottom-point.y+3);
   }
   else //connection
   {    
    SIMGraphConnection* conn=dynamic_cast<SIMGraphConnection*>(viewSelection);
    //if (viewSelection->parent==conn->startObj) //see which end are we dragging
    if (conn->startObj->rect.PtInRect(point))
    {
     r=conn->startObj->rect;
     r.DeflateRect(point.x-conn->rectStart.left+3,
                   point.y-conn->rectStart.top+3,
                   conn->rectStart.right-point.x+3,
                   conn->rectStart.bottom-point.y+3);
    }
    else
    {
     r=conn->endObj->rect;
     r.DeflateRect(point.x-conn->rectEnd.left+3,
                   point.y-conn->rectEnd.top+3,
                   conn->rectEnd.right-point.x+3,
                   conn->rectEnd.bottom-point.y+3);
    }
   }
   ClientToScreen(r);
   VERIFY(ClipCursor(r)); 
  }
  else //restrict dragging to client
  {
   CRect r;
   GetClientRect(&r);
   ClientToScreen(r);
   VERIFY(ClipCursor(r)); 
  }
 
  offsetPoint=scrolledPoint-dragStart;
  //dragObject->OnDragDraw(pDC,offsetPoint,dragObject->parent);
  ///*
  if (viewSelection->type!=MHI_SIM_CONNECTION)
  {
   dragObject->OnDragDraw(pDC,offsetPoint,dragObject->parent);
  }
  else
  {
   SIMGraphConnection* conn=dynamic_cast<SIMGraphConnection*>(viewSelection);
   //if (viewSelection->parent==conn->startObj) //see which end are we dragging
   if (conn->startObj->rect.PtInRect(point))
   {
    dragObject->OnDragDraw(pDC,offsetPoint,conn->startObj);
   }
   else
   {
    dragObject->OnDragDraw(pDC,offsetPoint,conn->endObj);
   }
  }
  //*/
  ReleaseDC(pDC);
 }
	CScrollView::OnMouseMove(nFlags, point);
}

void SIMView::OnDelete()
{
 	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
 	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
 
  if (viewSelection!=NULL)
  {
   //find the object and delete it
   lockDraw=true;
   RemoveObj(viewSelection);
   viewSelection=NULL;
   lockDraw=false;
   RedrawWindow();
   theApp.propPageList->ShowPage(bSimulatorActive ? IDD_STAT_SIM : IDD_PROP_BASE, NULL);
  }
}

void SIMView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
 if (nChar==VK_DELETE)
 {
 	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
 	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
 
  if (viewSelection!=NULL)
  {
   //find the object and delete it
   lockDraw=true;
   RemoveObj(viewSelection);
   viewSelection=NULL;
   lockDraw=false;
   RedrawWindow();
   theApp.propPageList->ShowPage(bSimulatorActive ? IDD_STAT_SIM : IDD_PROP_BASE, NULL);
  }
 }
	CScrollView::OnKeyUp(nChar, nRepCnt, nFlags);
}


//TEMP
#include "..\Ops\ISInfo.h"
ISInfo isInfo;
//TEMP

int SIMView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
 theApp.pMainForm->UpdateExpressionToolbarAndMenus(ID_DEFAULT);

 //now add all the property pages that we need
 BasePropPage::bNoUpdatePos=true;

 CompoundUnitPropPage* compUnitPropPage= new CompoundUnitPropPage();
 compUnitPropPage->Create(compUnitPropPage->pageID,this);
 theApp.propPageList->AddPage(compUnitPropPage->pageID,compUnitPropPage);

 UnitPropPage* unitPropPage= new UnitPropPage(&isInfo);
 unitPropPage->Create(unitPropPage->pageID,this);
 theApp.propPageList->AddPage(unitPropPage->pageID,unitPropPage);

 LatchPropPage* latchPropPage= new LatchPropPage();
 latchPropPage->Create(latchPropPage->pageID,this);
 theApp.propPageList->AddPage(latchPropPage->pageID,latchPropPage);

 PortPropPage* portPropPage= new PortPropPage();
 portPropPage->Create(portPropPage->pageID,this);
 theApp.propPageList->AddPage(portPropPage->pageID,portPropPage);

 ConnectionPropPage* connectionPropPage= new ConnectionPropPage();
 connectionPropPage->Create(connectionPropPage->pageID,this);
 theApp.propPageList->AddPage(connectionPropPage->pageID,connectionPropPage);

 BusPropPage* busPropPage= new BusPropPage();
 busPropPage->Create(busPropPage->pageID,this);
 theApp.propPageList->AddPage(busPropPage->pageID,busPropPage);

 StoragePropPage* storagePropPage= new StoragePropPage();
 storagePropPage->Create(storagePropPage->pageID,this);
 theApp.propPageList->AddPage(storagePropPage->pageID,storagePropPage);

 PipelineStagePropPage* pipelineStagePropPage= new PipelineStagePropPage();
 pipelineStagePropPage->Create(pipelineStagePropPage->pageID,this);
 theApp.propPageList->AddPage(pipelineStagePropPage->pageID,pipelineStagePropPage);

 // statistics pages
 DefaultStatProp* defaultStatProp= new DefaultStatProp();
 defaultStatProp->Create(defaultStatProp->pageID,this);
 theApp.propPageList->AddPage(defaultStatProp->pageID,defaultStatProp);

/*
 CompoundUnitStatPropPage* compoundUnitStatPropPage= new CompoundUnitStatPropPage();
 compoundUnitStatPropPage->Create(compoundUnitStatPropPage->pageID,this);
 theApp.propPageList->AddPage(compoundUnitStatPropPage->pageID,compoundUnitStatPropPage);

 UnitStatPropPage* unitStatPropPage= new UnitStatPropPage();
 unitStatPropPage->Create(unitStatPropPage->pageID,this);
 theApp.propPageList->AddPage(unitStatPropPage->pageID,unitStatPropPage);

 LatchStatPropPage* latchStatPropPage= new LatchStatPropPage();
 latchStatPropPage->Create(latchStatPropPage->pageID,this);
 theApp.propPageList->AddPage(latchStatPropPage->pageID,latchStatPropPage);

 StorageStatPropPage* storageStatPropPage= new StorageStatPropPage();
 storageStatPropPage->Create(storageStatPropPage->pageID,this);
 theApp.propPageList->AddPage(storageStatPropPage->pageID,storageStatPropPage);

 ConnectionStatPropPage* connectionStatPropPage= new ConnectionStatPropPage();
 connectionStatPropPage->Create(connectionStatPropPage->pageID,this);
 theApp.propPageList->AddPage(connectionStatPropPage->pageID,connectionStatPropPage);

 PortStatPropPage* portStatPropPage= new PortStatPropPage();
 portStatPropPage->Create(portStatPropPage->pageID,this);
 theApp.propPageList->AddPage(portStatPropPage->pageID,portStatPropPage);
*/

 BasePropPage::bNoUpdatePos=false;

	return 0;
}

void SIMView::AddSIMGraphObj(CPoint& point)
{
	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");

 CToolBarCtrl& toolbar=theApp.pMainForm->m_wndExpressionBar.GetToolBarCtrl();

 if (toolbar.IsButtonChecked(ID_DEFAULT))
 {
  return;
 }

 //if (toolbar.IsButtonChecked(ID_BREAK) && viewSelection!=NULL)
 //{
 // viewSelection->breakpoint=!viewSelection->breakpoint;
 // return;
 //}

 SIMGraphObj* obj;
 CRect rect;

 if (toolbar.IsButtonChecked(ID_ADD_COMP_UNIT))
 {
  rect.TopLeft()=point;
  rect.BottomRight()=point;
  rect.InflateRect(250,70);
  obj=new SIMGraphCompoundUnit(viewSelection, &rect);
 }

 if (toolbar.IsButtonChecked(ID_ADD_UNIT))
 {
  rect.TopLeft()=point;
  rect.BottomRight()=point;
  rect.InflateRect(60,25);
  obj=new SIMGraphUnit(viewSelection, &rect);
 }

 if (toolbar.IsButtonChecked(ID_ADD_STORAGE))
 {
  rect.TopLeft()=point;
  rect.BottomRight()=point;
  rect.InflateRect(60,60);
  obj=new SIMGraphStorage(viewSelection, &rect);
 }

 if (toolbar.IsButtonChecked(ID_ADD_LATCH))
 {
  rect.TopLeft()=point;
  rect.BottomRight()=point;
  rect.InflateRect(13,8);
  obj=new SIMGraphLatch(viewSelection, &rect);
 }

 if (toolbar.IsButtonChecked(ID_ADD_BUS))
 {
  rect.TopLeft()=point;
  rect.BottomRight()=point;
  rect.InflateRect(250,13);
  obj=new SIMGraphBus(viewSelection, &rect);
 }

 if (toolbar.IsButtonChecked(ID_ADD_CONNECTION))
 {
  //need coords of start and end points and pass them as a rect
  if (startObj->type==MHI_SIM_LATCH)
  {
   obj=new SIMGraphConnection(viewSelection, &rect, startObj, &startObjPoint, endObj, &endObjPoint);
  }
  else
  {
   obj=new SIMGraphConnection(viewSelection, &rect, endObj, &endObjPoint, startObj, &startObjPoint);
  }
 }

 if (toolbar.IsButtonChecked(ID_ADD_PORT))
 {
  rect.TopLeft()=point;
  rect.BottomRight()=point;
  rect.InflateRect(10,10);
  obj=new SIMGraphPort(viewSelection, &rect, ownerObj);
 }

 if (toolbar.IsButtonChecked(ID_ADD_PIPELINESTAGE))
 {
  rect.TopLeft()=point;
  rect.BottomRight()=point;
  rect.InflateRect(10,10);
  obj=new SIMGraphPipelineStage(this, &rect, &point);
 }

 if (viewSelection!=NULL) // add to parent
 {
  obj->parent=viewSelection;
  switch(obj->type)
  {
  case MHI_SIM_COMPOUND_UNIT:
   viewSelection->AddUnit(obj);
   break;

  case MHI_SIM_UNIT:
   viewSelection->AddUnit(obj);
   break;

  case MHI_SIM_LATCH:
   viewSelection->AddLatch(obj);
   break;

  case MHI_SIM_PORT:
   viewSelection->AddPort(obj);
   break;

  case MHI_SIM_CONNECTION:
   if ((startObj->type!=MHI_SIM_LATCH) && (endObj->type!=MHI_SIM_LATCH))
   {
    ((SIMGraphConnection*)obj)->startObj->AddConnection(obj);
    ((SIMGraphConnection*)obj)->endObj->_AddConnection(obj);
   }
   else //we're adding a connection that connects a latch with another unit -> special treatment :)
   {
    if ((startObj->type==MHI_SIM_LATCH) && (endObj->type==MHI_SIM_LATCH))
    {
     MessageBox("Can't add a connection between 2 latches.","GUI Warning");
     ((SIMGraphConnection*)obj)->startObj=NULL;
     ((SIMGraphConnection*)obj)->endObj=NULL;
     delete obj;
     return;
    }

    if (((SIMGraphConnection*)obj)->startObj->type==MHI_SIM_LATCH) //starts at latch
    {
     ((SIMGraphConnection*)obj)->startObj->AddConnection(obj);
     ((SIMGraphConnection*)obj)->endObj->__AddLatch(obj);
    }
    else //ends at latch
    {
     ASSERT(0);
     ((SIMGraphConnection*)obj)->startObj->__AddLatch(obj);
     ((SIMGraphConnection*)obj)->endObj->AddConnection(obj);
    }
   }
   break;

  case MHI_SIM_STORAGE:
   viewSelection->AddStorage(obj);
   break;
  }
 }
 else //add to top level
 {
  if (obj->type==MHI_SIM_PIPELINESTAGE)
  {
   pDoc->pipelineStages.AddTail(obj);
  }
  else
  {
   pDoc->simObjs.AddTail(obj);
  }
 }

 //push default button after an add
 toolbar.CheckButton(ID_DEFAULT);
}

BOOL SIMView::OnEraseBkgnd(CDC* pDC) 
{
 //set the default brush to black
 CBrush blackBrush;
 blackBrush.CreateStockObject(BLACK_BRUSH);
 CBrush* oldBrush=pDC->SelectObject(&blackBrush);
 CRect r;
 GetClientRect(r);
 pDC->Rectangle(r);
 pDC->SelectObject(oldBrush);
	
 return 1;
	//return CScrollView::OnEraseBkgnd(pDC);
}

void SIMView::xArch(CFile* file, int level)
{
	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");

 CString out;
 CString pipealt[50];
 int numalt=0;

 out="\n;///////////////////////////// Section 4: Components Specification //////////////////////////////\n\n";
 out+="(ARCHITECTURE_SECTION\n";
 file->Write(out, out.GetLength());

 pDoc->OutputSubtypeDefs(file,level+1);
 
 POSITION pos=pDoc->simObjs.GetHeadPosition();
 while (pos!=NULL)
 {
  pDoc->simObjs.GetNext(pos)->xArch(file,level+1);  
  out="\n";
  file->Write(out, out.GetLength());
 }

  out= " (Storage Connect\n )\n\n";
 out+=")\n";

 file->Write(out, out.GetLength());

 //PIPELINE SECTION

 out="\n;////////////////////////////////////// Section 5: Pipeline and Data-transfer paths /////////////////////////\n\n";
 out+="(PIPELINE_SECTION\n";;
 file->Write(out, out.GetLength());

 //first sort pipeline stages
 SIMGraphPipelineStage::SortPipelineStages(&pDoc->pipelineStages);
 SIMGraphPipelineStage::ComputePipelineStageRects(&pDoc->pipelineStages);

 //for all pipeline stages
 pos=pDoc->pipelineStages.GetHeadPosition();
 while (pos!=NULL)
 {
  SIMGraphObj* pipelineStage=pDoc->pipelineStages.GetNext(pos);
  //cleanup the stage
  ((SIMGraphPipelineStage*)pipelineStage)->stageObjs.RemoveAll();
  //for all objs in simulator
  POSITION pos2=pDoc->simObjs.GetHeadPosition();
  while (pos2!=NULL)
  {
   pDoc->simObjs.GetNext(pos2)->xPipe(file,pipelineStage);  
  }
 }

 if (pDoc->pipelineStages.GetCount()>0)
 {
  //output pipeline stages
  out=" (PIPELINE";
  out+=" FETCH DECODE READ_EXECUTE WB";

  out+=")\n";
  file->Write(out, out.GetLength());


  // changing generated pipeline section info
  // Sudeep (11/2/02) - need to remove this hard hack! 
  // But it works well

  //first construct array of units in read and execute stages

  int maxUnitsinStage=0, tmpCount=0;
  pos=pDoc->pipelineStages.GetHeadPosition();
  while (pos!=NULL)
  {
	  
	  SIMGraphPipelineStage* stage=(SIMGraphPipelineStage*)(pDoc->pipelineStages.GetNext(pos));
	  POSITION pos2=stage->stageObjs.GetHeadPosition();
	  while (pos2!=NULL)
	  {
		  
		  SIMGraphObj *grobj = stage->stageObjs.GetNext(pos2);
		  
		  tmpCount++;
	  }
	  if (tmpCount > maxUnitsinStage)
		  maxUnitsinStage = tmpCount;
	  tmpCount = 0;
  }
  
  char * temp = new char[50];
  out=" (READ_EXECUTE (ALTERNATE"; 
  for (int i=0;i<maxUnitsinStage;i++)
  {
	  out+=" read_execute";
	  out+= itoa(i,temp,10);
  }
  out+="))\n";
  file->Write(out, out.GetLength());

 int x=0, y=0, isfound=0;
 CString searchlatch;
 CString altarray[50][50];
 int altcount=0;

 while (!unit_outputlatch[x][0].IsEmpty())
 {
	y=1;
	while (!unit_outputlatch[x][y].IsEmpty())
	{
		int x2 = x+1;
		while (!unit_inputlatch[x2][0].IsEmpty())
		{
			int y2=1;
			while (!unit_inputlatch[x2][y2].IsEmpty())
			{
				if (unit_outputlatch[x][y] == unit_inputlatch[x2][y2])
				{
					int x3=0, found=0;
					while (!altarray[x3][0].IsEmpty())
					{
						int y3=0;
						while (!altarray[x3][y3].IsEmpty())
						{	
							if (altarray[x3][y3] == unit_outputlatch[x][0])
							{
								found = 1;
								altarray[x3][y3+1] = unit_inputlatch[x2][0];
							}
							y3++;
						}
						x3++;
					}

					if (found == 0)
					{
						altarray[altcount][0] = unit_outputlatch[x][0];
						altarray[altcount][1] = unit_inputlatch[x2][0];
						altcount++;
					}
				}

				y2++;
			}
			
			x2++;
		}
		y++;
	}
	 // move to next unit
	 x++;
 }

 out="";
 for (int k=0;k<altcount;k++)
 {
	 out+="   (read_execute";
	 out+= itoa(k,temp,10);
	 out+="( PIPELINE ";
	 int t=0;
	 while(!altarray[k][t].IsEmpty())
		 out+=altarray[k][t++]+" ";
	 out+="))\n";
 }

 // check for single unit connected to decode and writeback only
 int p=0, foundit=0;
 while (!unit_outputlatch[p][0].IsEmpty())
 {
	 for (int k=0;k<altcount;k++)
	 {
		 int t=0;
		 while(!altarray[k][t].IsEmpty())
		 {
			 if (unit_outputlatch[p][0] == altarray[k][t++])
				 foundit=1;
		 }
	 }
	 if (foundit == 0)
	 {
		 out+="   (read_execute";
		 out+= itoa(k,temp,10);
		 out+="( PIPELINE ";
		 out+=unit_outputlatch[p][0]+" ";
		 out+="))\n";
	 }
	 
	 foundit=0;
	 p++;
 }
 file->Write(out, out.GetLength());

 //cleanup - required if multiple saves occur in same session
 unitnumber=0;
 int t2=0, t3=0;
 while (!unit_outputlatch[t2][0].IsEmpty())
 {
	t3=0;
	while (!unit_outputlatch[t2][t3].IsEmpty())
	{
		unit_outputlatch[t2][t3++]="";
	}

	 t2++;
 }

 t2=0;
 t3=0;
 while (!unit_inputlatch[t2][0].IsEmpty())
 {
	t3=0;
	while (!unit_inputlatch[t2][t3].IsEmpty())
	{
		unit_inputlatch[t2][t3++]="";
	}

	 t2++;
 }

  /*
  //now expand on pipeline stages
  pos=pDoc->pipelineStages.GetHeadPosition();
  while (pos!=NULL)
  {
   SIMGraphPipelineStage* stage=(SIMGraphPipelineStage*)(pDoc->pipelineStages.GetNext(pos));
   out=" (" + stage->name + " (PARALLEL";
   //for all objs in stage
   POSITION pos2=stage->stageObjs.GetHeadPosition();
   while (pos2!=NULL)
   {
    out+=" " + stage->stageObjs.GetNext(pos2)->name;   
   }
   out+="))\n";
   file->Write(out, out.GetLength());
  }
  */

 }

 
 //output DTPATHS

 if (pDoc->dataPaths.GetCount()>0)
 {
  out="\n\n\n (DTPATHS\n\n";
  file->Write(out, out.GetLength());
  
//  out="  (TYPE BI\n"; //TODO: figure out if data path is UNI or BI
	out="  (TYPE UNI\n";
  file->Write(out, out.GetLength());
  
  pos=pDoc->dataPaths.GetHeadPosition();
  while (pos!=NULL)
  {
   out=pDoc->dataPaths.GetNext(pos);  
   if (out.Find("||STORAGE_PATH||") == -1)
   {
	   out="   (" +out+")\n";
	file->Write(out, out.GetLength());
   } else
   {
	   out="(" +out+" )";
	   StorageDataPath+=out ;
	   StorageDataPath.Replace(" ||STORAGE_PATH|| ","");
   }
  }

  out="\n  )\n";
  file->Write(out, out.GetLength());
  out="\n )\n";
  file->Write(out, out.GetLength());
 }

 out=")\n\n";
 file->Write(out, out.GetLength());

 //STORAGE SECTION

 out="\n;/////////////////////////////////// Section 6: Memory hierarchy //////////////////////////////////////\n\n";
 out+="(STORAGE_SECTION\n";
 file->Write(out, out.GetLength());

 pos=pDoc->simObjs.GetHeadPosition();
 while (pos!=NULL)
 {
  pDoc->simObjs.GetNext(pos)->xStor(file,level+1);  
 }

 out = "(Connect\n (TYPE CONNECTIVITY)\n (STORAGE_CONNECTIONS "+StorageDataPath+" )\n )";
 file->Write(out, out.GetLength());

 out="\n)";
 file->Write(out, out.GetLength());

 file->Close();
}

void SIMView::OnFileSaveExprDescr() 
{

/* int argc = 7, tmp;
 char *argv[] = { "expression console.exe",
 "acesMIPS.xmd",
 "-SIM",
 "-RI",
 "-ASM",
 "-DUMP",
 "-pIR",
 "" };*/

 CFileDialog fd(false, "xmd", "EXPRESSION Machine Description.xmd", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, "EXPRESSION Files (*.xmd)|*.xmd|All Files (*.*)|*.*||", this);
 if (fd.DoModal()==IDOK)
 {
  CFile file(fd.m_ofn.lpstrFile, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite);
  //start generating EXPRESSION

  CString out;
  out =";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n";
  out+=";;;;;;;;;;;;; Machine description generated by V-SAT GUI ;;;;;;;;;;;\n";
  out+=";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  v 1.00 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n";
  out+=";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n\n\n";

  out+=";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n";
  out+=";;;;; This file contains a specification of the a VLIW Processor based\n";
  out+=";;;;;;;;;;;;;;;;;; on the MIPS architecture' ;;;;;;;;;;;;;;;;;;;;;;;;;\n";
  out+=";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n\n";

  out+=";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n";
  out+=";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n";
  out+=";;\n";
  out+=";; This machine description is copyrighted by the Regents of the \n";
  out+=";; University of California, Irvine. The following terms apply to\n";
  out+=";; all files associated with the description unless explicitly \n";
  out+=";; disclaimed in individual files.  \n";
  out+=";; The authors hereby grant permission to use this description and \n";
  out+=";; its documentation for educational and non-commercial purpose. \n";
  out+=";; No written agreement, license, or royalty fee is required for  \n";
  out+=";; authorized use of this software. \n";
  out+=";;   \n";
  out+=";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; \n";
  out+=";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; \n";
  out+=";; \n";
  out+=";; IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE \n";
  out+=";; LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, \n";
  out+=";; INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT \n";
  out+=";; OF THE USE OF THIS DESCRIPTION, ITS DOCUMENTATION, \n";
  out+=";; OR ANY DERIVATIVES THEREOF, EVEN IF THE AUTHORS HAVE \n";
  out+=";; BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  \n";
  out+=";; THE AUTHORS AND DISTRIBUTORS SPECIFICALLY \n";
  out+=";; DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT \n";
  out+=";; LIMITED TO, THE IMPLIED WARRANTIES OF \n";
  out+=";; MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, \n";
  out+=";; AND NON-INFRINGEMENT. THIS DESCRIPTION IS PROVIDED \n";
  out+=";; ON AN \"AS IS\" BASIS, AND THE AUTHORS AND \n";
  out+=";; DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE \n";
  out+=";; MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR \n";
  out+=";; MODIFICATIONS.  \n";
  out+=";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; \n";
  out+=";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; \n";
  out+=" \n";
  out+=";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; \n";
  out+=";; \n";
  out+=";; Architectures and Compilers for Embedded Systems (ACES) Laboratory,  \n";
  out+=";; Center for Embedded Computer Systems (CECS), \n";
  out+=";; University of California, Irvine, \n";
  out+=";; United States of America. \n";
  out+=";; \n";
  out+=";; \n";
  out+=";; Developed by: \n";
  out+=";;		Dr. Nikil Dutt, Professor, UCI			(dutt@ics.uci.edu) \n";
  out+=";;		Dr. Alex Nicolau, Professor, UCI		(nicolau@ics.uci.edu) \n";
  out+=";;		Peter Grun, Graduate Researcher, UCI	(pgrun@ics.uci.edu) \n";
  out+=";;		Ashok Halambi, Graduate Researcher, UCI	(ahalambi@ics.uci.edu) \n";
  out+=";;		Nick Savoiu, Graduate Researcher, UCI	(savoiu@ics.uci.edu) \n";
  out+=";;		Aviral Shrivastava, Graduate Researcher, UCI (aviral@ics.uci.edu) \n";
  out+=";;		Partha Biswas, Graduate Researcher, UCI (partha@ics.uci.edu) \n";
  out+=";;		Prabhat Mishra, Graduate Researcher, UCI (pmishra@ics.uci.edu) \n";
  out+=";;		Sudeep Pasricha, Graduate Researcher, UCI (sudeep@ics.uci.edu) \n";
  out+=";; \n";
  out+=";; Email: express@cecs.uci.edu \n";
  out+=";;\n";
  out+=";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n\n";

  file.Write(out, out.GetLength());

  //InstrSet stuff first
  isInfo.xGen(&file, 0);

  //now for the hardware
  xArch(&file,0);

 }

  // 05/09/03 - TODO
 //tmp = expression_main(argc, argv);
}

void SIMView::OnFileLoadGraphicalMachineDescription() 
{
 CFileDialog fd(true, "gmd", "*.gmd", OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST, "GUI Machine Description Files (*.gmd)|*.gmd|All Files (*.*)|*.*||", this);
 if (fd.DoModal()==IDOK)
 {
 	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
 	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
 
  lockDraw=true; //prevent redrawing while loading
  //remove the selection
  viewSelection=NULL;
	 theApp.propPageList->ShowPage(bSimulatorActive ? IDD_STAT_SIM : IDD_PROP_BASE, NULL);

	 VERIFY(pDoc->OnOpenDocument(fd.m_ofn.lpstrFile));
  SIMGraphPipelineStage::window=this;
  pDoc->UpdatePipelineStages();
  lockDraw=false;
  RedrawWindow();
 }
}

void SIMView::OnFileSaveGraphicalMachineDescription() 
{
 CFileDialog fd(false, "gmd", "GUI Machine Description.gmd", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, "GUI Machine Description Files (*.gmd)|*.gmd|All Files (*.*)|*.*||", this);
 if (fd.DoModal()==IDOK)
 {
 	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
 	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");

	 VERIFY(pDoc->OnSaveDocument(fd.m_ofn.lpstrFile));
 }
}

void SIMView::OnSize(UINT nType, int cx, int cy) 
{
	CScrollView::OnSize(nType, cx, cy);
	
	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");

 pDoc->UpdatePipelineStages();
	
}

void SIMView::RemoveObj(SIMGraphObj * obj)
{
	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");

 if (obj->type==MHI_SIM_PIPELINESTAGE)
 {
  //remove it from the pipeline stage list
  pDoc->pipelineStages.RemoveAt(pDoc->pipelineStages.Find(obj));
  delete obj;
  return;
 }

 if (obj->parent==NULL) //top level component
 {
  pDoc->simObjs.RemoveAt(pDoc->simObjs.Find(obj));
  delete obj;
  return;
 }

 delete obj;
}


//TODO remove later when obj resizing is properly done

#include "../TempDlg.h"

void SIMView::OnResizeObj() 
{
	// TODO: Add your command handler code here
 int width=viewSelection->rect.Width();
 int height=viewSelection->rect.Height();

 TempDlg dlg(&width,&height,this);

 if (dlg.DoModal()==IDOK)
 {
  viewSelection->rect.right=viewSelection->rect.left+width;
  viewSelection->rect.bottom=viewSelection->rect.top+height;
  RedrawWindow();
 }
}

//TODO remove later


void SIMView::OnTempSwitchtostatistics() 
{
	bSimulatorActive = !bSimulatorActive;

}

void SIMView::OnISSetInstructionDescription() 
{
	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");

	//InstrDescrDialog dlg(&(pDoc->isInfo), this);
	InstrDescrDialog dlg(&isInfo, this);
 dlg.DoModal();	
}

void SIMView::OnISSetOpMappings() 
{
	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");

	//OpMappingsDialog dlg(&(pDoc->isInfo), this);
	OpMappingsDialog dlg(&isInfo, this);
 dlg.DoModal();	
}

void SIMView::OnISSetOperandMappings() 
{
	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");

	//OpMappingsDialog1 dlg(&(pDoc->isInfo), this);
	OpMappingsDialog1 dlg(&isInfo, this);
 dlg.DoModal();	
}

void SIMView::OnISSetOpGroups() 
{
	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");

 //OpGroupsDialog dlg(&(pDoc->isInfo), this);
 OpGroupsDialog dlg(&isInfo, this);
 dlg.DoModal();	
}

void SIMView::OnISSetVarGroups() 
{
	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");

 //VarGroupDialog dlg(&(pDoc->isInfo), this);
 VarGroupDialog dlg(&isInfo, this);
 dlg.DoModal();	
}

void SIMView::OnSaveISDescr() 
{
 CFileDialog fd(false, "isd", "Instruction Set Description.isd", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, "Instruction Set Description Files (*.isd)|*.isd|All Files (*.*)|*.*||", this);
 if (fd.DoModal()==IDOK)
 {
 	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
 	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");

  //create an file with this filename and archive to it
  CFile file(fd.m_ofn.lpstrFile, CFile::modeCreate | CFile::modeWrite );
  CArchive ar(&file, CArchive::store);
  //pDoc->isInfo.Serialize(ar);  
  isInfo.Serialize(ar);  
 }
}

void SIMView::OnLoadISDescr() 
{
 CFileDialog fd(true, "isd", "*.isd", OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST, "Instruction Set Description Files (*.isd)|*.isd|All Files (*.*)|*.*||", this);
 if (fd.DoModal()==IDOK)
 {
 	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
 	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
 
  //open an file with this filename and archive to it
  CFile file(fd.m_ofn.lpstrFile, CFile::modeRead);
  CArchive ar(&file, CArchive::load);
  //pDoc->isInfo.Serialize(ar);  
  isInfo.Serialize(ar);  
 }
}

void SIMView::OnFileSaveExprDescrAsC() 
{
 CFileDialog fd(false, "c", "EXPRESSION as C++.cpp", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, "C++ Files (*.cpp)|*.cpp|All Files (*.*)|*.*||", this);
 if (fd.DoModal()==IDOK)
 {
  CFile file(fd.m_ofn.lpstrFile, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite);
  //start generating EXPRESSION

  CString out;
  //out =";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n";
  //out+=";;;;;;;;;;;; Machine C description generated by V-SAT GUI ;;;;;;;;;;\n";
  //out+=";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  v 0.99 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n";
  //out+=";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n\n\n";
  //file.Write(out, out.GetLength());

  out="#include \"stdafx.h\"\n"
      "#include \"SIMGraphCompoundUnit.h\"\n"
      "#include \"SIMGraphUnit.h\"\n"
      "#include \"SIMGraphStorage.h\"\n"
      "#include \"SIMGraphLatch.h\"\n"
      "#include \"SIMGraphConnection.h\"\n"
      "#include \"SIMGraphBus.h\"\n"
      "#include \"SIMGraphPort.h\"\n"
      "#include \"SIMGraphPipelineStage.h\"\n"
      "\n";
  file.Write(out, out.GetLength());

  out="SIMGraphObjList tempList;\n"
      "\n";
  file.Write(out, out.GetLength());

  out="SIMGraphObj* FindSIMGraphObj(char* name)\n"
      "{\n"
      " POSITION pos=tempList.GetHeadPosition();\n"
      " while (pos!=NULL)\n"
      " {\n"
      "  SIMGraphObj* obj=tempList.GetNext(pos);\n"
      "  if (obj->name==name) return obj;\n"
      " }\n"
      " return NULL;\n"
      "}\n"
      "\n";
  file.Write(out, out.GetLength());

  out="void CreateMachine(SIMDoc* simDoc)\n"
      "{\n"
      " SIMGraphObj* obj;\n"
      "\n"
      " simDoc->simObjs.RemoveAll();\n"
      " tempList.RemoveAll();\n"
      "\n";
  file.Write(out, out.GetLength());

  //first the objects
  genO(&file,1);
  //then the lists
  genC(&file,1);

  out="\n}\n";
  file.Write(out, out.GetLength());
 }
}

void SIMView::genO(CFile* file, int level)
{
	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");

 CString out;
 out="\n///////////////////////////// OBJECTS //////////////////////////////\n\n";
 file->Write(out, out.GetLength());

 CString spaces(_T(' '), level);

 POSITION pos=pDoc->simObjs.GetHeadPosition();
 while (pos!=NULL)
 {
  pDoc->simObjs.GetNext(pos)->genO(file,level+1);  
 }

 out="\n";
 file->Write(out, out.GetLength());
}

void SIMView::genC(CFile* file, int level)
{
	SIMDoc* pDoc = dynamic_cast<SIMDoc*>(GetDocument());
	ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");

 CString spaces(_T(' '), level);
 CString out;

 out=spaces+"////  top-level objects for simulator  //////////////////////////////////////////////////\n\n";
 file->Write(out, out.GetLength());

 out=spaces+"simDoc->simObjs.RemoveAll();\n";
 file->Write(out, out.GetLength());

 POSITION pos=pDoc->simObjs.GetHeadPosition();
 while (pos!=NULL)
 {
  out.Format(spaces+"simDoc->simObjs.AddTail(FindSIMGraphObj(\"%s\"));\n", pDoc->simObjs.GetAt(pos)->name);
  file->Write(out, out.GetLength());
  pDoc->simObjs.GetNext(pos);
 }

 out="\n";
 file->Write(out, out.GetLength());

 pos=pDoc->simObjs.GetHeadPosition();
 while (pos!=NULL)
 {
  pDoc->simObjs.GetNext(pos)->genC(file,level+1);
 }

 out="\n";
 file->Write(out, out.GetLength());
}


void SIMView::SwitchToStats()
{
 if (viewSelection!=NULL)
 {
  MouseHitInfo mhi;
  mhi.hitType=((SIMGraphObj*)viewSelection)->type;
  mhi.node=viewSelection;
  //mhi.ProcessLeftClick(&viewSelection, true);
  mhi.ProcessLeftClick(GNT_SIM,(GenericNode**)&viewSelection, true);
  
  //RedrawWindow();
 }
 else //remove the current selection since user clicked outside of any obj
 {
  theApp.propPageList->ShowPage(IDD_STAT_SIM, viewSelection);
  ((DefaultStatProp*)theApp.propPageList->activePage)->GetDataFromObject();    
 }
}
