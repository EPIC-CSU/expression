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
// UnitStatPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "..\pcprogui.h"
#include "UnitStatPropPage.h"

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

/////////////////////////////////////////////////////////////////////////////
// UnitStatPropPage dialog


UnitStatPropPage::UnitStatPropPage(CWnd* pParent /*=NULL*/)
	: BasePropPage(UnitStatPropPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(UnitStatPropPage)
	m_Memo = _T("");
	//}}AFX_DATA_INIT
}


void UnitStatPropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UnitStatPropPage)
	DDX_Control(pDX, IDC_STATS, m_Stats);
	DDX_Text(pDX, IDC_SELECTION, m_Memo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UnitStatPropPage, CDialog)
	//{{AFX_MSG_MAP(UnitStatPropPage)
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UnitStatPropPage message handlers

void UnitStatPropPage::GetDataFromObject()
{
 SIMGraphObj* graphObj=(SIMGraphObj*)attachedObj;

 //first find the obj if not found yet
 if (graphObj->simObj==NULL)
 {
  graphObj->simObj=getComp((char*)(LPCSTR)(graphObj->name));
 }

 //now get all the stats that belong to the obj
	//BaseStat *getStat(BaseSimObject *key, StatType st)
	//StatsRow statMap=globalArchStatsDB.getStatList(graphObj->simObj);
	//BaseStat* stat=globalArchStatsDB.getStat(graphObj->simObj, _NUMTIMES_);
 //globalArchStatsDB.printUnitStats(cout);

/*
 __declspec( dllimport ) StatsRow& getStatsRow(BaseSimObject* obj);
 StatsRow& statMap=getStatsRow(graphObj->simObj);

 StatsRow::iterator i;
 for (i = statMap.begin(); i != statMap.end(); i++)
 {
  GUIBaseStat* gbs;
  BaseStat* stat=(*i).second;
  switch((*i).first)
  {
  case _NUMTIMES_:
   gbs=new PercentageStat();
   break;

  case _HAZARDSTALL_:
   gbs=new PercentageStat();
   break;

  case _TRACKSTAT_:
   gbs=new ScrollingStat();
   break;

  case _TRACKNUMTIMES_:
   gbs=new ScrollingStat();
   break;

  case _ALLSTAT_:
   break;

  case _NOSTAT_:
   break;

  default:
   ASSERT(0);
   break;
  }

  AddStat(gbs);
  stat->setGUIID(gbs);
 }
 */
}

void UnitStatPropPage::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	MovePage(this);
}

void UnitStatPropPage::AddStat(GUIBaseStat * stat)
{
 int i=m_Stats.AddString("NOS");
 ASSERT(i>=0);
 m_Stats.SetItemData(i,(DWORD)stat);
 m_Stats.SetItemHeight(i,stat->height);
}
