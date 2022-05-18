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
// pcProGUIView.cpp : implementation of the pcProGUIView class
//

#include "stdafx.h"
#include "pcProGUI.h"

#include <process.h>

#include "pcProGUIDoc.h"
#include "pcProGUIView.h"

#include "CFG/CFGView.h"
#include "CFG/CFGDoc.h"

#include "ILG/ILGView.h"
#include "ILG/ILGDoc.h"

#include "HTG/HTGView.h"
#include "HTG/HTGDoc.h"

#include "SIM/SIMView.h"
#include "SIM/SIMDoc.h"

#include "Prop/PropPageList.h"
#include "Prop/FilePropPage.h"
#include "Prop/RoutinePropPage.h"

#include "ProgramParams.h"

#include "routine.h"
#include "program.h"

#include <cderr.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//TEMP
__declspec (dllimport) bool volatile lockPCPRO;
__declspec (dllimport) bool volatile stepPCPRO;
//TEMP

//NOS
//int pcProGUIMain(char*, my_ostream*);
//unsigned int __stdcall pcProGUIMain(void* _pp);
unsigned int __stdcall pcProMain(void* _pp);
void UpdateConsoleCOUT(char* str, int streamcount);
void UpdateConsoleCERR(char* str, int streamcount);

__declspec( dllimport ) THREADINFO threadInfo;

__declspec (dllimport) void setCurrRoutine(Routine* routine);
__declspec (dllimport) void deadCodeElimination();
__declspec (dllimport) void Routine::buildCFG();
__declspec (dllimport) void Routine::printCFG();
__declspec (dllimport) void Routine::buildHTG();
__declspec (dllimport) void Routine::buildSSA();
__declspec (dllimport) void buildProgramMappings();
__declspec (dllimport) void Routine::deadLabelElimination();
__declspec (dllexport) void Routine::printHTG(void);
__declspec (dllimport) bool ICGenUDDU;
__declspec (dllimport) void Routine::fixIfHead(void);

/////////////////////////////////////////////////////////////////////////////
// pcProGUIView

IMPLEMENT_DYNCREATE(pcProGUIView, CTreeView)

BEGIN_MESSAGE_MAP(pcProGUIView, CTreeView)
//{{AFX_MSG_MAP(pcProGUIView)
ON_COMMAND(ID_PROJECT_ADD, OnProjectAdd)
ON_COMMAND(ID_PROJECT_REMOVE, OnProjectRemove)
ON_COMMAND(ID_PROJECT_BUILD, OnProjectBuild)
ON_COMMAND(ID_PROJECT_COMPILE, OnProjectCompile)
ON_COMMAND(ID_GRAPHS_CFG, OnGraphCFG)
ON_COMMAND(ID_GRAPHS_ILG, OnGraphILG)
ON_COMMAND(ID_GRAPHS_HTG, OnGraphHTG)
ON_COMMAND(ID_SIM_NEW, OnNewSIM)
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
	ON_COMMAND(ID_COMPILE_STEPCOMPILE_BUILDHTG, OnCompileStepcompileBuildHtg)
	ON_COMMAND(ID_COMPILE_STEPCOMPILE_SSA, OnCompileStepcompileSSA)
	ON_COMMAND(ID_COMPILE_STEPCOMPILE_TRAILBLAZING, OnCompileStepcompileTrailblazing)
	ON_COMMAND(ID_COMPILE_STEPCOMPILE_UDDUANALYSIS, OnCompileStepcompileUDLIVE)
	ON_COMMAND(ID_COMPILE_STEPCOMPILE_INITIALIZECOMPILER, OnCompileStepcompileInitializecompiler)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
	ON_COMMAND(ID_IFCONVERT, OnIfConvert)
	ON_COMMAND(ID_COMPILE_STEPCOMPILE_PRINTROUTINE, OnPrintRoutine)
	ON_COMMAND(ID_PERCOLATE, OnPercolate)
	ON_COMMAND(ID_MARK_BLOCKS, OnMarkBlocks)
	//}}AFX_MSG_MAP
// Standard printing commands
ON_COMMAND(ID_FILE_PRINT, CTreeView::OnFilePrint)
ON_COMMAND(ID_FILE_PRINT_DIRECT, CTreeView::OnFilePrint)
ON_COMMAND(ID_FILE_PRINT_PREVIEW, CTreeView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// pcProGUIView construction/destruction


bool isKeyPressed(int vKey)
{
 SHORT res=GetAsyncKeyState(vKey);
 return (res & 0x8000)!=0;
}


void NotifyDocChange()
{
 POSITION dtPos=theApp.GetFirstDocTemplatePosition();
 while (dtPos!=NULL)
 {
  CDocTemplate* dt=theApp.GetNextDocTemplate(dtPos);
  
  //see if it's a template we care about
  CString text;
  BOOL retVal=dt->GetDocString(text, CDocTemplate::docName);
  
  if (text!="pcPro") //not the project document so notify doc change
  {
   POSITION docPos=dt->GetFirstDocPosition();
   while (docPos!=NULL)
   {
    CDocument* doc=dt->GetNextDoc(docPos);

    //try all types of docs: CFG, ILG, HTG
    CFGDoc* cfgDoc=dynamic_cast<CFGDoc*>(doc);
    if (cfgDoc!=NULL)
    {
     //set the doc to dirty
     cfgDoc->SetModifiedFlag();
     //reset the view selections too
     POSITION pos = cfgDoc->GetFirstViewPosition();
     while (pos != NULL)
     {
      CFGView* pView = dynamic_cast<CFGView*>(cfgDoc->GetNextView(pos));
      ASSERT(pView!=NULL);
      if (pView->viewSelection!=NULL)
      {
       ((GenericNode*)pView->viewSelection)->ToggleSelected(GNT_CFG);
       pView->viewSelection=NULL;
      }
     }   
    }
    
    ILGDoc* ilgDoc=dynamic_cast<ILGDoc*>(doc);
    if (ilgDoc!=NULL)
    {
     //set the doc to dirty
     ilgDoc->SetModifiedFlag();
     //reset the view selections too
     POSITION pos = ilgDoc->GetFirstViewPosition();
     while (pos != NULL)
     {
      ILGView* pView = dynamic_cast<ILGView*>(ilgDoc->GetNextView(pos));
      ASSERT(pView!=NULL);
      if (pView->viewSelection!=NULL)
      {
       ((GenericNode*)pView->viewSelection)->ToggleSelected(GNT_ILG);
       pView->viewSelection=NULL;
      }
     }   
    }
    
    HTGDoc* htgDoc=dynamic_cast<HTGDoc*>(doc);
    if (htgDoc!=NULL)
    {
     //set the doc to dirty
     htgDoc->SetModifiedFlag();
     //reset the view selections too
     POSITION pos = htgDoc->GetFirstViewPosition();
     while (pos != NULL)
     {
      HTGView* pView = dynamic_cast<HTGView*>(htgDoc->GetNextView(pos));
      ASSERT(pView!=NULL);
      if (pView->viewSelection!=NULL)
      {
       ((GenericNode*)pView->viewSelection)->ToggleSelected(GNT_HTG);
       pView->viewSelection=NULL;
      }
     }   
    }

   }
  }
 }
}

pcProGUIView::pcProGUIView()
{
	viewSelection=NULL;

}

pcProGUIView::~pcProGUIView()
{
 //if (instrPropPage!=NULL) delete instrPropPage;
 theApp.propPageList->RemovePage(IDD_PROP_FILE);
 theApp.propPageList->RemovePage(IDD_PROP_ROUTINE);
}

BOOL pcProGUIView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
 
	return CTreeView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// pcProGUIView drawing

void pcProGUIView::OnDraw(CDC* pDC)
{
	pcProGUIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
 
	// TODO: add draw code for native data here
}

void pcProGUIView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();
 
	// TODO: You may populate your ListView with items by directly accessing
	//  its list control through a call to GetTreeCtrl().
 
 //setup the list control
 CTreeCtrl& tc=GetTreeCtrl();
 
 //setup view
 ::SetWindowLong(tc.m_hWnd,GWL_STYLE,::GetWindowLong(tc.m_hWnd,GWL_STYLE) | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT);
 
 //set the image list
	{
  HICON hIcon;
  
  // Create small icon image lists
  m_ImageList.Create(::GetSystemMetrics(SM_CXSMICON),::GetSystemMetrics(SM_CYSMICON), TRUE, 1, 1);
  
  //add the CFILE icon
  hIcon = AfxGetApp()->LoadIcon(IDI_CFILE); 
  m_ImageList.Add(hIcon); 
  ::DeleteObject(hIcon); 
  
  //add the CFILE_CHECK icon
  hIcon = AfxGetApp()->LoadIcon(IDI_CFILE_CHECK); 
  m_ImageList.Add(hIcon); 
  ::DeleteObject(hIcon); 
  
  //add the FUNC icon
  hIcon = AfxGetApp()->LoadIcon(IDI_FUNC); 
  m_ImageList.Add(hIcon); 
  ::DeleteObject(hIcon); 
  
  //add the FUNC_CHECK icon
  hIcon = AfxGetApp()->LoadIcon(IDI_FUNC_CHECK); 
  m_ImageList.Add(hIcon); 
  ::DeleteObject(hIcon); 
  
  CImageList* t=tc.SetImageList(&m_ImageList,TVSIL_NORMAL);
 }
 
	{
  ProjectFileList* fl = &(GetDocument()->m_Files);
  ProjectFile* pf;
  for(POSITION i=fl->GetHeadPosition();i!=NULL;fl->GetNext(i))
  {
   pf=fl->GetAt(i);
   HTREEITEM ti=tc.InsertItem(pf->name, 0, 1);
   tc.SetItemState(ti, pf->selected?TVIS_SELECTED:0, TVIF_STATE);
   tc.SetItemData(ti, (DWORD)pf);
  }  
  tc.SortChildren(NULL);
 }
 
 //CWinApp* app=AfxGetApp();
 
 //m_cout=new my_ostream(&UpdateConsole);
 //unsigned long hT=_beginthread( pcProGUIMain, 0, m_cout );
 
 //main( int argc, char *argv[ ], char *envp[ ] )
 
}

/////////////////////////////////////////////////////////////////////////////
// pcProGUIView printing

BOOL pcProGUIView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void pcProGUIView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CTreeView::OnBeginPrinting(pDC, pInfo);
}

void pcProGUIView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// pcProGUIView diagnostics

#ifdef _DEBUG
void pcProGUIView::AssertValid() const
{
	CTreeView::AssertValid();
}

void pcProGUIView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

pcProGUIDoc* pcProGUIView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(pcProGUIDoc)));
	return (pcProGUIDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// pcProGUIView message handlers


void pcProGUIView::OnProjectAdd() 
{
 char fileBuf[20000]; //big enough?!
 char* fileBufPtr=&fileBuf[0];
 CString file;
 static char BASED_CODE szFilter[] = "PROCS Files (*.procs)|*.procs||";
 
 CFileDialog fd(TRUE,".procs", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT, szFilter, this );
 
 fd.m_ofn.lpstrFile=fileBufPtr;
 fd.m_ofn.nMaxFile=sizeof(fileBuf)-1;
 strcpy(fileBuf, "*.procs");

 if (fd.DoModal()==IDOK)
 {
  char fileName[MAX_PATH];
  CTreeCtrl& tc=GetTreeCtrl();

  char* dot=strchr(fileBuf,'.');
  if (dot!=NULL) //single file
  {
   file=".\\";
   file+=fd.m_ofn.lpstrFileTitle;
   ProjectFileList* fl = &(GetDocument()->m_Files);
   ProjectFile* pf=new ProjectFile;
   pf->name=file;
   pf->selected=true;
   fl->AddTail(pf);
   HTREEITEM ti=tc.InsertItem(pf->name, 0, 1);
   tc.SetItemState(ti, pf->selected?TVIS_SELECTED:0, TVIF_STATE);
   tc.SetItemData(ti, (DWORD)pf);
  }
  else
  {
   //get the dir first and discard it
   strcpy(fileName,fileBufPtr);
   fileBufPtr+=strlen(fileName)+1;
   //now the first file name
   strcpy(fileName,fileBufPtr);
   fileBufPtr+=strlen(fileName)+1;
   do
   {
    file=".\\";
    file+=fileName;
    ProjectFileList* fl = &(GetDocument()->m_Files);
    ProjectFile* pf=new ProjectFile;
    pf->name=file;
    pf->selected=true;
    fl->AddTail(pf);
    HTREEITEM ti=tc.InsertItem(pf->name, 0, 1);
    tc.SetItemState(ti, pf->selected?TVIS_SELECTED:0, TVIF_STATE);
    tc.SetItemData(ti, (DWORD)pf);
    strcpy(fileName,fileBufPtr);
    fileBufPtr+=strlen(fileName)+1;
   }
   while (fileName[0]!='\0'); //have a file name
  }

  tc.SortChildren(NULL);
 }
 else
 {
  DWORD err=CommDlgExtendedError();
  if (err==FNERR_BUFFERTOOSMALL)
  {
   MessageBox("Too many files selected at one time.\nTry again with fewer files.",NULL,MB_ICONWARNING);
  }
  else
  {
   CString str;
   str.Format("Error: 0x%08X returned by DoModal(). Operation aborted.", err);
   MessageBox(str,NULL,MB_ICONWARNING);
  }
 }
}

void pcProGUIView::OnProjectRemove() 
{
	MessageBox("Not yet implemented.",NULL,MB_ICONINFORMATION);
}

void pcProGUIView::OnProjectBuild() 
{
	MessageBox("Not yet implemented.",NULL,MB_ICONINFORMATION);
}

ProgramParams pp(true); //true = we're allocating an argv[][]

void addRoutines(CTreeCtrl& tc, HTREEITEM ti)
{
 //remove the previous if any
 HTREEITEM nti;
 while ((nti=tc.GetChildItem(ti))!=NULL)
 {
  tc.DeleteItem(nti);
 }
 
 //__declspec( dllimport ) Program *curProgram;
 __declspec( dllexport ) Program* GetCurrentProgram();
 MyLinkedListIterator<Routine *> *routineIter=GetCurrentProgram()->routineListIteratorForw();
 while (routineIter->hasMoreElements())
 {
  CString name=routineIter->currentElement()->getName();
  name+="()";
  nti=tc.InsertItem(name, ti);
  tc.SetItemImage(nti, 2, 3);
  tc.SetItemData(nti, (DWORD)routineIter->currentElement());
  
  routineIter->nextElement();
 }
 delete routineIter;
 tc.SortChildren(ti);
 tc.RedrawWindow();
}

void pcProGUIView::OnProjectCompile() 
{
 CWaitCursor wait;

 CTreeCtrl& tc=GetTreeCtrl();
 
 HTREEITEM ti=tc.GetSelectedItem();
 
 if (ti==NULL)
 {
 	MessageBox("Please first select a file to compile.",NULL,MB_ICONINFORMATION);
 }
 else
 {
  CWaitCursor waitCursor;
  
  CreateCommandLineParams();
  
  lockPCPRO=true;
  stepPCPRO=false;
  
  //disable the toolbar button
  //m_wndToolBar
  unsigned thrdaddr;
  //threadInfo.handle=(HANDLE)_beginthreadex(NULL, 0, pcProGUIMain, &pp, CREATE_SUSPENDED, &thrdaddr);
  threadInfo.handle=(HANDLE)_beginthreadex(NULL, 0, pcProMain, &pp, CREATE_SUSPENDED, &thrdaddr);
  SetThreadPriority(threadInfo.handle,THREAD_PRIORITY_BELOW_NORMAL); //THREAD_PRIORITY_IDLE);
  ResumeThread(threadInfo.handle); 
  WaitForSingleObject(threadInfo.handle,INFINITE); 
  
  //after we've compiled add the routines to the tree item
  addRoutines(tc,ti);
 }
}


ProjectFile* pcProGUIView::GetSelectedFile() 
{
 CTreeCtrl& tc=GetTreeCtrl();
 
 HTREEITEM ti=tc.GetSelectedItem();

 ///HACK: a file name will have a '.' in it, a routine name not!
 if ((ti!=NULL) && (tc.GetItemText(ti).Find('.')!=-1))
 {
  return (ProjectFile*)tc.GetItemData(ti); 
 }

 return NULL;
}

void pcProGUIView::OnGraphCFG() 
{
 CTreeCtrl& tc=GetTreeCtrl();
 
 HTREEITEM ti=tc.GetSelectedItem();
 
 if (ti==NULL)
 {
  MessageBox("Please first select a routine.",NULL,MB_ICONINFORMATION);
  return;
 }
 
 //TODO : see if what is selected is a routine
 if (tc.GetItemText(ti).Find('.')>=0)
 {
  MessageBox("Please first select a routine.",NULL,MB_ICONINFORMATION);
  return;
 }

 Routine* routine=(Routine*)tc.GetItemData(ti);
 
 CWaitCursor waitCursor;
 CString windowTitle;
 
 cfgDoc = (CFGDoc*)theApp.m_CFG->CreateNewDocument();
 cfgDoc->m_routine=routine;
 
 CMDIChildWnd* cfgFrame	= static_cast<CMDIChildWnd*>(theApp.m_CFG->CreateNewFrame(cfgDoc,NULL));
 
 cfgFrame->InitialUpdateFrame(NULL,true);
 cfgView = static_cast<CFGView*>(cfgFrame->GetActiveView());
 cfgView->m_routine=routine;
 
 windowTitle=routine->getName();
 windowTitle+="() - CFG View";
 cfgDoc->SetTitle(windowTitle);
}


void pcProGUIView::OnGraphILG() 
{
 CTreeCtrl& tc=GetTreeCtrl();
 
 HTREEITEM ti=tc.GetSelectedItem();
 
 if (ti==NULL)
 {
  MessageBox("Please first select a routine.",NULL,MB_ICONINFORMATION);
  return;
 }
 
 //TODO : see if what is selected is a routine
 if (tc.GetItemText(ti).Find('.')>=0)
 {
  MessageBox("Please first select a routine.",NULL,MB_ICONINFORMATION);
  return;
 }

 Routine* routine=(Routine*)tc.GetItemData(ti);
 
 CWaitCursor waitCursor;
 CString windowTitle;

	ilgDoc = (ILGDoc*)theApp.m_ILG->CreateNewDocument();
 ilgDoc->m_routine=routine;
 
	CMDIChildWnd* ilgFrame	= static_cast<CMDIChildWnd*>(theApp.m_ILG->CreateNewFrame(ilgDoc,NULL));
 
	ilgFrame->InitialUpdateFrame(NULL,true);
	ilgView = static_cast<ILGView*>(ilgFrame->GetActiveView());
 ilgView->m_routine=routine;

 windowTitle=routine->getName();
 windowTitle+="() - ILG View";
	ilgDoc->SetTitle(windowTitle);
}


void pcProGUIView::OnGraphHTG() 
{
 CTreeCtrl& tc=GetTreeCtrl();
 
 HTREEITEM ti=tc.GetSelectedItem();
 
 if (ti==NULL)
 {
  MessageBox("Please first select a routine.",NULL,MB_ICONINFORMATION);
  return;
 }
 
 //TODO : see if what is selected is a routine
 if (tc.GetItemText(ti).Find('.')>=0)
 {
  MessageBox("Please first select a routine.",NULL,MB_ICONINFORMATION);
  return;
 }

 Routine* routine=(Routine*)tc.GetItemData(ti);
 
 CWaitCursor waitCursor;
 CString windowTitle;

	htgDoc = (HTGDoc*)theApp.m_HTG->CreateNewDocument();
 htgDoc->m_routine=routine;
 
	CMDIChildWnd* htgFrame	= static_cast<CMDIChildWnd*>(theApp.m_HTG->CreateNewFrame(htgDoc,NULL));
 
	htgFrame->InitialUpdateFrame(NULL,true);
	htgView = static_cast<HTGView*>(htgFrame->GetActiveView());
 htgView->m_routine=routine;

 windowTitle=routine->getName();
 windowTitle+="() - HTG View";
	htgDoc->SetTitle(windowTitle);
}



void pcProGUIView::OnNewSIM() 
{
 CWaitCursor waitCursor;
 CString windowTitle;

	simDoc = (SIMDoc*)theApp.m_SIM->CreateNewDocument();
 
	CMDIChildWnd* simFrame	= static_cast<CMDIChildWnd*>(theApp.m_SIM->CreateNewFrame(simDoc,NULL));
 
	simFrame->InitialUpdateFrame(NULL,true);
	simView = static_cast<SIMView*>(simFrame->GetActiveView());

 //windowTitle=GetSelectedFile();
 windowTitle+="SIM View";
	simDoc->SetTitle(windowTitle);
}



void pcProGUIView::OnSetFocus(CWnd* pOldWnd) 
{
 pcProGUIDoc* pDoc = dynamic_cast<pcProGUIDoc*>(GetDocument());
 ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");
 
 CTreeView::OnSetFocus(pOldWnd);

 ChangeSelection();
}

void pcProGUIView::ChangeSelection()
{
 viewSelection=GetSelectedFile();
 if (viewSelection!=NULL)
 {
	 theApp.propPageList->ShowPage(IDD_PROP_FILE, viewSelection);
  ((FilePropPage*)theApp.propPageList->activePage)->GetDataFromObject();
 }
 else
 {
  CTreeCtrl& tc=GetTreeCtrl();
  
  HTREEITEM ti=tc.GetSelectedItem();
  
  if (ti!=NULL) //if not NULL then it is a routine
  {
   viewSelection=(void*)tc.GetItemData(ti); 
 	 theApp.propPageList->ShowPage(IDD_PROP_ROUTINE, viewSelection);
   ((RoutinePropPage*)theApp.propPageList->activePage)->GetDataFromObject();
  }
  else //still nothing
  {
	  theApp.propPageList->ShowPage(IDD_PROP_BASE, NULL);
  }
 }
}

void pcProGUIView::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	ChangeSelection();

	*pResult = 0;
}


/*
void pcProGUIView::AddPropPages()
{
 //if (theApp.pPropSheet==NULL) return;

 pcProGUIDoc* pDoc = dynamic_cast<pcProGUIDoc*>(GetDocument());
 ASSERT_TRACE(pDoc,"Invalid GetDocument() return type");

 //remember the number of previous pages
 int p=theApp.pPropSheet->GetPageCount();
 if (viewSelection!=NULL)
 {
  //theApp.pPropSheet->AddPage(new FilePropPage);//&pDoc->filePropPage);
  DisplayProperties();
 }
 else
 {
  //theApp.pPropSheet->AddPage(&theApp.pPropSheet->m_defaultPage);
 }

 //theApp.pPropSheet->SetActivePage(p+1);

 //remove previous pages
 for (int i=0;i<p;i++)
 {
  //theApp.pPropSheet->RemovePage(i);
 }
}

*/

//void pcProGUIView::DisplayProperties()
//{
// //if (viewSelection!=NULL) viewSelection->DisplayProperties();
//}


int pcProGUIView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
 //add prop pages for this view
 BasePropPage::bNoUpdatePos=true;

 FilePropPage* filePropPage= new FilePropPage();
 filePropPage->Create(filePropPage->pageID,this);
 theApp.propPageList->AddPage(filePropPage->pageID,filePropPage);
 
 RoutinePropPage* routinePropPage= new RoutinePropPage();
 routinePropPage->Create(routinePropPage->pageID,this);
 theApp.propPageList->AddPage(routinePropPage->pageID,routinePropPage);
 
 BasePropPage::bNoUpdatePos=false;

	return 0;
}

//prototypes for the compile menus
__declspec( dllimport ) void init_compiler(void* _cp);
__declspec( dllimport ) void build_SSA(void);
__declspec( dllimport ) void build_HTG(void);
__declspec( dllimport ) void usedef_live(void);
__declspec( dllimport ) void trailblazing(void);
extern ProgramParams pp;


static int first_time=1;

void call_init_compiler(ProgramParams* pp)
{
	if(first_time)
 {
		first_time=0;
	
		lockPCPRO=true;
		stepPCPRO=false;
		
		init_compiler(pp);
	}
}

void pcProGUIView::OnCompileStepcompileBuildHtg() 
{
 CWaitCursor wait;

	//build the HTG 
	build_HTG();
 NotifyDocChange();
}

void pcProGUIView::OnCompileStepcompileSSA() 
{
 CWaitCursor wait;

	build_SSA();
 NotifyDocChange();
}

void pcProGUIView::OnCompileStepcompileTrailblazing() 
{
 CWaitCursor wait;

	//cout << endl << "Performing Trailblazing scheduling..." << endl;
	trailblazing();
	//cout << endl << "Done Trailblazing scheduling. " << endl;
 NotifyDocChange();
}

void pcProGUIView::OnCompileStepcompileUDLIVE() 
{
 CWaitCursor wait;

	usedef_live();
}

void pcProGUIView::OnCompileStepcompileInitializecompiler() 
{
 CWaitCursor wait;
 
 CTreeCtrl& tc=GetTreeCtrl();
 
 HTREEITEM ti=tc.GetSelectedItem();
 
 if (ti==NULL)
 {
 	MessageBox("Please first select a file to compile.",NULL,MB_ICONINFORMATION);
 }
 else
 {
 	//just initializes the compiler
  CreateCommandLineParams();
  
 	call_init_compiler(&pp);
  
  //NOSFIX disable this menu and enable the rest of the menu items
  
  //after we've compiled add the routines to the tree item
  addRoutines(tc,ti);
 }
}

void pcProGUIView::CreateCommandLineParams()
{
 ProjectFile* currFile;

 //'fake' the parameters
 //NOSFIX memory leaks to fix

 if (pp.cout==NULL) pp.cout=new my_ostream(&UpdateConsoleCOUT);
 if (pp.cerr==NULL) pp.cerr=new my_ostream(&UpdateConsoleCERR);
 
 if (pp.argv==NULL)
 {
  pp.argv=new char*[100]; //hopefully not more than 100 switches on the command line
 }

 currFile=GetSelectedFile();

 pp.argv[0]=strdup("./pcPro.exe");
 pp.argv[1]=strdup(LPCSTR(currFile->name));
 CString defs=currFile->name;
 //NOS cheesy hack to replace the .PROCS extension with .DEFS
 defs.SetAt(defs.GetLength()-5,'d');
 defs.SetAt(defs.GetLength()-4,'e');
 defs.SetAt(defs.GetLength()-3,'f');
 defs.SetAt(defs.GetLength()-2,'s');
 defs.SetAt(defs.GetLength()-1,'\0');
 pp.argv[2]=strdup(LPCSTR(defs));
 pp.argc=3;

 if (currFile->printIList) pp.argv[pp.argc++]=strdup("-pIList");
 if (currFile->printCFG) pp.argv[pp.argc++]=strdup("-pCFG");
 if (currFile->printHTG) pp.argv[pp.argc++]=strdup("-pHTG");
 if (currFile->afterTbz) pp.argv[pp.argc++]=strdup("-pTbz");
 if (currFile->simNoSSA) pp.argv[pp.argc++]=strdup("-sNoSSA");
 if (currFile->simSSA) pp.argv[pp.argc++]=strdup("-sSSA");
 if (currFile->simHTG) pp.argv[pp.argc++]=strdup("-sSSA");
 if (currFile->simISel) pp.argv[pp.argc++]=strdup("-sSSA");
 if (currFile->simTbz) pp.argv[pp.argc++]=strdup("-sTbz");
 if (currFile->doTbz) pp.argv[pp.argc++]=strdup("-Tbz");
 if (currFile->doISel) pp.argv[pp.argc++]=strdup("-ISel");
 if (currFile->pipelinedTbz) pp.argv[pp.argc++]=strdup("-pipeTbz");
 if (currFile->doMut) pp.argv[pp.argc++]=strdup("-Mut");
 if (currFile->doUnroll) pp.argv[pp.argc++]=strdup("-Unroll");
 if (currFile->printMC) pp.argv[pp.argc++]=strdup("-pMC");
 //if (linkEXPRESSION) pp.argv[pp.argc++]=strdup("-EXPR");

// if (strcmp(argv[i], "-DB") == 0)
// {
//  dbEnabled = 1;
// }
// if (strcmp(argv[i], "-REM") == 0)
// {
//  dbEnabled = 2;
// }
// if (strcmp(argv[i], "-ENAME") == 0)
// {
//  EXPR_NAME=strdup(argv[i+1]);
//  i++;
// }

}

void pcProGUIView::OnPrintRoutine() 
{
 CWaitCursor wait;

 CTreeCtrl& tc=GetTreeCtrl();
 
 HTREEITEM ti=tc.GetSelectedItem();
 
 if (ti==NULL)
 {
  MessageBox("Please first select a routine.",NULL,MB_ICONINFORMATION);
  return;
 }
 
 //TODO : see if what is selected is a routine
 if (tc.GetItemText(ti).Find('.')>=0)
 {
  MessageBox("Please first select a routine.",NULL,MB_ICONINFORMATION);
  return;
 }

 Routine* routine=(Routine*)tc.GetItemData(ti);
 
 routine->print();
 routine->printHTG();
}

void pcProGUIView::OnPercolate() 
{
 CWaitCursor wait;

 CTreeCtrl& tc=GetTreeCtrl();
 
 HTREEITEM ti=tc.GetSelectedItem();
 
 if (ti==NULL)
 {
  MessageBox("Please first select a routine.",NULL,MB_ICONINFORMATION);
  return;
 }
 
 //TODO : see if what is selected is a routine
 if (tc.GetItemText(ti).Find('.')>=0)
 {
  MessageBox("Please first select a routine.",NULL,MB_ICONINFORMATION);
  return;
 }

 Routine* routine=(Routine*)tc.GetItemData(ti);
 
 buildProgramMappings();
 routine->percolate();
 routine->deadCodeElimination();
 NotifyDocChange();
}

void pcProGUIView::OnMarkBlocks() 
{
 CWaitCursor wait;

 CTreeCtrl& tc=GetTreeCtrl();
 
 HTREEITEM ti=tc.GetSelectedItem();
 
 if (ti==NULL)
 {
  MessageBox("Please first select a routine.",NULL,MB_ICONINFORMATION);
  return;
 }
 
 //TODO : see if what is selected is a routine
 if (tc.GetItemText(ti).Find('.')>=0)
 {
  MessageBox("Please first select a routine.",NULL,MB_ICONINFORMATION);
  return;
 }

 Routine* routine=(Routine*)tc.GetItemData(ti);
 
 setCurrRoutine(routine);
	
 //NOSTEMP
 buildProgramMappings();
 //NOSTEMP

 routine->markBlocks();

 NotifyDocChange();
}

void pcProGUIView::OnIfConvert() 
{
 CWaitCursor wait;

 CTreeCtrl& tc=GetTreeCtrl();
 
 HTREEITEM ti=tc.GetSelectedItem();
 
 if (ti==NULL)
 {
  MessageBox("Please first select a routine.",NULL,MB_ICONINFORMATION);
  return;
 }
 
 //TODO : see if what is selected is a routine
 if (tc.GetItemText(ti).Find('.')>=0)
 {
  MessageBox("Please first select a routine.",NULL,MB_ICONINFORMATION);
  return;
 }

 Routine* routine=(Routine*)tc.GetItemData(ti);
 
 setCurrRoutine(routine);

	//routine->buildSSA();
 build_SSA();
 //routine->buildHTG();
	build_HTG();
 routine->print();
 routine->printHTG();
 routine->printCFG();
 ICGenUDDU=!isKeyPressed(VK_SHIFT);
 routine->ifConvert(NULL);
 routine->print();
 routine->deadLabelElimination();
 //routine->deadCodeElimination();
 routine->print();
 //buildProgramMappings();
 routine->buildCFG();
 routine->printCFG();
 routine->buildHTG();
 routine->fixIfHead();
 //routine->printHTG();
	usedef_live();
	//trailblazing();
 NotifyDocChange();
}

