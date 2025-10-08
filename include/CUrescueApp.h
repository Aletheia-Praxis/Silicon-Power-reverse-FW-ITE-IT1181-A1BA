#pragma once

#include "stdafx.h"

/**
 * CUrescueApp - Main MFC Application Class
 * Reconstructed from analysis at 0x00413960 (constructor) and 0x004143c0 (InitInstance)
 */
class CUrescueApp : public CWinApp {
public:
    CUrescueApp();
    virtual ~CUrescueApp();

    // Override InitInstance (called by MFC framework through vtable at offset +0x50)
    virtual BOOL InitInstance() override;

private:
    // Private implementation - actual member layout determined by Ghidra analysis
};

// Global application object (required by MFC)
extern CUrescueApp theApp;

// C interface functions
extern "C" {
/**
 * Main application initialization function
 * Reconstructed from CUrescueApp::InitInstance at 0x004143c0
 */
BOOL CUrescueApp_InitInstance();

// Entry point is now standard MFC WinMain in main.cpp

/**
 * Background thread functions
 */
UINT WINAPI BackgroundMonitorThread(LPVOID pParam);
UINT WINAPI BackgroundProcessingThread(LPVOID pParam);

/**
 * Utility functions for Windows version checking and system setup
 */
DWORD GetWindowsVersion();
BOOL CheckWindowsVersionSupport();
void DebugFlagRegistryCheck(const char* processName);
DWORD PrintWindowsVersionInfo();
int CheckAdminTokenMembership();

/**
 * Boot code file handler thread function (FUN_00413fd0)
 */
UINT WINAPI BootCodeFileHandler(LPVOID pParam);

/**
 * Helper functions for process and dialog management
 */
HANDLE CreateGlobalMutex(const char* mutexName, DWORD flags);
void* CUrescueDlg_Constructor(void* memory, void* deviceManager);
INT_PTR RunModalDialogWithResource(void* dialog);
void DialogDestructor(void* dialog);
void* iTEUFDrs__iTEUFDrs(void* memory);

/**
 * Debug logging functions
 */
void debug_log_message(const char* message);
}

// Global variables from CUrescueApp analysis
extern char g_cmdLineArgs[128];         // DAT_004ad6c0
extern BOOL g_calledFromURescueM;       // DAT_004ad740
extern BOOL g_systemReadyFlag;          // DAT_004ad744
extern char g_tempDirectory[MAX_PATH];  // DAT_004ad74c
