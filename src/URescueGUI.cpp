// clang-format off
#ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0601  // Windows 7+
#endif

#include "../include/URescueGUI.h"

#include "../include/FirmwareManager.h"
#include "../include/ITEController.h"
#include "../include/USBDevice.h"
#include "../include/WindowsHeaders.h"
#include "../include/Dialogs.h"
// clang-format on

// Main window constructor
CMainFrame::CMainFrame() : m_hDevice(INVALID_HANDLE_VALUE), m_pFirmware(NULL), m_firmwareSize(0) {
    // Creating main window
    Create(NULL, _T("URescue v81D.2.24.2 - ITE IT1181 Firmware Recovery Tool"));

    // Setting window size
    SetWindowPos(NULL, 0, 0, 800, 600, SWP_NOMOVE | SWP_NOZORDER);

    // Creating interface elements
    CreateControls();
}

// Main window destructor
CMainFrame::~CMainFrame() {
    // Resource cleanup
}

// Creating interface elements
void CMainFrame::CreateControls() {
    // Creating toolbar
    m_toolBar.Create(this);
    m_toolBar.LoadToolBar(IDR_MAINFRAME);

    // Creating status bar
    m_statusBar.Create(this);
    m_statusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));

    // Creating main dialog
    m_mainDialog.Create(IDD_MAIN, this);
    m_mainDialog.ShowWindow(SW_SHOW);
}

// Message handlers for CMainFrame
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
ON_WM_CREATE()
ON_WM_SIZE()
ON_COMMAND(ID_DEVICE_CONNECT, &CMainFrame::OnDeviceConnect)
ON_COMMAND(ID_DEVICE_DISCONNECT, &CMainFrame::OnDeviceDisconnect)
ON_COMMAND(ID_FIRMWARE_LOAD, &CMainFrame::OnFirmwareLoad)
ON_COMMAND(ID_FIRMWARE_WRITE, &CMainFrame::OnFirmwareWrite)
ON_COMMAND(ID_FIRMWARE_VERIFY, &CMainFrame::OnFirmwareVerify)
ON_COMMAND(ID_HELP_ABOUT, &CMainFrame::OnHelpAbout)
END_MESSAGE_MAP()

// Window creation handler
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    if(CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Interface initialization
    return 0;
}

// Window resize handler
void CMainFrame::OnSize(UINT nType, int cx, int cy) {
    CFrameWnd::OnSize(nType, cx, cy);

    // Redrawing the interface
    if(m_mainDialog.GetSafeHwnd()) {
        m_mainDialog.MoveWindow(0, 0, cx, cy);
    }
}

// Device connection handler
void CMainFrame::OnDeviceConnect() {
    // Displaying device selection dialog
    CDeviceSelectDialog dlg;
    if(dlg.DoModal() == IDOK) {
        // Connecting to selected device
        CString devicePath = dlg.GetSelectedDevice();
        HANDLE hDevice = InitializeUSBDevice(devicePath.GetString());
        if(hDevice) {
            // Controller initialization
            if(InitializeITEController(hDevice)) {
                m_hDevice = hDevice;
                m_statusBar.SetPaneText(0, _T("Device Connected"));

                // Getting controller information
                ITE_CONTROLLER_INFO info;
                if(GetControllerInfo(hDevice, &info)) {
                    CString strInfo;
                    strInfo.Format(_T("Controller: %s, Version: %s"), info.model, info.version);
                    m_statusBar.SetPaneText(1, strInfo);
                }
            } else {
                CloseUSBDevice(hDevice);
                MessageBox(
                    _T("Failed to initialize controller"), _T("Error"), MB_OK | MB_ICONERROR);
            }
        } else {
            MessageBox(_T("Failed to connect to device"), _T("Error"), MB_OK | MB_ICONERROR);
        }
    }
}

// Device disconnection handler
void CMainFrame::OnDeviceDisconnect() {
    if(m_hDevice && m_hDevice != INVALID_HANDLE_VALUE) {
        CloseUSBDevice(m_hDevice);
        m_hDevice = INVALID_HANDLE_VALUE;
        m_statusBar.SetPaneText(0, _T("Device Disconnected"));
        m_statusBar.SetPaneText(1, _T(""));
    }
}

// Firmware loading handler
void CMainFrame::OnFirmwareLoad() {
    // Displaying firmware file selection dialog
    CFileDialog dlg(
        TRUE,
        _T("bin"),
        _T("*.bin"),
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        _T("Firmware Files (*.bin)|*.bin||"));

    if(dlg.DoModal() == IDOK) {
        CString firmwarePath = dlg.GetPathName();

        // Loading firmware
        LPVOID pFirmware;
        DWORD firmwareSize;

        if(LoadFirmware(firmwarePath.GetString(), &pFirmware, &firmwareSize)) {
            m_pFirmware = pFirmware;
            m_firmwareSize = firmwareSize;

            CString strInfo;
            strInfo.Format(_T("Firmware loaded: %d bytes"), firmwareSize);
            m_statusBar.SetPaneText(2, strInfo);
        } else {
            MessageBox(_T("Failed to load firmware"), _T("Error"), MB_OK | MB_ICONERROR);
        }
    }
}

// Firmware writing handler
void CMainFrame::OnFirmwareWrite() {
    if(! m_hDevice || m_hDevice == INVALID_HANDLE_VALUE) {
        MessageBox(_T("No device connected"), _T("Error"), MB_OK | MB_ICONERROR);
        return;
    }

    if(! m_pFirmware || m_firmwareSize == 0) {
        MessageBox(_T("No firmware loaded"), _T("Error"), MB_OK | MB_ICONERROR);
        return;
    }

    // Setting write mode
    if(! SetControllerMode(m_hDevice, ITE_MODE_FLASH)) {
        MessageBox(_T("Failed to set flash mode"), _T("Error"), MB_OK | MB_ICONERROR);
        return;
    }

    // Writing firmware
    if(WriteFirmware(m_hDevice, m_pFirmware, m_firmwareSize)) {
        m_statusBar.SetPaneText(2, _T("Firmware written successfully"));
    } else {
        MessageBox(_T("Failed to write firmware"), _T("Error"), MB_OK | MB_ICONERROR);
    }

    // Returning to normal mode
    SetControllerMode(m_hDevice, ITE_MODE_NORMAL);
}

// Firmware verification handler
void CMainFrame::OnFirmwareVerify() {
    if(! m_hDevice || m_hDevice == INVALID_HANDLE_VALUE) {
        MessageBox(_T("No device connected"), _T("Error"), MB_OK | MB_ICONERROR);
        return;
    }

    if(! m_pFirmware || m_firmwareSize == 0) {
        MessageBox(_T("No firmware loaded"), _T("Error"), MB_OK | MB_ICONERROR);
        return;
    }

    // Verifying firmware
    if(VerifyFirmware(m_hDevice, m_pFirmware, m_firmwareSize)) {
        m_statusBar.SetPaneText(2, _T("Firmware verification successful"));
    } else {
        m_statusBar.SetPaneText(2, _T("Firmware verification failed"));
    }
}

// Help handler
void CMainFrame::OnHelpAbout() {
    CAboutDialog dlg;
    dlg.DoModal();
}

//=============================================================================
// Implementations for dialogs are now in Dialogs.cpp
//=============================================================================
