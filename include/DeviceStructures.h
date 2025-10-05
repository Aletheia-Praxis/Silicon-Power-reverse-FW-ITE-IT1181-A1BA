#ifndef DEVICE_STRUCTURES_H
#define DEVICE_STRUCTURES_H

#include "WindowsHeaders.h"

// Maximum device configuration
#define MAX_BANKS    16
#define MAX_SEGMENTS 8

// Forward declarations
typedef struct _DEVICE_BANK_INFO DEVICE_BANK_INFO, *PDEVICE_BANK_INFO;
typedef struct _MP_INFO MP_INFO, *PMP_INFO;
typedef struct _LUN_PARAMS LUN_PARAMS, *PLUN_PARAMS;
typedef struct _LUN_INFO LUN_INFO, *PLUN_INFO;

// MP Information structure
typedef struct _MP_INFO {
    BYTE majorVersion;
    BYTE minorVersion;
    BYTE vendorInfo[4];
    BYTE productInfo[12];
    BOOL isLoaded;
} MP_INFO, *PMP_INFO;

// LUN Parameters structure
typedef struct _LUN_PARAMS {
    BYTE lunType;
    BYTE lunFlags;
    WORD capacityArray[8];
    DWORD totalCapacity;
    BOOL isValid;
} LUN_PARAMS, *PLUN_PARAMS;

// LUN Information structure
typedef struct _LUN_INFO {
    DWORD lunId;
    BOOL isValid;
    BOOL isLoaded;
} LUN_INFO, *PLUN_INFO;

// Version information structure
typedef struct _VERSION_INFO {
    DWORD major;     // Major version number
    DWORD minor;     // Minor version number
    DWORD build;     // Build number
    DWORD revision;  // Revision number
    DWORD date;      // Build date
    DWORD time;      // Build time
    DWORD checksum;  // Checksum
    WORD flags;      // Version flags
    BYTE reserved;   // Reserved byte
    DWORD size;      // Size of version data
    DWORD offset;    // Offset to version data
} VERSION_INFO, *PVERSION_INFO;

// Bank information structure
typedef struct _DEVICE_BANK_INFO {
    DWORD bankId;              // Bank identifier
    DWORD bankType;            // Bank type (C, D, etc.)
    DWORD bankSize;            // Bank size in bytes
    DWORD bankOffset;          // Bank offset in device
    VERSION_INFO versionInfo;  // Version information
    CHAR versionString[256];   // Formatted version string
    BYTE bankData[4096];       // Bank data buffer
    DWORD dataSize;            // Actual data size
    BOOL isValid;              // Bank validity flag
    DWORD checksum;            // Bank checksum
    BOOL isProcessed;          // Bank processed flag
    BOOL bcmLoaded;            // BCM loaded flag
    BOOL fwSegmentNotified;    // FW segment notified flag
    BOOL flashMethodValid;     // Flash method valid flag
    BOOL flashDataFromMemory;  // Flash data from memory flag
    BYTE flashMethod;          // Flash method
    BYTE bcmInfo[0xE40];       // BCM information buffer
    BOOL rootTableValid[6];    // Root table validity flags
    DWORD sysAddrData[20];     // System address data
    BOOL sysAddrValid[20];     // System address validity flags
} DEVICE_BANK_INFO, *PDEVICE_BANK_INFO;

// Device volume information structure (based on decompiled code analysis)
typedef struct _DEVICE_VOLUME_INFO {
    BYTE volumeIndex;      // 0x00: Volume index (0-7)
    BYTE volumeLetter;     // 0x01: Volume letter (A-Z)
    BYTE driveType;        // 0x02: Drive type (GetDriveTypeA result)
    BYTE reserved1;        // 0x03: Reserved
    DWORD inquiryData1;    // 0x04-0x07: Inquiry data bytes 24-27
    DWORD inquiryData2;    // 0x08-0x0B: Inquiry data bytes 28-31
    DWORD inquiryData3;    // 0x0C-0x0F: Inquiry data bytes 32-35
    DWORD inquiryData4;    // 0x10-0x13: Inquiry data bytes 36-39
    BYTE lunIndex;         // 0x14: LUN index
    BYTE deviceId;         // 0x15: Device ID
    BYTE familyType;       // 0x16: Family type (0=1181, 1=A1BA, 2=1176)
    BYTE deviceFound;      // 0x17: Device found flag
    BYTE a1baFlag;         // 0x18: A1BA flag (0=A0AA, 1=A1BA)
    BYTE reserved2[9];     // 0x19-0x21: Reserved
    CHAR vendorName[8];    // 0x22-0x29: Vendor name (8 bytes)
    CHAR productName[16];  // 0x2A-0x39: Product name (16 bytes)
    BYTE reserved3[8];     // 0x3A-0x41: Reserved

    // Device handles and state
    HANDLE hDevice;               // Device handle
    BOOL isInitialized;           // Initialization flag
    BOOL ispCodeInitialized;      // Flag to check if ISP code is initialized
    BOOL firmwareSegmentsLoaded;  // Flag to check if firmware segments are loaded

    // Bank information
    DEVICE_BANK_INFO banks[MAX_BANKS];
    BOOL bcmLoaded;       // BCM loaded flag
    BYTE bcmInfo[0xE40];  // BCM information buffer

    // Segment information
    BYTE segmentInfo[128];   // Segment information
    BOOL fwSegmentNotified;  // FW segment notified flag

    // MP and LUN information
    MP_INFO mpInfo;
    LUN_INFO lunInfo[2];
    LUN_PARAMS lunParams;
    BYTE lunArray[0x10000];
    BOOL lunArrayLoaded;
    DWORD capacity;
    DWORD realCapacity;
    BYTE deviceParams[16];
    BYTE lunConfig[16];

    // Device flags and parameters
    BYTE controllerType;           // Controller type
    BYTE ceMask;                   // CE mask
    BYTE chMask;                   // Channel mask
    DWORD blockCount;              // Block count
    BYTE blockMap[8][2][0x10000];  // Block map
    DWORD deviceFlags;             // Device flags
} DEVICE_VOLUME_INFO, *PDEVICE_VOLUME_INFO;

// Device information structure (based on decompiled code analysis)
typedef struct _DEVICE_INFO {
    // System Information
    WORD wProcessorArchitecture;
    DWORD dwNumberOfProcessors;
    DWORD dwPageSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    CHAR szPlatformId[128];

    // Device state flags
    BOOL isInitialized;  // 0x00: Initialization flag
    BOOL deviceFound;    // 0x04: Device found flag
    BOOL driveOpened;    // 0x08: Drive opened flag
    BOOL ispLoaded;      // 0x0C: ISP loaded flag
    BOOL systemReady;    // 0x10: System ready flag
    BOOL repairMode;     // 0x14: Repair mode flag

    // Device identification
    CHAR deviceString[0x200];  // 0x18: Device identification string
    CHAR deviceName[0x40];     // 0x218: Device name
    CHAR deviceVersion[0x40];  // 0x258: Device version
    CHAR deviceModel[0x40];    // 0x298: Device model

    // Volume information
    BYTE volumeCount;     // 0x2D8: Number of volumes found
    BYTE currentVolume;   // 0x2D9: Current volume index
    BYTE selectedVolume;  // 0x2DA: Selected volume index
    BYTE reserved1;       // 0x2DB: Reserved

    // Volume array (8 volumes)
    DEVICE_VOLUME_INFO volumes[8];

    // Bank array (24 banks)
    DEVICE_BANK_INFO banks[24];

    // Additional device data
    BYTE deviceData[0x1000];    // Additional device data
    CHAR devicePath[MAX_PATH];  // Device path
    HANDLE hDevice;             // Device handle
    BYTE deviceParams[0xFF];    // Device parameters
    BOOL lunArrayLoaded;        // LUN array loaded flag
    BYTE firmwareLayout[512];   // Placeholder for firmware segment data

} DEVICE_INFO, *PDEVICE_INFO;

// Device constants
#define MAX_VOLUMES         8
#define MAX_CONTROLLERS     3
#define MAX_LUNS            2
#define MAX_CAPACITY_ARRAYS 8
#define VOLUME_INFO_SIZE    0x57
#define BANK_INFO_SIZE      0x1DAA
#define DEVICE_INFO_SIZE    0x107338

// Device family types
#define DEVICE_FAMILY_1181 0x1181
#define DEVICE_FAMILY_1176 0x1176
#define DEVICE_FAMILY_A0AA 0x00
#define DEVICE_FAMILY_A1BA 0x01

// Device status flags
#define DEVICE_STATUS_NOT_FOUND 0x00
#define DEVICE_STATUS_FOUND     0x01
#define DEVICE_STATUS_ERROR     0xFF

// Controller data structure (reconstructed from Ghidra analysis)
typedef struct _CONTROLLER_DATA {
    BOOL isValid;
    BOOL isReady;
    DWORD deviceId;
    BYTE pathId;
    BYTE targetId;
    BYTE lunId;
    BYTE busId;
    BYTE scsiId;
    BYTE reserved1;
    BYTE reserved2;
    BYTE volumeIndexes[4];
    BYTE volumeCount;
    BYTE productId;
    BYTE controllerType;

    // MP Info
    BOOL mpInfoLoaded;
    BYTE mpData[0x10];
    BYTE mpStatus;
    WORD mpChecksums[8];
    WORD totalMpChecksum;

    // LUN Info
    BOOL lunInfoLoaded;
    BYTE lunData[64];

    // Capacity Info
    DWORD capacity;
    DWORD precalculatedCapacity;

    DEVICE_BANK_INFO banks[2];

} CONTROLLER_DATA, *PCONTROLLER_DATA;

#endif  // DEVICE_STRUCTURES_H
