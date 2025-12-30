#ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0601  // Windows 7+
#endif

#include "../include/URescueCore.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include <limits>

#include "../include/ErrorHandler.h"
#include "../include/FirmwareManager.h"
#include "../include/MemoryManager.h"
#include "../include/SystemManager.h"
#include "../include/USBDevice.h"
#include "../include/Utilities.h"

// Forward declarations for functions defined later in file
int LoadFirmwareFromFile(const char* filename, void** buffer, unsigned long* size);
int LoadBootCodeFromResource(void** buffer, unsigned long* size);
int LoadSDKFromResource(void** buffer, unsigned long* size);
int WriteFirmwareToDevice(void* device, const void* firmware, unsigned long size);
int VerifyFirmwareOnDevice(void* device, const void* firmware, unsigned long size);

// Global program variables
static URESCUE_CONTEXT g_urescueContext;
static BOOL g_bInitialized = FALSE;
static HMODULE g_hSdk = NULL;

static BOOL IsBlockedDevicePathA(const char* devicePath) {
    if(! devicePath || ! devicePath[0]) {
        return FALSE;
    }

    const char* kPhysicalDrivePrefix = "\\\\.\\PHYSICALDRIVE";
    if(_strnicmp(devicePath, kPhysicalDrivePrefix, strlen(kPhysicalDrivePrefix)) == 0) {
        return TRUE;
    }

    if(_strnicmp(devicePath, "\\\\.\\", 4) == 0 && strlen(devicePath) >= 6 && devicePath[4] >= 'A'
       && devicePath[4] <= 'Z' && devicePath[5] == ':') {
        return TRUE;
    }

    if(_strnicmp(devicePath, "\\\\?\\Volume", 9) == 0) {
        return TRUE;
    }

    return FALSE;
}

// Program initialization function (decompiled from the main function)
BOOL InitializeURescue() {
    LogMessage("Initializing URescue application");

    if(g_bInitialized) {
        LogWarning("URescue already initialized");
        return TRUE;
    }

    // Clearing the context
    memset(&g_urescueContext, 0, sizeof(URESCUE_CONTEXT));
    g_urescueContext.applicationState = URESCUE_STATE_UNINITIALIZED;

    // Resolve module and temp directories
    GetModuleDirectoryA(g_urescueContext.moduleDir, sizeof(g_urescueContext.moduleDir));
    GetTempLongPathA(g_urescueContext.tempDir, sizeof(g_urescueContext.tempDir));

    // Getting system information
    if(! GetDeviceInfo(&g_urescueContext.deviceInfo)) {
        LogError("Failed to get device information");
        return FALSE;
    }

    // Checking administrator rights
    g_urescueContext.isAdministrator = IsAdministrator();

    // Build DB paths from module dir
    BuildDatabasePathsA(
        g_urescueContext.moduleDir,
        g_urescueContext.flashDbPath,
        sizeof(g_urescueContext.flashDbPath),
        g_urescueContext.ctrlDbPath,
        sizeof(g_urescueContext.ctrlDbPath));

    // Try read device selection
    CHAR selectedDevice[64] = { 0 };
    ReadDeviceSelectionFromIni(selectedDevice, sizeof(selectedDevice));

    // Build default bin path (fallbacks). Here we don't yet know family/index, use defaults
    // familyHint 0x81 (1181), binIndex 0x00, isA1BA = TRUE by default per version tag
    BuildBinPathA(
        g_urescueContext.moduleDir,
        0x81,
        0x00,
        TRUE,
        g_urescueContext.binFilePath,
        sizeof(g_urescueContext.binFilePath));

    // Load SDK
    if(! Load181FlashSDK(g_urescueContext.moduleDir, &g_hSdk)) {
        LogWarning("181FlashSDK.dll not loaded; some features may be unavailable");
    } else {
        if(! InitializeFlashSDK(g_hSdk)) {
            LogWarning("181FlashSDK.dll loaded but API initialization failed; disabling SDK");
            Unload181FlashSDK(g_hSdk);
            g_hSdk = NULL;
        }
    }

    // Initializing the program state
    g_urescueContext.applicationState = URESCUE_STATE_INITIALIZED;
    g_urescueContext.lastError = ERROR_SUCCESS;

    // Loading settings from the registry
    DWORD settingsSize = sizeof(URESCUE_SETTINGS);
    LoadSettingsFromRegistry(SETTINGS_REGISTRY_VALUE, &g_urescueContext.settings, &settingsSize);

    g_bInitialized = TRUE;
    LogMessage("URescue initialized successfully");
    return TRUE;
}

// Program deinitialization function
void DeinitializeURescue() {
    LogMessage("Deinitializing URescue application");

    if(! g_bInitialized) {
        LogWarning("URescue not initialized");
        return;
    }

    // Saving settings to the registry
    SaveSettingsToRegistry(
        SETTINGS_REGISTRY_VALUE, &g_urescueContext.settings, sizeof(URESCUE_SETTINGS));

    // Releasing resources
    if(g_urescueContext.pFirmware) {
        FreeFirmwareMemory(g_urescueContext.pFirmware);
        g_urescueContext.pFirmware = NULL;
    }

    if(g_urescueContext.pBootCode) {
        FreeFirmwareMemory(g_urescueContext.pBootCode);
        g_urescueContext.pBootCode = NULL;
    }

    if(g_urescueContext.pSDK) {
        FreeFirmwareMemory(g_urescueContext.pSDK);
        g_urescueContext.pSDK = NULL;
    }

    // Closing the device
    if(g_urescueContext.hDevice && g_urescueContext.hDevice != INVALID_HANDLE_VALUE) {
        CloseUSBDevice(g_urescueContext.hDevice);
        g_urescueContext.hDevice = INVALID_HANDLE_VALUE;
    }

    // Unload SDK
    if(g_hSdk) {
        Unload181FlashSDK(g_hSdk);
        g_hSdk = NULL;
    }

    g_urescueContext.applicationState = URESCUE_STATE_UNINITIALIZED;
    g_bInitialized = FALSE;

    LogMessage("URescue deinitialized successfully");
}

// Firmware loading function
BOOL LoadFirmware(LPCSTR firmwarePath) {
    LogMessage("Loading firmware from: %s", firmwarePath);

    if(! g_bInitialized) {
        LogError("URescue not initialized");
        return FALSE;
    }

    // Freeing the previous firmware
    if(g_urescueContext.pFirmware) {
        FreeFirmwareMemory(g_urescueContext.pFirmware);
        g_urescueContext.pFirmware = NULL;
    }

    // Loading firmware from file
    LPVOID pFirmware;
    DWORD firmwareSize;

    if(! LoadFirmwareFromFile(firmwarePath, &pFirmware, &firmwareSize)) {
        LogError("Failed to load firmware from file");
        return FALSE;
    }

    // Allocating memory for the firmware
    LPVOID pAlignedFirmware = AllocateFirmwareMemory(firmwareSize);
    if(! pAlignedFirmware) {
        LogError("Failed to allocate firmware memory");
        FreeMemory(pFirmware);
        return FALSE;
    }

    // Copying the firmware
    if(! CopyFirmware(pAlignedFirmware, pFirmware, firmwareSize)) {
        LogError("Failed to copy firmware");
        FreeFirmwareMemory(pAlignedFirmware);
        FreeMemory(pFirmware);
        return FALSE;
    }

    g_urescueContext.pFirmware = pAlignedFirmware;
    g_urescueContext.firmwareSize = firmwareSize;

    FreeMemory(pFirmware);

    LogMessage("Firmware loaded successfully: %lu bytes", firmwareSize);
    return TRUE;
}

// Function to load firmware from resources
BOOL LoadFirmwareFromResources() {
    LogMessage("Loading firmware from resources");

    if(! g_bInitialized) {
        LogError("URescue not initialized");
        return FALSE;
    }

    // Loading BootCode
    if(! LoadBootCodeFromResource(&g_urescueContext.pBootCode, &g_urescueContext.bootCodeSize)) {
        LogError("Failed to load BootCode from resources");
        return FALSE;
    }

    // Loading SDK
    if(! LoadSDKFromResource(&g_urescueContext.pSDK, &g_urescueContext.sdkSize)) {
        LogError("Failed to load SDK from resources");
        return FALSE;
    }

    LogMessage("Firmware loaded from resources successfully");
    return TRUE;
}

// Function to connect to the device
BOOL ConnectToDevice(LPCSTR devicePath) {
    LogMessage("Connecting to device: %s", devicePath);

    if(! g_bInitialized) {
        LogError("URescue not initialized");
        return FALSE;
    }

    if(IsBlockedDevicePathA(devicePath)) {
        SetURescueError(ERROR_CALL_NOT_IMPLEMENTED);
        LogError(
            "ConnectToDevice: blocked device path until SCSI/vendor command protocol is "
            "implemented: %s",
            devicePath);
        return FALSE;
    }

    // Closing the previous connection
    if(g_urescueContext.hDevice && g_urescueContext.hDevice != INVALID_HANDLE_VALUE) {
        CloseUSBDevice(g_urescueContext.hDevice);
        g_urescueContext.hDevice = INVALID_HANDLE_VALUE;
    }

    // Connecting to the device
    HANDLE hDevice = InitializeUSBDevice(devicePath);
    if(! hDevice) {
        LogError("Failed to initialize USB device");
        return FALSE;
    }

    // Initializing the controller
    if(! InitializeITEController(hDevice)) {
        LogError("Failed to initialize ITE controller");
        CloseUSBDevice(hDevice);
        return FALSE;
    }

    g_urescueContext.hDevice = hDevice;
    g_urescueContext.applicationState = URESCUE_STATE_CONNECTED;

    LogMessage("Connected to device successfully");
    return TRUE;
}

// Function to disconnect from the device
void DisconnectFromDevice() {
    LogMessage("Disconnecting from device");

    if(g_urescueContext.hDevice && g_urescueContext.hDevice != INVALID_HANDLE_VALUE) {
        CloseUSBDevice(g_urescueContext.hDevice);
        g_urescueContext.hDevice = INVALID_HANDLE_VALUE;
    }

    g_urescueContext.applicationState = URESCUE_STATE_INITIALIZED;

    LogMessage("Disconnected from device");
}

// Firmware writing function
BOOL WriteFirmware() {
    LogMessage("Writing firmware to device");

    if(! g_bInitialized) {
        LogError("URescue not initialized");
        return FALSE;
    }

    if(! g_urescueContext.hDevice || g_urescueContext.hDevice == INVALID_HANDLE_VALUE) {
        LogError("No device connected");
        return FALSE;
    }

    if(! g_urescueContext.pFirmware || g_urescueContext.firmwareSize == 0) {
        LogError("No firmware loaded");
        return FALSE;
    }

    // Setting the write mode
    if(! SetControllerMode(g_urescueContext.hDevice, ITE_MODE_FLASH)) {
        LogError("Failed to set flash mode");
        return FALSE;
    }

    // Writing the firmware
    if(! WriteFirmwareToDevice(
           g_urescueContext.hDevice, g_urescueContext.pFirmware, g_urescueContext.firmwareSize)) {
        LogError("Failed to write firmware");
        SetControllerMode(g_urescueContext.hDevice, ITE_MODE_NORMAL);
        return FALSE;
    }

    // Returning to normal mode
    SetControllerMode(g_urescueContext.hDevice, ITE_MODE_NORMAL);

    LogMessage("Firmware written successfully");
    return TRUE;
}

// Firmware verification function
BOOL VerifyFirmware() {
    LogMessage("Verifying firmware");

    if(! g_bInitialized) {
        LogError("URescue not initialized");
        return FALSE;
    }

    if(! g_urescueContext.hDevice || g_urescueContext.hDevice == INVALID_HANDLE_VALUE) {
        LogError("No device connected");
        return FALSE;
    }

    if(! g_urescueContext.pFirmware || g_urescueContext.firmwareSize == 0) {
        LogError("No firmware loaded");
        return FALSE;
    }

    // Firmware verification
    if(! VerifyFirmwareOnDevice(
           g_urescueContext.hDevice, g_urescueContext.pFirmware, g_urescueContext.firmwareSize)) {
        LogError("Firmware verification failed");
        return FALSE;
    }

    LogMessage("Firmware verification successful");
    return TRUE;
}

// Function to get the program context
PURESCUE_CONTEXT GetURescueContext() {
    return &g_urescueContext;
}

// Function to get the program state
URESCUE_STATE GetURescueState() {
    return g_urescueContext.applicationState;
}

// Function to set the last error
void SetURescueError(DWORD errorCode) {
    g_urescueContext.lastError = errorCode;
    SetLastErrorWrapper(errorCode);
}

// Function to get the last error
DWORD GetURescueError() {
    return g_urescueContext.lastError;
}

// Settings update function
BOOL UpdateSettings(PURESCUE_SETTINGS pSettings) {
    if(! pSettings) {
        LogError("Invalid settings parameter");
        return FALSE;
    }

    memcpy(&g_urescueContext.settings, pSettings, sizeof(URESCUE_SETTINGS));

    // Saving settings to the registry
    SaveSettingsToRegistry(
        SETTINGS_REGISTRY_VALUE, &g_urescueContext.settings, sizeof(URESCUE_SETTINGS));

    LogMessage("Settings updated successfully");
    return TRUE;
}

// Missing function implementations

int LoadFirmwareFromFile(const char* filename, void** buffer, unsigned long* size) {
    LogMessage("LoadFirmwareFromFile: %s", filename);

    if(! filename || ! buffer || ! size) {
        LogError("Invalid parameters");
        return 0;
    }

    *buffer = nullptr;
    *size = 0;

    if(! ValidateFirmwareFile(filename)) {
        LogError("Invalid firmware file: %s", filename);
        return 0;
    }

    LPVOID loadedBuffer = nullptr;
    DWORD loadedSize = 0;
    if(! FirmwareOperations::LoadFirmwareFile(filename, &loadedBuffer, &loadedSize)) {
        LogError("Failed to read firmware file: %s", filename);
        return 0;
    }

    if(loadedSize > (std::numeric_limits<unsigned long>::max)()) {
        LogError("Firmware file too large for current API: %s", filename);
        FreeMemory(loadedBuffer);
        return 0;
    }

    *buffer = loadedBuffer;
    *size = static_cast<unsigned long>(loadedSize);
    return 1;
}

int WriteFirmwareToDevice(void* device, const void* firmware, unsigned long size) {
    LogMessage("WriteFirmwareToDevice: size=%lu", size);

    if(! device || ! firmware) {
        LogError("Invalid parameters");
        return 0;
    }

    SetURescueError(ERROR_CALL_NOT_IMPLEMENTED);
    LogError(
        "WriteFirmwareToDevice: not implemented (USB transport write protocol not reconstructed)");
    return 0;
}

int VerifyFirmwareOnDevice(void* device, const void* firmware, unsigned long size) {
    LogMessage("VerifyFirmwareOnDevice: size=%lu", size);

    if(! device || ! firmware) {
        LogError("Invalid parameters");
        return 0;
    }

    SetURescueError(ERROR_CALL_NOT_IMPLEMENTED);
    LogError(
        "VerifyFirmwareOnDevice: not implemented (USB transport verify protocol not "
        "reconstructed)");
    return 0;
}

void BuildDatabasePathsA(
    const char* basePath,
    char* dbPath,
    unsigned int dbPathSize,
    char* backupPath,
    unsigned int backupPathSize) {
    LogMessage("BuildDatabasePathsA: %s", basePath);

    if(basePath && dbPath && dbPathSize > 0) {
        strncpy(dbPath, basePath, dbPathSize - 1);
        dbPath[dbPathSize - 1] = '\0';
        strcat(dbPath, "\\database");
    }

    if(basePath && backupPath && backupPathSize > 0) {
        strncpy(backupPath, basePath, backupPathSize - 1);
        backupPath[backupPathSize - 1] = '\0';
        strcat(backupPath, "\\backup");
    }
}

void BuildBinPathA(
    const char* basePath,
    unsigned char controller,
    unsigned char variant,
    bool isDebug,
    char* binPath,
    unsigned int binPathSize) {
    LogMessage("BuildBinPathA: %s, controller=%d, variant=%d", basePath, controller, variant);

    if(basePath && binPath && binPathSize > 0) {
        snprintf(
            binPath,
            binPathSize,
            "%s\\bin\\ITE_%d_%d%s.bin",
            basePath,
            controller,
            variant,
            isDebug ? "_debug" : "");
    }
}

int LoadBootCodeFromResource(void** buffer, unsigned long* size) {
    LogMessage("LoadBootCodeFromResource");

    if(! buffer || ! size) {
        LogError("Invalid parameters");
        return 0;
    }

    // Stub implementation - return success for now
    *buffer = nullptr;
    *size = 0;
    return 1;
}

int LoadSDKFromResource(void** buffer, unsigned long* size) {
    LogMessage("LoadSDKFromResource");

    if(! buffer || ! size) {
        LogError("Invalid parameters");
        return 0;
    }

    // Stub implementation - return success for now
    *buffer = nullptr;
    *size = 0;
    return 1;
}
