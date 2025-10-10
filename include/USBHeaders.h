#pragma once

// Minimal Windows headers for USB operations only
#ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0601  // Windows 7+
#endif

#ifndef WINVER
    #define WINVER 0x0601
#endif

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
    #define NOMINMAX
#endif

// Prevent any socket includes
#define _WINSOCKAPI_

// Fix type conflicts - define types before Windows headers
#ifndef ULONG
typedef unsigned long ULONG;
#endif

#ifndef USHORT
typedef unsigned short USHORT;
#endif

#ifndef UCHAR
typedef unsigned char UCHAR;
#endif

// Minimal Windows API for file operations
#include <windows.h>

// USB-specific headers (include carefully to avoid conflicts)
#include <setupapi.h>

// Declare WinUSB function pointers instead of including winusb.h directly
// This avoids conflicts with system headers
typedef PVOID WINUSB_INTERFACE_HANDLE, *PWINUSB_INTERFACE_HANDLE;
