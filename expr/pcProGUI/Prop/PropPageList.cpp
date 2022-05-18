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
// PropPageList.cpp: implementation of the PropPageList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../pcprogui.h"
#include "PropPageList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PropPageList::PropPageList()
{

}

PropPageList::~PropPageList()
{
 POSITION pos=propPageMap.GetStartPosition();
 while (pos!=NULL)
 {
  BasePropPageCombo* pageCombo;
  UINT key;
  propPageMap.GetNextAssoc(pos,key,pageCombo);  
  delete pageCombo;
 }
}

void PropPageList::AddPage(UINT pageID, BasePropPage* page)
{
 CRect rect;
 BasePropPageCombo* refPage;

 //first see if we have a page already
 if (propPageMap.Lookup(pageID, refPage)!=0) 
 {
  delete page;
  refPage->count++;
  return;
 }

 refPage=NULL;
 if (propPageMap.GetCount()>0)
 {
  UINT key;
  POSITION pos=propPageMap.GetStartPosition();
  propPageMap.GetNextAssoc(pos,key,refPage);  
  refPage->page->GetWindowRect(rect);
 }

 BasePropPageCombo* newPage= new BasePropPageCombo(page);
 propPageMap.SetAt(pageID, newPage);
 if (refPage!=NULL) //move the page at the same position as the previous pages
 {
  newPage->page->MoveWindow(rect);
 }
}

void PropPageList::RemovePage(UINT pageID)
{
 BasePropPageCombo* page;

 VERIFY(propPageMap.Lookup(pageID, page));

 if (page->count==1) //realy removing
 {
  propPageMap.RemoveKey(pageID);
  delete page;
 }
 else
 {
  page->count--;
 }
}

void PropPageList::ShowPage(UINT pageID, void* selectedObj)
{
 //first clean up the previous page
 POSITION pos=propPageMap.GetStartPosition();
 while (pos!=NULL)
 {
  BasePropPageCombo* pageCombo;
  UINT key;
  propPageMap.GetNextAssoc(pos,key,pageCombo);  
  if (key==pageID)
  {
   pageCombo->page->MyShowWindow(SW_SHOWNOACTIVATE);
   activePage=pageCombo->page;
   activePage->attachedObj=selectedObj;
  }
  else
  {
   pageCombo->page->MyShowWindow(SW_HIDE);
  }
 }
}


BasePropPage* PropPageList::GetPage(UINT pageID)
{
 BasePropPageCombo* pageCombo;

 if (!propPageMap.Lookup(pageID, pageCombo))
  return NULL;

 return pageCombo->page;
}

void PropPageList::UpdatePagePositions(BasePropPage* page)
{
 CRect rect;
 page->GetWindowRect(rect);
 POSITION pos=propPageMap.GetStartPosition();
 while (pos!=NULL)
 {
  BasePropPageCombo* pageCombo;
  UINT key;
  propPageMap.GetNextAssoc(pos,key,pageCombo);  
  if (key!=page->pageID)
  {
   pageCombo->page->MoveWindow(rect);
  }
 }
}

