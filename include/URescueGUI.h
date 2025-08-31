#pragma once

#include <afxwin.h>
#include <afxcmn.h>
#include <afxinet.h>
#include <afxdlgs.h>
#include "ITEController.h"
#include "FirmwareManager.h"

// Resource identifiers
#define IDR_MAINFRAME 128
#define IDD_MAIN 129
#define ID_DEVICE_CONNECT 130
#define ID_DEVICE_DISCONNECT 131
#define ID_FIRMWARE_LOAD 132
#define ID_FIRMWARE_WRITE 133
#define ID_FIRMWARE_VERIFY 134
#define ID_HELP_ABOUT 135

// Status bar indicators
static UINT indicators[] = {
    ID_SEPARATOR,
    ID_SEPARATOR,
    ID_SEPARATOR
};

// Main application window
class CMainFrame : public CFrameWnd
{
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
class CDeviceSelectDialog : public CDialog
{
public:
    CDeviceSelectDialog(CWnd* pParent = NULL);
    
    CString GetSelectedDevice() const { return m_selectedDevice; }
    
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    
    afx_msg void OnDeviceListSelChange();
    afx_msg void OnRefresh();
    
    CListBox m_deviceList;
    CString m_selectedDevice;
    
    DECLARE_MESSAGE_MAP()
};

// About dialog
class CAboutDialog : public CDialog
{
public:
    CAboutDialog(CWnd* pParent = NULL);
    
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    
    DECLARE_MESSAGE_MAP()
};
