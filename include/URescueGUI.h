#pragma once

#include <afxcmn.h>
#include <afxdlgs.h>
#include <afxext.h>
#include <afxinet.h>
#include <afxwin.h>

#include "FirmwareManager.h"
#include "ITEController.h"

// Resource identifiers
#define IDR_MAINFRAME        128
#define IDD_MAIN             129
#define IDD_DEVICE_SELECT    130
#define IDD_ABOUT            131
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

// Device selection dialog
class CDeviceSelectDialog : public CDialog {
public:
    CDeviceSelectDialog(CWnd* pParent = NULL);

    CString GetSelectedDevice() const { return m_selectedDevice; }

protected:
    virtual void DoDataExchange(CDataExchange* pDX) override;
    virtual BOOL OnInitDialog() override;

    afx_msg void OnDeviceListSelChange();
    afx_msg void OnRefresh();

    CListBox m_deviceList;
    CString m_selectedDevice;

    DECLARE_MESSAGE_MAP()
};

// About dialog
class CAboutDialog : public CDialog {
public:
    CAboutDialog(CWnd* pParent = NULL);

protected:
    virtual void DoDataExchange(CDataExchange* pDX) override;
    virtual BOOL OnInitDialog() override;

    DECLARE_MESSAGE_MAP()
};
