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

// Version information structure
typedef struct _VERSION_INFO {
    DWORD major;        // Major version number
    DWORD minor;        // Minor version number  
    DWORD build;        // Build number
    DWORD revision;     // Revision number
    DWORD date;         // Build date
    DWORD time;         // Build time
    DWORD checksum;     // Checksum
    WORD flags;         // Version flags
    BYTE reserved;      // Reserved byte
    DWORD size;         // Size of version data
    DWORD offset;       // Offset to version data
} VERSION_INFO, *PVERSION_INFO;

// Bank information structure
typedef struct _DEVICE_BANK_INFO {
    DWORD bankId;                    // Bank identifier
    DWORD bankType;                  // Bank type (C, D, etc.)
    DWORD bankSize;                  // Bank size in bytes
    DWORD bankOffset;                // Bank offset in device
    VERSION_INFO versionInfo;        // Version information
    CHAR versionString[256];          // Formatted version string
    BYTE bankData[4096];             // Bank data buffer
    DWORD dataSize;                  // Actual data size
    BOOL isValid;                    // Bank validity flag
    DWORD checksum;                  // Bank checksum
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
