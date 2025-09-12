#include "../include/Dialogs.h"

#include "../include/WindowsHeaders.h"
#include "../resources/resource.h"

// Device selection dialog constructor
CDeviceSelectDialog::CDeviceSelectDialog(CWnd *pParent) : CDialog(IDD_DEVICE_SELECT, pParent) {
    m_selectedDevice = _T("");
}

// Dialog initialization handler
BOOL CDeviceSelectDialog::OnInitDialog() {
    CDialog::OnInitDialog();

    // Get a reference to the device list
    m_deviceList.SubclassDlgItem(IDC_DEVICE_LIST, this);

    // Refresh the device list
    RefreshDeviceList();

    return TRUE;
}

// Data exchange handler
void CDeviceSelectDialog::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DEVICE_LIST, m_deviceList);
}

// Refresh the device list
void CDeviceSelectDialog::RefreshDeviceList() {
    m_deviceList.ResetContent();

    // Search for USB devices with ITE controllers
    HDEVINFO hDevInfo = SetupDiGetClassDevsA(&GUID_DEVCLASS_USB, NULL, NULL, DIGCF_PRESENT);
    if(hDevInfo == INVALID_HANDLE_VALUE) {
        return;
    }

    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for(DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &deviceInfoData); i++) {
        // Get the device description
        TCHAR deviceDesc[256];
        if(SetupDiGetDeviceRegistryProperty(
               hDevInfo,
               &deviceInfoData,
               SPDRP_DEVICEDESC,
               NULL,
               (PBYTE) deviceDesc,
               sizeof(deviceDesc),
               NULL)) {
            // Check if it's an ITE controller
            if(_tcsstr(deviceDesc, _T("ITE")) || _tcsstr(deviceDesc, _T("1181"))) {
                // Get the device path
                TCHAR devicePath[256];
                if(GetDevicePath(hDevInfo, &deviceInfoData, devicePath, sizeof(devicePath))) {
                    int index = m_deviceList.AddString(deviceDesc);
                    m_deviceList.SetItemData(index, (DWORD_PTR) new CString(devicePath));
                }
            }
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
}

// Get the device path
BOOL CDeviceSelectDialog::GetDevicePath(
    HDEVINFO hDevInfo,
    PSP_DEVINFO_DATA pDeviceInfoData,
    LPTSTR pDevicePath,
    DWORD devicePathSize) {
    // Get the device interface
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    if(! SetupDiEnumDeviceInterfaces(
           hDevInfo, pDeviceInfoData, &GUID_DEVINTERFACE_USB_DEVICE, 0, &deviceInterfaceData)) {
        return FALSE;
    }

    // Get the interface details
    DWORD requiredSize = 0;
    SetupDiGetDeviceInterfaceDetail(hDevInfo, &deviceInterfaceData, NULL, 0, &requiredSize, NULL);

    PSP_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData =
        (PSP_DEVICE_INTERFACE_DETAIL_DATA) malloc(requiredSize);
    if(! pDeviceInterfaceDetailData) {
        return FALSE;
    }

    pDeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

    if(SetupDiGetDeviceInterfaceDetail(
           hDevInfo, &deviceInterfaceData, pDeviceInterfaceDetailData, requiredSize, NULL, NULL)) {
        _tcscpy_s(pDevicePath, devicePathSize, pDeviceInterfaceDetailData->DevicePath);
        free(pDeviceInterfaceDetailData);
        return TRUE;
    }

    free(pDeviceInterfaceDetailData);
    return FALSE;
}

// Handler for selection change in the list
void CDeviceSelectDialog::OnDeviceListSelChange() {
    int selectedIndex = m_deviceList.GetCurSel();
    if(selectedIndex != LB_ERR) {
        CString *pDevicePath = (CString *) m_deviceList.GetItemData(selectedIndex);
        if(pDevicePath) {
            m_selectedDevice = *pDevicePath;
        }
    }
}

// Handler for the refresh button
void CDeviceSelectDialog::OnRefresh() {
    RefreshDeviceList();
}

// OK button handler
void CDeviceSelectDialog::OnOK() {
    if(m_selectedDevice.IsEmpty()) {
        MessageBox(_T("Please select a device"), _T("Warning"), MB_OK | MB_ICONWARNING);
        return;
    }

    CDialog::OnOK();
}

// Message handler
BEGIN_MESSAGE_MAP(CDeviceSelectDialog, CDialog)
ON_LBN_SELCHANGE(IDC_DEVICE_LIST, &CDeviceSelectDialog::OnDeviceListSelChange)
ON_BN_CLICKED(IDC_REFRESH_BUTTON, &CDeviceSelectDialog::OnRefresh)
END_MESSAGE_MAP()

// About dialog constructor
CAboutDialog::CAboutDialog(CWnd *pParent) : CDialog(IDD_ABOUT, pParent) {}

// About dialog initialization handler
BOOL CAboutDialog::OnInitDialog() {
    CDialog::OnInitDialog();

    // Set the about text
    CString aboutText;
    aboutText.Format(
        _T("URescue v81D.2.24.2\n\n")
        _T("ITE IT1181 Firmware Recovery Tool\n\n")
        _T("This is a reverse-engineered version of the original URescue utility.\n")
        _T("Use with caution and only on devices you own.\n\n")
        _T("Reverse Engineering Project\n")
        _T("For educational purposes only"));

    SetDlgItemText(IDC_ABOUT_TEXT, aboutText);

    return TRUE;
}

// Data exchange handler for the about dialog
void CAboutDialog::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
}

// Message handler for the about dialog
BEGIN_MESSAGE_MAP(CAboutDialog, CDialog)
END_MESSAGE_MAP()
