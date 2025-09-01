#pragma once
#include <windows.h>

// Device volume information structure (based on decompiled code analysis)
typedef struct _DEVICE_VOLUME_INFO {
    BYTE volumeIndex;         // 0x00: Volume index (0-7)
    BYTE volumeLetter;        // 0x01: Volume letter (A-Z)
    BYTE driveType;           // 0x02: Drive type (GetDriveTypeA result)
    BYTE reserved1;           // 0x03: Reserved
    DWORD inquiryData1;       // 0x04-0x07: Inquiry data bytes 24-27
    DWORD inquiryData2;       // 0x08-0x0B: Inquiry data bytes 28-31
    DWORD inquiryData3;       // 0x0C-0x0F: Inquiry data bytes 32-35
    DWORD inquiryData4;       // 0x10-0x13: Inquiry data bytes 36-39
    BYTE lunIndex;            // 0x14: LUN index
    BYTE deviceId;            // 0x15: Device ID
    BYTE familyType;          // 0x16: Family type (0=1181, 1=A1BA, 2=1176)
    BYTE deviceFound;         // 0x17: Device found flag
    BYTE a1baFlag;            // 0x18: A1BA flag (0=A0AA, 1=A1BA)
    BYTE reserved2[9];        // 0x19-0x21: Reserved
    CHAR vendorName[8];       // 0x22-0x29: Vendor name (8 bytes)
    CHAR productName[16];     // 0x2A-0x39: Product name (16 bytes)
    BYTE reserved3[8];        // 0x3A-0x41: Reserved
} DEVICE_VOLUME_INFO, *PDEVICE_VOLUME_INFO;

// Device bank information structure (based on decompiled code analysis)
typedef struct _DEVICE_BANK_INFO {
    BYTE bankIndex;           // 0x00: Bank index
    BYTE bankStatus;          // 0x01: Bank status
    BYTE bankType;            // 0x02: Bank type
    BYTE bankReady;           // 0x03: Bank ready flag
    BYTE bankLoaded;          // 0x04: Bank loaded flag
    BYTE bankError;           // 0x05: Bank error flag
    BYTE reserved1[2];        // 0x06-0x07: Reserved
    DWORD bankSize;           // 0x08-0x0B: Bank size
    DWORD bankOffset;         // 0x0C-0x0F: Bank offset
    BYTE bankData[0x1000];    // 0x10-0x100F: Bank data (4KB)
    BYTE bcmInfo[0x100];      // 0x1010-0x110F: BCM information
    BYTE ispInfo[0x100];      // 0x1110-0x120F: ISP information
    BYTE reserved2[0x100];    // 0x1210-0x130F: Reserved
    BYTE bankPath[0x100];     // 0x1310-0x140F: Bank path
    BYTE reserved3[0x100];    // 0x1410-0x150F: Reserved
    BYTE reserved4[0x100];    // 0x1510-0x160F: Reserved
    BYTE reserved5[0x100];    // 0x1610-0x170F: Reserved
    BYTE reserved6[0x100];    // 0x1710-0x180F: Reserved
    BYTE reserved7[0x100];    // 0x1810-0x190F: Reserved
    BYTE reserved8[0x100];    // 0x1910-0x1A0F: Reserved
    BYTE reserved9[0x100];    // 0x1A10-0x1B0F: Reserved
    BYTE reserved10[0x100];   // 0x1B10-0x1C0F: Reserved
    BYTE reserved11[0x100];   // 0x1C10-0x1D0F: Reserved
} DEVICE_BANK_INFO, *PDEVICE_BANK_INFO;

// Device information structure (based on decompiled code analysis)
typedef struct _DEVICE_INFO {
    // Device state flags
    BOOL isInitialized;       // 0x00: Initialization flag
    BOOL deviceFound;         // 0x04: Device found flag
    BOOL driveOpened;         // 0x08: Drive opened flag
    BOOL ispLoaded;           // 0x0C: ISP loaded flag
    BOOL systemReady;         // 0x10: System ready flag
    BOOL repairMode;          // 0x14: Repair mode flag
    
    // Device identification
    CHAR deviceString[0x200]; // 0x18: Device identification string
    CHAR deviceName[0x40];    // 0x218: Device name
    CHAR deviceVersion[0x40]; // 0x258: Device version
    CHAR deviceModel[0x40];   // 0x298: Device model
    
    // Volume information
    BYTE volumeCount;         // 0x2D8: Number of volumes found
    BYTE currentVolume;       // 0x2D9: Current volume index
    BYTE selectedVolume;      // 0x2DA: Selected volume index
    BYTE reserved1;           // 0x2DB: Reserved
    
    // Volume array (8 volumes)
    DEVICE_VOLUME_INFO volumes[8];
    
    // Bank array (24 banks)
    DEVICE_BANK_INFO banks[24];
    
    // Additional device data
    BYTE deviceData[0x1000];  // Additional device data
    CHAR devicePath[MAX_PATH]; // Device path
    HANDLE hDevice;            // Device handle
    
} DEVICE_INFO, *PDEVICE_INFO;

// Device constants
#define MAX_VOLUMES 8
#define MAX_BANKS 24
#define VOLUME_INFO_SIZE 0x57
#define BANK_INFO_SIZE 0x1DAA
#define DEVICE_INFO_SIZE 0x107338

// Device family types
#define DEVICE_FAMILY_1181 0x1181
#define DEVICE_FAMILY_1176 0x1176
#define DEVICE_FAMILY_A0AA 0x00
#define DEVICE_FAMILY_A1BA 0x01

// Device status flags
#define DEVICE_STATUS_NOT_FOUND 0x00
#define DEVICE_STATUS_FOUND 0x01
#define DEVICE_STATUS_ERROR 0xFF
