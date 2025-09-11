#pragma once

// Prevent conflicts and ensure minimal includes
#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
    #define NOMINMAX
#endif

#include <windows.h>

#include "DeviceStructures.h"

// System functions (decompiled from Ghidra)
BOOL GetSystemVersion(OSVERSIONINFOA* pVersionInfo);

BOOL GetSystemInfoWrapper(SYSTEM_INFO* pSystemInfo);

int GetSystemMetricsWrapper(int nIndex);

// Registry functions (decompiled from Ghidra)
LONG OpenRegistryKey(HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult, REGSAM samDesired);

LONG CloseRegistryKey(HKEY hKey);

LONG ReadRegistryValue(
    HKEY hKey,
    LPCSTR lpValueName,
    LPDWORD lpType,
    LPBYTE lpData,
    LPDWORD lpcbData);

LONG WriteRegistryValue(
    HKEY hKey,
    LPCSTR lpValueName,
    DWORD dwType,
    CONST BYTE* lpData,
    DWORD cbData);

LONG CreateRegistryKey(HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult, LPDWORD lpdwDisposition);

LONG DeleteRegistryKey(HKEY hKey, LPCSTR lpSubKey);

LONG DeleteRegistryValue(HKEY hKey, LPCSTR lpValueName);

// Additional system functions
BOOL GetDeviceInfo(DEVICE_INFO* pDeviceInfo);

BOOL SaveSettingsToRegistry(LPCSTR settingsPath, LPCVOID pSettings, DWORD settingsSize);

BOOL LoadSettingsFromRegistry(LPCSTR settingsPath, LPVOID pSettings, DWORD* pSettingsSize);

BOOL DeleteSettingsFromRegistry(LPCSTR settingsPath);

BOOL IsAdministrator();

BOOL GetSystemDirectoryPath(LPSTR lpBuffer, DWORD nSize);

BOOL GetWindowsDirectoryPath(LPSTR lpBuffer, DWORD nSize);

// Device.ini helpers
BOOL ReadDeviceSelectionFromIni(LPSTR lpOut, DWORD nSize);

// Constants for system operations
#define URESCUE_REGISTRY_KEY    "Software\\URescue"
#define SETTINGS_REGISTRY_VALUE "Settings"
#define VERSION_REGISTRY_VALUE  "Version"
#define MAX_PATH_LENGTH         260
