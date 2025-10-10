#include "../include/USBDevice.h"

// Function to read data from the USB device
BOOL Read(
    HANDLE hDevice,
    LPVOID lpBuffer,
    DWORD nNumberOfBytesToRead,
    LPDWORD lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped) {
    // Parameter check
    if(! hDevice || ! lpBuffer || ! lpNumberOfBytesRead) {
        return FALSE;
    }

    // Reading data from the device
    BOOL result =
        ReadFile(hDevice, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);

    // Error handling
    if(! result) {
        DWORD error = GetLastError();
        if(error != ERROR_IO_PENDING) {
            return FALSE;
        }
    }

    return TRUE;
}

// Function to write data to the USB device
BOOL Write(
    HANDLE hDevice,
    LPCVOID lpBuffer,
    DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped) {
    // Parameter check
    if(! hDevice || ! lpBuffer || ! lpNumberOfBytesWritten) {
        return FALSE;
    }

    // Writing data to the device
    BOOL result =
        WriteFile(hDevice, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);

    // Error handling
    if(! result) {
        DWORD error = GetLastError();
        if(error != ERROR_IO_PENDING) {
            return FALSE;
        }
    }

    return TRUE;
}

// Function to initialize the USB device
HANDLE InitializeUSBDevice(LPCSTR devicePath) {
    // Opening the device
    HANDLE hDevice = CreateFileA(
        devicePath,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
        NULL);

    if(hDevice == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    return hDevice;
}

// Function to close the USB device
void CloseUSBDevice(HANDLE hDevice) {
    if(hDevice && hDevice != INVALID_HANDLE_VALUE) {
        CloseHandle(hDevice);
    }
}
