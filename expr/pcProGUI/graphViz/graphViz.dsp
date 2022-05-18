# Microsoft Developer Studio Project File - Name="graphViz" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=graphViz - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "graphViz.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "graphViz.mak" CFG="graphViz - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "graphViz - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "graphViz - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "graphViz - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "./include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386

!ELSEIF  "$(CFG)" == "graphViz - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "graphViz"
# PROP BASE Intermediate_Dir "graphViz"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../pcProDll/bin"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D yylval=aglval /D yyparse=agparse /D yylex=aglex /D yyerror=agerror /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /pdb:"../../acesMIPSDll/bin/graphViz.pdb" /debug /machine:I386 /def:".\graphViz.def" /out:"../../acesMIPSDll/bin/graphViz.dll" /implib:"../../acesMIPSDll/bin/graphViz.lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "graphViz - Win32 Release"
# Name "graphViz - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "libgraph Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libgraph\attribs.cpp
# End Source File
# Begin Source File

SOURCE=.\libgraph\edge.cpp
# End Source File
# Begin Source File

SOURCE=.\libgraph\graph.cpp
# End Source File
# Begin Source File

SOURCE=.\libgraph\graphio.cpp
# End Source File
# Begin Source File

SOURCE=.\libgraph\lexer.cpp
# End Source File
# Begin Source File

SOURCE=.\libgraph\node.cpp
# End Source File
# Begin Source File

SOURCE=.\libgraph\parser.cpp
# End Source File
# Begin Source File

SOURCE=.\libgraph\parser.y
# End Source File
# Begin Source File

SOURCE=.\libgraph\refstr.cpp
# End Source File
# Begin Source File

SOURCE=.\libgraph\trie.cpp
# End Source File
# Begin Source File

SOURCE=.\libgraph\triefa.cpp

!IF  "$(CFG)" == "graphViz - Win32 Release"

!ELSEIF  "$(CFG)" == "graphViz - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "libcdt Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libcdt\FEATURE\cdt
# End Source File
# Begin Source File

SOURCE=.\libcdt\dtclose.cpp
# End Source File
# Begin Source File

SOURCE=.\libcdt\dtdisc.cpp
# End Source File
# Begin Source File

SOURCE=.\libcdt\dtextract.cpp
# End Source File
# Begin Source File

SOURCE=.\libcdt\dtflatten.cpp
# End Source File
# Begin Source File

SOURCE=.\libcdt\dthash.cpp
# End Source File
# Begin Source File

SOURCE=.\libcdt\dtlist.cpp
# End Source File
# Begin Source File

SOURCE=.\libcdt\dtmethod.cpp
# End Source File
# Begin Source File

SOURCE=.\libcdt\dtopen.cpp
# End Source File
# Begin Source File

SOURCE=.\libcdt\dtrenew.cpp
# End Source File
# Begin Source File

SOURCE=.\libcdt\dtrestore.cpp
# End Source File
# Begin Source File

SOURCE=.\libcdt\dtsize.cpp
# End Source File
# Begin Source File

SOURCE=.\libcdt\dtstat.cpp
# End Source File
# Begin Source File

SOURCE=.\libcdt\dtstrhash.cpp
# End Source File
# Begin Source File

SOURCE=.\libcdt\dttree.cpp
# End Source File
# Begin Source File

SOURCE=.\libcdt\dtview.cpp
# End Source File
# Begin Source File

SOURCE=.\libcdt\dtwalk.cpp
# End Source File
# End Group
# Begin Group "libdot Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libdot\acyclic.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\class1.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\class2.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\cluster.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\colxlate.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\conc.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\decomp.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\dot.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\emit.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\fastgr.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\flat.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\gifgen.cpp

!IF  "$(CFG)" == "graphViz - Win32 Release"

!ELSEIF  "$(CFG)" == "graphViz - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libdot\globals.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\hpglgen.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\input.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\ismapgen.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\mifgen.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\mincross.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\ns.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\output.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\picgen.cpp

!IF  "$(CFG)" == "graphViz - Win32 Release"

!ELSEIF  "$(CFG)" == "graphViz - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\libdot\position.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\postproc.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\psgen.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\rank.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\routespl.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\sameport.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\shapes.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\splines.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\timing.cpp
# End Source File
# Begin Source File

SOURCE=.\libdot\utils.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\graphViz.cpp
# End Source File
# Begin Source File

SOURCE=.\graphViz.def

!IF  "$(CFG)" == "graphViz - Win32 Release"

!ELSEIF  "$(CFG)" == "graphViz - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\graphViz.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "misc Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\include\cdt.h
# End Source File
# Begin Source File

SOURCE=.\include\freetype.h
# End Source File
# Begin Source File

SOURCE=.\include\gd.h
# End Source File
# Begin Source File

SOURCE=.\include\gdttf.h
# End Source File
# Begin Source File

SOURCE=.\include\graph.h
# End Source File
# End Group
# Begin Group "libgraph Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libgraph\libgraph.h
# End Source File
# Begin Source File

SOURCE=.\libgraph\parser.h
# End Source File
# Begin Source File

SOURCE=.\libgraph\triefa.h
# End Source File
# End Group
# Begin Group "libcdt Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libcdt\dthdr.h
# End Source File
# End Group
# Begin Group "libdot Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libdot\colortbl.h
# End Source File
# Begin Source File

SOURCE=.\libdot\const.h
# End Source File
# Begin Source File

SOURCE=.\libdot\dot.h
# End Source File
# Begin Source File

SOURCE=.\libdot\globals.h
# End Source File
# Begin Source File

SOURCE=.\libdot\macros.h
# End Source File
# Begin Source File

SOURCE=.\libdot\procs.h
# End Source File
# Begin Source File

SOURCE=.\libdot\ps.h
# End Source File
# Begin Source File

SOURCE=.\libdot\types.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\graphViz.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\graphViz.rc2
# End Source File
# End Group
# End Target
# End Project
