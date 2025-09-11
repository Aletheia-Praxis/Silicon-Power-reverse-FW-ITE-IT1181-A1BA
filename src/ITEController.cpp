#include "ITEController.h"

#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "USBDevice.h"

// Function to initialize the ITE controller
BOOL InitializeITEController(HANDLE hDevice) {
    // Parameter check
    if(! hDevice || hDevice == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    // Controller initialization command
    BYTE initCommand[] = { 0x55, 0xAA, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };
    DWORD bytesWritten;

    BOOL result = Write(hDevice, initCommand, sizeof(initCommand), &bytesWritten, NULL);
    if(! result || bytesWritten != sizeof(initCommand)) {
        return FALSE;
    }

    // Wait for response
    Sleep(100);

    // Read response
    BYTE response[8];
    DWORD bytesRead;

    result = Read(hDevice, response, sizeof(response), &bytesRead, NULL);
    if(! result || bytesRead != sizeof(response)) {
        return FALSE;
    }

    // Check response
    if(response[0] != 0xAA || response[1] != 0x55) {
        return FALSE;
    }

    return TRUE;
}

// Function to get controller information
BOOL GetControllerInfo(HANDLE hDevice, ITE_CONTROLLER_INFO* pInfo) {
    // Parameter check
    if(! hDevice || ! pInfo) {
        return FALSE;
    }

    // Command to get information
    BYTE infoCommand[] = { 0x55, 0xAA, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00 };
    DWORD bytesWritten;

    BOOL result = Write(hDevice, infoCommand, sizeof(infoCommand), &bytesWritten, NULL);
    if(! result || bytesWritten != sizeof(infoCommand)) {
        return FALSE;
    }

    // Wait for response
    Sleep(100);

    // Read information
    result = Read(hDevice, pInfo, sizeof(ITE_CONTROLLER_INFO), &bytesWritten, NULL);
    if(! result || bytesWritten != sizeof(ITE_CONTROLLER_INFO)) {
        return FALSE;
    }

    return TRUE;
}

// Function to set the operating mode
BOOL SetControllerMode(HANDLE hDevice, BYTE mode) {
    // Parameter check
    if(! hDevice || hDevice == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    // Command to set mode
    BYTE modeCommand[] = { 0x55, 0xAA, 0x03, mode, 0x00, 0x00, 0x00, 0x00 };
    DWORD bytesWritten;

    BOOL result = Write(hDevice, modeCommand, sizeof(modeCommand), &bytesWritten, NULL);
    if(! result || bytesWritten != sizeof(modeCommand)) {
        return FALSE;
    }

    // Wait for confirmation
    Sleep(100);

    // Read confirmation
    BYTE response[8];
    DWORD bytesRead;

    result = Read(hDevice, response, sizeof(response), &bytesRead, NULL);
    if(! result || bytesRead != sizeof(response)) {
        return FALSE;
    }

    // Check confirmation
    if(response[0] != 0xAA || response[1] != 0x55 || response[2] != 0x03) {
        return FALSE;
    }

    return TRUE;
}

// Function to check the controller status
BOOL GetControllerStatus(HANDLE hDevice, ITE_CONTROLLER_STATUS* pStatus) {
    // Parameter check
    if(! hDevice || ! pStatus) {
        return FALSE;
    }

    // Command to get status
    BYTE statusCommand[] = { 0x55, 0xAA, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00 };
    DWORD bytesWritten;

    BOOL result = Write(hDevice, statusCommand, sizeof(statusCommand), &bytesWritten, NULL);
    if(! result || bytesWritten != sizeof(statusCommand)) {
        return FALSE;
    }

    // Wait for response
    Sleep(100);

    // Read status
    result = Read(hDevice, pStatus, sizeof(ITE_CONTROLLER_STATUS), &bytesWritten, NULL);
    if(! result || bytesWritten != sizeof(ITE_CONTROLLER_STATUS)) {
        return FALSE;
    }

    return TRUE;
}

// Function to reset the controller
BOOL ResetController(HANDLE hDevice) {
    // Parameter check
    if(! hDevice || hDevice == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    // Reset command
    BYTE resetCommand[] = { 0x55, 0xAA, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00 };
    DWORD bytesWritten;

    BOOL result = Write(hDevice, resetCommand, sizeof(resetCommand), &bytesWritten, NULL);
    if(! result || bytesWritten != sizeof(resetCommand)) {
        return FALSE;
    }

    // Wait for reset
    Sleep(1000);

    return TRUE;
}
