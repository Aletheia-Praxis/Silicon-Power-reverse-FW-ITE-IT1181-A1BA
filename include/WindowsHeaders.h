#pragma once

// Use stdafx.h for all Windows includes to avoid conflicts
// This file is kept for backward compatibility but defers to stdafx.h

#include "stdafx.h"

// Additional types and definitions not in stdafx.h

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

// COM interfaces - include after winsock2 to avoid conflicts
#ifndef _WINSOCKAPI_
    #define _WINSOCKAPI_  // Prevent winsock.h inclusion from COM headers
#endif
#include <objbase.h>
#include <oleauto.h>

// Standard C++ includes
#include <algorithm>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Forward declarations for global GUIDs defined in Globals.cpp
extern const GUID GUID_DEVCLASS_USB;
extern const GUID GUID_DEVINTERFACE_USB_DEVICE;
