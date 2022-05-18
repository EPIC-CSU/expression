# Microsoft Developer Studio Project File - Name="expression dll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=expression dll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "expression dll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "expression dll.mak" CFG="expression dll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "expression dll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "expression dll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "expression dll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "BackEnd/rtgen" /I "Parser" /I "Symtab" /I "IR" /I "BackEnd" /I "..\pcProDll\include" /I "..\systemDll\baseClassDll" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../PcProDll/bin/expression dll.dll"

!ELSEIF  "$(CFG)" == "expression dll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "expressi"
# PROP BASE Intermediate_Dir "expressi"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GX /ZI /Od /I "BackEnd/rtgen" /I "Parser" /I "Symtab" /I "IR" /I "BackEnd" /I "..\pcProDll\include" /I "..\systemDll\baseClassDll" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../acesMIPSdll/bin/expression dll.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "expression dll - Win32 Release"
# Name "expression dll - Win32 Debug"
# Begin Group "bin"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\c6xPcProDll\bin\c62.exp
# End Source File
# Begin Source File

SOURCE=.\bin\c62.exp
# End Source File
# Begin Source File

SOURCE=.\bin\c62.fmd
# End Source File
# Begin Source File

SOURCE=.\bin\C62.ifd
# End Source File
# Begin Source File

SOURCE=.\bin\C62.xmd
# End Source File
# Begin Source File

SOURCE=..\c6xPcProDll\bin\c62_2RF.exp
# End Source File
# Begin Source File

SOURCE=..\c6xPcProDll\bin\c62_mem.fmd
# End Source File
# Begin Source File

SOURCE=..\c6xPcProDll\bin\c62_mem.ifd
# End Source File
# Begin Source File

SOURCE=..\c6xPcProDll\bin\c62_mem.xmd
# End Source File
# Begin Source File

SOURCE=..\c6xPcProDll\bin\c62_mem_no_latency.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\c62_v1.xmd
# End Source File
# Begin Source File

SOURCE=..\pcProDll\bin\DLX.exp
# End Source File
# Begin Source File

SOURCE=..\pcProDll\bin\dlx.gmd
# End Source File
# Begin Source File

SOURCE=..\pcProDll\bin\dlx.xmd
# End Source File
# Begin Source File

SOURCE=..\pcProDll\bin\dlx_mi.gmd
# End Source File
# Begin Source File

SOURCE=..\pcProDll\bin\dlx_mi.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\elf.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\HPLPD_mdconn.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\HPLPD_mdconn_sharedp.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\HPLPD_mdconn_sharedp_mcyc123.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\HPLPD_sp_dummy.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\HPLPD_sp_dummy1.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\HPLPD_sp_dummy2.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\HPLPD_sp_dummy3.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\HPLPD_sp_dummy4.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\HPLPD_sp_dummy5.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\HPLPD_sp_dummy6.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\HPLPD_sp_dummy7.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\HPLPD_sp_dummy8.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\HPLPD_V1.fmd
# End Source File
# Begin Source File

SOURCE=.\bin\HPLPD_V1.ifd
# End Source File
# Begin Source File

SOURCE=.\bin\HPLPD_v1.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\HPLPD_v2.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\HPLPD_v3.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\HPLPD_v4.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\HPLPD_v5.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\HPLPD_v6.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\HPLPD_v7.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\HPLPD_v8.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\HPLPD_v9.xmd
# End Source File
# Begin Source File

SOURCE=.\bin\instr_formats
# End Source File
# Begin Source File

SOURCE=..\pcProDll\bin\Modified_DLX.exp
# End Source File
# Begin Source File

SOURCE=..\R10KPcProDll\bin\r10kv1.xmd
# End Source File
# Begin Source File

SOURCE=..\pcprodll\bin\vliw.xmd
# End Source File
# End Group
# Begin Group "Include files"

# PROP Default_Filter ""
# Begin Group "Parser Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Parser\errors.h
# End Source File
# Begin Source File

SOURCE=.\Parser\expression.cpp.h
# End Source File
# Begin Source File

SOURCE=.\Parser\parser.h
# End Source File
# Begin Source File

SOURCE=..\pcprodll\bin\unistd.h
# End Source File
# End Group
# Begin Group "Symtab Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Symtab\symbols.h
# End Source File
# Begin Source File

SOURCE=.\Symtab\symtabs.h
# End Source File
# End Group
# Begin Group "IR Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\IR\forwrefs.h
# End Source File
# Begin Source File

SOURCE=.\IR\node.h
# End Source File
# End Group
# Begin Group "BackEnd Header Files"

# PROP Default_Filter ""
# Begin Group "Simulator Generator Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Backend\SimulatorGeneratorSupportFuncs.h
# End Source File
# End Group
# Begin Group "RT Generator Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Backend\rtgen\GenerateRT.h
# End Source File
# End Group
# Begin Group "PredInfoGen Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Backend\PredicationInfoGen\PredicationInfoGen.h
# End Source File
# Begin Source File

SOURCE=.\Backend\PredicationInfoGen\PredTable.h
# End Source File
# Begin Source File

SOURCE=.\Backend\PredicationInfoGen\PredTableNode.h
# End Source File
# End Group
# Begin Group "Reg Info Gen Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Backend\RegInfoGen\ClassTypeDataTypePair.h
# End Source File
# Begin Source File

SOURCE=.\Backend\RegInfoGen\regInfoGen.h
# End Source File
# Begin Source File

SOURCE=.\Backend\RegInfoGen\TargetRegInfo.h
# End Source File
# End Group
# End Group
# Begin Group "ArchModel Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ArchModel\FUandOpcodeDS.h
# End Source File
# End Group
# End Group
# Begin Group "Source files"

# PROP Default_Filter ""
# Begin Group "Parser"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Parser\errors.cpp
# End Source File
# Begin Source File

SOURCE=.\Parser\expression.cpp
# End Source File
# Begin Source File

SOURCE=.\Parser\expression.lex

!IF  "$(CFG)" == "expression dll - Win32 Release"

!ELSEIF  "$(CFG)" == "expression dll - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - ../tools/flex -oParser/lexer.cpp Parser/expression.lex
InputPath=.\Parser\expression.lex

"Parser/lexer.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\tools\flex -oParser/lexer.cpp Parser/expression.lex

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Parser\expression.y

!IF  "$(CFG)" == "expression dll - Win32 Release"

!ELSEIF  "$(CFG)" == "expression dll - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - ../tools/bison -dvt -oParser/expression.cpp Parser/expression.y
InputPath=.\Parser\expression.y

BuildCmds= \
	..\tools\bison -dvt -oParser/expression.cpp Parser/expression.y

"Parser/expression.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Parser/expression.cpp.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Parser\lexer.cpp
# End Source File
# End Group
# Begin Group "Symtab"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Symtab\symbols.cpp
# End Source File
# Begin Source File

SOURCE=.\Symtab\symtabs.cpp
# End Source File
# End Group
# Begin Group "Main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Main\main.cpp
# End Source File
# End Group
# Begin Group "IR"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\IR\architecture_section.cpp
# End Source File
# Begin Source File

SOURCE=.\IR\forwrefs.cpp
# End Source File
# Begin Source File

SOURCE=.\IR\node.cpp
# End Source File
# End Group
# Begin Group "BackEnd"

# PROP Default_Filter ""
# Begin Group "Simulator Generator Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Backend\SimulatorGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\Backend\SimulatorGeneratorSupportFuncs.cpp
# End Source File
# End Group
# Begin Group "RT Generator Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Backend\rtgen\append_dt_segments.cpp
# End Source File
# Begin Source File

SOURCE=.\Backend\rtgen\compose_transfers.cpp
# End Source File
# Begin Source File

SOURCE=.\Backend\rtgen\decorate_pipeline.cpp
# End Source File
# Begin Source File

SOURCE=.\Backend\rtgen\express_interface.cpp
# End Source File
# Begin Source File

SOURCE=.\Backend\rtgen\gen_instrs.cpp
# End Source File
# Begin Source File

SOURCE=.\Backend\rtgen\gen_pipeline.cpp
# End Source File
# Begin Source File

SOURCE=.\Backend\rtgen\GenerateRT.cpp
# End Source File
# Begin Source File

SOURCE=.\Backend\rtgen\get_restab.cpp
# End Source File
# Begin Source File

SOURCE=.\Backend\rtgen\instr_formats.cpp
# End Source File
# Begin Source File

SOURCE=.\Backend\rtgen\resource_identifier.cpp
# End Source File
# Begin Source File

SOURCE=.\Backend\rtgen\rt_database.cpp
# End Source File
# End Group
# Begin Group "MdesGen Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Backend\mdesgen\mdesgen.cpp
# End Source File
# End Group
# Begin Group "Timing Generator Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Backend\timgen\GenerateTiming.cpp
# End Source File
# End Group
# Begin Group "PredInfoGen Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Backend\PredicationInfoGen\PredicationInfoGen.cpp
# End Source File
# Begin Source File

SOURCE=.\Backend\PredicationInfoGen\PredTable.cpp
# End Source File
# Begin Source File

SOURCE=.\Backend\PredicationInfoGen\PredTableNode.cpp
# End Source File
# End Group
# Begin Group "Reg Info Gen Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Backend\RegInfoGen\regInfoGen.cpp
# End Source File
# Begin Source File

SOURCE=.\Backend\RegInfoGen\targetRegInfoGen.cpp
# End Source File
# End Group
# Begin Group "AsmFormatGen Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Backend\AsmFormatGen\asmFormatGen.cpp
# End Source File
# Begin Source File

SOURCE=.\Backend\AsmFormatGen\irDumpGen.cpp
# End Source File
# End Group
# End Group
# End Group
# End Target
# End Project
