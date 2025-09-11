#pragma once

#include <windows.h>

// ITE Controller Info structure
typedef struct _ITE_CONTROLLER_INFO {
    BYTE signature[8];  // Controller signature
    BYTE version[32];   // Firmware version
    BYTE model[16];     // Controller model
    DWORD flashSize;    // Flash memory size
    DWORD blockSize;    // Block size
    BYTE reserved[64];  // Reserved
} ITE_CONTROLLER_INFO, *PITE_CONTROLLER_INFO;

// ITE Controller Status structure
typedef struct _ITE_CONTROLLER_STATUS {
    BYTE state;         // Current state
    BYTE mode;          // Operating mode
    BYTE progress;      // Operation progress
    DWORD errorCode;    // Error code
    BYTE reserved[64];  // Reserved
} ITE_CONTROLLER_STATUS, *PITE_CONTROLLER_STATUS;

// ITE Controller functions
BOOL InitializeITEController(HANDLE hDevice);
BOOL GetControllerInfo(HANDLE hDevice, ITE_CONTROLLER_INFO* pInfo);
BOOL SetControllerMode(HANDLE hDevice, BYTE mode);
BOOL GetControllerStatus(HANDLE hDevice, ITE_CONTROLLER_STATUS* pStatus);
BOOL ResetController(HANDLE hDevice);

// ITE Controller constants
#define ITE_SIGNATURE     "ITE1181"
#define ITE_MODE_NORMAL   0x00
#define ITE_MODE_FLASH    0x01
#define ITE_MODE_RECOVERY 0x02
#define ITE_STATE_READY   0x00
#define ITE_STATE_BUSY    0x01
#define ITE_STATE_ERROR   0xFF
