# Microsoft Developer Studio Project File - Name="pcProGUI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=pcProGUI - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pcProGUI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pcProGUI.mak" CFG="pcProGUI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pcProGUI - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "pcProGUI - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pcProGUI - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../pcProDll/include" /I "../pcProDll/disamb" /I "../pcProDll/mutation" /I "./GraphViz/include" /I "../systemDll/NewBaseClassDll" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ../pcProDll/bin/pcProDllMFC.lib ../pcProDll/bin/graphViz.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "pcProGUI - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../pcProDll/include" /I "../pcProDll/disamb" /I "../pcProDll/mutation" /I "./GraphViz/include" /I "../systemDll/NewBaseClassDll" /I "../pcProDll/regalloc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "WIN32_GUI" /D "_AFXDLL" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ../acesMIPSdll/bin/acesMIPSdll.lib ../acesMIPSDll/bin/graphViz.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../acesMIPSdll/bin/pcProGUI.exe"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "pcProGUI - Win32 Release"
# Name "pcProGUI - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Console Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Console\console.cpp

!IF  "$(CFG)" == "pcProGUI - Win32 Release"

# ADD CPP /I ".."

!ELSEIF  "$(CFG)" == "pcProGUI - Win32 Debug"

# ADD CPP /I ".." /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Console\my_ostream.cpp
# End Source File
# Begin Source File

SOURCE=.\Console\StdAfx.cpp

!IF  "$(CFG)" == "pcProGUI - Win32 Release"

!ELSEIF  "$(CFG)" == "pcProGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Main Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\DOTGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MouseHitInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\pcProGUI.cpp
# End Source File
# Begin Source File

SOURCE=.\hlp\pcProGUI.hpj

!IF  "$(CFG)" == "pcProGUI - Win32 Release"

USERDEP__PCPRO="$(ProjDir)\hlp\AfxCore.rtf"	"$(ProjDir)\hlp\AfxPrint.rtf"	
# Begin Custom Build - Making help file...
OutDir=.\Release
ProjDir=.
TargetName=pcProGUI
InputPath=.\hlp\pcProGUI.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	call "$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "pcProGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "HTG Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\HTG\HTGDoc.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\HTG\HTGFrame.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\HTG\HTGView.cpp
# ADD CPP /I ".."
# End Source File
# End Group
# Begin Group "Project Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pcProGUIDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\pcProGUIView.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectFile.cpp
# End Source File
# End Group
# Begin Group "CFG Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CFG\CFGDoc.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\CFG\CFGFrame.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\CFG\CFGView.cpp
# ADD CPP /I ".."
# End Source File
# End Group
# Begin Group "ILG Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ILG\ILGDoc.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\ILG\ILGFrame.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\ILG\ILGView.cpp
# ADD CPP /I ".."
# End Source File
# End Group
# Begin Group "Simulator Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SIM\DataPathsDialog.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMGraphBus.cpp
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMGraphCompoundUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMGraphConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMGraphLatch.cpp
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMGraphObj.cpp
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMGraphPipelineStage.cpp
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMGraphPort.cpp
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMGraphStorage.cpp
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMGraphUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMView.cpp
# End Source File
# End Group
# Begin Group "Properties Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Prop\BasePropPage.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Prop\BBlockPropPage.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Prop\BusPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\Prop\CompoundUnitPropPage.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Prop\ConnectionPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\Prop\FilePropPage.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Prop\HTGCompundPropPage.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Prop\HTGIfPropPage.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Prop\HTGLoopPropPage.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Prop\HTGSimplePropPage.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Prop\InstrPropPage.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Prop\LatchPropPage.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Prop\PipelineStagePropPage.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Prop\PortPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\Prop\PropPageList.cpp
# End Source File
# Begin Source File

SOURCE=.\Prop\RoutinePropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\Prop\StoragePropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\Prop\UnitPropPage.cpp
# End Source File
# End Group
# Begin Group "Stats Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Stat\AddStatDlg.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Stat\CompoundUnitStatPropPage.cpp

!IF  "$(CFG)" == "pcProGUI - Win32 Release"

# ADD CPP /I ".."

!ELSEIF  "$(CFG)" == "pcProGUI - Win32 Debug"

# PROP Exclude_From_Build 1
# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Stat\ConnectionStatPropPage.cpp

!IF  "$(CFG)" == "pcProGUI - Win32 Release"

# ADD CPP /I ".."

!ELSEIF  "$(CFG)" == "pcProGUI - Win32 Debug"

# PROP Exclude_From_Build 1
# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Stat\DefaultStatProp.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Stat\GUIBaseStat.cpp
# End Source File
# Begin Source File

SOURCE=.\Stat\LatchStatPropPage.cpp

!IF  "$(CFG)" == "pcProGUI - Win32 Release"

# ADD CPP /I ".."

!ELSEIF  "$(CFG)" == "pcProGUI - Win32 Debug"

# PROP Exclude_From_Build 1
# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Stat\PercentageStat.cpp
# End Source File
# Begin Source File

SOURCE=.\Stat\PortStatPropPage.cpp

!IF  "$(CFG)" == "pcProGUI - Win32 Release"

# ADD CPP /I ".."

!ELSEIF  "$(CFG)" == "pcProGUI - Win32 Debug"

# PROP Exclude_From_Build 1
# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Stat\ScrollingStat.cpp
# End Source File
# Begin Source File

SOURCE=.\Stat\StorageStatPropPage.cpp

!IF  "$(CFG)" == "pcProGUI - Win32 Release"

# ADD CPP /I ".."

!ELSEIF  "$(CFG)" == "pcProGUI - Win32 Debug"

# PROP Exclude_From_Build 1
# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Stat\TextStat.cpp
# End Source File
# Begin Source File

SOURCE=.\Stat\UnitStatPropPage.cpp

!IF  "$(CFG)" == "pcProGUI - Win32 Release"

# ADD CPP /I ".."

!ELSEIF  "$(CFG)" == "pcProGUI - Win32 Debug"

# PROP Exclude_From_Build 1
# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Stat\ZeroOneScrollingStat.cpp
# End Source File
# End Group
# Begin Group "Temp Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TempDlg.cpp
# End Source File
# End Group
# Begin Group "Ops Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Ops\InstrDescr.cpp
# End Source File
# Begin Source File

SOURCE=.\Ops\InstrDescrDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Ops\ISInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Ops\OpGroups.cpp
# End Source File
# Begin Source File

SOURCE=.\Ops\OpGroupsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Ops\OpMappings.cpp
# End Source File
# Begin Source File

SOURCE=.\Ops\OpMappingsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Ops\OpMappingsDialog1.cpp
# End Source File
# Begin Source File

SOURCE=.\Ops\TreeThruList.cpp
# End Source File
# Begin Source File

SOURCE=.\Ops\VarGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\Ops\VarGroupDialog.cpp
# ADD CPP /I ".."
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Console Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Console\console.h
# End Source File
# Begin Source File

SOURCE=.\Console\my_ostream.h
# End Source File
# Begin Source File

SOURCE=.\Console\StdAfx.h
# End Source File
# End Group
# Begin Group "Main Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\ColorCodes.h
# End Source File
# Begin Source File

SOURCE=.\DOTGraph.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MouseHitInfo.h
# End Source File
# Begin Source File

SOURCE=.\pcProGUI.h
# End Source File
# Begin Source File

SOURCE=.\ProgramParams.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "HTG Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\HTG\HTGDoc.h
# End Source File
# Begin Source File

SOURCE=.\HTG\HTGFrame.h
# End Source File
# Begin Source File

SOURCE=.\HTG\HTGView.h
# End Source File
# End Group
# Begin Group "Project Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pcProGUIDoc.h
# End Source File
# Begin Source File

SOURCE=.\pcProGUIView.h
# End Source File
# Begin Source File

SOURCE=.\ProjectFile.h
# End Source File
# End Group
# Begin Group "CFG Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CFG\CFGDoc.h
# End Source File
# Begin Source File

SOURCE=.\CFG\CFGFrame.h
# End Source File
# Begin Source File

SOURCE=.\CFG\CFGView.h
# End Source File
# End Group
# Begin Group "ILG Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ILG\ILGDoc.h
# End Source File
# Begin Source File

SOURCE=.\ILG\ILGFrame.h
# End Source File
# Begin Source File

SOURCE=.\ILG\ILGView.h
# End Source File
# End Group
# Begin Group "Properties Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Prop\BasePropPage.h
# End Source File
# Begin Source File

SOURCE=.\Prop\BBlockPropPage.h
# End Source File
# Begin Source File

SOURCE=.\Prop\BusPropPage.h
# End Source File
# Begin Source File

SOURCE=.\Prop\CompoundUnitPropPage.h
# End Source File
# Begin Source File

SOURCE=.\Prop\ConnectionPropPage.h
# End Source File
# Begin Source File

SOURCE=.\Prop\FilePropPage.h
# End Source File
# Begin Source File

SOURCE=.\Prop\HTGCompundPropPage.h
# End Source File
# Begin Source File

SOURCE=.\Prop\HTGIfPropPage.h
# End Source File
# Begin Source File

SOURCE=.\Prop\HTGLoopPropPage.h
# End Source File
# Begin Source File

SOURCE=.\Prop\HTGSimplePropPage.h
# End Source File
# Begin Source File

SOURCE=.\Prop\instrproppage.h
# End Source File
# Begin Source File

SOURCE=.\Prop\LatchPropPage.h
# End Source File
# Begin Source File

SOURCE=.\Prop\PipelineStagePropPage.h
# End Source File
# Begin Source File

SOURCE=.\Prop\PortPropPage.h
# End Source File
# Begin Source File

SOURCE=.\Prop\PropPageList.h
# End Source File
# Begin Source File

SOURCE=.\Prop\RoutinePropPage.h
# End Source File
# Begin Source File

SOURCE=.\Prop\StoragePropPage.h
# End Source File
# Begin Source File

SOURCE=.\Prop\UnitPropPage.h
# End Source File
# End Group
# Begin Group "Simulator Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SIM\DataPathsDialog.h
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMDoc.h
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMFrame.h
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMGraphBus.h
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMGraphCompoundUnit.h
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMGraphConnection.h
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMGraphLatch.h
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMGraphObj.h
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMGraphPipelineStage.h
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMGraphPort.h
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMGraphStorage.h
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMGraphUnit.h
# End Source File
# Begin Source File

SOURCE=.\SIM\SIMView.h
# End Source File
# End Group
# Begin Group "Stats Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Stat\AddStatDlg.h
# End Source File
# Begin Source File

SOURCE=.\Stat\CompoundUnitStatPropPage.h
# End Source File
# Begin Source File

SOURCE=.\Stat\ConnectionStatPropPage.h
# End Source File
# Begin Source File

SOURCE=.\Stat\DefaultStatProp.h
# End Source File
# Begin Source File

SOURCE=.\Stat\GUIBaseStat.h
# End Source File
# Begin Source File

SOURCE=.\Stat\LatchStatPropPage.h
# End Source File
# Begin Source File

SOURCE=.\Stat\PercentageStat.h
# End Source File
# Begin Source File

SOURCE=.\Stat\PortStatPropPage.h
# End Source File
# Begin Source File

SOURCE=.\Stat\ScrollingStat.h
# End Source File
# Begin Source File

SOURCE=.\Stat\StorageStatPropPage.h
# End Source File
# Begin Source File

SOURCE=.\Stat\TextStat.h
# End Source File
# Begin Source File

SOURCE=.\Stat\UnitStatPropPage.h
# End Source File
# Begin Source File

SOURCE=.\Stat\ZeroOneScrollingStat.h
# End Source File
# End Group
# Begin Group "Temp Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TempDlg.h
# End Source File
# End Group
# Begin Group "Ops Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Ops\InstrDescr.h
# End Source File
# Begin Source File

SOURCE=.\Ops\InstrDescrDialog.h
# End Source File
# Begin Source File

SOURCE=.\Ops\ISInfo.h
# End Source File
# Begin Source File

SOURCE=.\Ops\OpGroups.h
# End Source File
# Begin Source File

SOURCE=.\Ops\OpGroupsDialog.h
# End Source File
# Begin Source File

SOURCE=.\Ops\OpMappings.h
# End Source File
# Begin Source File

SOURCE=.\Ops\OpMappingsDialog.h
# End Source File
# Begin Source File

SOURCE=.\Ops\OpMappingsDialog1.h
# End Source File
# Begin Source File

SOURCE=.\Ops\TreeThruList.h
# End Source File
# Begin Source File

SOURCE=.\Ops\VarGroup.h
# End Source File
# Begin Source File

SOURCE=.\Ops\VarGroupDialog.h
# End Source File
# End Group
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\add.ico
# End Source File
# Begin Source File

SOURCE=.\res\add_grou.ico
# End Source File
# Begin Source File

SOURCE=.\res\add_op.ico
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cfile.ico
# End Source File
# Begin Source File

SOURCE=.\res\cfile_ch.ico
# End Source File
# Begin Source File

SOURCE=.\res\check.ico
# End Source File
# Begin Source File

SOURCE=.\res\checkbox.bmp
# End Source File
# Begin Source File

SOURCE=.\res\consolet.bmp
# End Source File
# Begin Source File

SOURCE=.\res\del_grou.ico
# End Source File
# Begin Source File

SOURCE=.\res\delete.ico
# End Source File
# Begin Source File

SOURCE=.\res\down.ico
# End Source File
# Begin Source File

SOURCE=.\res\func.ico
# End Source File
# Begin Source File

SOURCE=.\res\func_che.ico
# End Source File
# Begin Source File

SOURCE=.\res\graphs.bmp
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_pcpr.ico
# End Source File
# Begin Source File

SOURCE=.\res\nocheck.ico
# End Source File
# Begin Source File

SOURCE=.\res\oldmain.ico
# End Source File
# Begin Source File

SOURCE=.\res\passes.bmp
# End Source File
# Begin Source File

SOURCE=.\res\pcProGUI.ico
# End Source File
# Begin Source File

SOURCE=.\pcProGUI.rc
# End Source File
# Begin Source File

SOURCE=.\res\pcProGUI.rc2
# End Source File
# Begin Source File

SOURCE=.\res\pcProGUIDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\plain.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\up.ico
# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\hlp\AfxCore.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\AfxPrint.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\AppExit.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Bullet.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurArw2.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurArw4.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurHelp.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditCopy.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditCut.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditPast.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditUndo.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileNew.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileOpen.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FilePrnt.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileSave.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\HlpSBar.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\HlpTBar.bmp
# End Source File
# Begin Source File

SOURCE=.\MakeHelp.bat
# End Source File
# Begin Source File

SOURCE=.\hlp\pcProGUI.cnt
# End Source File
# Begin Source File

SOURCE=.\hlp\RecFirst.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecLast.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecNext.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecPrev.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Scmax.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ScMenu.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Scmin.bmp
# End Source File
# End Group
# Begin Group "Misc Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\bin\LivermoreLoops.pcp
# End Source File
# Begin Source File

SOURCE=.\pcProGUI.reg
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Group
# End Target
# End Project
