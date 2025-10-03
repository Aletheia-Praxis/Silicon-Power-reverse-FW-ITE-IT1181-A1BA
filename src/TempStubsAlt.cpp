#include "../include/WindowsHeaders.h"

// Alternative stub file with correct signatures based on linker errors

// SDK functions - using HMODULE like original
BOOL Load181FlashSDK(const char* path, HMODULE* outModule) {
    if(outModule)
        *outModule = nullptr;
    return TRUE;  // Return TRUE like original
}

BOOL InitializeFlashSDK(HMODULE hModule) {
    return TRUE;  // Return TRUE like original
}

void Unload181FlashSDK(HMODULE hModule) {}

// Other functions from TempStubs.cpp
int LoadFirmware(const char* path, void** data, unsigned long* size) {
    return 0;
}

int WriteFirmware(void* device, const void* data, unsigned long size) {
    return 0;
}

int VerifyFirmware(void* device, const void* data, unsigned long size) {
    return 0;
}

void* InitializeUSBDevice(const char* path) {
    return nullptr;
}

void CloseUSBDevice(void* device) {}

int InitializeITEController(void* device) {
    return 0;
}

int GetControllerInfo(void* device, struct _ITE_CONTROLLER_INFO* info) {
    return 0;
}

int SetControllerMode(void* device, unsigned char mode) {
    return 0;
}

int Read(
    void* device,
    void* buffer,
    unsigned long bytesToRead,
    unsigned long* bytesRead,
    struct _OVERLAPPED* overlapped) {
    return 0;
}

int Write(
    void* device,
    const void* buffer,
    unsigned long bytesToWrite,
    unsigned long* bytesWritten,
    struct _OVERLAPPED* overlapped) {
    return 0;
}

extern "C" int CUrescueApp_InitInstance() {
    return 1;
}
