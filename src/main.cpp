#include <windows.h>
#include <tchar.h>
#include <afxwin.h>
#include "CRTFunctions.h"

// Forward declarations
int ___tmainCRTStartup(void);
int FUN_004845e0(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);

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
    UINT isGuiApp = 0;
    
    // Initialize startup info
    GetStartupInfoA(&startupInfo);
    
    // Check if this is GUI app (PE header validation)
    if (((IMAGE_DOS_HEADER*)0x400000)->e_magic == IMAGE_DOS_SIGNATURE) {
        PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)0x400000 + ((IMAGE_DOS_HEADER*)0x400000)->e_lfanew);
        if (ntHeaders->Signature == IMAGE_NT_SIGNATURE && 
            ntHeaders->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC &&
            ntHeaders->OptionalHeader.NumberOfRvaAndSizes > 14) {
            isGuiApp = (ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].Size != 0) ? 1 : 0;
        }
    }
    
    // Initialize heap
    if (!__heap_init()) {
        fast_error_exit(0x1c);
    }
    
    // Initialize multithreading
    if (!__mtinit()) {
        fast_error_exit(0x10);
    }
    
    // Initialize runtime checks
    __RTC_Initialize();
    
    // Initialize I/O
    if (__ioinit() < 0) {
        __amsg_exit(0x1b);
    }
    
    // Get command line and environment
    LPSTR cmdLine = GetCommandLineA();
    LPSTR envStrings = ___crtGetEnvironmentStringsA();
    
    // Set up arguments and environment
    if (__setargv() < 0) {
        __amsg_exit(8);
    }
    if (__setenvp() < 0) {
        __amsg_exit(9);
    }
    
    // Initialize C runtime
    int cinitResult = __cinit(1);
    if (cinitResult != 0) {
        __amsg_exit(cinitResult);
    }
    
    // Get command line for Windows
    LPTSTR winCmdLine = __wincmdln();
    
    // Set show window state
    if ((startupInfo.dwFlags & STARTF_USESHOWWINDOW) == 0) {
        startupInfo.wShowWindow = SW_SHOWDEFAULT;
    }
    
    // Call main application function
    result = FUN_004845e0((HINSTANCE)0x400000, NULL, winCmdLine, startupInfo.wShowWindow);
    
    if (isGuiApp) {
        __cexit();
        return result;
    } else {
        _exit(result);
    }
}

// Main application wrapper (decompiled from 0x004845e0)
int FUN_004845e0(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    return AfxWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}


