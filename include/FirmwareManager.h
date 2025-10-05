#pragma once

#include <windows.h>

// Firmware management functions
BOOL LoadFirmware(LPCSTR firmwarePath, LPVOID* ppBuffer, DWORD* pSize);
BOOL LoadFirmwareFromFile(LPCSTR firmwarePath, LPVOID* ppBuffer, DWORD* pSize);
BOOL WriteFirmware(HANDLE hDevice, LPCVOID pFirmware, DWORD firmwareSize);
BOOL WriteFirmwareToDevice(HANDLE hDevice, LPCVOID pFirmware, DWORD firmwareSize);
BOOL VerifyFirmware(HANDLE hDevice, LPCVOID pFirmware, DWORD firmwareSize);
BOOL VerifyFirmwareOnDevice(HANDLE hDevice, LPCVOID pFirmware, DWORD firmwareSize);
void CleanupFirmware(LPVOID pBuffer);

// Constants for firmware management
#define FIRMWARE_HEADER_SIZE  512
#define FIRMWARE_SIGNATURE    "ITE_FW"
#define FIRMWARE_VERSION_SIZE 32

// Background thread functions from CUrescueApp
UINT WINAPI BackgroundMonitorThread(LPVOID pParam);
UINT WINAPI BackgroundProcessingThread(LPVOID pParam);

// Helper functions for background threads
BOOL ProcessBootCodeArchive(LPCSTR archivePath);
BOOL ExtractResourceToFile(LPCSTR resourceType, UINT resourceID, LPCSTR outputPath);
