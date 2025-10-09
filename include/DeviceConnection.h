#pragma once

#include <afx.h>

#include "WindowsHeaders.h"

// Forward declarations
class CUrescueDlg;
class iTEUFDrs;

// DeviceConnectionManager: device connection management
namespace DeviceConnectionManager {

//
// ConnectToDevice
// Connects to the device.
// Parameters:
//   pDialog - pointer to main dialog for UI updates
//   pDeviceManager - pointer to iTEUFDrs device manager
// Returns:
//   TRUE if connection is successful, FALSE otherwise
//
BOOL ConnectToDevice(CUrescueDlg* pDialog, iTEUFDrs* pDeviceManager);

//
// DisconnectFromDevice
// Disconnects from the device.
// Parameters:
//   pDialog - pointer to main dialog for UI updates
//   pDeviceManager - pointer to iTEUFDrs device manager
// Returns:
//   TRUE if disconnection is successful, FALSE otherwise
//
BOOL DisconnectFromDevice(CUrescueDlg* pDialog, iTEUFDrs* pDeviceManager);

//
// IsDeviceConnected
// Checks connection status.
// Parameters:
//   pDeviceManager - pointer to iTEUFDrs device manager
// Returns:
//   TRUE if device is connected, FALSE otherwise
//
BOOL IsDeviceConnected(iTEUFDrs* pDeviceManager);

//
// GetDeviceConnectionInfo
// Gets information about the connected device.
// Parameters:
//   pDeviceManager - pointer to iTEUFDrs device manager
// Returns:
//   CString with device information
//
CString GetDeviceConnectionInfo(iTEUFDrs* pDeviceManager);

//
// AutoConnectDevice
// Attempts automatic connection on startup.
// Parameters:
//   pDialog - pointer to main dialog for UI updates
//   pDeviceManager - pointer to iTEUFDrs device manager
// Returns:
//   TRUE if auto-connect is successful, FALSE otherwise
//
BOOL AutoConnectDevice(CUrescueDlg* pDialog, iTEUFDrs* pDeviceManager);

}  // namespace DeviceConnectionManager
