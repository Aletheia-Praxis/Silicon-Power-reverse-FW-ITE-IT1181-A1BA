#pragma once

#include <afxwin.h>

// Main function of the URescue program
int FUN_004845e0(void);

// MFC WinMain function
int AfxWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);

// Main window of the program
class CMainFrame : public CFrameWnd {
public:
    CMainFrame();
    virtual ~CMainFrame();

protected:
    DECLARE_MESSAGE_MAP()
};
