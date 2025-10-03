#pragma once

// Define _WIN32_WINNT before any includes
#ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0601  // Windows 7+
#endif

#ifndef WINVER
    #define WINVER 0x0601
#endif

// Lean and mean to reduce conflicts
#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
    #define NOMINMAX
#endif

// Fix USB spec struct conflicts by defining types before any Windows includes
#ifndef ULONG
typedef unsigned long ULONG;
#endif

#ifndef USHORT
typedef unsigned short USHORT;
#endif

#ifndef UCHAR
typedef unsigned char UCHAR;
#endif

// For projects that don't need networking, exclude winsock entirely
#ifndef NO_WINSOCK
    // Prevent old winsock.h inclusion
    #ifndef _WINSOCKAPI_
        #define _WINSOCKAPI_
    #endif
    // Include winsock2 first to prevent redefinition errors
    #include <winsock2.h>
    #include <ws2tcpip.h>
#endif

// Standard Windows API
#include <windows.h>

// Skip MFC for now to avoid networking conflicts
#ifndef NO_MFC
    // MFC headers - only if needed
    #include <afxdisp.h>
    #include <afxext.h>
    #include <afxwin.h>

#endif

// Additional Windows APIs
#include <cfgmgr32.h>
#include <devguid.h>
#include <setupapi.h>
#include <winioctl.h>

// COM interfaces
#include <objbase.h>
#include <oleauto.h>

// USB and device I/O - exclude problematic usbioctl.h for now
// #include <usbioctl.h>

// Standard C++ includes
#include <algorithm>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

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
typedef void *LPVOID;
#endif

#ifndef LPCVOID
typedef const void *LPCVOID;
#endif

#ifndef HANDLE
typedef void *HANDLE;
#endif

#ifndef INVALID_HANDLE_VALUE
    #define INVALID_HANDLE_VALUE ((HANDLE) (LONG_PTR) - 1)
#endif

// Array size macro
#ifndef ARRAYSIZE
    #define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))
#endif

// Forward declarations for global GUIDs defined in Globals.cpp
extern const GUID GUID_DEVCLASS_USB;
extern const GUID GUID_DEVINTERFACE_USB_DEVICE;
