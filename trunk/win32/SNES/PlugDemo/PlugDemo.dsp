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
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ../ext_libs/zlib.lib /nologo /subsystem:windows /dll /machine:I386

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ../ext_libs/zlib.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "PlugDemo - Win32 Release"
# Name "PlugDemo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\C\C89\cheat.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\codeGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\COpCode.cpp
# End Source File
# Begin Source File

SOURCE=.\image.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\shared\madmonkey\md5int.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\MyPlug.cpp
# End Source File
# Begin Source File

SOURCE=.\PlugDemo.cpp
# End Source File
# Begin Source File

SOURCE=.\PlugDemo.def
# End Source File
# Begin Source File

SOURCE=.\PlugDemo.rc
# End Source File
# Begin Source File

SOURCE=..\shared\mic_\vgmz_encoder\psgcodec.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\shared\mic_\vgmz_encoder\rlecodec.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\shared\mic_\snespatcher\SnesPatcher.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\shared\mic_\snespatcher\SnesRegionPatcher.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\tracePlug.cpp
# End Source File
# Begin Source File

SOURCE=..\C\C89\utility.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\shared\mic_\vgmz_encoder\vgm2spc.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\VirtualMachine.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\C\C89\cheat.h
# End Source File
# Begin Source File

SOURCE=..\shared\mic_\vgmz_encoder\codec.h
# End Source File
# Begin Source File

SOURCE=.\codeGenerator.hpp
# End Source File
# Begin Source File

SOURCE=.\COpCode.hpp
# End Source File
# Begin Source File

SOURCE=.\image.hpp
# End Source File
# Begin Source File

SOURCE=..\shared\madmonkey\md5int.h
# End Source File
# Begin Source File

SOURCE=.\MyPlug.h
# End Source File
# Begin Source File

SOURCE=..\shared\mic_\vgmz_encoder\nullcodec.h
# End Source File
# Begin Source File

SOURCE=..\PlugBase.h
# End Source File
# Begin Source File

SOURCE=.\plugCommon.hpp
# End Source File
# Begin Source File

SOURCE=.\PlugDemo.h
# End Source File
# Begin Source File

SOURCE=..\shared\mic_\vgmz_encoder\psgcodec.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\shared\mic_\vgmz_encoder\rlecodec.h
# End Source File
# Begin Source File

SOURCE=..\shared\mic_\snespatcher\SnesPatcher.h
# End Source File
# Begin Source File

SOURCE=..\shared\mic_\snespatcher\SnesRegionPatcher.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\tracePlug.hpp
# End Source File
# Begin Source File

SOURCE=..\C\C89\utility.h
# End Source File
# Begin Source File

SOURCE=..\shared\mic_\vgmz_encoder\vgm2spc_snes_plug_interface.hpp
# End Source File
# Begin Source File

SOURCE=.\VirtualMachine.hpp
# End Source File
# Begin Source File

SOURCE=..\ext_libs\zlib.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\PlugDemo.rc2
# End Source File
# End Group
# End Target
# End Project
