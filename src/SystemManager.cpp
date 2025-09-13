#include "../include/SystemManager.h"

#include <shlwapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysinfoapi.h>
#include <windows.h>
#include <winreg.h>

#include "../include/Utilities.h"

// Function to get the system version (decompiled from FUN_0046bc3d)
BOOL GetSystemVersion(OSVERSIONINFOA* pVersionInfo) {
    if(! pVersionInfo) {
        LogError("Invalid parameter for GetSystemVersion");
        return FALSE;
    }

    LogMessage("Getting system version information");

    pVersionInfo->dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);

    BOOL result = GetVersionExA(pVersionInfo);

    if(! result) {
        DWORD error = GetLastError();
        LogError("GetVersionEx failed with error: %lu", error);
        return FALSE;
    }

    LogMessage(
        "System version: %lu.%lu.%lu",
        pVersionInfo->dwMajorVersion,
        pVersionInfo->dwMinorVersion,
        pVersionInfo->dwBuildNumber);

    return TRUE;
}

// Function to get system information (decompiled from FUN_0046ba6c)
BOOL GetSystemInfoWrapper(SYSTEM_INFO* pSystemInfo) {
    if(! pSystemInfo) {
        LogError("Invalid parameter for GetSystemInfoWrapper");
        return FALSE;
    }

    LogMessage("Getting system information");

    GetSystemInfo(pSystemInfo);

    LogMessage("Processor architecture: %lu", pSystemInfo->wProcessorArchitecture);
    LogMessage("Number of processors: %lu", pSystemInfo->dwNumberOfProcessors);
    LogMessage("Page size: %lu", pSystemInfo->dwPageSize);

    return TRUE;
}

// Function to get system metrics (decompiled from FUN_0045719c)
int GetSystemMetricsWrapper(int nIndex) {
    LogMessage("Getting system metric: %d", nIndex);

    int metric = GetSystemMetrics(nIndex);

    LogMessage("System metric %d: %d", nIndex, metric);
    return metric;
}

// Registry function - open key (decompiled from FUN_00457171)
LONG OpenRegistryKey(HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult, REGSAM samDesired) {
    LogMessage("Opening registry key: %s", lpSubKey ? lpSubKey : "NULL");

    LONG result = RegOpenKeyExA(hKey, lpSubKey, 0, samDesired, phkResult);

    if(result == ERROR_SUCCESS) {
        LogMessage("Registry key opened successfully");
    } else {
        LogError("RegOpenKeyEx failed with error: %lu", result);
    }

    return result;
}

// Registry function - close key (decompiled from FUN_0045f280)
LONG CloseRegistryKey(HKEY hKey) {
    LogMessage("Closing registry key");

    LONG result = RegCloseKey(hKey);

    if(result == ERROR_SUCCESS) {
        LogMessage("Registry key closed successfully");
    } else {
        LogError("RegCloseKey failed with error: %lu", result);
    }

    return result;
}

// Registry function - read value (decompiled from FUN_0045f2b0)
LONG ReadRegistryValue(
    HKEY hKey,
    LPCSTR lpValueName,
    LPDWORD lpType,
    LPBYTE lpData,
    LPDWORD lpcbData) {
    LogMessage("Reading registry value: %s", lpValueName ? lpValueName : "NULL");

    LONG result = RegQueryValueExA(hKey, lpValueName, NULL, lpType, lpData, lpcbData);

    if(result == ERROR_SUCCESS) {
        LogMessage("Registry value read successfully");
    } else {
        LogError("RegQueryValueEx failed with error: %lu", result);
    }

    return result;
}

// Registry function - write value (decompiled from FUN_00458a8f)
LONG WriteRegistryValue(
    HKEY hKey,
    LPCSTR lpValueName,
    DWORD dwType,
    CONST BYTE* lpData,
    DWORD cbData) {
    LogMessage("Writing registry value: %s", lpValueName ? lpValueName : "NULL");

    LONG result = RegSetValueExA(hKey, lpValueName, 0, dwType, lpData, cbData);

    if(result == ERROR_SUCCESS) {
        LogMessage("Registry value written successfully");
    } else {
        LogError("RegSetValueEx failed with error: %lu", result);
    }

    return result;
}

// Function to create a registry key (decompiled from FUN_00459f98)
LONG CreateRegistryKey(HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult, LPDWORD lpdwDisposition) {
    LogMessage("Creating registry key: %s", lpSubKey ? lpSubKey : "NULL");

    LONG result = RegCreateKeyExA(
        hKey,
        lpSubKey,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS,
        NULL,
        phkResult,
        lpdwDisposition);

    if(result == ERROR_SUCCESS) {
        if(lpdwDisposition) {
            if(*lpdwDisposition == REG_CREATED_NEW_KEY) {
                LogMessage("Registry key created successfully");
            } else {
                LogMessage("Registry key opened successfully");
            }
        }
    } else {
        LogError("RegCreateKeyEx failed with error: %lu", result);
    }

    return result;
}

// Function to delete a registry key (decompiled from FUN_00459fdd)
LONG DeleteRegistryKey(HKEY hKey, LPCSTR lpSubKey) {
    LogMessage("Deleting registry key: %s", lpSubKey ? lpSubKey : "NULL");

    LONG result = RegDeleteKeyA(hKey, lpSubKey);

    if(result == ERROR_SUCCESS) {
        LogMessage("Registry key deleted successfully");
    } else {
        LogError("RegDeleteKey failed with error: %lu", result);
    }

    return result;
}

// Function to delete a registry value (decompiled from FUN_004625cc)
LONG DeleteRegistryValue(HKEY hKey, LPCSTR lpValueName) {
    LogMessage("Deleting registry value: %s", lpValueName ? lpValueName : "NULL");

    LONG result = RegDeleteValueA(hKey, lpValueName);

    if(result == ERROR_SUCCESS) {
        LogMessage("Registry value deleted successfully");
    } else {
        LogError("RegDeleteValue failed with error: %lu", result);
    }

    return result;
}

// Function to get device information
BOOL GetDeviceInfo(DEVICE_INFO* pDeviceInfo) {
    if(! pDeviceInfo) {
        LogError("Invalid parameter for GetDeviceInfo");
        return FALSE;
    }

    LogMessage("Getting device information");

    // Getting system information
    SYSTEM_INFO sysInfo;
    GetSystemInfoWrapper(&sysInfo);

    // Getting system version
    OSVERSIONINFOA osInfo;
    GetSystemVersion(&osInfo);

    // Filling the device information structure
    pDeviceInfo->wProcessorArchitecture = sysInfo.wProcessorArchitecture;
    pDeviceInfo->dwNumberOfProcessors = sysInfo.dwNumberOfProcessors;
    pDeviceInfo->dwPageSize = sysInfo.dwPageSize;
    pDeviceInfo->dwMajorVersion = osInfo.dwMajorVersion;
    pDeviceInfo->dwMinorVersion = osInfo.dwMinorVersion;
    pDeviceInfo->dwBuildNumber = osInfo.dwBuildNumber;

    // Copying the platform name
    strncpy_s(
        pDeviceInfo->szPlatformId,
        sizeof(pDeviceInfo->szPlatformId),
        osInfo.szCSDVersion,
        _TRUNCATE);
    pDeviceInfo->szPlatformId[sizeof(pDeviceInfo->szPlatformId) - 1] = '\0';

    LogMessage("Device information retrieved successfully");
    return TRUE;
}

// Function to save settings to the registry
BOOL SaveSettingsToRegistry(LPCSTR settingsPath, LPCVOID pSettings, DWORD settingsSize) {
    LogMessage("Saving settings to registry: %s", settingsPath);

    HKEY hKey;
    LONG result = CreateRegistryKey(HKEY_CURRENT_USER, "Software\\URescue", &hKey, NULL);

    if(result != ERROR_SUCCESS) {
        LogError("Failed to create registry key for settings");
        return FALSE;
    }

    result =
        WriteRegistryValue(hKey, settingsPath, REG_BINARY, (CONST BYTE*) pSettings, settingsSize);

    RegCloseKey(hKey);

    if(result != ERROR_SUCCESS) {
        LogError("Failed to write settings to registry");
        return FALSE;
    }

    LogMessage("Settings saved to registry successfully");
    return TRUE;
}

// Function to load settings from the registry
BOOL LoadSettingsFromRegistry(LPCSTR settingsPath, LPVOID pSettings, DWORD* pSettingsSize) {
    LogMessage("Loading settings from registry: %s", settingsPath);

    HKEY hKey;
    LONG result = OpenRegistryKey(HKEY_CURRENT_USER, "Software\\URescue", &hKey, KEY_READ);

    if(result != ERROR_SUCCESS) {
        LogError("Failed to open registry key for settings");
        return FALSE;
    }

    DWORD dataType;
    result = ReadRegistryValue(hKey, settingsPath, &dataType, (LPBYTE) pSettings, pSettingsSize);

    RegCloseKey(hKey);

    if(result != ERROR_SUCCESS) {
        LogError("Failed to read settings from registry");
        return FALSE;
    }

    LogMessage("Settings loaded from registry successfully");
    return TRUE;
}

// Function to delete settings from the registry
BOOL DeleteSettingsFromRegistry(LPCSTR settingsPath) {
    LogMessage("Deleting settings from registry: %s", settingsPath);

    HKEY hKey;
    LONG result = OpenRegistryKey(HKEY_CURRENT_USER, "Software\\URescue", &hKey, KEY_WRITE);

    if(result != ERROR_SUCCESS) {
        LogError("Failed to open registry key for settings deletion");
        return FALSE;
    }

    result = DeleteRegistryValue(hKey, settingsPath);

    RegCloseKey(hKey);

    if(result != ERROR_SUCCESS) {
        LogError("Failed to delete settings from registry");
        return FALSE;
    }

    LogMessage("Settings deleted from registry successfully");
    return TRUE;
}

// Function to check administrator privileges
BOOL IsAdministrator() {
    LogMessage("Checking administrator privileges");

    BOOL isAdmin = FALSE;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID AdministratorsGroup;

    if(AllocateAndInitializeSid(
           &NtAuthority,
           2,
           SECURITY_BUILTIN_DOMAIN_RID,
           DOMAIN_ALIAS_RID_ADMINS,
           0,
           0,
           0,
           0,
           0,
           0,
           &AdministratorsGroup)) {
        if(! CheckTokenMembership(NULL, AdministratorsGroup, &isAdmin)) {
            isAdmin = FALSE;
        }
        FreeSid(AdministratorsGroup);
    }

    LogMessage("Administrator privileges: %s", isAdmin ? "Yes" : "No");
    return isAdmin;
}

// Function to get the system directory path
BOOL GetSystemDirectoryPath(LPSTR lpBuffer, DWORD nSize) {
    LogMessage("Getting system directory path");

    DWORD result = GetSystemDirectoryA(lpBuffer, nSize);

    if(result == 0) {
        DWORD error = GetLastError();
        LogError("GetSystemDirectory failed with error: %lu", error);
        return FALSE;
    }

    if(result > nSize) {
        LogError("Buffer too small for system directory path");
        return FALSE;
    }

    LogMessage("System directory: %s", lpBuffer);
    return TRUE;
}

// Function to get the Windows directory path
BOOL GetWindowsDirectoryPath(LPSTR lpBuffer, DWORD nSize) {
    LogMessage("Getting Windows directory path");

    DWORD result = GetWindowsDirectoryA(lpBuffer, nSize);

    if(result == 0) {
        DWORD error = GetLastError();
        LogError("GetWindowsDirectory failed with error: %lu", error);
        return FALSE;
    }

    if(result > nSize) {
        LogError("Buffer too small for Windows directory path");
        return FALSE;
    }

    LogMessage("Windows directory: %s", lpBuffer);
    return TRUE;
}

// Read Device.ini -> [Device] SelectDevice
BOOL ReadDeviceSelectionFromIni(LPSTR lpOut, DWORD nSize) {
    if(! lpOut || nSize == 0)
        return FALSE;
    CHAR moduleDir[MAX_PATH] = { 0 };
    if(! GetModuleDirectoryA(moduleDir, sizeof(moduleDir)))
        return FALSE;
    CHAR iniPath[MAX_PATH] = { 0 };
    if(! JoinPathA(iniPath, sizeof(iniPath), moduleDir, "Device.ini"))
        return FALSE;
    DWORD read = GetPrivateProfileStringA("Device", "SelectDevice", "", lpOut, nSize, iniPath);
    return read > 0;
}
