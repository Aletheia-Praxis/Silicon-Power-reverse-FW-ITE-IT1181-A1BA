#include <afxwin.h>
#include <afxcmn.h>
#include <afxinet.h>
#include "URescueMain.h"
#include "iTEUFDrs.h"
#include "Utilities.h"

// Global iTEUFDrs instance
static iTEUFDrs* g_pURescueApp = nullptr;

// Main function of the URescue program
int RunURescueApplication(void)
{
    LogMessage("URescue application starting...");
    
    // Get module directory for SDK loading
    CHAR moduleDir[MAX_PATH];
    if (!GetModuleDirectoryA(moduleDir, sizeof(moduleDir))) {
        LogError("Failed to get module directory");
        return -1;
    }
    
    // Initialize main URescue object (equivalent to iTEUFDrs constructor call)
    g_pURescueApp = new iTEUFDrs(moduleDir);
    if (!g_pURescueApp || !g_pURescueApp->IsInitialized()) {
        LogError("Failed to initialize iTEUFDrs: Error code %d", 
                 g_pURescueApp ? g_pURescueApp->GetLastError() : 0);
        delete g_pURescueApp;
        g_pURescueApp = nullptr;
        return -1;
    }
    
    // Initialize MFC application
    CWinApp app;
    
    // Create the main window
    CMainFrame* pMainFrame = new CMainFrame();
    if (!pMainFrame) {
        LogError("Failed to create main frame window");
        delete g_pURescueApp;
        g_pURescueApp = nullptr;
        return -1;
    }
    
    // Show the main window
    pMainFrame->ShowWindow(SW_SHOW);
    pMainFrame->UpdateWindow();
    
    LogMessage("URescue application initialized successfully");
    
    // Run the message loop
    int result = app.Run();
    
    // Cleanup
    delete g_pURescueApp;
    g_pURescueApp = nullptr;
    
    LogMessage("URescue application exiting with code %d", result);
    return result;
}

// MFC WinMain function
int AfxWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    // Initialize MFC
    if (!AfxWinInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow)) {
        return 0;
    }
    
    // Run the main function
    return RunURescueApplication();
}
