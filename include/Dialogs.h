#pragma once

#include <afxcmn.h>
#include <afxdlgs.h>
#include <afxinet.h>
#include <afxwin.h>
#include <setupapi.h>
#include <winusb.h>

// Dialog Identifiers
#define IDD_DEVICE_SELECT 140
#define IDD_ABOUT         141

// Global identifiers for USB
extern const GUID GUID_DEVCLASS_USB;
extern const GUID GUID_DEVINTERFACE_USB_DEVICE;

// Device Selection Dialog
class CDeviceSelectDialog : public CDialog {
public:
    CDeviceSelectDialog(CWnd* pParent = NULL);

    CString GetSelectedDevice() const { return m_selectedDevice; }

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    afx_msg void OnDeviceListSelChange();
    afx_msg void OnRefresh();
    virtual void OnOK();

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
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()
};
