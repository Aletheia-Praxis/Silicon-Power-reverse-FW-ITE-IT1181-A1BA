#pragma once

#include <afxcmn.h>
#include <afxdlgs.h>
#include <afxinet.h>
#include <afxwin.h>
#include <setupapi.h>
// #include <winusb.h>  // Excluded to avoid usbspec.h conflicts

// Dialog Identifiers
#define IDD_DEVICE_SELECT 140
#define IDD_ABOUT         141

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

// Main application dialog - CUrescueDlg
// Reconstructed from Ghidra analysis (e.g., constructor at 0x00415780)
class CUrescueDlg : public CDialog {
public:
    CUrescueDlg(CWnd* pParent = NULL);  // Constructor

    // Dialog Data
    enum { IDD = 102 };  // Assuming IDD_URESCUE_DIALOG is 102

protected:
    // DDX/DDV support
    virtual void DoDataExchange(CDataExchange* pDX) override;

    // Implementation
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog() override;
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

private:
    // Reconstructed member variables based on constructor analysis
    // These are placeholders for custom controls
    CStatic m_staticCtrl1;  // Placeholder for a static control at offset 0x11c
    CStatic m_staticCtrl2;  // Placeholder for a static control at offset 0x170
    CStatic m_staticCtrl3;  // Placeholder for a static control at offset 0x1c4
    CButton m_buttonCtrl;   // Placeholder for a button control at offset 0x218
    // CTextProgressCtrl m_progress; // Placeholder for custom progress control at 0x26c
    // CPieChartCtrl m_pieChart;     // Placeholder for custom pie chart control at 0xc0

    CString m_string1;  // at offset 0x2ec
    CString m_string2;  // at offset 0x2f0

    // The original constructor takes an additional undefined4 parameter
    // which is stored at offset 0x2f8. Its purpose is unknown.
    void* m_unknownParam;
};
