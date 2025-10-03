/**
 * URescue Entry Point Reconstruction
 * Based on detailed Ghidra analysis of URescue_v81D.2.24.2.exe
 *
 * Entry point chain:
 * entry (0x004577f4) -> ___tmainCRTStartup (0x00457676) -> FUN_004845e0 (0x004845e0) -> AfxWinMain
 */

#ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0601  // Windows 7+
#endif

#define WIN32_LEAN_AND_MEAN
#include <afxwin.h>
#include <process.h>

#include "../include/CRTFunctions.h"
#include "../include/CUrescueApp.h"
#include "../include/Utilities.h"
#include "../include/WindowsHeaders.h"

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

// CRT Functions (simplified implementations for linking)
void URescue_security_init_cookie(void);
int URescue_tmainCRTStartup(void);
int URescue_heap_init(void);
int URescue_mtinit(void);
void URescue_RTC_Initialize(void);
int URescue_ioinit(void);
void URescue_fast_error_exit(int code);
void URescue_amsg_exit(int code);
LPSTR URescue_crtGetEnvironmentStringsA(void);
int URescue_setargv(void);
int URescue_setenvp(void);
int URescue_cinit(int full_init);
LPTSTR URescue_wincmdln(void);
void URescue_cexit(void);
void URescue_exit(int code);

// Global variables (based on Ghidra analysis)
extern LPSTR g_szCmdLine;
extern LPSTR g_pEnvStrs;

//==============================================================================
// CURESCUEAPP CLASS IMPLEMENTATION
//==============================================================================

/**
 * CUrescueApp - Main MFC application class
 * Reconstructed based on constructor at 0x00413960 and InitInstance at 0x004143c0
 */
class CUrescueApp : public CWinApp {
public:
    CUrescueApp();
    virtual ~CUrescueApp();

    // Override InitInstance (called by MFC framework)
    virtual BOOL InitInstance() override;

    // Virtual table pointer (first member in any C++ object with virtual functions)
    // This points to CUrescueApp::vftable as seen in Ghidra

private:
    // Member variables based on Ghidra analysis
    BOOL m_isInitialized;
    DWORD m_lastError;
    HANDLE m_hMutex;  // For preventing multiple instances
};

//==============================================================================
// GLOBAL APPLICATION OBJECT
//==============================================================================

/**
 * The one and only CUrescueApp object
 * This is the global application object that MFC expects
 * In the original binary, this would be created statically
 */
CUrescueApp theApp;

//==============================================================================
// CURESCUEAPP IMPLEMENTATION
//==============================================================================

/**
 * CUrescueApp constructor - based on analysis at 0x00413960
 */
CUrescueApp::CUrescueApp() : m_isInitialized(FALSE), m_lastError(0), m_hMutex(NULL) {
    LogMessage("CUrescueApp: Constructor called");
    // Constructor calls CWinApp::CWinApp() first (handled by base class)
    // Sets up vtable pointer to CUrescueApp::vftable
    // Initialize other members as seen in decompiled code
}

/**
 * CUrescueApp destructor
 */
CUrescueApp::~CUrescueApp() {
    if(m_hMutex) {
        CloseHandle(m_hMutex);
        m_hMutex = NULL;
    }
    LogMessage("CUrescueApp: Destructor called");
}

/**
 * CUrescueApp::InitInstance - Main application initialization
 * Reconstructed from detailed analysis at 0x004143c0
 * This is called by AfxWinMain through the vtable at offset 0x50
 */
BOOL CUrescueApp::InitInstance() {
    LogMessage("CUrescueApp::InitInstance - Starting application initialization");

    // Call base class InitInstance first
    if(! CWinApp::InitInstance()) {
        LogError("CUrescueApp::InitInstance - Base class InitInstance failed");
        return FALSE;
    }

    // Call our reconstructed initialization logic
    BOOL result = CUrescueApp_InitInstance();

    if(result) {
        m_isInitialized = TRUE;
        LogMessage("CUrescueApp::InitInstance - Application initialized successfully");
    } else {
        LogError("CUrescueApp::InitInstance - Application initialization failed");
    }

    return result;
}

//==============================================================================
// ENTRY POINT FUNCTIONS
//==============================================================================

/**
 * Main entry point - reconstructed from 0x004577f4
 * This is the actual program entry point set in the PE header
 */
void entry(void) {
    URescue_security_init_cookie();
    URescue_tmainCRTStartup();
    // Never returns - process exits in URescue_tmainCRTStartup
}

/**
 * CRT Startup function - reconstructed from 0x00457676
 * This performs all C runtime initialization and calls the main function
 */
int URescue_tmainCRTStartup(void) {
    int result;
    STARTUPINFOA startupInfo;

    // Get startup information
    GetStartupInfoA(&startupInfo);

    // Check if this is a GUI application by examining PE headers
    // Based on the decompiled logic at 0x00457676
    BOOL isGuiApp = FALSE;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER) GetModuleHandle(NULL);
    if(pDosHeader && pDosHeader->e_magic == IMAGE_DOS_SIGNATURE) {
        PIMAGE_NT_HEADERS pNtHeaders =
            (PIMAGE_NT_HEADERS) ((BYTE*) pDosHeader + pDosHeader->e_lfanew);
        if(pNtHeaders->Signature == IMAGE_NT_SIGNATURE
           && pNtHeaders->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC
           && pNtHeaders->OptionalHeader.NumberOfRvaAndSizes
                  > IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR) {
            // Check if COM descriptor exists (indicates managed code)
            isGuiApp =
                (pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].Size
                 != 0);
        }
    }

    // Initialize heap
    if(! URescue_heap_init()) {
        URescue_fast_error_exit(0x1c);
        return -1;
    }

    // Initialize multithreading
    if(! URescue_mtinit()) {
        URescue_fast_error_exit(0x10);
        return -1;
    }

    // Initialize runtime checks
    URescue_RTC_Initialize();

    // Initialize I/O
    if(URescue_ioinit() < 0) {
        URescue_amsg_exit(0x1b);
        return -1;
    }

    // Get command line and environment
    g_szCmdLine = GetCommandLineA();
    g_pEnvStrs = URescue_crtGetEnvironmentStringsA();

    // Parse command line arguments
    if(URescue_setargv() < 0) {
        URescue_amsg_exit(8);
        return -1;
    }

    // Set up environment variables
    if(URescue_setenvp() < 0) {
        URescue_amsg_exit(9);
        return -1;
    }

    // Initialize C runtime
    int cinitResult = URescue_cinit(1);
    if(cinitResult != 0) {
        URescue_amsg_exit(cinitResult);
        return -1;
    }

    // Get Windows command line
    LPTSTR winCmdLine = URescue_wincmdln();

    // Set show window parameter if not specified
    if((startupInfo.dwFlags & STARTF_USESHOWWINDOW) == 0) {
        startupInfo.wShowWindow = SW_SHOWDEFAULT;
    }

    // Call the main application function (equivalent to FUN_004845e0)
    result =
        FUN_004845e0((HINSTANCE) GetModuleHandle(NULL), NULL, winCmdLine, startupInfo.wShowWindow);

    // Clean up and exit
    if(isGuiApp) {
        URescue_cexit();
    } else {
        URescue_exit(result);
    }

    return result;
}

/**
 * Application main function - reconstructed from 0x004845e0
 * This is called by the CRT startup code and simply calls AfxWinMain
 */
int FUN_004845e0(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
    LogMessage("FUN_004845e0 - Calling AfxWinMain");

    // The original function at 0x004845e0 simply calls AfxWinMain
    // AfxWinMain will handle all MFC initialization and call our CUrescueApp::InitInstance
    return AfxWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}

//==============================================================================
// SIMPLIFIED CRT FUNCTION IMPLEMENTATIONS
//==============================================================================

// Global variables
LPSTR g_szCmdLine = nullptr;
LPSTR g_pEnvStrs = nullptr;

void URescue_security_init_cookie(void) {
    // Initialize security cookie for buffer overflow protection
    // In release builds, this is usually a simple initialization
    LogMessage("URescue_security_init_cookie: Initializing security cookie");
}

int URescue_heap_init(void) {
    // Initialize the C runtime heap
    // In most cases, this just verifies the heap is working
    LogMessage("URescue_heap_init: Initializing heap");
    return 1;  // Success
}

int URescue_mtinit(void) {
    // Initialize multithreading support
    LogMessage("URescue_mtinit: Initializing multithreading");
    return 1;  // Success
}

void URescue_RTC_Initialize(void) {
    // Initialize runtime checks (debug builds)
    LogMessage("URescue_RTC_Initialize: Initializing runtime checks");
}

int URescue_ioinit(void) {
    // Initialize I/O streams (stdin, stdout, stderr)
    LogMessage("URescue_ioinit: Initializing I/O");
    return 0;  // Success (0 for I/O init)
}

void URescue_fast_error_exit(int code) {
    LogError("URescue_fast_error_exit: Fatal error %d", code);
    ExitProcess(code);
}

void URescue_amsg_exit(int code) {
    LogError("URescue_amsg_exit: Runtime error %d", code);
    ExitProcess(code);
}

LPSTR URescue_crtGetEnvironmentStringsA(void) {
    return GetEnvironmentStringsA();
}

int URescue_setargv(void) {
    // Parse command line arguments (simplified)
    LogMessage("URescue_setargv: Parsing command line");
    return 0;  // Success
}

int URescue_setenvp(void) {
    // Set up environment variables (simplified)
    LogMessage("URescue_setenvp: Setting up environment");
    return 0;  // Success
}

int URescue_cinit(int full_init) {
    // Initialize C runtime global constructors
    LogMessage("URescue_cinit: Initializing C runtime (full_init=%d)", full_init);
    return 0;  // Success
}

LPTSTR URescue_wincmdln(void) {
    return GetCommandLineA();
}

void URescue_cexit(void) {
    // Clean exit with destructors
    LogMessage("URescue_cexit: Clean exit");
    exit(0);
}

void URescue_exit(int code) {
    // Direct exit
    LogMessage("URescue_exit: Direct exit with code %d", code);
    _exit(code);
}
