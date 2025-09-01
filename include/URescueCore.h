#pragma once

#include <windows.h>
#include "SystemManager.h"
#include "ITEController.h"

// Application states
typedef enum _URESCUE_STATE {
    URESCUE_STATE_UNINITIALIZED = 0,
    URESCUE_STATE_INITIALIZED,
    URESCUE_STATE_CONNECTED,
    URESCUE_STATE_FIRMWARE_LOADED,
    URESCUE_STATE_WRITING,
    URESCUE_STATE_VERIFYING,
    URESCUE_STATE_ERROR
} URESCUE_STATE;

// Application settings
typedef struct _URESCUE_SETTINGS {
    BOOL autoConnect;
    BOOL autoLoadFirmware;
    BOOL verifyAfterWrite;
    BOOL createBackup;
    DWORD timeout;
    DWORD retryCount;
    CHAR defaultFirmwarePath[MAX_PATH];
    CHAR logFilePath[MAX_PATH];
} URESCUE_SETTINGS, *PURESCUE_SETTINGS;

// Application context
typedef struct _URESCUE_CONTEXT {
    URESCUE_STATE applicationState;
    DWORD lastError;
    BOOL isAdministrator;
    DEVICE_INFO deviceInfo;
    URESCUE_SETTINGS settings;
    
    // Device
    HANDLE hDevice;
    
    // Firmware
    LPVOID pFirmware;
    DWORD firmwareSize;
    
    // BootCode
    LPVOID pBootCode;
    DWORD bootCodeSize;
    
    // SDK
    LPVOID pSDK;
    DWORD sdkSize;

    // Paths
    CHAR moduleDir[MAX_PATH];
    CHAR tempDir[MAX_PATH];
    CHAR flashDbPath[MAX_PATH];
    CHAR ctrlDbPath[MAX_PATH];
    CHAR binFilePath[MAX_PATH];
} URESCUE_CONTEXT, *PURESCUE_CONTEXT;

// Main application functions (decompiled from Ghidra)
BOOL InitializeURescue();

void DeinitializeURescue();

BOOL LoadFirmware(LPCSTR firmwarePath);

BOOL LoadFirmwareFromResources();

BOOL ConnectToDevice(LPCSTR devicePath);

void DisconnectFromDevice();

BOOL WriteFirmware();

BOOL VerifyFirmware();

// Context access functions
PURESCUE_CONTEXT GetURescueContext();

URESCUE_STATE GetURescueState();

void SetURescueError(DWORD errorCode);

DWORD GetURescueError();

BOOL UpdateSettings(PURESCUE_SETTINGS pSettings);

// Application constants
#define URESCUE_VERSION "81D.2.24.2"
#define URESCUE_NAME "URescue"
#define URESCUE_DESCRIPTION "ITE IT1181 Firmware Recovery Tool"
#define URESCUE_AUTHOR "Silicon Power"
#define URESCUE_COPYRIGHT "Copyright (c) 2024 Silicon Power"

// Default settings constants
#define DEFAULT_TIMEOUT 5000
#define DEFAULT_RETRY_COUNT 3
#define DEFAULT_LOG_FILE "URescue.log"
