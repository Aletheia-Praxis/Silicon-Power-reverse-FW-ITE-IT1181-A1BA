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
CDeviceSelectDialog::CDeviceSelectDialog(CWnd* pParent) : CDialog(IDD_DEVICE_SELECT, pParent) {
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
void CDeviceSelectDialog::DoDataExchange(CDataExchange* pDX) {
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
        CString* pDevicePath = (CString*) m_deviceList.GetItemData(selectedIndex);
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
CAboutDialog::CAboutDialog(CWnd* pParent) : CDialog(IDD_ABOUT, pParent) {}

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
void CAboutDialog::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
}

// Message handler for the about dialog
BEGIN_MESSAGE_MAP(CAboutDialog, CDialog)
END_MESSAGE_MAP()

// --- CUrescueDlg Implementation ---
// Reconstructed from Ghidra analysis at 0x00415780 (Constructor)

/**
 * Constructor for CUrescueDlg - reconstructed from 0x00415780
 * Initializes all controls and loads icon from resource 0x80
 */
CUrescueDlg::CUrescueDlg(CWnd* pParent, void* pUnknown) : CDialog(0x66, pParent) {
    // Initialize CString members at offsets 0x2ec and 0x2f0
    m_string1 = "";
    m_string2 = "";

    // Store unknown parameter at offset 0x2f8 (iTEUFDrs device manager)
    m_unknownParam = pUnknown;

    // Load application icon from resource 0x80 (128)
    AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
    m_hIcon = LoadIconA(pModuleState->m_hCurrentInstanceHandle, MAKEINTRESOURCE(0x80));
}

void CUrescueDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    // DDX mapping for controls would go here
    // Example: DDX_Control(pDX, IDC_MY_STATIC, m_staticCtrl1);
}

/**
 * OnInitDialog - reconstructed from 0x00415930
 * Complete initialization including version info, device checking, and UI setup
 */
BOOL CUrescueDlg::OnInitDialog() {
    CDialog::OnInitDialog();

    // Set dialog icons (WM_SETICON messages 0x80)
    SendMessage(WM_SETICON, ICON_BIG, (LPARAM) m_hIcon);
    SendMessage(WM_SETICON, ICON_SMALL, (LPARAM) m_hIcon);

    // Subclass control at ID 1000 (0x3e8)
    CWnd* pStaticCtrl = GetDlgItem(1000);
    if(pStaticCtrl) {
        pStaticCtrl->SubclassDlgItem(1000, this);
    }

    // Hide global window object (equivalent to DAT_004ad750)
    // CWnd::ShowWindow(globalWindowPtr, SW_HIDE);

    // Set window position with specific flags
    SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);

    // Initialize file version info system (simplified version)
    LogMessage("CFileVersionInfoInitAndAssignStrings - Called");

    // Build window title with version information
    char windowTitle[64] = { 0 };

    // Simulate version extraction (based on Ghidra analysis)
    UINT version1 = 81;  // Version component 1
    UINT version2 = 2;   // Version component 2
    UINT version3 = 24;  // Version component 3
    UINT version4 = 2;   // Version component 4

    // Format version string as in original: "URescue v%dD.%d.%d.%d"
    sprintf_s(
        windowTitle,
        sizeof(windowTitle),
        "%s v%dD.%d.%d.%d",
        "URescue",
        version4,
        version3,
        version2,
        version1);

    // Set window title
    SetWindowText(windowTitle);

    // Store title in global app state (allocate 0x104 bytes as in original)
    LPSTR globalTitle = (LPSTR) operator new(0x104);
    lstrcpyA(globalTitle, windowTitle);
    AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
    if(pModuleState && pModuleState->m_pCurrentWinApp) {
        pModuleState->m_pCurrentWinApp->m_pszAppName = globalTitle;
    }

    // Check device manager object
    iTEUFDrs* pDeviceManager = (iTEUFDrs*) m_unknownParam;
    if(! pDeviceManager) {
        EndDialog(IDCANCEL);
        return TRUE;
    }

    // Check for initialization errors (offset +5 from device manager)
    char errorCode = *((char*) pDeviceManager + 5);
    if(errorCode != 0) {
        LogMessage("ERROR, Error Code=%d", (int) errorCode);
    }

    // Check if device is initialized (offset +4 from device manager)
    char isInitialized = *((char*) pDeviceManager + 4);
    if(isInitialized == 0) {
        // No device found - show error message from string resource 0xd (13)
        ShowWindow(SW_HIDE);

        char errorMessage[256];
        AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
        LoadStringA(pModuleState->m_hCurrentInstanceHandle, 0xd, errorMessage, 0x100);
        AfxMessageBox(errorMessage, 0, 0);

        EndDialog(IDCANCEL);
        return TRUE;
    }

    // Device found and initialized - success path
    char deviceErrorCode = *((char*) pDeviceManager + 5);
    if(deviceErrorCode == 0) {
        // Setup device display - extract filename from global path
        // In original: globalPath = *(char**)0x004af8f0
        // For now, use simplified approach
        LogMessage("Device initialization successful");

        // Send message to progress control (ID 0x3ec = 1004)
        CWnd* pProgressCtrl = GetDlgItem(0x3ec);
        if(pProgressCtrl) {
            pProgressCtrl->SendMessage(0xf1, 1, 0);  // PBM_SETRANGE32 equivalent
        }

        LogMessage("Dialog initialization completed successfully");
    } else {
        // Error in device initialization
        LogMessage("Device initialization failed with error code: %d", (int) deviceErrorCode);

        char errorMessage[256];
        sprintf_s(
            errorMessage,
            sizeof(errorMessage),
            "Device initialization failed with error code: %d",
            (int) deviceErrorCode);
        AfxMessageBox(errorMessage, MB_OK | MB_ICONERROR);

        EndDialog(IDCANCEL);
        return TRUE;
    }

    return TRUE;
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

//=============================================================================
// CMainDialog Implementation
//=============================================================================

// CMainDialog constructor
CMainDialog::CMainDialog(CWnd* pParent) : CDialog(IDD_MAIN, pParent) {
    // Initialize members if needed
}

// Data exchange handler
void CMainDialog::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    // DDX_Control calls would go here when controls are added
}

// Dialog initialization handler
BOOL CMainDialog::OnInitDialog() {
    CDialog::OnInitDialog();

    // Dialog initialization code would go here
    SetWindowText(_T("URescue Main Dialog"));

    return TRUE;
}

// Message map for CMainDialog
BEGIN_MESSAGE_MAP(CMainDialog, CDialog)
// Message handlers would go here
END_MESSAGE_MAP()
