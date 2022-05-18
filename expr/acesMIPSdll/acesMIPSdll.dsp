# Microsoft Developer Studio Project File - Name="acesMIPSdll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=acesMIPSdll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "acesMIPSdll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "acesMIPSdll.mak" CFG="acesMIPSdll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "acesMIPSdll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "acesMIPSdll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACESMIPSDLL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACESMIPSDLL_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACESMIPSDLL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GR /GX /ZI /Od /I ".\include" /I ".\instrsel" /I ".\mutation" /I ".\support" /I "..\pcProDll" /I "..\pcProDll\include" /I "..\pcProDll\disamb" /I "..\pcProDll\gui" /I "..\pcProDll\mutation" /I "..\pcProDll\instrsel" /I "..\systemDll\NewBaseClassDll" /I "..\systemDll\acesMIPSSimulatorFuncDll" /I "..\pcProDll\regalloc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "WIN32_GUI" /D "__EXPRESSION__" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 "../systemDll/acesMIPS Simulator Functions Lib/debug/acesMIPS Simulator Functions Lib.lib" "../systemDll/acesMIPS Base Class Lib/debug/acesMIPS Base Class Lib.lib" "../systemDll/acesMIPS Derived Class Lib/debug/acesMIPS Derived Class Lib.lib" "../systemDll/acesMIPS Build System Lib/debug/acesMIPS Build System Lib.lib" "../expression dll/Debug/expression dll.lib" /nologo /subsystem:windows /dll /pdb:"../acesMIPSdll/bin/acesMIPSdll.pdb" /debug /machine:I386 /nodefaultlib:"nafxcwd.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"libcmtd.lib" /out:"../acesMIPSdll/bin/acesMIPSdll.dll" /implib:"../acesMIPSdll/bin/acesMIPSdll.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "acesMIPSdll - Win32 Release"
# Name "acesMIPSdll - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Transmutations Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\transmutations\Optimizations.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\transmutations\OptimizationsSupport.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\transmutations\optparse.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\transmutations\OptParse.lex

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\pcProDll\transmutations\OptParse.lex

"../pcProDll/transmutations/OptParse_lexer.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\tools\flex -POpt -o../pcProDll/transmutations/OptParse_lexer.cpp ../pcProDll/transmutations/OptParse.lex

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\transmutations\OptParse.y

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\pcProDll\transmutations\OptParse.y

BuildCmds= \
	..\tools\bison -pOpt -dvt -o../pcProDll/transmutations/OptParse.cpp ../pcProDll/transmutations/OptParse.y

"../pcProDll/transmutations/OptParse.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"../pcProDll/transmutations/OptParse.cpp.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\transmutations\OptParse_lexer.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# Begin Group "Thumb Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\thumb\profit_function.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\thumb\RegSetMapping.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# ADD CPP /I "..\pcProDll\asmgen" /I "\include"
# SUBTRACT CPP /I ".\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\thumb\rISASchedulerSupport.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\thumb\ThumbPass.cpp
# End Source File
# End Group
# Begin Group "AsmGen Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\asmgen\AsmBB.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\asmgen\AsmGen.cpp
# End Source File
# End Group
# Begin Group "Symtab Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\support\symbols.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\symtabs.cpp
# End Source File
# End Group
# Begin Group "IfConvert Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\IfConversion\ICBaseHTGNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\IfConversion\ICBaseOperation.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\IfConversion\ICBasicBlockNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\IfConversion\ICIfHTGNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\IfConversion\ICLoopHTGNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\IfConversion\ICNormalInstr.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\IfConversion\ICOperation.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\IfConversion\ICRoutine.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\IfConversion\ICSimpleHTGNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\IfConversion\PredicationInformation.cpp
# End Source File
# End Group
# Begin Group "Memory Alloc Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\storagealloc\global_memory_alloc.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\storagealloc\liferanges.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\ReadDefsFile.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\storagealloc\replace_global_addresses.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\storagealloc\storagealloc.cpp
# End Source File
# End Group
# Begin Group "Percolation Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\percolation\PercControlOp.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\percolation\PercDataOp.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\percolation\PercNormalInstr.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\percolation\PercRoutine.cpp
# End Source File
# End Group
# Begin Group "RegAlloc Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\DoubleMCNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\IGNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\IGraph.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\InitRegMapping.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\InterferenceGraph.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\IRHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\MCNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\MetaMCNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\MultiChain.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\RegAlloc.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\RegArchModel.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\RegCoalesce.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\RegisterAlloc.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\RegisterMapping.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\RegNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\Spills.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\SpillSupport.cpp
# End Source File
# End Group
# Begin Group "Transformation Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\support\CompExecPossib.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\CopyElim.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\CopyProp.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\DeadCodeElimination.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\DeadLabelElimination.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\NewCopyElimination.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\ValueProp.cpp
# End Source File
# End Group
# Begin Group "Analysis Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\support\CompDefs.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\CompLiveDead.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\CompLiveDeadInstr.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\CompMultiChain.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\CompRegClass.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\CompUDChain.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\MaintainUD.cpp
# End Source File
# End Group
# Begin Group "Trailblazing Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\trailblazing\ReservationTables.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\trailblazing\SchedulerSupport.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\trailblazing\SpreadInstrs.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\trailblazing\TbzBaseHTGNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\trailblazing\TbzBaseOperation.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\trailblazing\TbzBasicBlockNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\trailblazing\TbzIfHTGNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\trailblazing\TbzLoopHTGNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\trailblazing\TbzNormalInstr.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\trailblazing\TbzRoutine.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\trailblazing\TbzSimpleHTGNode.cpp
# End Source File
# End Group
# Begin Group "InstrSel Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\instrsel\InstrSel.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\InstrSelIburg.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\InstrSelSA.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\InstrSelSAPrim.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\ISBaseOperation.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\ISControlOp.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\iselparse.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\ISelParse.lex

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# Begin Custom Build
InputPath=..\pcProDll\instrsel\ISelParse.lex

"../pcProDll/instrsel/ISelParse_lexer.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\tools\flex -PISel -o../pcProDll/instrsel/ISelParse_lexer.cpp ../pcProDll/instrsel/ISelParse.lex

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\ISelParse.y

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\pcProDll\instrsel\ISelParse.y

BuildCmds= \
	..\tools\bison -pISel -dvt -o../pcProDll/instrsel/ISelParse.cpp ../pcProDll/instrsel/ISelParse.y

"../pcProDll/instrsel/ISelParse.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"../pcProDll/instrsel/ISelParse.cpp.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\ISelParse_lexer.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\ISIfHTGNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\ISLoopHTGNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\ISNormalInstr.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\ISSimpleHTGNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\MutPass.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\NewInstrSel.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\NewInstrSelSupport.cpp
# End Source File
# End Group
# Begin Group "Mutation Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\mutation\IburgToIR.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\mutation\MutBaseOperation.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\mutation\MutComputeOp.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\mutation\MutFConstArg.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\mutation\MutIConstArg.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\mutation\MutIfHTGNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\mutation\MutLableArg.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\mutation\MutLoopHTGNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\mutation\MutMemoryOp.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\mutation\MutNormalInstr.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\mutation\MutRealRegArg.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\mutation\MutSimpleHTGNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\mutation\SemActPrimitives.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\mutation\SemanticAction.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\mutation\TreeBuilding.cpp
# End Source File
# Begin Source File

SOURCE=..\ElfPcProDll\mutation\TreeParser.brg
# End Source File
# Begin Source File

SOURCE=..\pcProDll\mutation\TreeParser.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\mutation\UDDU.cpp
# End Source File
# End Group
# Begin Group "GUI Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\gui\Arc.c

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\gui\callback.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\gui\Graph.c

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\gui\graphObj.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\gui\GUIMain.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\gui\GUIProgram.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\gui\GUIRoutine.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\gui\highlight.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\gui\menu.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\gui\pixmap.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\gui\simulatorWin.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\gui\simWin.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Disamb Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\disamb\auxiliary.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\BasicSet.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\charString.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\dependency.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\disamb.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\disamb_parser.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\DisambComputeOp.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\DisambIConstArg.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\DisambMemoryOp.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\DisambNormalInstr.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\DisambRealRegArg.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\DisambRegisterArg.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\DisambRoutine.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\error.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\expr_utils.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\exprs.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\gcd.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\intMatrix.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\myvector.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\PartofProperty.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\poly.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\polyhedron.cpp
# End Source File
# End Group
# Begin Group "bin"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bin\acesMIPS.fmd
# End Source File
# Begin Source File

SOURCE=.\bin\acesMIPS.gmd
# End Source File
# Begin Source File

SOURCE=.\bin\acesMIPS.ifd
# End Source File
# Begin Source File

SOURCE=.\bin\acesMIPS.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\controlParms
# End Source File
# Begin Source File

SOURCE=.\bin\dumpIRFormat.txt
# End Source File
# Begin Source File

SOURCE=.\bin\genericOpCodeFile
# End Source File
# Begin Source File

SOURCE=.\bin\genericOpcodeSpecFile
# End Source File
# Begin Source File

SOURCE=.\bin\genericRegClasses.txt
# End Source File
# Begin Source File

SOURCE=.\bin\genericRegClassToRegsMapping.txt
# End Source File
# Begin Source File

SOURCE=.\bin\genericRegFileList.txt
# End Source File
# Begin Source File

SOURCE=.\bin\ifOppositesFile
# End Source File
# Begin Source File

SOURCE=.\bin\instr_formats
# End Source File
# Begin Source File

SOURCE=.\bin\ISelMapping.txt
# End Source File
# Begin Source File

SOURCE=.\bin\mem.config
# End Source File
# Begin Source File

SOURCE=.\bin\MIPS_OpCodeTiming.txt
# End Source File
# Begin Source File

SOURCE=.\bin\operandsMappingFile.txt
# End Source File
# Begin Source File

SOURCE=.\bin\regClassToRegClassMapping.txt
# End Source File
# Begin Source File

SOURCE=.\bin\run_vocoder.bat
# End Source File
# Begin Source File

SOURCE=.\bin\runAll.bat
# End Source File
# Begin Source File

SOURCE=.\bin\runAllToFile.bat
# End Source File
# Begin Source File

SOURCE=.\bin\runArrToFile.bat
# End Source File
# Begin Source File

SOURCE=.\bin\runpcPro.bat
# End Source File
# Begin Source File

SOURCE=.\bin\runSimDiff.bat
# End Source File
# Begin Source File

SOURCE=.\bin\runSimDiff_tab.bat
# End Source File
# Begin Source File

SOURCE=.\bin\specialCallFile
# End Source File
# Begin Source File

SOURCE=.\bin\SpillReloadTarget.txt
# End Source File
# Begin Source File

SOURCE=.\bin\targetOpCodeFile
# End Source File
# Begin Source File

SOURCE=.\bin\targetRegClasses.txt
# End Source File
# Begin Source File

SOURCE=.\bin\targetRegClassToRegsMapping.txt
# End Source File
# Begin Source File

SOURCE=.\bin\targetRegFileList.txt
# End Source File
# Begin Source File

SOURCE=.\bin\transformFile
# End Source File
# Begin Source File

SOURCE=.\bin\vliw.xmd
# End Source File
# End Group
# Begin Group "IR Source"

# PROP Default_Filter ""
# Begin Group "Argument Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\support\FConstArg.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\IConstArg.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\ImmediateArg.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\LabelArg.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\LatchRegArg.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\RealRegArg.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\RegisterArg.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\VirtualRegArg.cpp
# End Source File
# End Group
# Begin Group "Operation Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\support\BaseOperation.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\CallOp.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\ComputeOp.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\ControlOp.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\DataOp.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\FlowOp.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\GotoOp.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\HeaderOp.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\IfOp.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\JumpOp.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\MemoryOp.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\NormalOp.cpp
# End Source File
# End Group
# Begin Group "Instruction Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\support\BaseInstruction.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\ControlOpSlot.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\DataOpSlot.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\FlowOpSlot.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\NormalInstr.cpp
# End Source File
# End Group
# Begin Group "CFG Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\support\BasicBlockEdge.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\BasicBlockNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\BasicBlockPredecessors.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\ControlFlowGraph.cpp
# End Source File
# End Group
# Begin Group "HTG Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\support\BaseHTGNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\BuildHTG.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\HTGTraversal.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\IfHTGNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\LoopHTGNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\SimpleHTGNode.cpp
# End Source File
# End Group
# Begin Group "Loop Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\support\BuildLoop.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\Loop.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\LoopList.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\LoopManipulate.cpp
# End Source File
# End Group
# Begin Group "Properties Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\support\Assertions.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\BaseProperties.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\BBProperty.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\BranchProperty.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\ClusterProperty.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\CnstProp.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\CompDefBBProperty.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\DeleteProperty.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\DUChainProperty.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\ExecNumBBProperty.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\ExecPossibBBProperty.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\GToTRegMapProperty.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\IBSLoopProperty.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\InstrTypeProperty.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\LiveDeadBBProperty.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\LiveDeadInstrProperty.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\MultiChainProperty.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\OpPredicate.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\PredidArgProperty.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\RegOrderArgProperty.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\RegPressProperty.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\SSAArgProperty.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\UDChainProperty.cpp
# End Source File
# End Group
# Begin Group "Arch Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\support\GenericOpCodes.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\OpCode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\OpCodeFileDSSupport.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\OpCodeTable.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\RegisterFile.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\RegisterFileList.cpp
# End Source File
# End Group
# Begin Group "Support Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\support\Array.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\BinGraphDFS.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\ErrorMesgs.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\FilePointer.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\LinkedList.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\LinkedListIterator.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\MyLinkedList.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\RegisterPressure.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\Stack.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Parse Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\support\express.lex

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# Begin Custom Build
InputPath=..\pcProDll\support\express.lex

"..\pcProDll\support\express_lexer.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\tools\flex -o../pcProDll/support/express_lexer.cpp ../PcProDll/support/express.lex

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\express.y

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\pcProDll\support\express.y

BuildCmds= \
	..\tools\bison -dvt -o../pcProDll/support/express_parser.cpp ../pcProDll/support/express.y

"..\pcProDll\support\express_parser.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\pcProDll\support\express_parser.cpp.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\express_defs.lex

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# Begin Custom Build
InputPath=..\pcProDll\support\express_defs.lex

"../pcProDll/support/express_defs_lexer.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\tools\flex -Pdefs -o../pcProDll/support/express_defs_lexer.cpp      ../pcProDll/support/express_defs.lex

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\express_defs.y

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\pcProDll\support\express_defs.y

BuildCmds= \
	..\tools\bison -pdefs -dvt -o../pcProDll/support/express_defs_parser.cpp ../pcProDll/support/express_defs.y

"../pcProDll/support/express_defs_parser.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"../pcProDll/support/express_defs_parser.cpp.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\express_defs_lexer.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\express_defs_parser.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\express_lexer.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\express_parser.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\lex_parse.txt
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\lexer.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\lexer.l
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\opcodefile.lex

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# Begin Custom Build
InputPath=..\pcProDll\support\opcodefile.lex

"../pcProDll/support/opcodefile_lexer.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\tools\flex -Popcode -o../pcProDll/support/opcodefile_lexer.cpp      ../pcProDll/support/opcodefile.lex

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\opcodefile.y

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# Begin Custom Build
InputPath=..\pcProDll\support\opcodefile.y

BuildCmds= \
	..\tools\bison -popcode -dvt -o../pcProDll/support/opcodefile_parser.cpp           ../pcProDll/support/opcodefile.y

"../pcProDll/support/opcodefile_parser.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"../pcProDll/support/opcodefile_parser.cpp.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\opcodefile_lexer.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\opcodefile_parser.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\parser.y
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\parserSupport.cpp
# End Source File
# End Group
# Begin Group "Misc Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\support\AddCallNameProperty.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\BuildIR.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\BuildIRSupportFuncs.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\BuildSSA.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\CallPatch.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\CompilerRTStatus.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\ControlParms.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\Flags.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\GenericNode.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\IfOppositesTable.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\InitCall.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\IRSupportFuncs.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\IRTraversal.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\Main.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\NewBuildIR.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\NewBuildSSA.cpp
# End Source File
# Begin Source File

SOURCE=.\support\NoGUIMain.cpp
# End Source File
# Begin Source File

SOURCE=.\support\NoGUIMainSupportFuncs.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\PatchForDoubles.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\Program.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\RenameTable.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\Routine.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\SetScheduleProperties.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\SymbolTable.cpp
# End Source File
# End Group
# Begin Group "Obsolete Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\support\PhiInstr.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\PhiOp.cpp
# End Source File
# End Group
# End Group
# Begin Group "Win32 Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\win32\BezierEdge.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\win32\GraphNode.cpp
# End Source File
# End Group
# Begin Group "Main Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\pcProDll.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\StdAfx.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Transmutations Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\transmutations\Optimizations.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\transmutations\optparse.cpp.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\transmutations\OptParse.h
# End Source File
# End Group
# Begin Group "Thumb Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ElfPcProDll\Thumb\profit_function.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\Thumb\RegSetMapping.h
# End Source File
# Begin Source File

SOURCE=..\ElfPcProDll\Thumb\ThumbPass.h
# End Source File
# End Group
# Begin Group "AsmGen Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\asmgen\AsmBB.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\asmgen\AsmGen.h
# End Source File
# End Group
# Begin Group "IfConvert Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\IfConversion\PredicationInformation.h
# End Source File
# End Group
# Begin Group "Symtab Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\include\defs_node.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\symbols.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\symtabs.h
# End Source File
# End Group
# Begin Group "Percolation Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\percolation\PercDefines.h
# End Source File
# End Group
# Begin Group "Trailblazing Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\trailblazing\ReservationTables.h
# End Source File
# End Group
# Begin Group "RegAlloc Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\IGNode.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\IGraph.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\InterferenceGraph.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\IRHelper.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\MCNode.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\MetaMCNode.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\MultiChain.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\RegArchModel.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\RegCoalesce.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\RegisterMapping.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\RegNode.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\RegOrder.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\RegPressure.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\RegAlloc\Spills.h
# End Source File
# End Group
# Begin Group "InstrSel Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\instrsel\InstrSel.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\InstrSelIburg.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\InstrSelPat.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\InstrSelSA.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\InstrSelSAPrim.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\iselparse.cpp.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\ISelParse.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\LabelMoving.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\MutPass.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\NewInstrSel.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\instrsel\unistd.h
# End Source File
# End Group
# Begin Group "Mutation Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\mutation\IburgDefs.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\mutation\IburgToIR.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\mutation\SemActPrimitives.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\mutation\TreeBuilding.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\mutation\TreeParser.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\mutation\UDDU.h
# End Source File
# End Group
# Begin Group "GUI Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\gui\Arc.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\gui\ArcP.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\gui\common.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\gui\global.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\gui\Graph.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\gui\graphObj.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\gui\GraphP.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\gui\GUIProgram.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\gui\GUIRoutine.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\gui\highlight.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\gui\menu.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\gui\pixmap.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\gui\simulatorWin.h
# End Source File
# End Group
# Begin Group "Disamb Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\disamb\auxiliary.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\BasicSet.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\charString.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\dependency.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\disamb.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\disamb_parser.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\error.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\expr_utils.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\exprs.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\gcd.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\intMatrix.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\myvector.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\PartofProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\poly.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\PolyDisamb.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\polyhedron.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\std_types.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\disamb\types.h
# End Source File
# End Group
# Begin Group "IR Headers"

# PROP Default_Filter ""
# Begin Group "Argument Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\include\ArgumentIncludes.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\BaseArgument.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\FConstArg.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\IConstArg.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\ImmediateArg.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\LabelArg.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\LatchRegArg.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\PseudoRegArg.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\RealRegArg.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\RegisterArg.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\VirtualRegArg.h
# End Source File
# End Group
# Begin Group "Operation Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\include\BaseOperation.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\CallOp.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\ComputeOp.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\ControlOp.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\DataOp.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\FlowOp.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\GotoOp.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\HeaderOp.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\IfOp.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\JumpOp.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\MemoryOp.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\MiscOp.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\NormalOp.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\OperationIncludes.h
# End Source File
# End Group
# Begin Group "CFG Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\include\BasicBlockEdge.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\BasicBlockIncludes.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\BasicBlockNode.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\BasicBlockPredecessors.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\BBIterator.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\ControlFlowGraph.h
# End Source File
# End Group
# Begin Group "HTG Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\include\BaseHTGNode.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\CallHTGNode.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\HTGTraversal.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\IfHTGNode.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\LoopHTGNode.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\SimpleHTGNode.h
# End Source File
# End Group
# Begin Group "Loop Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\include\Loop.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\LoopList.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\TrackStack.h
# End Source File
# End Group
# Begin Group "Properties Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\support\Assertions.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\BaseArgProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\BaseBBProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\BaseInstrProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\BaseLoopProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\BaseOpProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\BaseRoutineProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\BranchProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\ClusterProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\CnstProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\CodeLocProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\CompDefBBProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\DeleteProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\DUProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\ExecNumBBProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\ExecPossibBBProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\GToTRegMapProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\IBSLoopProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\InstrTypeProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\LiveDeadBBProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\MultiChainProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\OpPredicate.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\PredidArgProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\PredProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\PropertyIncludes.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\PropertyList.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\RAProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\RoutParmProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\ScheduleOpProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\SSAArgProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\UDProperty.h
# End Source File
# End Group
# Begin Group "Arch Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\include\ArchitectureIncludes.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\ComputeOpCodeStruct.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\GenericOpCodes.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\OpCode.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\OpCodeTable.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\RegisterFile.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\RegisterFileList.h
# End Source File
# End Group
# Begin Group "Support Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\include\Array.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\ArrayIterator.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\ArrayOutOfBoundsException.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\BBProperty.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\BinGraphDFS.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\CouldNotOpenFileException.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\Exception.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\Exceptions.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\FilePointer.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\GlobalIncludes.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\LinkedList.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\LinkedListIterator.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\MyGraph.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\MyLinkedList.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\MyLinkedListIterator.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\MyStack.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\MyStackIterator.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\MyString.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\NoSuchElementException.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\SetList.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\Stack.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\StackIterator.h
# End Source File
# End Group
# Begin Group "Parse Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\include\expressParser.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\parser.cpp.h
# End Source File
# End Group
# Begin Group "Misc Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\include\BuildSSA.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\CompilerRTStatus.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\ControlParms.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\support\EVEsemret.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\Flags.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\FlagsDefs.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\GenericNode.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\IfOppositesTable.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\IRTraversal.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\ltstr.h
# End Source File
# Begin Source File

SOURCE=..\ElfPcProDll\include\NoGUIMain.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\Program.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\RenameTable.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\ReverseMap.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\Routine.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\SpecialInclude.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\STLIncludes.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\SymbolTable.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\ValuesSET.h
# End Source File
# End Group
# Begin Group "Obsolete Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\include\PhiInstr.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\PhiOp.h
# End Source File
# End Group
# Begin Group "Instruction Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\include\BaseInstruction.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\BaseOpSlot.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\ControlOpSlot.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\DataOpSlot.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\FlowOpSlot.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\Instruction.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\InstructionIncludes.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\include\NormalInstr.h
# End Source File
# End Group
# End Group
# Begin Group "Win32 Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\win32\BezierEdge.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\win32\GraphNode.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\win32\MouseHitInfoStruct.h
# End Source File
# End Group
# Begin Group "Main Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\pcProDll.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\Resource.h
# End Source File
# End Group
# End Group
# Begin Group "Tools Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\tools\bison.exe
# End Source File
# Begin Source File

SOURCE=..\tools\bison.simple
# End Source File
# Begin Source File

SOURCE=..\tools\flex.exe
# End Source File
# Begin Source File

SOURCE=..\tools\Iburg.exe
# End Source File
# End Group
# Begin Group "Misc Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ElfPcProDll\bison.simple
# End Source File
# Begin Source File

SOURCE=..\pcProDll\DebugSwitches.cpp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\DebugSwitches.h
# End Source File
# Begin Source File

SOURCE=..\pcProDll\mutation\Iburg.exe
# End Source File
# Begin Source File

SOURCE=..\c6xPcProDll\RedirectOutput.cpp

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\c6xPcProDll\RedirectOutput.h
# End Source File
# Begin Source File

SOURCE=..\pcprodll\bin\unistd.h
# End Source File
# End Group
# Begin Group "Test Files"

# PROP Default_Filter ""
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pcProDll\pcProDllMFC.def

!IF  "$(CFG)" == "acesMIPSdll - Win32 Release"

!ELSEIF  "$(CFG)" == "acesMIPSdll - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\pcProDll\pcProDllMFC.rc
# End Source File
# Begin Source File

SOURCE=..\pcProDll\res\pcProDllMFC.rc2
# End Source File
# End Group
# End Target
# End Project
