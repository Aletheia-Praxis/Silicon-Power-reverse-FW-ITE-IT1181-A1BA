#pragma once

// MFC-compatible Windows headers with proper winsock handling
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

// MFC requires Winsock2.h, so include it properly
#ifndef _WINSOCKAPI_
    #define _WINSOCKAPI_  // Prevent old winsock.h inclusion
#endif

#include <winsock2.h>
#include <ws2tcpip.h>

// Standard Windows API
#include <windows.h>

// MFC headers
#include <afxdisp.h>
#include <afxext.h>
#include <afxwin.h>
