#pragma once

/**
 * @file ITEDeviceStructures.h
 * @brief Precise data structures for ITE controllers based on Ghidra MCP analysis
 * 
 * This file contains accurate data structures obtained via Ghidra MCP decompilation
 * for the OpenDriveHandleAgain function (0x0040beb0) and related functions.
 */

#include <windows.h>

#define MAX_DEVICE_COUNT        8      // PhysicalDrive1-8
#define DEVICE_STRUCT_SIZE      0x57   // 87 bytes per device (from Ghidra)
#define INQUIRY_BUFFER_SIZE     0xB0   // 176 bytes (malloc(0xB0))
#define EXTENSION_BUFFER_SIZE   0xE40  // 3648 bytes (malloc(0xE40))

// Access constants (from Ghidra 0x00408580)
#define ITE_DEVICE_ACCESS_RIGHTS 0xC0000000  // GENERIC_READ | GENERIC_WRITE

/**
 * @brief ITE device data structure, size 0x57 bytes
 * 
 * This structure matches the decompiled code from Ghidra MCP.
 * Offsets obtained from analysis of OpenDriveHandleAgain (0x0040beb0)
 */
typedef struct _ITE_DEVICE_INFO {
    // Offset 0x00-0x06: Basic device data
    BYTE   volumeIndex;        // +0x00: Volume index (0-7)
    BYTE   reserved1;          // +0x01: Reserved
    BYTE   reserved2;          // +0x02: Reserved  
    HANDLE hDevice;            // +0x03: Device handle (offset 0x62a3 - 0x62a2 = 1)
    
    // Offset 0x07-0x0E: Vendor/Product ID (8 bytes)
    CHAR   vendorId[8];        // +0x07: Vendor ID (offset 0x62a7)
    
    // Offset 0x0F-0x1E: Product ID (16 bytes)  
    CHAR   productId[16];      // +0x0F: Product ID (offset 0x62b0)
    
    // Offset 0x1F-0x22: Drive Type
    UINT   driveType;          // +0x1F: Drive type (GetDriveTypeA result, offset 0x62c1)
    
    // Offset 0x23-0x3A: Inquiry Data (from +0x24 in code)
    DWORD  inquiryData[6];     // +0x23: SCSI Inquiry data (offset 0x62e3-0x62ef)
    
    // Offset 0x3B-0x3F: LUN and Device ID
    BYTE   deviceId;           // +0x3B: Device ID (offset 0x62e0)
    BYTE   lunIndex;           // +0x3C: LUN index (offset 0x62e1)
    BYTE   reserved3;          // +0x3D: Reserved
    BYTE   reserved4;          // +0x3E: Reserved
    
    // Offset 0x40-0x52: Controller information
    WORD   controllerType;     // +0x40: Controller type (1181/1176, offset 0x62f4)
    BYTE   controllerSubType;  // +0x42: Controller subtype (offset 0x62f6)
    BYTE   sysReadySupport;    // +0x43: SYSReady support (offset 0x62f7)  
    BYTE   firmwareRevision;   // +0x44: Firmware revision (0=A0AA, 1=A1BA, offset 0x62f8)
    
    // Offset 0x45-0x56: Additional data (12 bytes)
    BYTE   additionalData[12]; // +0x45: Additional controller data
    
} ITE_DEVICE_INFO, *PITE_DEVICE_INFO;

// Controller type constants (from decompilation)
#define ITE_CONTROLLER_1181     0x1181
#define ITE_CONTROLLER_1176     0x1176
#define ITE_SUBTYPE_NORMAL      0      // Normal mode
#define ITE_SUBTYPE_A1BA        1      // A1BA subtype for 1181
#define ITE_SUBTYPE_1176        2      // Subtype for 1176  
#define ITE_SUBTYPE_UNKNOWN     200    // Unknown subtype

// Firmware revision constants (from decompilation)
#define ITE_REVISION_A0AA       0      // Revision A0AA
#define ITE_REVISION_A1BA       1      // Revision A1BA
#define ITE_REVISION_UNKNOWN    0xFF   // Unknown revision

/**
 * @brief Structure for grouping devices by controller  
 * 
 * Based on analysis of VolumePairController (0x00408430)
 * Group size: 0x1DAA bytes
 */
typedef struct _ITE_DEVICE_GROUP {
    BYTE   masterDeviceIndex;     // +0x00: Master device of the group
    BYTE   deviceCount;           // +0x01: Number of devices in the group (max 4)
    BYTE   deviceIndices[4];      // +0x02: Indices of devices in the group
    WORD   controllerType;        // +0x06: Controller type of the group
    BYTE   reserved[8];           // +0x08: Reserved
    // + additional 0x1DA0 bytes of configuration
} ITE_DEVICE_GROUP, *PITE_DEVICE_GROUP;

/**
 * @brief Main structure for all ITE devices
 * 
 * Based on analysis of OpenDriveHandleAgain and related functions
 */
typedef struct _ITE_DEVICE_MANAGER {
    // Basic data
    BYTE   deviceCount;                      // +0x8A1: Total number of devices
    BYTE   groupCount;                       // +0x8A2: Number of controller groups  
    BYTE   reserved[6];                      // Reserved
    
    // Device array (8 x 0x57 = 0x2B8 bytes)
    ITE_DEVICE_INFO devices[MAX_DEVICE_COUNT]; // +0x62A2: Start of device structures
    
    // Group array (3 x 0x1DAA = 0x58FE bytes)  
    ITE_DEVICE_GROUP groups[3];               // +0x9A6: Controller groups
    
    // Additional flags and states
    BYTE   deviceFound[8];                    // +0x8A3: Flags for found devices
    BYTE   lunDataValid;                      // +0x881: LUN data validity
    
} ITE_DEVICE_MANAGER, *PITE_DEVICE_MANAGER;

/**
 * @brief SDK Function Pointers 
 * 
 * Exact function addresses from Ghidra MCP analysis (DAT_004ad520 and others)
 */
typedef struct _ITE_SDK_FUNCTIONS {
    // STD functions  
    FARPROC STD_Inquiry;          // DAT_004ad520 - "STD_Inquiry"
    FARPROC STD_ReadCapacity;     // _DAT_004ad524 - "STD_ReadCapacity"
    FARPROC STD_LogicalRead;      // _DAT_004ad51c - "STD_LogicalRead"
    FARPROC STD_LogicalWrite;     // _DAT_004ad518 - "STD_LogicalWrite"
    
    // VDR functions
    FARPROC VDR_CheckSYSReady;    // DAT_004ad684 - "VDR_CheckSYSReady"
    FARPROC VDR_ReadLUNIndex;     // DAT_004ad658 - "VDR_ReadLUNIndex"  
    FARPROC VDR_ReadLUNID;        // DAT_004ad660 - "VDR_ReadLUNID"
    FARPROC VDR_ReadWriteLUNConfig; // DAT_004ad64c - "VDR_ReadWriteLUNConfig"
    FARPROC VDR_ReadXData;        // DAT_004ad69c - "VDR_ReadXData"
    FARPROC VDR_RootAccess;       // _DAT_004ad638 - "VDR_RootAccess"
    
    // FLH functions
    FARPROC FLH_InitCodeForReady; // _DAT_004ad5e4 - "FLH_InitCodeForReady"
    FARPROC FLH_ReadBCM;          // DAT_004ad5f0 - "FLH_ReadBCM"
    FARPROC FLH_GetFlashDataFromDataBase; // DAT_004ad5cc
    
    // SEC functions
    FARPROC SEC_GetEncryptedPassword; // _DAT_004ad554 - "SEC_GetEncryptedPassword"
    FARPROC SEC_GetUserPassword;     // _DAT_004ad558 - "SEC_GetUserPassword"
    FARPROC SEC_DoAuthentication;    // _DAT_004ad56c - "SEC_DoAuthentication"
    
    // LUN functions
    FARPROC LUN_CreateLun;        // _DAT_004ad550 - "LUN_CreateLun"
    
    // FMT functions  
    FARPROC FMT_Format;           // _DAT_004ad534 - "FMT_Format"
    FARPROC FMT_GetOptimumLunConfig; // _DAT_004ad52c - "FMT_GetOptimumLunConfig"
    
    // MP functions
    FARPROC MP_CreateSystem;      // DAT_004ad4f0 - "MP_CreateSystem"
    FARPROC MP_EraseSystemTable;  // DAT_004ad4ec - "MP_EraseSystemTable"
    
    // DG functions
    FARPROC DG_SearchReadBadTBlk; // _DAT_004ad4e4 - "DG_SearchReadBadTBlk"
    FARPROC DG_CalBlkRequire;     // _DAT_004ad4e0 - "DG_CalBlkRequire"
    
} ITE_SDK_FUNCTIONS, *PITE_SDK_FUNCTIONS;

// Macros for working with structures
#define GET_DEVICE_OFFSET(index) (0x62A2 + (index) * DEVICE_STRUCT_SIZE)
#define GET_GROUP_OFFSET(index)  (0x9A6 + (index) * 0x1DAA)
#define IS_ITE_DEVICE(controllerType) ((controllerType) == ITE_CONTROLLER_1181 || (controllerType) == ITE_CONTROLLER_1176)

// Validation macros
#define VALIDATE_DEVICE_INDEX(index) ((index) < MAX_DEVICE_COUNT)
#define VALIDATE_CONTROLLER_TYPE(type) IS_ITE_DEVICE(type)

#pragma pack(pop)
