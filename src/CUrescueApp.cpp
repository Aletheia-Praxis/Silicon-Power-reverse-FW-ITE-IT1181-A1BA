#include "../include/ErrorHandler.h"
#include "../include/URescueMain.h"
#include "../include/Utilities.h"
#include "../include/WindowsHeaders.h"
#include "../include/iTEUFDrs.h"

// Global variables based on Ghidra analysis
extern char g_cmdLineArgs[128];         // DAT_004ad6c0
extern BOOL g_calledFromURescueM;       // DAT_004ad740
extern BOOL g_systemReadyFlag;          // DAT_004ad744
extern char g_tempDirectory[MAX_PATH];  // DAT_004ad74c

// Background thread function prototypes
UINT WINAPI BackgroundMonitorThread(LPVOID pParam);
UINT WINAPI BackgroundProcessingThread(LPVOID pParam);

/**
 * Main application initialization function - reconstructed from CUrescueApp::InitInstance
 * Located at 0x004143c0 in the original binary
 *
 * This implements a sophisticated self-copying mechanism:
 * 1. Checks Windows version compatibility
 * 2. Processes command line arguments from URescueM
 * 3. Compares current location with temp directory
 * 4. If not in temp: copies to temp, launches copy, and exits
 * 5. If in temp: initializes UI and runs normally
 * 6. Creates background monitoring threads
 * 7. Prevents multiple instances
 */
BOOL CUrescueApp_InitInstance() {
    LogMessage("CUrescueApp::InitInstance - Starting application initialization");

    // Initialize global variables
    memset(g_cmdLineArgs, 0, sizeof(g_cmdLineArgs));
    g_calledFromURescueM = FALSE;
    g_systemReadyFlag = FALSE;
    memset(g_tempDirectory, 0, sizeof(g_tempDirectory));

    // Step 1: Check Windows version compatibility
    // Based on Ghidra analysis: checks version range 0x893-0xddd (2195-3549)
    DWORD windowsVersion = GetWindowsVersion();
    if(windowsVersion >= 0x893 && windowsVersion <= 0xddd) {
        BOOL isVersionSupported = CheckWindowsVersionSupport();
        if(! isVersionSupported) {
            // Load error message from resources (string ID 4)
            char errorMessage[260];
            if(LoadStringA(GetModuleHandle(NULL), 4, errorMessage, sizeof(errorMessage))) {
                MessageBoxA(NULL, errorMessage, "URescue", MB_OK | MB_ICONERROR);
            } else {
                MessageBoxA(NULL, "Unsupported Windows version", "URescue", MB_OK | MB_ICONERROR);
            }
            LogError(
                "CUrescueApp::InitInstance - Unsupported Windows version: 0x%X", windowsVersion);
            return FALSE;
        }
    }

    // Step 2: Process command line arguments
    LPSTR cmdLine = GetCommandLineA();
    if(cmdLine && *cmdLine != '\0') {
        g_calledFromURescueM = TRUE;

        // Skip executable name, find first argument
        LPSTR firstArg = cmdLine;
        if(*firstArg == '"') {
            firstArg = strchr(firstArg + 1, '"');
            if(firstArg)
                firstArg++;
        } else {
            firstArg = strchr(firstArg, ' ');
        }

        if(firstArg) {
            while(*firstArg == ' ')
                firstArg++;
            if(*firstArg) {
                strncpy_s(g_cmdLineArgs, sizeof(g_cmdLineArgs), firstArg, _TRUNCATE);
                LogMessage(
                    "CUrescueApp::InitInstance - Called from URescueM with args: %s",
                    g_cmdLineArgs);
            }
        }
    }

    // Step 3: Get paths and determine if we need to self-copy
    char tempPath[MAX_PATH];
    char longTempPath[MAX_PATH];
    char currentModulePath[MAX_PATH];
    char currentModuleDir[MAX_PATH];

    // Get temp directory
    memset(tempPath, 0, sizeof(tempPath));
    if(! GetTempPathA(sizeof(tempPath), tempPath)) {
        LogError("CUrescueApp::InitInstance - Failed to get temp path");
        return FALSE;
    }

    // Get long path name for temp directory
    memset(longTempPath, 0, sizeof(longTempPath));
    if(! GetLongPathNameA(tempPath, longTempPath, sizeof(longTempPath))) {
        strcpy_s(longTempPath, sizeof(longTempPath), tempPath);
    }

    // Store temp directory globally
    strcpy_s(g_tempDirectory, sizeof(g_tempDirectory), longTempPath);

    // Get current module path and directory
    memset(currentModulePath, 0, sizeof(currentModulePath));
    memset(currentModuleDir, 0, sizeof(currentModuleDir));

    if(! GetModuleFileNameA(NULL, currentModulePath, sizeof(currentModulePath))) {
        LogError("CUrescueApp::InitInstance - Failed to get module filename");
        return FALSE;
    }

    strcpy_s(currentModuleDir, sizeof(currentModuleDir), currentModulePath);
    char* lastBackslash = strrchr(currentModuleDir, '\\');
    if(lastBackslash) {
        *lastBackslash = '\0';
    }

    LogMessage("CUrescueApp::InitInstance - Current directory: %s", currentModuleDir);
    LogMessage("CUrescueApp::InitInstance - Temp directory: %s", longTempPath);

    // Step 4: Compare paths and decide on self-copying
    int pathComparison = _stricmp(longTempPath, currentModuleDir);

    if(pathComparison == 0) {
        // We're already running from temp directory - initialize normally
        LogMessage("CUrescueApp::InitInstance - Running from temp directory - initializing UI");

        g_systemReadyFlag = FALSE;

        // Create background threads
        LogMessage("CUrescueApp::InitInstance - Creating background threads");

        HANDLE hMonitorThread =
            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) BackgroundMonitorThread, NULL, 0, NULL);

        HANDLE hProcessingThread = CreateThread(
            NULL, 0, (LPTHREAD_START_ROUTINE) BackgroundProcessingThread, NULL, 0, NULL);

        if(! hMonitorThread || ! hProcessingThread) {
            LogError("CUrescueApp::InitInstance - Failed to create background threads");
            if(hMonitorThread)
                CloseHandle(hMonitorThread);
            if(hProcessingThread)
                CloseHandle(hProcessingThread);
            return FALSE;
        }

        // Wait for system ready flag
        DWORD exitCode;
        do {
            if(hMonitorThread) {
                GetExitCodeThread(hMonitorThread, &exitCode);
            }
            Sleep(100);
        } while(! g_systemReadyFlag);

        LogMessage("CUrescueApp::InitInstance - System ready flag set");

        // Create main iTEUFDrs dialog
        iTEUFDrs* pMainDialog = new iTEUFDrs(longTempPath);
        if(! pMainDialog) {
            LogError("CUrescueApp::InitInstance - Failed to create iTEUFDrs dialog");
            if(hMonitorThread)
                CloseHandle(hMonitorThread);
            if(hProcessingThread)
                CloseHandle(hProcessingThread);
            return FALSE;
        }

        // Store dialog pointer globally and initialize
        // This would be stored in the CWinApp-derived class
        LogMessage("CUrescueApp::InitInstance - iTEUFDrs dialog created successfully");

        // Initialize additional UI components and message loop
        // The actual UI loop would be handled by the framework

        // Cleanup thread handles
        if(hMonitorThread)
            CloseHandle(hMonitorThread);
        if(hProcessingThread)
            CloseHandle(hProcessingThread);

        return TRUE;

    } else {
        // We need to copy to temp directory and restart
        LogMessage("CUrescueApp::InitInstance - Need to copy to temp directory and restart");

        // Step 5: Self-copying mechanism
        WIN32_FIND_DATAA findData;
        HANDLE hFind = FindFirstFileA(longTempPath, &findData);

        BOOL tempDirExists = (hFind != INVALID_HANDLE_VALUE);
        if(tempDirExists) {
            FindClose(hFind);
            if(! (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                tempDirExists = FALSE;
            }
        }

        // Create temp directory if it doesn't exist
        if(! tempDirExists) {
            if(! CreateDirectoryA(longTempPath, NULL)) {
                LogError(
                    "CUrescueApp::InitInstance - Failed to create temp directory: %s",
                    longTempPath);
                return FALSE;
            }
        }

        // Set current directory to temp
        if(! SetCurrentDirectoryA(longTempPath)) {
            LogError("CUrescueApp::InitInstance - Failed to set current directory to temp");
            return FALSE;
        }

        // Build target executable path
        char targetPath[MAX_PATH];
        strcpy_s(targetPath, sizeof(targetPath), longTempPath);
        strcat_s(targetPath, sizeof(targetPath), "\\URescue.exe");

        // Check if target already exists and remove attributes
        hFind = FindFirstFileA(targetPath, &findData);
        if(hFind != INVALID_HANDLE_VALUE) {
            FindClose(hFind);
            SetFileAttributesA(targetPath, FILE_ATTRIBUTE_NORMAL);
        }

        // Copy current executable to temp directory
        if(! CopyFileA(currentModulePath, targetPath, FALSE)) {
            DWORD copyError = GetLastError();
            LogError("CUrescueApp::InitInstance - Copy failed with error: %lu", copyError);

            // Check if another instance is already running
            if(copyError == ERROR_SHARING_VIOLATION || copyError == ERROR_ACCESS_DENIED) {
                HWND existingWindow = FindWindowA(NULL, "URescue");
                if(existingWindow) {
                    LogMessage(
                        "CUrescueApp::InitInstance - Found existing URescue window, bringing to "
                        "front");
                    SetForegroundWindow(existingWindow);

                    if(IsIconic(existingWindow)) {
                        ShowWindow(existingWindow, SW_RESTORE);
                    }
                }
            }
            return FALSE;
        }

        // Set file attributes
        if(! SetFileAttributesA(targetPath, FILE_ATTRIBUTE_NORMAL)) {
            LogError("CUrescueApp::InitInstance - Failed to set file attributes");
            return FALSE;
        }

        // Launch the copied executable
        STARTUPINFOA startupInfo;
        PROCESS_INFORMATION processInfo;

        memset(&startupInfo, 0, sizeof(startupInfo));
        startupInfo.cb = sizeof(startupInfo);

        char commandLine[512];
        if(g_calledFromURescueM) {
            // Include command line arguments
            sprintf_s(commandLine, sizeof(commandLine), "%s %s", targetPath, g_cmdLineArgs);

            BOOL processCreated = CreateProcessA(
                NULL,          // lpApplicationName
                commandLine,   // lpCommandLine
                NULL,          // lpProcessAttributes
                NULL,          // lpThreadAttributes
                FALSE,         // bInheritHandles
                0,             // dwCreationFlags
                NULL,          // lpEnvironment
                NULL,          // lpCurrentDirectory
                &startupInfo,  // lpStartupInfo
                &processInfo   // lpProcessInformation
            );

            if(processCreated) {
                CloseHandle(processInfo.hProcess);
                CloseHandle(processInfo.hThread);
                LogMessage(
                    "CUrescueApp::InitInstance - Successfully launched copied executable with "
                    "args");
            } else {
                LogError("CUrescueApp::InitInstance - Failed to create process with args");
            }
        } else {
            // No command line arguments
            BOOL processCreated = CreateProcessA(
                targetPath,    // lpApplicationName
                NULL,          // lpCommandLine
                NULL,          // lpProcessAttributes
                NULL,          // lpThreadAttributes
                FALSE,         // bInheritHandles
                0,             // dwCreationFlags
                NULL,          // lpEnvironment
                NULL,          // lpCurrentDirectory
                &startupInfo,  // lpStartupInfo
                &processInfo   // lpProcessInformation
            );

            if(processCreated) {
                CloseHandle(processInfo.hProcess);
                CloseHandle(processInfo.hThread);
                LogMessage("CUrescueApp::InitInstance - Successfully launched copied executable");
            } else {
                LogError("CUrescueApp::InitInstance - Failed to create process");
            }
        }

        // Exit this instance since we've launched the copy
        LogMessage("CUrescueApp::InitInstance - Exiting original instance");
        return FALSE;  // This will cause the original instance to exit
    }
}

/**
 * Background monitor thread - monitors device status and system state
 * Based on function at FUN_00413fd0 in Ghidra analysis
 */
UINT WINAPI BackgroundMonitorThread(LPVOID pParam) {
    UNREFERENCED_PARAMETER(pParam);

    LogMessage("BackgroundMonitorThread - Started");

    // Monitor loop - simplified implementation
    for(int i = 0; i < 100; i++) {
        Sleep(50);

        // Simulate monitor work
        if(i == 50) {
            g_systemReadyFlag = TRUE;
            LogMessage("BackgroundMonitorThread - Set system ready flag");
        }
    }

    LogMessage("BackgroundMonitorThread - Exiting");
    return 0;
}

/**
 * Background processing thread - handles background tasks
 * Based on function at LAB_00413dd0 in Ghidra analysis
 */
UINT WINAPI BackgroundProcessingThread(LPVOID pParam) {
    UNREFERENCED_PARAMETER(pParam);

    LogMessage("BackgroundProcessingThread - Started");

    // Processing loop - simplified implementation
    for(int i = 0; i < 100; i++) {
        Sleep(75);
        // Simulate background processing work
    }

    LogMessage("BackgroundProcessingThread - Exiting");
    return 0;
}

/**
 * Utility functions for version checking
 */
DWORD GetWindowsVersion() {
    OSVERSIONINFOA versionInfo;
    memset(&versionInfo, 0, sizeof(versionInfo));
    versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);

    if(GetVersionExA(&versionInfo)) {
        return versionInfo.dwBuildNumber;
    }

    return 0;
}

BOOL CheckWindowsVersionSupport() {
    // Simplified version check - in real implementation this would be more complex
    OSVERSIONINFOA versionInfo;
    memset(&versionInfo, 0, sizeof(versionInfo));
    versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);

    if(! GetVersionExA(&versionInfo)) {
        return FALSE;
    }

    // Support Windows 7+ (version 6.1+)
    if(versionInfo.dwMajorVersion > 6
       || (versionInfo.dwMajorVersion == 6 && versionInfo.dwMinorVersion >= 1)) {
        return TRUE;
    }

    return FALSE;
}
