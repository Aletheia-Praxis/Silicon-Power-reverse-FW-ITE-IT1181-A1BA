#pragma once

#include "../resources/resource.h"
#include "WindowsHeaders.h"

// Dialog Identifiers
#define IDD_DEVICE_SELECT 140
#define IDD_ABOUT         141

// Forward declaration
class iTEUFDrs;

// Device Selection Dialog
class CDeviceSelectDialog : public CDialog {
public:
    CDeviceSelectDialog(CWnd* pParent = NULL);

    CString GetSelectedDevice() const { return m_selectedDevice; }

protected:
    void DoDataExchange(CDataExchange* pDX) override;
    BOOL OnInitDialog() override;

    afx_msg void OnDeviceListSelChange();
    afx_msg void OnRefresh();
    void OnOK() override;

    CListBox m_deviceList;
    CString m_selectedDevice;

    void RefreshDeviceList();
    BOOL GetDevicePath(
        HDEVINFO hDevInfo,
        PSP_DEVINFO_DATA pDeviceInfoData,
        LPTSTR pDevicePath,
        DWORD devicePathSize);

    DECLARE_MESSAGE_MAP()
};

// About Dialog
class CAboutDialog : public CDialog {
public:
    CAboutDialog(CWnd* pParent = NULL);

protected:
    void DoDataExchange(CDataExchange* pDX) override;
    BOOL OnInitDialog() override;

    DECLARE_MESSAGE_MAP()
};

// Main application dialog
class CMainDialog : public CDialog {
public:
    CMainDialog(CWnd* pParent = NULL);

protected:
    void DoDataExchange(CDataExchange* pDX) override;
    BOOL OnInitDialog() override;

    DECLARE_MESSAGE_MAP()
};

// Main application dialog - CUrescueDlg
// Reconstructed from Ghidra analysis at 0x00415780 (Constructor) and 0x00415930 (OnInitDialog)
class CUrescueDlg : public CDialog {
public:
    CUrescueDlg(CWnd* pParent, void* pUnknown);  // Constructor matches Ghidra analysis

    // Dialog Data - matches Ghidra analysis (IDD = 0x66 = 102)
    enum { IDD = 0x66 };

protected:
    // DDX/DDV support
    virtual void DoDataExchange(CDataExchange* pDX) override;

    // Implementation
    HICON m_hIcon;  // Icon loaded from resource 0x80 (128)

    // Generated message map functions
    virtual BOOL OnInitDialog() override;
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

private:
    // Member variables reconstructed from Ghidra constructor analysis
    // Exact layout based on memory offsets from constructor at 0x00415780

    // Custom controls initialized in constructor
    // CPieChartCtrl at offset 0xc0 (commented out - custom control)
    // CTextProgressCtrl at offset 0x26c (commented out - custom control)

    // Standard MFC controls at specific offsets
    CStatic m_staticCtrl1;  // Control at offset 0x11c
    CStatic m_staticCtrl2;  // Control at offset 0x170
    CStatic m_staticCtrl3;  // Control at offset 0x1c4
    CButton m_buttonCtrl;   // Control at offset 0x218

    // CString members initialized via global data operations
    CString m_string1;  // CString at offset 0x2ec
    CString m_string2;  // CString at offset 0x2f0

    // Parameter passed to constructor, stored at offset 0x2f8
    // Contains pointer to iTEUFDrs device manager object
    void* m_unknownParam;
};

// Helper function declarations for CUrescueDlg
extern "C" {
void CFileVersionInfoInitAndAssignStrings();
int GetModuleFileVersionInfo(int param);
UINT GetUshortFieldByIndex(int index);
int FormatStringToBuffer(char* buffer, int bufferSize, const char* format, ...);
void GetSelectedDeviceFromIni();
void UpdateDialogVersionAndInfo();
void ShowErrorMessageByCode(char errorCode);
}
