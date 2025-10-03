/**
 * Device Detection Helper Functions - Exact Reconstruction
 * Based on detailed Ghidra decompilation
 *
 * These functions handle the device detection and initialization process
 * for ITE USB flash drives (IT1181, IT1176 controllers).
 */

#include <afxwin.h>
#include <stdlib.h>
#include <windows.h>

#include "../include/FlashSDK.h"
#include "../include/Utilities.h"
#include "../include/iTEUFDrs_Ghidra.h"


// External SDK function pointers (defined globally)
extern SDK_FUNCTION_PTR g_pSTD_Inquiry;
extern SDK_FUNCTION_PTR g_pSTD_TestUnitReady;
extern SDK_FUNCTION_PTR g_pSTD_GetLUNIndex;
extern SDK_FUNCTION_PTR g_pSTD_GetDeviceID;

// External logging function
extern void FUN_00406170(LPCSTR msg);
extern char FUN_00409500(int deviceIndex);
extern void FUN_00408630(int deviceIndex);

/**
 * InitializeParaValue - Initialize device parameters and arrays
 * Based on decompilation at 0x00408370
 *
 * This function clears and initializes all device-related data structures,
 * preparing them for device detection and enumeration.
 *
 * @param param_1 - Pointer to iTEUFDrs object
 * @return Always returns TRUE (1)
 */
BOOL InitializeParaValue(int param_1) {
    BYTE* deviceBuffer;
    int deviceIndex;
    int initLoop;
    DWORD* statusArray;
    int statusIndex;

    // Clear device status flags
    *(BYTE*) (param_1 + 0x8a1) = 0;  // m_driveExistStatus = 0
    *(BYTE*) (param_1 + 0x8a2) = 0;  // m_deviceListCount = 0

    // Clear device mapping array (255 bytes)
    memset((void*) (param_1 + 0x8a3), 0, 0xff);

    // Clear controller data array (0x828 = 2088 bytes)
    memset((void*) (param_1 + 0x62a2), 0, 0x828);

    // Initialize 4 device slots (0x1daa = 7594 bytes per device)
    deviceBuffer = (BYTE*) (param_1 + 0x9a5);
    initLoop = 3;  // Loop for 4 devices (0, 1, 2, 3)

    do {
        // Clear entire device data structure (7594 bytes)
        memset(deviceBuffer - 1, 0, 0x1daa);

        // Set device as valid/present
        *deviceBuffer = 1;

        // Initialize status values to -1 (0xffffffff)
        *(DWORD*) (deviceBuffer + 1) = 0xffffffff;     // Device handle
        *(DWORD*) (deviceBuffer + 0x5c) = 0xffffffff;  // Status 1
        *(DWORD*) (deviceBuffer + 0x60) = 0xffffffff;  // Status 2
        *(DWORD*) (deviceBuffer + 100) = 0xffffffff;   // Status 3
        *(DWORD*) (deviceBuffer + 0x68) = 0xffffffff;  // Status 4
        *(DWORD*) (deviceBuffer + 0x6c) = 0xffffffff;  // Status 5
        *(DWORD*) (deviceBuffer + 0x70) = 0xffffffff;  // Status 6
        *(DWORD*) (deviceBuffer + 0x74) = 0xffffffff;  // Status 7

        // Initialize additional status array at end of device structure
        statusArray = (DWORD*) (deviceBuffer + 0x1d1b);
        statusIndex = 2;

        do {
            *(DWORD*) ((int) statusArray - 10) = 0xffffffff;
            *statusArray = 0xffffffff;
            *(DWORD*) ((int) statusArray + 10) = 0xffffffff;
            statusArray = statusArray + 1;
            statusIndex = statusIndex - 1;
        } while(statusIndex != 0);

        // Final status value
        *(DWORD*) (deviceBuffer + 0x1d3a) = 0xffffffff;

        // Move to next device (7594 bytes offset)
        deviceBuffer = deviceBuffer + 0x1daa;
        initLoop = initLoop - 1;
    } while(initLoop != 0);

    return TRUE;
}

/**
 * CheckDriveExist - Scan all drive letters for ITE USB devices
 * Based on decompilation at 0x0040b940
 *
 * This function scans all system drive letters (A: through Z:) to find
 * ITE USB flash devices. For each drive, it:
 * 1. Opens device handle and performs SCSI Inquiry
 * 2. Checks for 'ITEu' identifier in inquiry data
 * 3. Identifies controller type (IT1181/IT1176) and revision (A0AA/A1BA)
 * 4. Reads LUN index and device ID
 * 5. Stores device information in global arrays
 */
void CheckDriveExist(void) {
    char* currentDriveLetter;
    HANDLE deviceHandle;
    char deviceFound;
    int* stringManager;
    int deviceOffset;
    void* inquiryBuffer;
    int deviceIndex;
    BYTE* inquiryData;
    UINT driveType;
    BYTE* deviceString;

    // Drive letter array pointers (all 26 drive letters A: to Z:)
    char* driveLetters[24] = { "A:", "B:", "C:", "D:", "E:", "F:", "G:", "H:",
                               "I:", "J:", "K:", "L:", "M:", "N:", "O:", "P:",
                               "Q:", "R:", "S:", "T:", "U:", "V:", "W:", "X:" };

    int globalDeviceIndex = 0;
    int driveIndex = 0;

    BYTE lunIndex = 0xff;
    BYTE deviceId = 0xff;
    void* responseBuffer;
    void* inquiryDataBuffer;

    // Allocate buffers for SCSI operations
    inquiryBuffer = malloc(0xb0);    // 176 bytes for inquiry data
    responseBuffer = malloc(0xe40);  // 3648 bytes for response buffer

    if(inquiryBuffer == NULL) {
        return;
    }

    // Scan all 24 drive letters
    while(driveIndex < 0x18) {  // 24 drive letters
        currentDriveLetter = driveLetters[driveIndex];
        deviceOffset = globalDeviceIndex * 0x57;  // 87 bytes per device entry

        // Initialize device detection flags
        lunIndex = 0xff;
        deviceId = 0xff;

        // Set drive letter in device structure
        *(char*) (deviceOffset + 0x62a2) = *currentDriveLetter;

        // Attempt to open device handle for this drive
        deviceFound = FUN_00409500(globalDeviceIndex);

        if(deviceFound == 0) {
            // Device not accessible, skip to next drive
            driveIndex++;
            continue;
        }

        // Get device handle from global structure
        deviceHandle = *(HANDLE*) (deviceOffset + 0x62a3);

        // Clear inquiry buffer
        memset(inquiryBuffer, 0, 0xb0);

        // Execute SCSI Inquiry command
        int inquiryResult = (*g_pSTD_Inquiry)(inquiryBuffer, deviceHandle);

        if(inquiryResult == 0) {
            FUN_00406170("Can't get inquiry data ...");
            FUN_00408630(globalDeviceIndex);
            driveIndex++;
            continue;
        }

        // Extract and format inquiry string
        inquiryData = (BYTE*) ((int) inquiryBuffer + 0x24);  // Vendor+Product at offset 0x24

        // Check for ITE USB device identifier
        BYTE* iteMarker = strstr((char*) inquiryData, "ITEu");
        if(iteMarker == NULL) {
            FUN_00406170("It's not our device ...");
            CloseHandle(deviceHandle);
            driveIndex++;
            continue;
        }

        FUN_00406170("Vol = %C, Found ITE device", *currentDriveLetter);

        // Identify controller type and revision
        BYTE* controllerType = strstr((char*) inquiryData, "1181");
        if(controllerType != NULL) {
            // IT1181 controller
            *(WORD*) (deviceOffset + 0x62f4) = 0x1181;  // Controller type
            *(BYTE*) (deviceOffset + 0x62f6) = 0;       // Base type
            *(BYTE*) (deviceOffset + 0x62f8) = 0xff;    // Unknown revision

            // Check for specific revisions
            BYTE* revisionA0AA = strstr((char*) inquiryData, "A0AA");
            if(revisionA0AA != NULL) {
                *(BYTE*) (deviceOffset + 0x62f8) = 0;  // A0AA revision
            }

            BYTE* revisionA1BA = strstr((char*) inquiryData, "A1BA");
            if(revisionA1BA != NULL) {
                *(BYTE*) (deviceOffset + 0x62f6) = 1;  // A1BA type
                *(BYTE*) (deviceOffset + 0x62f8) = 1;  // A1BA revision
            }
        } else {
            // Check for IT1176 controller
            controllerType = strstr((char*) inquiryData, "1176");
            if(controllerType != NULL) {
                *(WORD*) (deviceOffset + 0x62f4) = 0x1176;  // Controller type
                *(BYTE*) (deviceOffset + 0x62f6) = 2;       // 1176 type
                *(BYTE*) (deviceOffset + 0x62f8) = 0xff;    // Unknown revision

                BYTE* revisionA0AA = strstr((char*) inquiryData, "A0AA");
                if(revisionA0AA != NULL) {
                    *(BYTE*) (deviceOffset + 0x62f8) = 0;  // A0AA revision
                }
            } else {
                // Unknown controller, mark as unsupported
                *(BYTE*) (deviceOffset + 0x62f6) = 200;  // Unsupported type
            }
        }

        // Skip unsupported devices
        if(*(char*) (deviceOffset + 0x62f6) == -56) {  // 200 = -56 signed
            FUN_00406170("Not support device ...");
            CloseHandle(deviceHandle);
            driveIndex++;
            continue;
        }

        // Copy vendor and product strings (8+16 bytes from inquiry data)
        *(DWORD*) (deviceOffset + 0x62e3) = *(DWORD*) ((int) inquiryBuffer + 0x24);
        *(DWORD*) (deviceOffset + 0x62e7) = *(DWORD*) ((int) inquiryBuffer + 0x28);
        *(DWORD*) (deviceOffset + 0x62eb) = *(DWORD*) ((int) inquiryBuffer + 0x2c);
        *(DWORD*) (deviceOffset + 0x62ef) = *(DWORD*) ((int) inquiryBuffer + 0x30);

        // Test unit ready
        char unitReady = (*g_pSTD_TestUnitReady)(responseBuffer, deviceHandle);
        *(char*) (deviceOffset + 0x62f7) = unitReady;

        if(unitReady != 0) {
            // Get LUN index
            int lunResult = (*g_pSTD_GetLUNIndex)(&lunIndex, responseBuffer, deviceHandle);
            if(lunResult == 0) {
                FUN_00406170("Can't get volume's Lun index ...");
            } else {
                *(BYTE*) (deviceOffset + 0x62e1) = lunIndex;
            }

            // Get device ID
            int deviceIdResult = (*g_pSTD_GetDeviceID)(&deviceId, responseBuffer, deviceHandle);
            if(deviceIdResult == 0) {
                FUN_00406170("Can't get volume's DeviceID ...");
            } else {
                *(BYTE*) (deviceOffset + 0x62e0) = deviceId;
                if(deviceId != 0xff) {
                    // Mark device as valid in device mapping array
                    *(BYTE*) (deviceId + 0x8a3) = 1;
                }
            }
        }

        // Copy vendor string (8 bytes, pad with spaces if needed)
        for(int i = 0; i < 8; i++) {
            char vendorChar = *(char*) (i + 8 + (int) inquiryBuffer);
            if(vendorChar == 0) {
                *(BYTE*) (deviceOffset + 0x62a7 + i) = 0x20;  // Space padding
            } else {
                *(char*) (deviceOffset + 0x62a7 + i) = vendorChar;
            }
        }

        // Copy product string (16 bytes, pad with spaces if needed)
        for(int i = 0; i < 16; i++) {
            char productChar = *(char*) (i + 0x10 + (int) inquiryBuffer);
            if(productChar == 0) {
                *(BYTE*) (deviceOffset + 0x62b0 + i) = 0x20;  // Space padding
            } else {
                *(char*) (deviceOffset + 0x62b0 + i) = productChar;
            }
        }

        // Clean up device handle
        FUN_00408630(globalDeviceIndex);

        // Get drive type (removable, fixed, etc.)
        char drivePathBuffer[4] = "C:\\";
        drivePathBuffer[0] = *currentDriveLetter;
        driveType = GetDriveTypeA(drivePathBuffer);
        *(UINT*) (deviceOffset + 0x62c1) = driveType;

        // Move to next device slot
        globalDeviceIndex++;
        driveIndex++;
    }

    // Clean up allocated buffers
    free(responseBuffer);
    free(inquiryBuffer);
}
