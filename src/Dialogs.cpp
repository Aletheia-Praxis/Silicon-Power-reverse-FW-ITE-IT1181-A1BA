// clang-format off
#ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0601  // Windows 7+
#endif

#include "../include/WindowsHeaders.h"
#include "../resources/resource.h"
#include "../include/Dialogs.h"
#include "../include/iTEUFDrs.h" // Include iTEUFDrs header for device manager
// clang-format on

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

// --- CUrescueDlg Implementation ---

// Constructor for the main dialog
// Based on Ghidra analysis of function at 0x00415780
CUrescueDlg::CUrescueDlg(CWnd *pParent, void *pUnknown) : CDialog(CUrescueDlg::IDD, pParent) {
    // The original constructor initializes custom controls (CPieChartCtrl, CTextProgressCtrl)
    // and CString members here. It also loads the application icon.

    // Initialize CString members (reconstructed from offsets 0x2ec and 0x2f0)
    m_string1 = "";
    m_string2 = "";

    // Store the unknown parameter (reconstructed from offset 0x2f8)
    m_unknownParam = pUnknown;

    // Load the application icon (IDR_MAINFRAME, which is typically 128)
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUrescueDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    // DDX mapping for controls would go here
    // Example: DDX_Control(pDX, IDC_MY_STATIC, m_staticCtrl1);
}

BOOL CUrescueDlg::OnInitDialog() {
    CDialog::OnInitDialog();

    // Set the icon for this dialog
    SetIcon(m_hIcon, TRUE);   // Set big icon
    SetIcon(m_hIcon, FALSE);  // Set small icon

    // Subclass controls and set initial states
    // The original code subclasses a control at IDC_STATIC_INFO (assumed 1000)
    // and hides a global CWnd object (DAT_004ad750).
    GetDlgItem(1000)->ShowWindow(SW_HIDE);  // Assuming 1000 is the ID for the info static text

    // Set window position to top
    SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);

    // --- Version and Title Setup ---
    char windowTitle[64];
    // This part of the code gets the file version info and formats the window title.
    // We will use a simplified version for now.
    // In the original, it calls a CFileVersionInfo class.
    sprintf_s(windowTitle, sizeof(windowTitle), "URescue v%dD.%d.%d.%d", 2, 24, 2, 81);  // Example
                                                                                         // version
    SetWindowText(windowTitle);

    // The original code stores the title in a global app state.
    // AfxGetApp()->m_pszAppName = _strdup(windowTitle);

    // --- Main Logic ---
    // The core logic depends on the iTEUFDrs object passed via m_unknownParam.
    iTEUFDrs *pDeviceManager = (iTEUFDrs *) m_unknownParam;
    if(! pDeviceManager) {
        AfxMessageBox("Device manager object is null!", MB_OK | MB_ICONERROR);
        EndDialog(IDCANCEL);
        return TRUE;
    }

    // Pass 'this' pointer to the device manager
    pDeviceManager->SetParentDialog(this);

    // Check for initialization errors from the device manager
    DWORD lastError = pDeviceManager->GetLastError();
    if(lastError != ITEUFDRS_ERROR_NONE) {
        // The original code has a complex error reporting mechanism.
        // We will show a simple message based on the error code.
        CString errorMsg;
        errorMsg.Format("Initialization failed with error code: %d", lastError);
        AfxMessageBox(errorMsg, MB_OK | MB_ICONERROR);
        EndDialog(IDCANCEL);
        return TRUE;
    }

    if(! pDeviceManager->IsInitialized()) {
        // This case handles when no device is found.
        // The original shows a message box with string ID 13.
        AfxMessageBox("No ITE device found.", MB_OK | MB_ICONWARNING);
        EndDialog(IDCANCEL);
        return TRUE;
    }

    // If initialization is successful, proceed with UI updates.
    // The original code extracts the filename from a path and sets it as a window text.
    // It also sends a message to a progress bar control.

    // Example of updating a control
    // CString deviceInfo = pDeviceManager->GetFormattedDeviceInfo();
    // GetDlgItem(IDC_DEVICE_INFO_STATIC)->SetWindowText(deviceInfo);

    // Send a message to the progress bar (assumed ID 1004)
    // The original sends PBM_SETRANGE32 (0x406) and PBM_SETPOS (0x402)
    CProgressCtrl *pProgress = (CProgressCtrl *) GetDlgItem(1004);
    if(pProgress) {
        pProgress->SendMessage(PBM_SETRANGE32, 0, 100);
        pProgress->SendMessage(PBM_SETPOS, 1, 0);
    }

    // Further UI updates would happen here based on the device state.

    return TRUE;  // return TRUE unless you set the focus to a control
}

void CUrescueDlg::OnSysCommand(UINT nID, LPARAM lParam) {
    // Handle system commands, like the About dialog
    CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
// to draw the icon. For MFC applications using the document/view model,
// this is automatically done for you by the framework.
void CUrescueDlg::OnPaint() {
    if(IsIconic()) {
        CPaintDC dc(this);  // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    } else {
        CDialog::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
// the minimized window.
HCURSOR CUrescueDlg::OnQueryDragIcon() {
    return static_cast<HCURSOR>(m_hIcon);
}

BEGIN_MESSAGE_MAP(CUrescueDlg, CDialog)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
// Other message handlers would go here
END_MESSAGE_MAP()
