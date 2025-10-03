#pragma once

#include "WindowsHeaders.h"

#ifdef __cplusplus
extern "C" {
#endif

// Global variables from CUrescueApp
extern char g_cmdLineArgs[128];
extern BOOL g_calledFromURescueM;
extern BOOL g_systemReadyFlag;
extern char g_tempDirectory[MAX_PATH];

/**
 * Main application initialization function
 * Reconstructed from CUrescueApp::InitInstance at 0x004143c0
 */
BOOL CUrescueApp_InitInstance();

/**
 * Background thread functions
 */
UINT WINAPI BackgroundMonitorThread(LPVOID pParam);
UINT WINAPI BackgroundProcessingThread(LPVOID pParam);

/**
 * Utility functions for Windows version checking
 */
DWORD GetWindowsVersion();
BOOL CheckWindowsVersionSupport();

#ifdef __cplusplus
}
#endif
