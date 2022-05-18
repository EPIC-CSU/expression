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
// DefaultStatProp.cpp : implementation file
//

#include "stdafx.h"
#include "..\pcprogui.h"
#include "DefaultStatProp.h"

#include "..\SIM\SimGraphObj.h"
#include "..\SIM\SimDoc.h"
#include "ScrollingStat.h"
#include "ZeroOneScrollingStat.h"
#include "TextStat.h"
#include "PercentageStat.h"

#include "AddStatDlg.h"

#include "StatList.h"
#include "ArchStats.h"

__declspec( dllimport ) BaseSimObject* getComp(char* testStr);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

__declspec (dllimport) bool volatile lockPCPRO;
//__declspec (dllimport) bool volatile stepPCPRO;
__declspec (dllimport) bool volatile donePCPRO;

/////////////////////////////////////////////////////////////////////////////
// DefaultStatProp dialog


DefaultStatProp::DefaultStatProp(CWnd* pParent /*=NULL*/)
	: BasePropPage(DefaultStatProp::IDD, pParent)
{
	//{{AFX_DATA_INIT(DefaultStatProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


DefaultStatProp::~DefaultStatProp()
{
 KillTimer(1);

 ClearStats();
}


void DefaultStatProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DefaultStatProp)
	DDX_Control(pDX, IDC_STATS, m_Stats);
	DDX_Text(pDX, IDC_SELECTION, m_Memo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DefaultStatProp, CDialog)
	//{{AFX_MSG_MAP(DefaultStatProp)
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_LBN_SELCHANGE(IDC_STATS, OnSelChangeStats)
	ON_LBN_KILLFOCUS(IDC_STATS, OnKillFocusStats)
	ON_LBN_SETFOCUS(IDC_STATS, OnSetFocusStats)
	ON_WM_TIMER()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_ADD_STAT, OnAddStat)
	ON_BN_CLICKED(IDC_REMOVE_STAT, OnRemoveStat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DefaultStatProp message handlers

BOOL DefaultStatProp::OnInitDialog() 
{
	CDialog::OnInitDialog();

	bStatsHasFocus=true;

 CRect rect;
 m_Stats.GetClientRect(&rect);
 GUIBaseStat::width=rect.Width();

// GUIBaseStat* bs=new TextStat();
// ((TextStat*)bs)->text="Unit2 Usage: 40%";
// AddStat(bs);
//
// bs=new ScrollingStat();
// AddStat(bs);
//
// bs=new ZeroOneScrollingStat();
// AddStat(bs);
//
// bs=new PercentageStat();
// AddStat(bs);

 //now create a timer
 SetTimer(1,100, NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




void DefaultStatProp::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
 if (lpDrawItemStruct->itemID!=-1)
 {
  GUIBaseStat* bs=(GUIBaseStat*)m_Stats.GetItemData(lpDrawItemStruct->itemID);
  ASSERT(bs);

  int saveID=SaveDC(lpDrawItemStruct->hDC);
  ASSERT(saveID);

  CPen newPen;
//  if (lpDrawItemStruct->itemState & ODS_FOCUS) //draw a focus rectangle
//  {
//   newPen.CreatePen(PS_DOT, 1, ::GetSysColor(COLOR_GRAYTEXT));
//  }
//  else
//  {
   newPen.CreateStockObject(NULL_PEN);
//  }

  HGDIOBJ oldPen = ::SelectObject(lpDrawItemStruct->hDC,newPen);
  CBrush newBrush;

  if (lpDrawItemStruct->itemState & ODS_SELECTED) //use the system selected color
  {
   newBrush.CreateSolidBrush(bStatsHasFocus ? ::GetSysColor(COLOR_HIGHLIGHT) : ::GetSysColor(COLOR_GRAYTEXT));
  }
  else //no brush
  {
   newBrush.CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
  }

  HGDIOBJ oldBrush = ::SelectObject(lpDrawItemStruct->hDC,newBrush);
  ::Rectangle(lpDrawItemStruct->hDC, 
             lpDrawItemStruct->rcItem.left, 
             lpDrawItemStruct->rcItem.top, 
             lpDrawItemStruct->rcItem.right, 
             lpDrawItemStruct->rcItem.bottom);
  ::SelectObject(lpDrawItemStruct->hDC,oldPen);

  RestoreDC(lpDrawItemStruct->hDC,saveID);
  
  //now let the stat item draw itself
  bs->OnDraw(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem);
 }
}

void DefaultStatProp::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
 CRect rect;
 SIZE size;
 CWnd* statList=GetDlgItem(nIDCtl);
 CDC* statDC=statList->GetDC();

 GetTextExtentPoint32(statDC->m_hDC, "Ty", 2, &size);

 //for now all items have same size
 GetDlgItem(nIDCtl)->GetClientRect(&rect);
 lpMeasureItemStruct->itemHeight=((GUIBaseStat*)(lpMeasureItemStruct->itemData))->height;
 lpMeasureItemStruct->itemWidth=rect.right;
	
	//CDialog::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void DefaultStatProp::OnSelChangeStats() 
{
 GUIBaseStat* bs=(GUIBaseStat*)m_Stats.GetItemData(m_Stats.GetCurSel());
 m_Memo=bs->memo;
 UpdateData(false);
}

void DefaultStatProp::OnKillFocusStats() 
{
	//bStatsHasFocus=false;
 //m_Stats.RedrawWindow();
}

void DefaultStatProp::OnSetFocusStats() 
{
	//bStatsHasFocus=true;
 //m_Stats.RedrawWindow();
}

void DefaultStatProp::AddStat(GUIBaseStat* stat)
{
 int i=m_Stats.AddString("NOS");
 ASSERT(i>=0);
 m_Stats.SetItemData(i,(DWORD)stat);
 m_Stats.SetItemHeight(i,stat->height);
}

void DefaultStatProp::OnTimer(UINT nIDEvent) 
{
 //for (int i=0;i<m_Stats.GetCount();i++)
 //{
 // ((GUIBaseStat*)(m_Stats.GetItemData(i)))->Notify();
 //}
 if (!donePCPRO && !lockPCPRO) m_Stats.RedrawWindow();
	
	CDialog::OnTimer(nIDEvent);
}

void DefaultStatProp::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	MovePage(this);
}

void DefaultStatProp::ProcessSimStat(CString& name, GUIBaseStatList* statList, BaseStat * stat)
{
 GUIBaseStat* gbs;
 switch(stat->getType())
 {
 case _PERCENTUSAGE_:
  gbs=new PercentageStat();
  gbs->memo.Format("Percentage utilization for %s", name);
  //((PercentageStat*)gbs)->text="Active Count = ";
  statList->AddTail(gbs);
  stat->setGUIID(gbs);
  break;
  
 case _NUMTIMES_:
  gbs=new TextStat();
  gbs->memo.Format("Number of cycles %s was active", name);
  ((TextStat*)gbs)->text="Active Cycle Count = ";
  statList->AddTail(gbs);
  stat->setGUIID(gbs);
  break;
  
 case _HAZARDSTALL_:
  gbs=new TextStat();
  gbs->memo.Format("Number of hazard stalls for %s", name);
  ((TextStat*)gbs)->text="Hazard Stall Count = ";
  statList->AddTail(gbs);
  stat->setGUIID(gbs);
  break;
  
 case _TRACKSTAT_:
  gbs=new ScrollingStat();
  statList->AddTail(gbs);
  stat->setGUIID(gbs);
  break;
  
 case _TRACKNUMTIMES_:
  gbs=new ZeroOneScrollingStat();
  gbs->memo.Format("Activation record for %s", name);
  statList->AddTail(gbs);
  stat->setGUIID(gbs);
  break;
  
 case _ALLSTAT_:
  break;
  
 case _NOSTAT_:
  break;
  
 default:
  ASSERT(0);
  break;
 }
 
}


__declspec( dllimport ) BaseStat* getStatsIter(BaseSimObject* obj, bool bInit = false);
__declspec( dllimport ) BaseStat* getTrackStatsIter(BaseSimObject* obj, bool bInit = false);

/*
void DefaultStatProp::GetDataFromObject()
{
 SIMGraphObj* graphObj=(SIMGraphObj*)attachedObj;

 if (graphObj==NULL)
 {
  ClearStats();
  return;
 }

 //first find the obj if not found yet
 if (graphObj->simObj==NULL)
 {
  graphObj->simObj=getComp((char*)(LPCSTR)(graphObj->name));
 }

 ASSERT(graphObj->simObj!=NULL);

 //now get all the stats that belong to the obj
	//BaseStat *getStat(BaseSimObject *key, StatType st)
	//StatsRow statMap=globalArchStatsDB.getStatList(graphObj->simObj);
	//BaseStat* stat=globalArchStatsDB.getStat(graphObj->simObj, _NUMTIMES_);
 //globalArchStatsDB.printUnitStats(cout);

 ClearStats();
 
 BaseStat* stat=getStatsIter(graphObj->simObj,true);
 while (stat!=NULL)
 {
  GUIBaseStat* gbs;
  switch(stat->getType())
  {
  case _NUMTIMES_:
   gbs=new TextStat();
   gbs->memo.Format("Number of times %s was active", graphObj->name);
   ((TextStat*)gbs)->text="Active Count = ";
   AddStat(gbs);
   stat->setGUIID(gbs);
   break;

  case _HAZARDSTALL_:
   gbs=new TextStat();
   gbs->memo.Format("Number of hazard stalls for %s", graphObj->name);
   ((TextStat*)gbs)->text="Hazard Stall Count = ";
   AddStat(gbs);
   stat->setGUIID(gbs);
   break;

  case _ALLSTAT_:
   break;

  case _NOSTAT_:
   break;

  default:
   ASSERT(0);
   break;
  }
  stat=getStatsIter(graphObj->simObj);
 }

 stat=getTrackStatsIter(graphObj->simObj,true);
 while (stat!=NULL)
 {
  GUIBaseStat* gbs;
  switch(stat->getType())
  {
  case _TRACKSTAT_:
   gbs=new ScrollingStat();
   AddStat(gbs);
   stat->setGUIID(gbs);
   break;

  case _TRACKNUMTIMES_:
   gbs=new ZeroOneScrollingStat();
   gbs->memo.Format("Activation record for %s", graphObj->name);
   AddStat(gbs);
   stat->setGUIID(gbs);
   break;

  default:
   ASSERT(0);
   break;
  }
  stat=getTrackStatsIter(graphObj->simObj);
 }

}
*/

void DefaultStatProp::GetDataFromObject()
{
 SIMGraphObj* graphObj=(SIMGraphObj*)attachedObj;
 
 if (graphObj==NULL) //display simulator wide stats
 {
  __declspec (dllimport) vector<BaseSimObject *> globalStatsObjects;
  vector<BaseSimObject*>::iterator i;
  
  GUIBaseStatList* simStatList=NULL;
  simStatList=&(theApp.simDoc->simStatList);
  ASSERT(simStatList!=NULL);
  
  if (simStatList->IsEmpty())
  {
   for (i = globalStatsObjects.begin(); i != globalStatsObjects.end(); i++)
   {
    BaseSimObject* simObj=*i;
    //ProcessSimStat("simulator", &simStatList, stat);
    BaseStat* stat=getStatsIter(simObj,true);
    while (stat!=NULL)
    {
     //POSITION pos;
     CString str("simulator");
     ProcessSimStat(str, simStatList, stat);
     stat=getStatsIter(simObj);
    }
   }
  }
  
  ClearStats();
  //now copy the stat list from graphObj here
  POSITION pos=simStatList->GetHeadPosition();
  while (pos!=NULL)
  {
   AddStat(simStatList->GetNext(pos));
  }
  return;
 }
 
 //first find the obj if not found yet and fill out its list of stats
 if (graphObj->simObj==NULL)
 {
  graphObj->simObj=getComp((char*)(LPCSTR)(graphObj->name));
  ASSERT(graphObj->simObj!=NULL);

  //fill in the stats
  
  BaseStat* stat=getStatsIter(graphObj->simObj,true);
  while (stat!=NULL)
  {
   ProcessSimStat(graphObj->name, &graphObj->statList, stat);
   stat=getStatsIter(graphObj->simObj);
  }
  
  //__declspec( dllimport ) BaseStat* getTrackStatsIter(BaseSimObject* obj, bool bInit = false);
  
  stat=getTrackStatsIter(graphObj->simObj,true);
  while (stat!=NULL)
  {
   ProcessSimStat(graphObj->name, &graphObj->statList, stat);
   stat=getTrackStatsIter(graphObj->simObj);
  }
 }

 ClearStats();
 //now copy the stat list from graphObj here
 POSITION pos=graphObj->statList.GetHeadPosition();
 while (pos!=NULL)
 {
  AddStat(graphObj->statList.GetNext(pos));
 }
}

void DefaultStatProp::OnAddStat() 
{
 //BaseSimObject* comp=getComp(str);


 AddStatDlg dlg(this);
 if (dlg.DoModal()==IDOK)
 {
  //add the stat
  AddStat(dlg.newStat);
 }
}

void DefaultStatProp::OnRemoveStat() 
{
	// TODO: Add your control notification handler code here
	
}

void DefaultStatProp::ClearStats()
{
 //for (int i=0;i<m_Stats.GetCount();i++)
 //{
 // delete (GUIBaseStat*)(m_Stats.GetItemData(i));
 //}
 m_Stats.ResetContent();
}
