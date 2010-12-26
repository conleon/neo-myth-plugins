# Microsoft Developer Studio Project File - Name="PlugDemo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=PlugDemo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PlugDemo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PlugDemo.mak" CFG="PlugDemo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PlugDemo - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "PlugDemo - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PlugDemo - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 mfcs42.lib MSVCRT.lib /nologo /subsystem:windows /dll /machine:I386

!ELSEIF  "$(CFG)" == "PlugDemo - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 mfcs42d.lib MSVCRTd.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "PlugDemo - Win32 Release"
# Name "PlugDemo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\SuperN64_Engine\checksum.c

!IF  "$(CFG)" == "PlugDemo - Win32 Release"

# ADD CPP /Ob0
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "PlugDemo - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\CP.c

!IF  "$(CFG)" == "PlugDemo - Win32 Release"

# ADD CPP /Ob0
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "PlugDemo - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\crc.c

!IF  "$(CFG)" == "PlugDemo - Win32 Release"

# ADD CPP /Ob0
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "PlugDemo - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\database.c

!IF  "$(CFG)" == "PlugDemo - Win32 Release"

# ADD CPP /Ob0
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "PlugDemo - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\E16.c

!IF  "$(CFG)" == "PlugDemo - Win32 Release"

# ADD CPP /Ob0
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "PlugDemo - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\E4.c

!IF  "$(CFG)" == "PlugDemo - Win32 Release"

# ADD CPP /Ob0
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "PlugDemo - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\FR.c

!IF  "$(CFG)" == "PlugDemo - Win32 Release"

# ADD CPP /Ob0
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "PlugDemo - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\io.c

!IF  "$(CFG)" == "PlugDemo - Win32 Release"

# ADD CPP /Ob0
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "PlugDemo - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\lex_comp.c

!IF  "$(CFG)" == "PlugDemo - Win32 Release"

# ADD CPP /Ob0
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "PlugDemo - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\lexer.c

!IF  "$(CFG)" == "PlugDemo - Win32 Release"

# ADD CPP /Ob0
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "PlugDemo - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\md5.c

!IF  "$(CFG)" == "PlugDemo - Win32 Release"

# ADD CPP /Ob0
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "PlugDemo - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\md5_IO.c

!IF  "$(CFG)" == "PlugDemo - Win32 Release"

# ADD CPP /Ob0
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "PlugDemo - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MyPlug.cpp

!IF  "$(CFG)" == "PlugDemo - Win32 Release"

# ADD CPP /Ob0

!ELSEIF  "$(CFG)" == "PlugDemo - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\n64_image.c

!IF  "$(CFG)" == "PlugDemo - Win32 Release"

# ADD CPP /Ob0
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "PlugDemo - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\NEO_SN64Engine_DynaStatsShared.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\patch_IO.c

!IF  "$(CFG)" == "PlugDemo - Win32 Release"

# ADD CPP /Ob0
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "PlugDemo - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PlugDemo.cpp

!IF  "$(CFG)" == "PlugDemo - Win32 Release"

# ADD CPP /Ob0

!ELSEIF  "$(CFG)" == "PlugDemo - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PlugDemo.def
# End Source File
# Begin Source File

SOURCE=.\PlugDemo.rc
# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\sharedBuffers.c

!IF  "$(CFG)" == "PlugDemo - Win32 Release"

# ADD CPP /Ob0
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "PlugDemo - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\SR.c

!IF  "$(CFG)" == "PlugDemo - Win32 Release"

# ADD CPP /Ob0
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "PlugDemo - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "PlugDemo - Win32 Release"

# ADD CPP /Ob0 /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "PlugDemo - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\utility.c

!IF  "$(CFG)" == "PlugDemo - Win32 Release"

# ADD CPP /Ob0
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "PlugDemo - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\xPS.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\SuperN64_Engine\checksum.h
# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\crc.h
# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\database.h
# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\io.h
# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\lexer.h
# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\md5.h
# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\md5_IO.h
# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\memoryDef.h
# End Source File
# Begin Source File

SOURCE=.\MyPlug.h
# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\n64_image.h
# End Source File
# Begin Source File

SOURCE=..\NEO_SN64Engine_DynaStatsShared.h
# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\NEO_Types.h
# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\patch_IO.h
# End Source File
# Begin Source File

SOURCE=.\PlugDemo.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\sharedBuffers.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\utility.h
# End Source File
# Begin Source File

SOURCE=..\SuperN64_Engine\xPS.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\PlugDemo.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
