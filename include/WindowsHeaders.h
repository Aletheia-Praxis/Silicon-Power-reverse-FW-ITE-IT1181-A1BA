#pragma once

// Windows header configuration
#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0601  // Windows 7+
#endif

#ifndef WINVER
    #define WINVER 0x0601
#endif

// Exclude rarely-used stuff from Windows headers
#ifndef NOGDICAPMASKS
    #define NOGDICAPMASKS
#endif

#ifndef NOVIRTUALKEYCODES
    #define NOVIRTUALKEYCODES
#endif

#ifndef NOWINMESSAGES
    #define NOWINMESSAGES
#endif

#ifndef NOWINSTYLES
    #define NOWINSTYLES
#endif

#ifndef NOSYSMETRICS
    #define NOSYSMETRICS
#endif

#ifndef NOMENUS
    #define NOMENUS
#endif

#ifndef NOICONS
    #define NOICONS
#endif

#ifndef NOKEYSTATES
    #define NOKEYSTATES
#endif

#ifndef NOSYSCOMMANDS
    #define NOSYSCOMMANDS
#endif

#ifndef NORASTEROPS
    #define NORASTEROPS
#endif

#ifndef NOSHOWWINDOW
    #define NOSHOWWINDOW
#endif

#ifndef OEMRESOURCE
    #define OEMRESOURCE
#endif

#ifndef NOATOM
    #define NOATOM
#endif

#ifndef NOCLIPBOARD
    #define NOCLIPBOARD
#endif

#ifndef NOCOLOR
    #define NOCOLOR
#endif

#ifndef NOCTLMGR
    #define NOCTLMGR
#endif

#ifndef NODRAWTEXT
    #define NODRAWTEXT
#endif

#ifndef NOGDI
    #define NOGDI
#endif

#ifndef NOKERNEL
    #define NOKERNEL
#endif

#ifndef NOUSER
    #define NOUSER
#endif

#ifndef NONLS
    #define NONLS
#endif

#ifndef NOMB
    #define NOMB
#endif

#ifndef NOMEMMGR
    #define NOMEMMGR
#endif

#ifndef NOMETAFILE
    #define NOMETAFILE
#endif

#ifndef NOMINMAX
    #define NOMINMAX
#endif

#ifndef NOMSG
    #define NOMSG
#endif

#ifndef NOOPENFILE
    #define NOOPENFILE
#endif

#ifndef NOSCROLL
    #define NOSCROLL
#endif

#ifndef NOSERVICE
    #define NOSERVICE
#endif

#ifndef NOSOUND
    #define NOSOUND
#endif

#ifndef NOTEXTMETRIC
    #define NOTEXTMETRIC
#endif

#ifndef NOWH
    #define NOWH
#endif

#ifndef NOWINOFFSETS
    #define NOWINOFFSETS
#endif

#ifndef NOCOMM
    #define NOCOMM
#endif

#ifndef NOKANJI
    #define NOKANJI
#endif

#ifndef NOHELP
    #define NOHELP
#endif

#ifndef NOPROFILER
    #define NOPROFILER
#endif

#ifndef NODEFERWINDOWPOS
    #define NODEFERWINDOWPOS
#endif

#ifndef NOMCX
    #define NOMCX
#endif

// Windows Socket включаем ПЕРВЫМ
#include <winsock2.h>
#include <ws2tcpip.h>

// Затем windows.h
#include <windows.h>

// COM interfaces
#include <objbase.h>
#include <oleauto.h>

// Additional Windows APIs
#include <cfgmgr32.h>
#include <devguid.h>
#include <setupapi.h>
#include <winioctl.h>

// Standard C++ includes
#include <algorithm>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// MFC включаем ПОСЛЕ всех windows headers
#ifdef _AFXDLL
    #include <afxdisp.h>
    #include <afxdtctl.h>
    #include <afxext.h>
    #include <afxwin.h>
    #ifndef _AFX_NO_AFXCMN_SUPPORT
        #include <afxcmn.h>
    #endif
#endif

// USB definitions
#include <usb.h>
#include <usbioctl.h>

// Ensure basic types are defined
#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef WORD
typedef unsigned short WORD;
#endif

#ifndef DWORD
typedef unsigned long DWORD;
#endif

#ifndef BOOL
typedef int BOOL;
#endif

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

#ifndef UINT
typedef unsigned int UINT;
#endif

#ifndef LPVOID
typedef void* LPVOID;
#endif

#ifndef LPCVOID
typedef const void* LPCVOID;
#endif

#ifndef HANDLE
typedef void* HANDLE;
#endif

#ifndef INVALID_HANDLE_VALUE
    #define INVALID_HANDLE_VALUE ((HANDLE) (LONG_PTR) - 1)
#endif

// Array size macro
#ifndef ARRAYSIZE
    #define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))
#endif
