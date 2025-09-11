#pragma once

#include <windows.h>

// Resource management functions (decompiled from Ghidra)
HRSRC FindResourceWrapper(HMODULE hModule, LPCSTR lpName, LPCSTR lpType);

HGLOBAL LoadResourceWrapper(HMODULE hModule, HRSRC hResInfo);

LPVOID LockResourceWrapper(HGLOBAL hResData);

DWORD SizeofResourceWrapper(HMODULE hModule, HRSRC hResInfo);

// Functions for loading firmware from resources
BOOL LoadFirmwareFromResource(LPCSTR resourceName, LPVOID* ppBuffer, DWORD* pSize);

BOOL LoadBootCodeFromResource(LPVOID* ppBuffer, DWORD* pSize);

BOOL LoadSDKFromResource(LPVOID* ppBuffer, DWORD* pSize);

// Additional resource management functions
BOOL ResourceExists(LPCSTR resourceName, LPCSTR resourceType);

DWORD GetResourceList(LPCSTR resourceType, LPCSTR* resourceNames, DWORD maxCount);

BOOL SaveResourceToFile(LPCSTR resourceName, LPCSTR resourceType, LPCSTR fileName);

BOOL LoadResourceFromFile(LPCSTR fileName, LPVOID* ppBuffer, DWORD* pSize);

// Constants for resource management
#define FIRMWARE_RESOURCE_TYPE RT_RCDATA
#define BOOTCODE_RESOURCE_NAME "BootCode"
#define SDK_RESOURCE_NAME      "FlashSDK"
#define MAX_RESOURCE_NAME      256
