// /c:/Users/oleksandr/Documents/GitHub/Silicon-Power-reverse-FW-ITE-IT1181-A1BA/src/DeviceConnection.cpp

// DeviceConnection.cpp - Device connection implementation
// Integrates iTEUFDrs functionality for connecting to ITE IT1181-A1BA USB devices
// according to Ghidra analysis and reconstructed logic.

#include "../include/Dialogs.h"
#include "../include/Utilities.h"
#include "../include/iTEUFDrs.h"

// DeviceConnectionManager - device connection management
namespace DeviceConnectionManager {

// Connect to device
// Uses iTEUFDrs to detect and initialize devices
BOOL ConnectToDevice(CUrescueDlg* pDialog, iTEUFDrs* pDeviceManager) {
    if(! pDialog || ! pDeviceManager) {
        LogError("ConnectToDevice: Invalid parameters");
        return FALSE;
    }

    LogMessage("ConnectToDevice: Starting device connection process");

    // Set dialog as parent for callbacks
    pDeviceManager->SetParentDialog(pDialog);

    // Check SDK initialization
    if(! pDeviceManager->IsInitialized()) {
        LogError("ConnectToDevice: Device manager not initialized");
        pDialog->SetStatusText("Error: SDK not initialized");
        pDialog->ShowProgressBar(FALSE);
        return FALSE;
    }

    // Show connection progress
    pDialog->SetStatusText("Searching for ITE devices...");
    pDialog->ShowProgressBar(TRUE);
    pDialog->SetProgress(10);

    // Start device detection process
    char deviceDetectionResult = pDeviceManager->GetDeviceInfo();
    pDialog->SetProgress(50);

    if(deviceDetectionResult == 0) {
        LogError("ConnectToDevice: No compatible devices found");
        pDialog->SetStatusText("Device not found. Check USB connection.");
        pDialog->ShowProgressBar(FALSE);
        return FALSE;
    }

    // Get detailed device info (const version)
    const _DEVICE_INFO& deviceInfo = static_cast<const iTEUFDrs*>(pDeviceManager)->GetDeviceInfo();
    pDialog->SetProgress(80);

    // Check if device is found
    if(! deviceInfo.deviceFound) {
        LogError("ConnectToDevice: Device found but not accessible");
        pDialog->SetStatusText("Device found but not accessible");
        pDialog->ShowProgressBar(FALSE);
        return FALSE;
    }

    // Compose success connection message
    CString connectionMessage;
    if(deviceInfo.volumes[0].deviceFound && deviceInfo.volumes[0].volumeLetter != 0) {
        connectionMessage.Format(
            "Connected: ITE IT1181 (Disk %c:)\nController: %04X\nType: %02X",
            deviceInfo.volumes[0].volumeLetter,
            deviceInfo.volumes[0].controllerType,
            deviceInfo.volumes[0].familyType);
    } else {
        connectionMessage = "Connected: ITE IT1181 device";
    }

    pDialog->SetStatusText(connectionMessage);
    pDialog->SetProgress(100);

    // Short delay for progress display
    Sleep(500);
    pDialog->ShowProgressBar(FALSE);

    LogMessage("ConnectToDevice: Successfully connected to device");
    return TRUE;
}

// Disconnect from device
BOOL DisconnectFromDevice(CUrescueDlg* pDialog, iTEUFDrs* pDeviceManager) {
    if(! pDialog) {
        LogError("DisconnectFromDevice: Invalid dialog parameter");
        return FALSE;
    }

    LogMessage("DisconnectFromDevice: Disconnecting from device");

    pDialog->SetStatusText("Disconnecting from device...");

    // If device manager is available, clear association
    if(pDeviceManager) {
        pDeviceManager->SetParentDialog(nullptr);
    }

    pDialog->SetStatusText("Device disconnected");

    LogMessage("DisconnectFromDevice: Device disconnected successfully");
    return TRUE;
}

// Check connection status
BOOL IsDeviceConnected(iTEUFDrs* pDeviceManager) {
    if(! pDeviceManager) {
        return FALSE;
    }

    if(! pDeviceManager->IsInitialized()) {
        return FALSE;
    }

    const _DEVICE_INFO& deviceInfo = static_cast<const iTEUFDrs*>(pDeviceManager)->GetDeviceInfo();
    return deviceInfo.deviceFound && deviceInfo.isInitialized;
}

// Get information about connected device
CString GetDeviceConnectionInfo(iTEUFDrs* pDeviceManager) {
    if(! pDeviceManager || ! IsDeviceConnected(pDeviceManager)) {
        return "Device not connected";
    }

    const _DEVICE_INFO& deviceInfo = static_cast<const iTEUFDrs*>(pDeviceManager)->GetDeviceInfo();

    CString info;
    info.Format(
        "ITE IT1181 Flash Drive\n"
        "Status: Connected\n"
        "Disk: %c:\n"
        "Controller: %04X\n"
        "Type: %02X\n"
        "Ready: %s",
        deviceInfo.volumes[0].volumeLetter,
        deviceInfo.volumes[0].controllerType,
        deviceInfo.volumes[0].familyType,
        deviceInfo.systemReady ? "Ready" : "Not ready");

    return info;
}

// Automatic connection on startup
BOOL AutoConnectDevice(CUrescueDlg* pDialog, iTEUFDrs* pDeviceManager) {
    LogMessage("AutoConnectDevice: Attempting automatic device connection");

    if(! pDialog || ! pDeviceManager) {
        LogError("AutoConnectDevice: Invalid parameters");
        return FALSE;
    }

    // Try to connect automatically
    pDialog->SetStatusText("Automatic device search...");

    BOOL result = ConnectToDevice(pDialog, pDeviceManager);

    if(result) {
        LogMessage("AutoConnectDevice: Automatic connection successful");
        // Add message about successful auto connection
        CString statusMsg = GetDeviceConnectionInfo(pDeviceManager);
        statusMsg += "\n(Automatically connected)";
        pDialog->SetStatusText(statusMsg);
    } else {
        LogMessage("AutoConnectDevice: Automatic connection failed");
        pDialog->SetStatusText("Ready to connect device");
    }

    return result;
}

}  // namespace DeviceConnectionManager
