#pragma once

// clang-format off
#include <afxcmn.h>
#include <afxdlgs.h>
#include <afxext.h>
#include <afxinet.h>
#include <afxwin.h>

#include "FirmwareManager.h"
#include "ITEController.h"
#include "Dialogs.h"
// clang-format on

// Resource identifiers
#define IDR_MAINFRAME        128
#define IDD_MAIN             129
#define ID_DEVICE_CONNECT    132
#define ID_DEVICE_DISCONNECT 133
#define ID_FIRMWARE_LOAD     134
#define ID_FIRMWARE_WRITE    135
#define ID_FIRMWARE_VERIFY   136
#define ID_HELP_ABOUT        137
#define IDC_DEVICE_LIST      1001
#define IDC_SELECTED_DEVICE  1002
#define IDC_REFRESH          1003

// Status bar indicators
static UINT indicators[] = { ID_SEPARATOR, ID_SEPARATOR, ID_SEPARATOR };

// Main application window
class CMainFrame : public CFrameWnd {
public:
    CMainFrame();
    virtual ~CMainFrame();

protected:
    // UI elements
    CToolBar m_toolBar;
    CStatusBar m_statusBar;
    CDialog m_mainDialog;

    // Device data
    HANDLE m_hDevice;
    LPVOID m_pFirmware;
    DWORD m_firmwareSize;

    // Functions
    void CreateControls();

    // Message handlers
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDeviceConnect();
    afx_msg void OnDeviceDisconnect();
    afx_msg void OnFirmwareLoad();
    afx_msg void OnFirmwareWrite();
    afx_msg void OnFirmwareVerify();
    afx_msg void OnHelpAbout();

    DECLARE_MESSAGE_MAP()
};
