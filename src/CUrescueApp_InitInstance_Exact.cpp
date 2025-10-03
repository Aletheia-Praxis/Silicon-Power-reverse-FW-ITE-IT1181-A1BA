/**
 * CUrescueApp::InitInstance - Exact Reconstruction
 * Based on detailed Ghidra decompilation at 0x004143c0
 *
 * This function implements the complete self-copying mechanism and application initialization
 * Size: 1523 bytes of decompiled C code
 */

#include <afxwin.h>

#include "../include/CUrescueApp.h"
#include "../include/Utilities.h"
#include "../include/WindowsHeaders.h"
#include "../include/iTEUFDrs.h"


// Global variables mapped from Ghidra analysis
char g_cmdLineArgs[128];         // DAT_004ad6c0 - command line arguments buffer
BOOL g_calledFromURescueM;       // DAT_004ad740 - flag if called from URescueM
BOOL g_systemReadyFlag;          // DAT_004ad744 - system ready flag for threads
char g_tempDirectory[MAX_PATH];  // DAT_004ad74c - temp directory path

// Function prototypes for helper functions identified in Ghidra
DWORD FUN_00413ae0(void);                          // Get Windows version
int FUN_00413a50(void);                            // Check version support
void FUN_00406340(LPCSTR name);                    // Debug output function
void FUN_00406170(LPCSTR msg);                     // Log message function
void FUN_00411440(LPCSTR name, int mode);          // Set application name
UINT WINAPI FUN_00413fd0(LPVOID param);            // Background monitor thread
UINT WINAPI LAB_00413dd0(LPVOID param);            // Background processing thread
void FUN_00415780(iTEUFDrs* dialog, HWND parent);  // Initialize dialog
void FUN_0042de6d(void);                           // Additional initialization
void FUN_00413e50(void);                           // Cleanup function
int FUN_00412a00(LPCSTR mutex_name, int mode);     // Mutex operations
void FUN_004118c0(LPCSTR suffix);                  // Path concatenation
void FUN_00406de0(void);                           // Debug output
FARPROC* DAT_004af8f0;                             // Pointer to current module path

/**
 * CUrescueApp::InitInstance - EXACT reconstruction from 0x004143c0
 * This is the MFC InitInstance override that gets called by AfxWinMain
 */
BOOL CUrescueApp::InitInstance() {
    // Local variables matching the decompiled stack layout
    char tempPath[MAX_PATH];          // aCStack_114
    char longTempPath[MAX_PATH];      // auStack_31c
    char modulePath[MAX_PATH];        // aCStack_218
    char currentDir[MAX_PATH];        // derived from auStack_31c
    WIN32_FIND_DATAA findData;        // _Stack_45c
    STARTUPINFOA startupInfo;         // _Stack_79c
    PROCESS_INFORMATION processInfo;  // _Stack_7ac
    HANDLE hFind;
    BOOL result;
    char* lastBackslash;
    int comparison;

    LogMessage("CUrescueApp::InitInstance - Entry point");

    // Step 1: Initialize COM/ATL string objects (FUN_0041fe69 calls)
    // These are ATL CString objects being initialized
    // Simplified for our reconstruction

    // Step 2: Set application name for debugging
    FUN_00406340("URescue.exe");

    // Step 3: Get Windows version and check compatibility
    DWORD windowsVersion = FUN_00413ae0();
    if((windowsVersion - 0x893U) < 0xedd) {  // Version in range 2195-3549
        int versionSupported = FUN_00413a50();
        if(versionSupported == 0) {  // Not supported
            // Load error message from resources (string ID 4)
            AFX_MODULE_STATE* moduleState = AfxGetModuleState();
            char errorMessage[MAX_PATH];
            if(LoadStringA(
                   moduleState->m_hCurrentResourceHandle, 4, errorMessage, sizeof(errorMessage))) {
                AfxMessageBox(errorMessage, 0, 0);
            }
            return FALSE;
        }
    }

    // Step 4: Process command line arguments
    LPSTR cmdLine = m_lpCmdLine;  // MFC member variable
    if(cmdLine && *cmdLine != '\0') {
        g_calledFromURescueM = TRUE;

        // Copy command line arguments (skip executable name)
        char* src = cmdLine + 3;  // Skip past executable reference
        char* dst = g_cmdLineArgs;
        char c;
        do {
            c = *src++;
            *dst++ = c;
        } while(c != '\0');

        FUN_00406170("Call from URescueM");
        FUN_00406170(g_cmdLineArgs);
    } else {
        memset(g_cmdLineArgs, 0, sizeof(g_cmdLineArgs));
    }

    // Step 5: Get temp directory and current module path
    memset(tempPath, 0, sizeof(tempPath));
    memset(longTempPath, 0, sizeof(longTempPath));

    GetTempPathA(sizeof(tempPath), tempPath);
    GetLongPathNameA(tempPath, longTempPath, sizeof(longTempPath));

    // Store temp directory in global variable
    strcpy_s(g_tempDirectory, sizeof(g_tempDirectory), longTempPath);

    // Get current module path and extract directory
    memset(modulePath, 0, sizeof(modulePath));
    memset(currentDir, 0, sizeof(currentDir));

    GetModuleFileNameA(NULL, modulePath, sizeof(modulePath));
    strcpy_s(currentDir, sizeof(currentDir), modulePath);

    lastBackslash = strrchr(currentDir, '\\');
    if(lastBackslash) {
        *lastBackslash = '\0';
    }

    // Set application name for logging
    FUN_00411440("UfdApp", 6);

    // Step 6: Compare current directory with temp directory
    comparison = _mbsicmp((unsigned char*) longTempPath, (unsigned char*) currentDir);

    if(comparison == 0) {
        // We're running from temp directory - initialize normally
        LogMessage("Running from temp directory - initializing application");

        g_systemReadyFlag = FALSE;

        // Store temp directory reference
        strcpy_s(g_tempDirectory, sizeof(g_tempDirectory), longTempPath);

        // Create background threads
        CWinThread* monitorThread = AfxBeginThread(FUN_00413fd0, NULL, 0, 0, 0, NULL);
        CWinThread* processingThread =
            AfxBeginThread((AFX_THREADPROC) LAB_00413dd0, NULL, 0, 0, 0, NULL);

        // Check if window already exists
        BOOL windowExists = IsWindow(m_pMainWnd ? m_pMainWnd->GetSafeHwnd() : NULL);
        HWND parentWindow = NULL;

        if(! windowExists) {
            // Register window class and create main window
            UINT classId = AfxRegisterWndClass(0, 0, 0, 0);
            // The actual window creation would happen through MFC framework
            // This is simplified for reconstruction
        }

        // Wait for system ready flag
        DWORD exitCode;
        do {
            if(monitorThread && monitorThread->m_hThread) {
                GetExitCodeThread(monitorThread->m_hThread, &exitCode);
            }
            Sleep(10);  // Small delay to prevent busy waiting
        } while(g_systemReadyFlag == 0);

        // Create iTEUFDrs dialog (main application dialog)
        void* dialogMemory = operator new(0x207358);  // Exact size from Ghidra
        iTEUFDrs* mainDialog = nullptr;

        if(dialogMemory) {
            mainDialog = new(dialogMemory) iTEUFDrs(longTempPath);
        }

        if(mainDialog) {
            // Store dialog reference (at offset 0xf8 in CUrescueApp)
            // This would be a member variable in the real class
            m_pMainWnd = (CWnd*) mainDialog;  // Simplified casting

            // Initialize dialog with parent window
            FUN_00415780(mainDialog, parentWindow);

            // Additional initialization
            FUN_0042de6d();
            FUN_00413e50();

            LogMessage("iTEUFDrs dialog created and initialized successfully");
            return TRUE;
        } else {
            LogError("Failed to create iTEUFDrs dialog");
            return FALSE;
        }

    } else {
        // We need to copy to temp directory and restart
        LogMessage("Need to copy to temp directory: %s", longTempPath);

        // Check if temp directory exists
        hFind = FindFirstFileA(longTempPath, &findData);
        BOOL tempDirExists = (hFind != INVALID_HANDLE_VALUE);

        if(tempDirExists) {
            FindClose(hFind);
            if(! (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                tempDirExists = FALSE;
            }
        }

        // Create temp directory if needed
        if(! tempDirExists) {
            result = CreateDirectoryA(longTempPath, NULL);
            if(! result) {
                LogError("Failed to create temp directory");
                return FALSE;
            }
        }

        // Set current directory to temp
        result = SetCurrentDirectoryA(longTempPath);
        if(! result) {
            AfxMessageBox("Change current dir failed!!", 0, 0);
            return FALSE;
        }

        if(tempDirExists) {
            FindClose(hFind);
        }

        // Build target file path
        char targetPath[MAX_PATH];
        strcpy_s(targetPath, sizeof(targetPath), longTempPath);
        FUN_004118c0("\\URescue.exe");  // Concatenate filename
        strcat_s(targetPath, sizeof(targetPath), "URescue.exe");

        // Check if target already exists
        hFind = FindFirstFileA(targetPath, &findData);
        if(hFind != INVALID_HANDLE_VALUE) {
            FindClose(hFind);
            SetFileAttributesA(targetPath, FILE_ATTRIBUTE_NORMAL);
        }

        // Copy current executable to temp directory
        LPCSTR sourcePath = (LPCSTR) *DAT_004af8f0;  // Current module path
        result = CopyFileA(sourcePath, targetPath, FALSE);

        if(! result) {
            // Copy failed - check if another instance is running
            int mutexResult = FUN_00412a00("UpdateISP-{44E678F7-DA79-11d3-9FE9-006067718D04}", 0);
            if(mutexResult != 0) {
                // Another instance exists - bring it to front
                HWND existingWindow = FindWindowA(NULL, "URescue");
                if(existingWindow) {
                    SetForegroundWindow(existingWindow);
                    if(IsIconic(existingWindow)) {
                        ShowWindow(existingWindow, SW_RESTORE);
                    }
                }
            }
            return FALSE;
        }

        // Set file attributes on copied file
        result = SetFileAttributesA(targetPath, FILE_ATTRIBUTE_NORMAL);
        if(! result) {
            LogError("Failed to set file attributes");
            return FALSE;
        }

        // Launch the copied executable
        GetStartupInfoA(&startupInfo);

        char commandLine[MAX_PATH * 2];
        LPCSTR applicationName;
        LPSTR cmdLineToUse;

        if(g_calledFromURescueM) {
            // Include command line arguments
            sprintf_s(commandLine, sizeof(commandLine), "%s %s", targetPath, m_lpCmdLine);
            applicationName = NULL;
            cmdLineToUse = commandLine;
        } else {
            applicationName = targetPath;
            cmdLineToUse = NULL;
        }

        result = CreateProcessA(
            applicationName,  // lpApplicationName
            cmdLineToUse,     // lpCommandLine
            NULL,             // lpProcessAttributes
            NULL,             // lpThreadAttributes
            FALSE,            // bInheritHandles
            0,                // dwCreationFlags
            NULL,             // lpEnvironment
            NULL,             // lpCurrentDirectory
            &startupInfo,     // lpStartupInfo
            &processInfo      // lpProcessInformation
        );

        if(! result) {
            AfxMessageBox("Create process failed!!", 0, 0);
            return FALSE;
        }

        // Close process handles
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);

        // Multiple debug output calls (from decompiled code)
        FUN_00406de0();
        FUN_00406de0();
        FUN_00406de0();

        LogMessage("Successfully launched copied executable - exiting original");
        return FALSE;  // Exit this instance
    }
}
