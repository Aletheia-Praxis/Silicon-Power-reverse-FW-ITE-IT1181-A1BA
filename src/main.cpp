#include <windows.h>
#include <tchar.h>
#include <afxwin.h>
#include "CRTFunctions.h"

// Main application wrapper (this is what the real AfxWinMain calls)
int RunURescueApplication(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);

// Actual entry point function (decompiled from 0x004577f4)
void entry(void)
{
    ___security_init_cookie();
    ___tmainCRTStartup();
}

// CRT Startup function (decompiled from 0x00457676)
int ___tmainCRTStartup(void)
{
    int result;
    STARTUPINFOA startupInfo;
    
    GetStartupInfoA(&startupInfo);
    
    // Simplified check for GUI app from Ghidra's decompilation
    BOOL isGuiApp = FALSE;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)0x400000;
    if (pDosHeader->e_magic == IMAGE_DOS_SIGNATURE) {
        PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((BYTE*)pDosHeader + pDosHeader->e_lfanew);
        if (pNtHeaders->Signature == IMAGE_NT_SIGNATURE &&
            pNtHeaders->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC &&
            pNtHeaders->OptionalHeader.NumberOfRvaAndSizes > IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR) {
            if (pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].Size != 0) {
                isGuiApp = TRUE;
            }
        }
    }

    if (!__heap_init()) {
        fast_error_exit(0x1c);
    }
    
    if (!__mtinit()) {
        fast_error_exit(0x10);
    }
    
    __RTC_Initialize();
    
    if (__ioinit() < 0) {
        __amsg_exit(0x1b);
    }
    
    g_szCmdLine = GetCommandLineA();
    g_pEnvStrs = ___crtGetEnvironmentStringsA();
    
    if (__setargv() < 0) {
        __amsg_exit(8);
    }
    if (__setenvp() < 0) {
        __amsg_exit(9);
    }
    
    int cinitResult = __cinit(1);
    if (cinitResult != 0) {
        __amsg_exit(cinitResult);
    }
    
    LPTSTR winCmdLine = __wincmdln();
    
    if ((startupInfo.dwFlags & STARTF_USESHOWWINDOW) == 0) {
        startupInfo.wShowWindow = SW_SHOWDEFAULT;
    }
    
    // Call the actual main application function (wrapper for AfxWinMain)
    result = AfxWinMain((HINSTANCE)GetModuleHandle(NULL), NULL, winCmdLine, startupInfo.wShowWindow);
    
    if (isGuiApp) {
        __cexit();
    } else {
        _exit(result);
    }
    
    return result;
}

// This is the equivalent of the original FUN_004845e0 which just calls AfxWinMain
int FUN_004845e0(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    // In the original binary, this function simply calls AfxWinMain.
    // The actual application logic is in the CWinApp-derived class's InitInstance.
    // Here, we call our renamed application entry point.
    return RunURescueApplication(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
