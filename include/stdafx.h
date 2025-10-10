// stdafx.h - Precompiled Header
// Pure WinAPI, no MFC dependencies

#pragma once

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define NOMINMAX

// Target Windows 7 and later
#include "targetver.h"

// C Runtime Headers
#include <malloc.h>
#include <memory.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

// Windows Headers (winsock2.h MUST be before windows.h!)
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Windows API Headers
#include <cfgmgr32.h>
#include <devguid.h>
#include <setupapi.h>
#include <shellapi.h>
#include <shlobj.h>
#include <usbiodef.h>
#include <winioctl.h>
#include <winusb.h>

// USB and Storage Headers
#include <initguid.h>
#include <ntddscsi.h>
// Note: ntddstor.h and usbioctl.h have conflicts with other headers
// Include them only in specific files that need them

// Logging macros
#ifdef _DEBUG
    #define DEBUG_PRINT(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) ((void) 0)
#endif

#define LOG_INFO(fmt, ...)  printf("[INFO]  " fmt "\n", ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) fprintf(stderr, "[ERROR] " fmt "\n", ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  fprintf(stderr, "[WARN]  " fmt "\n", ##__VA_ARGS__)

// Utility macros
#define SAFE_CLOSE_HANDLE(h)                         \
    if((h) != NULL && (h) != INVALID_HANDLE_VALUE) { \
        CloseHandle(h);                              \
        (h) = NULL;                                  \
    }
#define SAFE_FREE(p)  \
    if((p) != NULL) { \
        free(p);      \
        (p) = NULL;   \
    }
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// Link required libraries
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "winusb.lib")
#pragma comment(lib, "ws2_32.lib")
