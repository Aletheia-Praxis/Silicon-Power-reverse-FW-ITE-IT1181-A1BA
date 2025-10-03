#ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0601  // Windows 7+
#endif

#define WIN32_LEAN_AND_MEAN
#include <afxwin.h>

#include "../include/CRTFunctions.h"
#include "../include/CUrescueApp.h"
#include "../include/WindowsHeaders.h"


// Main application wrapper (this is what the real AfxWinMain calls)
int RunURescueApplication(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR lpCmdLine,
    int nCmdShow);

// Actual entry point function (decompiled from 0x004577f4)
void entry(void) {
    URescue_security_init_cookie();
    URescue_tmainCRTStartup();
}

// CRT Startup function (decompiled from 0x00457676)
int URescue_tmainCRTStartup(void) {
    int result;
    STARTUPINFOA startupInfo;

    GetStartupInfoA(&startupInfo);

    // Simplified check for GUI app from Ghidra's decompilation
    BOOL isGuiApp = FALSE;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER) 0x400000;
    if(pDosHeader->e_magic == IMAGE_DOS_SIGNATURE) {
        PIMAGE_NT_HEADERS pNtHeaders =
            (PIMAGE_NT_HEADERS) ((BYTE *) pDosHeader + pDosHeader->e_lfanew);
        if(pNtHeaders->Signature == IMAGE_NT_SIGNATURE
           && pNtHeaders->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC
           && pNtHeaders->OptionalHeader.NumberOfRvaAndSizes
                  > IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR) {
            if(pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].Size
               != 0) {
                isGuiApp = TRUE;
            }
        }
    }

    if(! URescue_heap_init()) {
        URescue_fast_error_exit(0x1c);
    }

    if(! URescue_mtinit()) {
        URescue_fast_error_exit(0x10);
    }

    URescue_RTC_Initialize();

    if(URescue_ioinit() < 0) {
        URescue_amsg_exit(0x1b);
    }

    g_szCmdLine = GetCommandLineA();
    g_pEnvStrs = URescue_crtGetEnvironmentStringsA();

    if(URescue_setargv() < 0) {
        URescue_amsg_exit(8);
    }
    if(URescue_setenvp() < 0) {
        URescue_amsg_exit(9);
    }

    int cinitResult = URescue_cinit(1);
    if(cinitResult != 0) {
        URescue_amsg_exit(cinitResult);
    }

    LPTSTR winCmdLine = URescue_wincmdln();

    if((startupInfo.dwFlags & STARTF_USESHOWWINDOW) == 0) {
        startupInfo.wShowWindow = SW_SHOWDEFAULT;
    }

    // Call the actual main application function
    result = RunURescueApplication(
        (HINSTANCE) GetModuleHandle(NULL), NULL, winCmdLine, startupInfo.wShowWindow);

    if(isGuiApp) {
        URescue_cexit();
    } else {
        URescue_exit(result);
    }

    return result;
}

// This is the equivalent of the original FUN_004845e0 which just calls AfxWinMain
int FUN_004845e0(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
    // In the original binary, this function simply calls AfxWinMain.
    // The actual application logic is in the CWinApp-derived class's InitInstance.
    // Call the reconstructed CUrescueApp::InitInstance logic
    if(! CUrescueApp_InitInstance()) {
        return -1;
    }

    return RunURescueApplication(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
