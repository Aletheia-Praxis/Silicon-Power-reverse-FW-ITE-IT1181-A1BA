/**
 * URescue Entry Point Header
 * Based on detailed Ghidra analysis of URescue_v81D.2.24.2.exe
 */

#pragma once

#include <afxwin.h>

#include "../include/WindowsHeaders.h"


#ifdef __cplusplus
extern "C" {
#endif

//==============================================================================
// ENTRY POINT FUNCTIONS
//==============================================================================

/**
 * Main entry point - reconstructed from 0x004577f4
 * This is the actual program entry point set in the PE header
 */
void entry(void);

/**
 * CRT Startup function - reconstructed from 0x00457676
 * This performs all C runtime initialization and calls the main function
 */
int URescue_tmainCRTStartup(void);

/**
 * Application main function - reconstructed from 0x004845e0
 * This is called by the CRT startup code and simply calls AfxWinMain
 */
int FUN_004845e0(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);

//==============================================================================
// CRT FUNCTION DECLARATIONS
//==============================================================================

void URescue_security_init_cookie(void);
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

//==============================================================================
// GLOBAL VARIABLES
//==============================================================================

extern LPSTR g_szCmdLine;
extern LPSTR g_pEnvStrs;

#ifdef __cplusplus
}
#endif

//==============================================================================
// C++ CLASSES (Only in C++ mode)
//==============================================================================

#ifdef __cplusplus

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

private:
    // Member variables based on Ghidra analysis
    BOOL m_isInitialized;
    DWORD m_lastError;
    HANDLE m_hMutex;  // For preventing multiple instances
};

// Global application object
extern CUrescueApp theApp;

#endif  // __cplusplus
