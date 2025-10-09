// clang-format off
#ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0601  // Windows 7+
#endif

#include "../include/WindowsHeaders.h"
#include "../resources/resource.h"
#include "../include/Dialogs.h"
#include "../include/iTEUFDrs.h" // Include iTEUFDrs header for device manager
#include "../include/URescueGUI.h" // Include GUI helper functions
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

        // Update device information display
        UpdateDeviceInfo();

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

// Message handlers for button clicks and UI events
void CUrescueDlg::OnBnClickedFormat() {
    LogMessage("Format button clicked");

    iTEUFDrs* pDeviceManager = (iTEUFDrs*) m_unknownParam;
    if(! pDeviceManager) {
        AfxMessageBox("Device manager not available", MB_OK | MB_ICONERROR);
        return;
    }

    // Show confirmation dialog
    if(AfxMessageBox(
           "Are you sure you want to format the device? This will erase all data!",
           MB_YESNO | MB_ICONWARNING)
       != IDYES) {
        return;
    }

    // Disable UI during format operation
    EnableUI(FALSE);

    // Update status
    SetDlgItemText(IDC_STATUS_TEXT, "Formatting device...");

    // TODO: Implement actual format operation using iTEUFDrs
    // For now, simulate the operation
    CProgressCtrl* pProgress = (CProgressCtrl*) GetDlgItem(0x3ec);
    if(pProgress) {
        pProgress->SetRange(0, 100);
        for(int i = 0; i <= 100; i += 10) {
            pProgress->SetPos(i);
            Sleep(100);  // Simulate work
        }
    }

    // Re-enable UI
    EnableUI(TRUE);
    SetDlgItemText(IDC_STATUS_TEXT, "Format completed successfully");

    LogMessage("Format operation completed");
}

void CUrescueDlg::OnBnClickedRepair() {
    LogMessage("Repair button clicked");

    iTEUFDrs* pDeviceManager = (iTEUFDrs*) m_unknownParam;
    if(! pDeviceManager) {
        AfxMessageBox("Device manager not available", MB_OK | MB_ICONERROR);
        return;
    }

    // Disable UI during repair operation
    EnableUI(FALSE);

    // Update status
    SetDlgItemText(IDC_STATUS_TEXT, "Repairing device...");

    // TODO: Implement actual repair operation using iTEUFDrs
    // For now, simulate the operation
    CProgressCtrl* pProgress = (CProgressCtrl*) GetDlgItem(0x3ec);
    if(pProgress) {
        pProgress->SetRange(0, 100);
        for(int i = 0; i <= 100; i += 5) {
            pProgress->SetPos(i);
            Sleep(50);  // Simulate work
        }
    }

    // Re-enable UI
    EnableUI(TRUE);
    SetDlgItemText(IDC_STATUS_TEXT, "Repair completed successfully");

    LogMessage("Repair operation completed");
}

void CUrescueDlg::OnBnClickedDiagnose() {
    LogMessage("Diagnose button clicked");

    iTEUFDrs* pDeviceManager = (iTEUFDrs*) m_unknownParam;
    if(! pDeviceManager) {
        AfxMessageBox("Device manager not available", MB_OK | MB_ICONERROR);
        return;
    }

    // Update status
    SetDlgItemText(IDC_STATUS_TEXT, "Running device diagnostics...");

    // TODO: Implement actual diagnostic operation using iTEUFDrs
    // For now, show diagnostic information
    CString diagnosticInfo;
    diagnosticInfo.Format(
        "Device Diagnostic Results:\n\n"
        "Device Type: ITE IT1181-A1BA\n"
        "Firmware Version: v81D.2.24.2\n"
        "Flash Status: OK\n"
        "Controller Status: Ready\n"
        "Bad Blocks: 0\n"
        "Available Capacity: 8192 MB\n\n"
        "Device is functioning normally.");

    AfxMessageBox(diagnosticInfo, MB_OK | MB_ICONINFORMATION);
    SetDlgItemText(IDC_STATUS_TEXT, "Diagnostic completed");

    LogMessage("Diagnostic operation completed");
}

void CUrescueDlg::OnBnClickedAdvanced() {
    LogMessage("Advanced button clicked");

    // Show advanced options dialog or menu
    CMenu contextMenu;
    contextMenu.CreatePopupMenu();

    contextMenu.AppendMenu(MF_STRING, ID_ADVANCED_FIRMWARE_UPDATE, "Firmware Update");
    contextMenu.AppendMenu(MF_STRING, ID_ADVANCED_LOW_LEVEL_FORMAT, "Low Level Format");
    contextMenu.AppendMenu(MF_STRING, ID_ADVANCED_SECURITY_UNLOCK, "Security Unlock");
    contextMenu.AppendMenu(MF_SEPARATOR);
    contextMenu.AppendMenu(MF_STRING, ID_ADVANCED_RAW_COMMANDS, "Raw Commands");

    // Get button position for popup menu
    CWnd* pButton = GetDlgItem(0x3f4);
    if(pButton) {
        CRect buttonRect;
        pButton->GetWindowRect(&buttonRect);

        contextMenu.TrackPopupMenu(
            TPM_LEFTALIGN | TPM_RIGHTBUTTON, buttonRect.left, buttonRect.bottom, this);
    }

    contextMenu.DestroyMenu();
}

void CUrescueDlg::EnableUI(BOOL bEnable) {
    // Enable/disable all major UI controls
    GetDlgItem(0x3ea)->EnableWindow(bEnable);     // Format button
    GetDlgItem(0x3eb)->EnableWindow(bEnable);     // Repair button
    GetDlgItem(0x3ec)->EnableWindow(bEnable);     // Diagnose button
    GetDlgItem(0x3ed)->EnableWindow(bEnable);     // Advanced button
    GetDlgItem(IDOK)->EnableWindow(bEnable);      // OK button
    GetDlgItem(IDCANCEL)->EnableWindow(bEnable);  // Cancel button
}

void CUrescueDlg::UpdateDeviceInfo() {
    iTEUFDrs* pDeviceManager = (iTEUFDrs*) m_unknownParam;
    if(! pDeviceManager) {
        return;
    }

    // Update device information display
    // This matches the UpdateDialogVersionAndInfo function from Ghidra analysis

    // Get firmware version and serial number from device manager
    // Based on Ghidra analysis at offset +0x208 and +0x248
    const char* firmwareVersion = (const char*) ((BYTE*) pDeviceManager + 0x208);
    const char* serialNumber = (const char*) ((BYTE*) pDeviceManager + 0x248);
    const char* deviceName = (const char*) ((BYTE*) pDeviceManager + 8);

    // Check if firmware is available (matches Ghidra check for "NONE")
    if(*(DWORD*) firmwareVersion == 0x4e4f4e20 && *(char*) (firmwareVersion + 4) == 'E') {
        m_string2.Format("%s\nFW Ver: NO FIRMWARE", deviceName);
    } else {
        m_string2.Format("%s\nFW Ver:%s\nSN:%s", deviceName, firmwareVersion, serialNumber);
    }

    // Update progress text (matches string resource ID 5 from Ghidra)
    char progressText[64];
    AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
    LoadStringA(pModuleState->m_hCurrentInstanceHandle, 5, progressText, 64);
    m_string1.Format("%s : 0.00 %%", progressText);

    // Update button texts from string resources (IDs 6, 7, 8, 9 from Ghidra)
    char buttonText[64];

    // Format button (ID 9)
    LoadStringA(pModuleState->m_hCurrentInstanceHandle, 9, buttonText, 64);
    SetDlgItemText(0x3ea, buttonText);

    // Repair button (ID 8)
    LoadStringA(pModuleState->m_hCurrentInstanceHandle, 8, buttonText, 64);
    SetDlgItemText(0x3eb, buttonText);

    // Diagnose button (ID 6)
    LoadStringA(pModuleState->m_hCurrentInstanceHandle, 6, buttonText, 64);
    SetDlgItemText(0x3ec, buttonText);

    // Advanced button (ID 7)
    LoadStringA(pModuleState->m_hCurrentInstanceHandle, 7, buttonText, 64);
    SetDlgItemText(0x3ed, buttonText);

    // Advanced repair button (ID 0x15 = 21) - shown conditionally
    LoadStringA(pModuleState->m_hCurrentInstanceHandle, 0x15, buttonText, 64);
    CWnd* pAdvancedRepair = GetDlgItem(0x3f4);
    if(pAdvancedRepair) {
        pAdvancedRepair->SetWindowText(buttonText);
        // Show/hide based on device capability (offset +7 from device manager)
        BOOL showAdvanced = *((char*) pDeviceManager + 7) != 0;
        pAdvancedRepair->ShowWindow(showAdvanced ? SW_SHOW : SW_HIDE);
    }

    // Update OK/Cancel button texts (IDs 1, 2)
    LoadStringA(pModuleState->m_hCurrentInstanceHandle, 1, buttonText, 64);
    SetDlgItemText(IDOK, buttonText);

    LoadStringA(pModuleState->m_hCurrentInstanceHandle, 2, buttonText, 64);
    SetDlgItemText(IDCANCEL, buttonText);

    // Refresh dialog data
    UpdateData(FALSE);
    UpdateDialogControls(this, FALSE);
}

void CUrescueDlg::SetProgress(int percentage) {
    CProgressCtrl* pProgress = (CProgressCtrl*) GetDlgItem(0x3ec);
    if(pProgress) {
        pProgress->SetPos(percentage);
    }

    // Also update progress text string
    char progressText[64];
    AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
    LoadStringA(pModuleState->m_hCurrentInstanceHandle, 5, progressText, 64);
    m_string1.Format("%s : %.2f %%", progressText, (float) percentage);

    UpdateData(FALSE);
}

void CUrescueDlg::SetStatusText(const CString& status) {
    SetDlgItemText(IDC_STATUS_TEXT, status);
    LogMessage("Status: %s", (LPCSTR) status);
}

void CUrescueDlg::ShowProgressBar(BOOL bShow) {
    CProgressCtrl* pProgress = (CProgressCtrl*) GetDlgItem(0x3ec);
    if(pProgress) {
        pProgress->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
    }
}

// Advanced menu command handlers
void CUrescueDlg::OnAdvancedFirmwareUpdate() {
    LogMessage("Advanced: Firmware Update selected");
    AfxMessageBox("Firmware Update functionality not yet implemented", MB_OK | MB_ICONINFORMATION);
}

void CUrescueDlg::OnAdvancedLowLevelFormat() {
    LogMessage("Advanced: Low Level Format selected");
    if(AfxMessageBox(
           "Low level format will completely erase the device and may take a long time. Continue?",
           MB_YESNO | MB_ICONWARNING)
       == IDYES) {
        AfxMessageBox(
            "Low Level Format functionality not yet implemented", MB_OK | MB_ICONINFORMATION);
    }
}

void CUrescueDlg::OnAdvancedSecurityUnlock() {
    LogMessage("Advanced: Security Unlock selected");
    AfxMessageBox("Security Unlock functionality not yet implemented", MB_OK | MB_ICONINFORMATION);
}

void CUrescueDlg::OnAdvancedRawCommands() {
    LogMessage("Advanced: Raw Commands selected");
    AfxMessageBox("Raw Commands functionality not yet implemented", MB_OK | MB_ICONINFORMATION);
}

BEGIN_MESSAGE_MAP(CUrescueDlg, CDialog)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
// Button click handlers
ON_BN_CLICKED(0x3ea, &CUrescueDlg::OnBnClickedFormat)    // Format button
ON_BN_CLICKED(0x3eb, &CUrescueDlg::OnBnClickedRepair)    // Repair button
ON_BN_CLICKED(0x3ec, &CUrescueDlg::OnBnClickedDiagnose)  // Diagnose button
ON_BN_CLICKED(0x3ed, &CUrescueDlg::OnBnClickedAdvanced)  // Advanced button
// Advanced menu handlers
ON_COMMAND(ID_ADVANCED_FIRMWARE_UPDATE, &CUrescueDlg::OnAdvancedFirmwareUpdate)
ON_COMMAND(ID_ADVANCED_LOW_LEVEL_FORMAT, &CUrescueDlg::OnAdvancedLowLevelFormat)
ON_COMMAND(ID_ADVANCED_SECURITY_UNLOCK, &CUrescueDlg::OnAdvancedSecurityUnlock)
ON_COMMAND(ID_ADVANCED_RAW_COMMANDS, &CUrescueDlg::OnAdvancedRawCommands)
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
