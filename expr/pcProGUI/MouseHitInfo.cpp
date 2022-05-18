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
// MouseHitInfo.cpp: implementation of the MouseHitInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcprogui.h"
#include "MouseHitInfo.h"

#include "../../pcProDll/include/GenericNode.h"

#include "../../pcProDll/include/BasicBlockNode.h"
#include "SIM/SIMGraphObj.h"

#include "Prop/CompoundUnitPropPage.h"
#include "Prop/UnitPropPage.h"
#include "Prop/LatchPropPage.h"
#include "Prop/StoragePropPage.h"
#include "Prop/ConnectionPropPage.h"
#include "Prop/BusPropPage.h"
#include "Prop/PortPropPage.h"
#include "Prop/PipelineStagePropPage.h"

#include "Stat/DefaultStatProp.h"
#include "Stat/CompoundUnitStatPropPage.h"
#include "Stat/UnitStatPropPage.h"
#include "Stat/LatchStatPropPage.h"
#include "Stat/StorageStatPropPage.h"
#include "Stat/ConnectionStatPropPage.h"
#include "Stat/PortStatPropPage.h"

#include "Prop/PropPageList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MouseHitInfo::MouseHitInfo(MHI_HIT_TYPE _type, void* _node)
{
 hitType=_type;
 node=_node;
}

MouseHitInfo::~MouseHitInfo()
{
 
}

void MouseHitInfo::ProcessLeftClick(GRAPH_NODE_TYPE nodeType, GenericNode** prevSelection, bool simulatorMode)
{ 
 switch (hitType)
 {
 case MHI_INSTR:
 case MHI_BASICBLOCK:
 case MHI_SIMPLE_HTG_NODE:
 case MHI_IF_HTG_NODE:
 case MHI_LOOP_HTG_NODE:
 case MHI_COMPOUND_HTG_NODE:
  if (*prevSelection!=NULL) //unselect the previous one
  {
   (*prevSelection)->ToggleSelected(nodeType); 
  }
  ((GenericNode*)node)->ToggleSelected(nodeType);
  *prevSelection=(GenericNode*)node;
  switch (hitType)
  {
  case MHI_INSTR:
	  theApp.propPageList->ShowPage(IDD_PROP_INSTR, (SIMGraphObj*)*prevSelection);
   break;

  case MHI_BASICBLOCK:
	  theApp.propPageList->ShowPage(IDD_PROP_BBLOCK, (SIMGraphObj*)*prevSelection);
   break;

  case MHI_SIMPLE_HTG_NODE:
	  theApp.propPageList->ShowPage(IDD_PROP_HTG_SIMPLE, (SIMGraphObj*)*prevSelection);
   break;

  case MHI_IF_HTG_NODE:
	  theApp.propPageList->ShowPage(IDD_PROP_HTG_IF, (SIMGraphObj*)*prevSelection);
   break;

  case MHI_LOOP_HTG_NODE:
	  theApp.propPageList->ShowPage(IDD_PROP_HTG_LOOP, (SIMGraphObj*)*prevSelection);
   break;

  case MHI_COMPOUND_HTG_NODE:
	  theApp.propPageList->ShowPage(IDD_PROP_HTG_COMPOUND, (SIMGraphObj*)*prevSelection);
   break;
  }
 	((GenericNode*)node)->DisplayProperties(theApp.propPageList->activePage);
  break;

 case MHI_SIM_COMPOUND_UNIT:
 case MHI_SIM_UNIT:
 case MHI_SIM_LATCH:
 case MHI_SIM_PORT:
 case MHI_SIM_CONNECTION:
 case MHI_SIM_BUS:
 case MHI_SIM_STORAGE:
 case MHI_SIM_PIPELINESTAGE:
  if ((SIMGraphObj*)(*prevSelection)!=NULL) //unselect the previous one
  {
   ((SIMGraphObj*)(*prevSelection))->ToggleSelected(); 
  }
  ((SIMGraphObj*)node)->ToggleSelected();
  *prevSelection=(GenericNode*)node;
  switch (hitType)
  {   
  case MHI_SIM_COMPOUND_UNIT:
	  if (simulatorMode)
   {
    //theApp.propPageList->ShowPage(IDD_STAT_COMP_UNIT, (SIMGraphObj*)*prevSelection);
    theApp.propPageList->ShowPage(IDD_STAT_SIM, (SIMGraphObj*)*prevSelection);
    ((DefaultStatProp*)theApp.propPageList->activePage)->GetDataFromObject();    
    //((CompoundUnitStatPropPage*)theApp.propPageList->activePage)->GetDataFromObject();
   }
   else
   {
    theApp.propPageList->ShowPage(IDD_PROP_COMP_UNIT, (SIMGraphObj*)*prevSelection);
    ((CompoundUnitPropPage*)theApp.propPageList->activePage)->GetDataFromObject();
   }
   break;

  case MHI_SIM_UNIT:
	  if (simulatorMode)
   {
    //theApp.propPageList->ShowPage(IDD_STAT_UNIT, (SIMGraphObj*)*prevSelection);
    theApp.propPageList->ShowPage(IDD_STAT_SIM, (SIMGraphObj*)*prevSelection);
    ((DefaultStatProp*)theApp.propPageList->activePage)->GetDataFromObject();    
    //((UnitStatPropPage*)theApp.propPageList->activePage)->GetDataFromObject();
   }
   else
   {
    theApp.propPageList->ShowPage(IDD_PROP_UNIT, (SIMGraphObj*)*prevSelection);
    ((UnitPropPage*)theApp.propPageList->activePage)->GetDataFromObject();
   }
   break;
   
  case MHI_SIM_LATCH:
	  if (simulatorMode)
   {
    //theApp.propPageList->ShowPage(IDD_STAT_LATCH, (SIMGraphObj*)*prevSelection);
    theApp.propPageList->ShowPage(IDD_STAT_SIM, (SIMGraphObj*)*prevSelection);
    ((DefaultStatProp*)theApp.propPageList->activePage)->GetDataFromObject();    
    //((LatchStatPropPage*)theApp.propPageList->activePage)->GetDataFromObject();
   }
   else
   {
    theApp.propPageList->ShowPage(IDD_PROP_LATCH, (SIMGraphObj*)*prevSelection);
    ((LatchPropPage*)theApp.propPageList->activePage)->GetDataFromObject();
   }
   break;
   
  case MHI_SIM_PORT:
	  if (simulatorMode)
   {
    //theApp.propPageList->ShowPage(IDD_STAT_PORT, (SIMGraphObj*)*prevSelection);
    theApp.propPageList->ShowPage(IDD_STAT_SIM, (SIMGraphObj*)*prevSelection);
    ((DefaultStatProp*)theApp.propPageList->activePage)->GetDataFromObject();    
    //((PortStatPropPage*)theApp.propPageList->activePage)->GetDataFromObject();
   }
   else
   {
    theApp.propPageList->ShowPage(IDD_PROP_PORT, (SIMGraphObj*)*prevSelection);
    ((PortPropPage*)theApp.propPageList->activePage)->GetDataFromObject();
   }
   break;
   
  case MHI_SIM_CONNECTION:
	  if (simulatorMode)
   {
    //theApp.propPageList->ShowPage(IDD_STAT_CONN, (SIMGraphObj*)*prevSelection);
    theApp.propPageList->ShowPage(IDD_STAT_SIM, (SIMGraphObj*)*prevSelection);
    ((DefaultStatProp*)theApp.propPageList->activePage)->GetDataFromObject();    
    //((ConnectionStatPropPage*)theApp.propPageList->activePage)->GetDataFromObject();
   }
   else
   {
    theApp.propPageList->ShowPage(IDD_PROP_CONN, (SIMGraphObj*)*prevSelection);
    ((ConnectionPropPage*)theApp.propPageList->activePage)->GetDataFromObject();
   }
   break;
   
  case MHI_SIM_BUS:
	  if (simulatorMode)
   {
    //theApp.propPageList->ShowPage(IDD_STAT_CONN, (SIMGraphObj*)*prevSelection);
    theApp.propPageList->ShowPage(IDD_STAT_SIM, (SIMGraphObj*)*prevSelection);
    ((DefaultStatProp*)theApp.propPageList->activePage)->GetDataFromObject();    
    //((ConnectionStatPropPage*)theApp.propPageList->activePage)->GetDataFromObject();
   }
   else
   {
    theApp.propPageList->ShowPage(IDD_PROP_BUS, (SIMGraphObj*)*prevSelection);
    ((BusPropPage*)theApp.propPageList->activePage)->GetDataFromObject();
   }
   break;
   
  case MHI_SIM_STORAGE:
	  if (simulatorMode)
   {
    //theApp.propPageList->ShowPage(IDD_STAT_STOR, (SIMGraphObj*)*prevSelection);
    theApp.propPageList->ShowPage(IDD_STAT_SIM, (SIMGraphObj*)*prevSelection);
    ((DefaultStatProp*)theApp.propPageList->activePage)->GetDataFromObject();    
    //((StorageStatPropPage*)theApp.propPageList->activePage)->GetDataFromObject();
   }
   else
   {
    theApp.propPageList->ShowPage(IDD_PROP_STOR, (SIMGraphObj*)*prevSelection);
    ((StoragePropPage*)theApp.propPageList->activePage)->GetDataFromObject();
   }
   break;
   
  case MHI_SIM_PIPELINESTAGE:
	  if (simulatorMode)
   {
    theApp.propPageList->ShowPage(IDD_STAT_SIM, (SIMGraphObj*)*prevSelection);
    ((DefaultStatProp*)theApp.propPageList->activePage)->GetDataFromObject();    
    //((PipelineStagePropPage*)theApp.propPageList->activePage)->GetDataFromObject();
   }
   else
   {
    theApp.propPageList->ShowPage(IDD_PROP_PIPELINESTAGE, (SIMGraphObj*)*prevSelection);
    ((PipelineStagePropPage*)theApp.propPageList->activePage)->GetDataFromObject();
   }
   break;
  }
  //((SIMGraphObj*)(*prevSelection))->DisplayProperties();
  break;
  
 default:
  ASSERT_TRACE(0,"Ouch! Unknown object was hit!");
  break;
 }
}


void MouseHitInfo::ProcessRightClick(GRAPH_NODE_TYPE nodeType, CPoint& point, CWnd* wnd)
{ 
 CMenu* pPopup;
 UINT trackingFlags = TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON;
 
 switch (nodeType)
 {
 case GNT_ILG:
  VERIFY(menu.LoadMenu(IDR_MENU_ILG));
  break;
  
 case GNT_CFG:
  VERIFY(menu.LoadMenu(IDR_MENU_CFG));
  break;
  
 case GNT_HTG:
  VERIFY(menu.LoadMenu(IDR_MENU_HTG));
  break;
  
 case GNT_SIM:
  VERIFY(menu.LoadMenu(IDR_MENU_SIM));
  break;
  
 default:
  ASSERT(0);
  break;
 }
 
 switch (hitType)
 {
 case MHI_INSTR:
  pPopup = menu.GetSubMenu(0);
  ASSERT(pPopup != NULL);
  VERIFY(pPopup->TrackPopupMenu(trackingFlags, point.x, point.y, wnd));
  break;
  
 case MHI_BASICBLOCK:
  pPopup = menu.GetSubMenu(1);
  ASSERT(pPopup != NULL);
  VERIFY(pPopup->TrackPopupMenu(trackingFlags, point.x, point.y, wnd));
  break;
  
 case MHI_SIMPLE_HTG_NODE:
  pPopup = menu.GetSubMenu(2);
  ASSERT(pPopup != NULL);
  VERIFY(pPopup->TrackPopupMenu(trackingFlags, point.x, point.y, wnd));
  break;
  
 case MHI_IF_HTG_NODE:
  pPopup = menu.GetSubMenu(3);
  ASSERT(pPopup != NULL);
  VERIFY(pPopup->TrackPopupMenu(trackingFlags, point.x, point.y, wnd));
  break;
  
 case MHI_LOOP_HTG_NODE:
  pPopup = menu.GetSubMenu(4);
  ASSERT(pPopup != NULL);
  VERIFY(pPopup->TrackPopupMenu(trackingFlags, point.x, point.y, wnd));
  break;
  
 case MHI_COMPOUND_HTG_NODE:
  pPopup = menu.GetSubMenu(5);
  ASSERT(pPopup != NULL);
  VERIFY(pPopup->TrackPopupMenu(trackingFlags, point.x, point.y, wnd));
  break;
  
 case MHI_SIM_COMPOUND_UNIT:
  pPopup = menu.GetSubMenu(0);
  ASSERT(pPopup != NULL);
  VERIFY(pPopup->TrackPopupMenu(trackingFlags, point.x, point.y, wnd));
  break;

 case MHI_SIM_UNIT:
  pPopup = menu.GetSubMenu(1);
  ASSERT(pPopup != NULL);
  VERIFY(pPopup->TrackPopupMenu(trackingFlags, point.x, point.y, wnd));
  break;

 case MHI_SIM_LATCH:
  pPopup = menu.GetSubMenu(2);
  ASSERT(pPopup != NULL);
  VERIFY(pPopup->TrackPopupMenu(trackingFlags, point.x, point.y, wnd));
  break;

 case MHI_SIM_PORT:
  pPopup = menu.GetSubMenu(3);
  ASSERT(pPopup != NULL);
  VERIFY(pPopup->TrackPopupMenu(trackingFlags, point.x, point.y, wnd));
  break;

 case MHI_SIM_CONNECTION:
  pPopup = menu.GetSubMenu(4);
  ASSERT(pPopup != NULL);
  VERIFY(pPopup->TrackPopupMenu(trackingFlags, point.x, point.y, wnd));
  break;

 case MHI_SIM_BUS:
  pPopup = menu.GetSubMenu(5);
  ASSERT(pPopup != NULL);
  VERIFY(pPopup->TrackPopupMenu(trackingFlags, point.x, point.y, wnd));
  break;

 case MHI_SIM_STORAGE:
  pPopup = menu.GetSubMenu(6);
  ASSERT(pPopup != NULL);
  VERIFY(pPopup->TrackPopupMenu(trackingFlags, point.x, point.y, wnd));
  break;
  
 default:
  ASSERT_TRACE(0,"Ouch! Unknown object was hit!");
  break;
 }
 
 VERIFY(menu.DestroyMenu());
}
