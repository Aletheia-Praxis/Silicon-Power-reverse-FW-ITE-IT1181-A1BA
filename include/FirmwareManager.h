#pragma once

#include <windows.h>

// Firmware management functions
BOOL LoadFirmware(LPCSTR firmwarePath, LPVOID* ppBuffer, DWORD* pSize);
BOOL WriteFirmware(HANDLE hDevice, LPCVOID pFirmware, DWORD firmwareSize);
BOOL VerifyFirmware(HANDLE hDevice, LPCVOID pFirmware, DWORD firmwareSize);
void CleanupFirmware(LPVOID pBuffer);

// Constants for firmware management
#define FIRMWARE_HEADER_SIZE  512
#define FIRMWARE_SIGNATURE    "ITE_FW"
#define FIRMWARE_VERSION_SIZE 32
