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
// TreeThruList.cpp: implementation of the TreeThruList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\pcprogui.h"
#include "TreeThruList.h"

#include "OpGroups.h"
#include "ISInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TreeThruList::TreeThruList()
{

}

TreeThruList::~TreeThruList()
{

}

void TreeThruList::OnDrawItem(OG* og, int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
 SIZE size;
 RECT r;
 memcpy(&r, &lpDrawItemStruct->rcItem, sizeof(RECT));
 r.left+=og->level*15;
 VERIFY(GetTextExtentPoint32(lpDrawItemStruct->hDC, og->text, og->text.GetLength(), &size));
 r.right=r.left+size.cx+3;
 
 int saveID=SaveDC(lpDrawItemStruct->hDC);
 ASSERT(saveID);
 
 CBrush eraseBrush;
 CPen erasePen;
 HGDIOBJ oldBrush;
 HGDIOBJ oldPen;
 
 //erase the background
 erasePen.CreateStockObject(NULL_PEN);
 eraseBrush.CreateSolidBrush(GetSysColor(COLOR_WINDOW));
 oldBrush = SelectObject(lpDrawItemStruct->hDC,eraseBrush);
 oldPen = SelectObject(lpDrawItemStruct->hDC,erasePen);
 Rectangle(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom);
 SelectObject(lpDrawItemStruct->hDC,oldPen);
 SelectObject(lpDrawItemStruct->hDC,oldBrush);
 
 CBrush newBrush;
 CPen newPen;
 //if (lpDrawItemStruct->itemState & ODS_FOCUS) //draw a focus rectangle
 //{
 // newPen.CreatePen(PS_DOT, 1, GetSysColor(COLOR_GRAYTEXT));
 //}
 //else
 //{
 newPen.CreateStockObject(NULL_PEN);
 //}
 
 oldPen = SelectObject(lpDrawItemStruct->hDC,newPen);
 
 if (lpDrawItemStruct->itemState & ODS_SELECTED) //use the system selected color
 {
  newBrush.CreateSolidBrush(lpDrawItemStruct->itemState & ODS_DISABLED ? GetSysColor(COLOR_BTNFACE) : GetSysColor(COLOR_HIGHLIGHT));
 }
 else //no brush
 {
  newBrush.CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
 }
 
 oldBrush = SelectObject(lpDrawItemStruct->hDC,newBrush);
 //Rectangle(lpDrawItemStruct->hDC, 
 //           lpDrawItemStruct->rcItem.left, 
 //           lpDrawItemStruct->rcItem.top, 
 //           lpDrawItemStruct->rcItem.right, 
 //           lpDrawItemStruct->rcItem.bottom);
 Rectangle(lpDrawItemStruct->hDC, r.left, r.top, r.right, r.bottom);
 
 SelectObject(lpDrawItemStruct->hDC,oldPen);
 SelectObject(lpDrawItemStruct->hDC,oldBrush);
 
 if (lpDrawItemStruct->itemState & ODS_DISABLED) //use the system selected color
 {
  SetTextColor(lpDrawItemStruct->hDC, GetSysColor(COLOR_GRAYTEXT));
 }
 else
 {
  SetTextColor(lpDrawItemStruct->hDC, lpDrawItemStruct->itemState & ODS_SELECTED ? GetSysColor(COLOR_HIGHLIGHTTEXT) : GetSysColor(COLOR_WINDOWTEXT));
 }
 
 //now let the stat item draw itself
 //og->OnDraw(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem);
 og->OnDraw(lpDrawItemStruct->hDC, &r);
 RestoreDC(lpDrawItemStruct->hDC,saveID);  
}


void TreeThruList::FillGroupsList(ISInfo* ISInfo, CListBox& listBox)
{
 bool skipping=false;
 int skipLevel=10000; //if more than 10000 group levels then shoot the programmer
 int newItem;
 OGList& ogl=ISInfo->opGroups.opGroupItemList;

 listBox.ResetContent();

 for(POSITION i=ogl.GetHeadPosition();i!=NULL;ogl.GetNext(i))
 {
  OG& item=ogl.GetAt(i);

  if (0)
  {
   newItem=listBox.AddString(item.text);
   if (item.isSelected) 
   {
    listBox.SetSel(newItem);
    item.isSelected=false;
   }
   listBox.SetItemData(newItem, (DWORD)&item);
   continue;
  }

  if (skipping)
  {
   if (item.level<=skipLevel)
   {
    //listBox.SetItemData(listBox.AddString(item.text), (DWORD)&item);
    newItem=listBox.AddString(item.text);
    if (item.isSelected) 
    {
     listBox.SetSel(newItem);
     item.isSelected=false;
    }
    listBox.SetItemData(newItem, (DWORD)&item);

    skipping=!item.isExpanded;
    if ((item.isGroup()) && (!item.isExpanded))
    {
     skipLevel=item.level;
    }
   }
  }
  else
  {
   if (!item.isExpanded)
   {
    skipLevel=item.level;
    skipping=true;
   }

   //listBox.SetItemData(listBox.AddString(item.text), (DWORD)&item);
   newItem=listBox.AddString(item.text);
   if (item.isSelected) 
   {
    listBox.SetSel(newItem);
    item.isSelected=false;
   }
   listBox.SetItemData(newItem, (DWORD)&item);
  }
 }  
}
