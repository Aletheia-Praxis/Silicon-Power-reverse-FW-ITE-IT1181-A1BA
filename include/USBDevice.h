#pragma once

#include "USBHeaders.h"

// USB device functions
BOOL Read(
    HANDLE hDevice,
    LPVOID lpBuffer,
    DWORD nNumberOfBytesToRead,
    LPDWORD lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped);
BOOL Write(
    HANDLE hDevice,
    LPCVOID lpBuffer,
    DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped);
HANDLE InitializeUSBDevice(LPCSTR devicePath);
void CloseUSBDevice(HANDLE hDevice);

// Constants for USB operations
#define USB_TIMEOUT     5000
#define USB_BUFFER_SIZE 4096
#define USB_MAX_DEVICES 10
