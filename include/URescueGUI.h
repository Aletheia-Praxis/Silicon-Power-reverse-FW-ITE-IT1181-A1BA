#pragma once

#include "MFCHeaders.h"
// #include "resource.h" // This is included via MFCHeaders.h now indirectly or should be included
// in cpp files
#include "Dialogs.h"
#include "URescueCore.h"

// CUrescueGUI class
class CUrescueGUI : public CWinApp {
    // ... existing code ...
};

// Main application window
class CMainFrame : public CFrameWnd {
public:
    CMainFrame();
    virtual ~CMainFrame();

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDeviceConnect();
    afx_msg void OnDeviceDisconnect();
    afx_msg void OnFirmwareLoad();
    afx_msg void OnFirmwareWrite();
    afx_msg void OnFirmwareVerify();
    afx_msg void OnHelpAbout();
    DECLARE_MESSAGE_MAP()

private:
    void CreateControls();

    CToolBar m_toolBar;
    CStatusBar m_statusBar;
    CMainDialog m_mainDialog;
    HANDLE m_hDevice;
    LPVOID m_pFirmware;
    DWORD m_firmwareSize;
};

// Forward declarations
class iTEUFDrs;
class CUrescueDlg;

// Progress callback function type
typedef void(CALLBACK* ProgressCallbackFunc)(int percentage, LPCSTR status, LPVOID userdata);

// Progress callback function for long operations
void CALLBACK ProgressCallback(int percentage, LPCSTR status, LPVOID userdata);

// Device operation wrapper functions
namespace DeviceOperations {
BOOL FormatDevice(iTEUFDrs* pDevice, CUrescueDlg* pDialog);
BOOL RepairDevice(iTEUFDrs* pDevice, CUrescueDlg* pDialog);
BOOL DiagnoseDevice(iTEUFDrs* pDevice, CUrescueDlg* pDialog);
}  // namespace DeviceOperations

// Device Information Display Functions
namespace DeviceInfo {
CString GetDeviceTypeString(iTEUFDrs* pDevice);
CString GetCapacityString(iTEUFDrs* pDevice);
CString GetSerialNumber(iTEUFDrs* pDevice);
CString GetFirmwareVersion(iTEUFDrs* pDevice);
}  // namespace DeviceInfo

// Dialog Helper Functions
namespace DialogHelpers {
void LoadStringResource(UINT resourceId, CString& result);
void ShowErrorDialog(const CString& message, const CString& title = "Error");
void ShowInfoDialog(const CString& message, const CString& title = "Information");
BOOL ConfirmOperation(const CString& message, const CString& title = "Confirm");
}  // namespace DialogHelpers
