#include <windows.h>
#include <tchar.h>

// Entry point
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    // MFC initialization and main program launch
    return AfxWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}

// CRT Startup function
int __tmainCRTStartup(void)
{
    // C Runtime initialization
    if (!_heap_init(1)) {
        return 255;
    }
    
    // Launch of the main program function
    return main();
}
