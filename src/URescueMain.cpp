#include <afxwin.h>
#include <afxcmn.h>
#include <afxinet.h>
#include "URescueMain.h"

// Main function of the URescue program
int FUN_004845e0(void)
{
    // Initialize MFC application
    CWinApp app;
    
    // Create the main window
    CMainFrame* pMainFrame = new CMainFrame();
    if (!pMainFrame) {
        return -1;
    }
    
    // Show the main window
    pMainFrame->ShowWindow(SW_SHOW);
    pMainFrame->UpdateWindow();
    
    // Run the message loop
    return app.Run();
}

// MFC WinMain function
int AfxWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    // Initialize MFC
    if (!AfxWinInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow)) {
        return 0;
    }
    
    // Run the main function
    return FUN_004845e0();
}
