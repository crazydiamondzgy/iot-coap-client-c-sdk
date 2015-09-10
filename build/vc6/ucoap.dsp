# Microsoft Developer Studio Project File - Name="ucoap" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ucoap - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ucoap.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ucoap.mak" CFG="ucoap - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ucoap - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ucoap - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ucoap - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ucoap - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ucoap___Win32_Debug"
# PROP BASE Intermediate_Dir "ucoap___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ucoap___Win32_Debug"
# PROP Intermediate_Dir "ucoap___Win32_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\inc" /I "..\..\inc" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "DEBUG" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "ucoap - Win32 Release"
# Name "ucoap - Win32 Debug"
# Begin Source File

SOURCE=..\..\src\coap_endpoint.c
# End Source File
# Begin Source File

SOURCE=..\..\src\coap_endpoint.h
# End Source File
# Begin Source File

SOURCE=..\..\src\coap_log.c
# End Source File
# Begin Source File

SOURCE=..\..\src\coap_log.h
# End Source File
# Begin Source File

SOURCE=..\..\src\coap_opt.c
# End Source File
# Begin Source File

SOURCE=..\..\src\coap_opt.h
# End Source File
# Begin Source File

SOURCE=..\..\src\coap_pkt.c
# End Source File
# Begin Source File

SOURCE=..\..\src\coap_pkt.h
# End Source File
# Begin Source File

SOURCE=..\..\src\coap_type.h
# End Source File
# Begin Source File

SOURCE=..\..\src\coap_util.c
# End Source File
# Begin Source File

SOURCE=..\..\src\coap_util.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mutex.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mutex.h
# End Source File
# Begin Source File

SOURCE=..\..\src\os_support.h
# End Source File
# Begin Source File

SOURCE=..\..\src\queue.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ucoap.c
# End Source File
# Begin Source File

SOURCE=..\..\inc\ucoap.h
# End Source File
# End Target
# End Project
