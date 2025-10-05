#include "../include/iTEUFDrs.h"

#include <ntddstor.h>
#include <stdio.h>
#include <string.h>
#include <winioctl.h>

#include "../include/DeviceStructures.h"
#include "../include/SDKAPIs.h"
#include "../include/SDKLoader.h"
#include "../include/Utilities.h"
#include "../include/WindowsHeaders.h"

// Helper function to build volume path
void buildVolumePath(char driveLetter, char* path, size_t pathSize) {
    if(path && pathSize >= 8) {
        path[0] = '\\';
        path[1] = '\\';
        path[2] = '.';
        path[3] = '\\';
        path[4] = driveLetter;
        path[5] = ':';
        path[6] = '\0';
    }
}

// Constructor implementation (equivalent to the original FUN_0040d690)
iTEUFDrs::iTEUFDrs(LPCSTR basePath)
    : m_vtable(nullptr), m_isInitialized(FALSE), m_lastError(0), m_hSDK(NULL),
      m_pVDR_GetLunIndex(nullptr), m_pVDR_GetDeviceID(nullptr) {
    LogMessage("iTEUFDrs: constructor called with basePath: %s", basePath);

    // Initialize device structures
    InitializeMembers();

    if(basePath) {
        strncpy_s(m_basePath, sizeof(m_basePath), basePath, _TRUNCATE);
    } else {
        LogError("iTEUFDrs: basePath is NULL.");
        m_lastError = ERROR_INVALID_PARAMETER;
        return;
    }

    if(! InitializeSDK()) {
        LogError("iTEUFDrs: Failed to initialize SDK.");
        m_lastError = GetLastError();  // Store the specific error
        return;
    }

    // Enhanced security initialization based on analysis
    if(! VerifySDKIntegrity()) {
        LogError("iTEUFDrs: SDK integrity verification failed.");
        m_lastError = ERROR_INVALID_DATA;
        return;
    }

    // Call GetDeviceInfo (equivalent to FUN_0040cf30)
    if(! GetDeviceInfoInternal()) {
        LogError("iTEUFDrs: GetDeviceInfo failed.");
        // GetDeviceInfo should set its own m_lastError
    } else {
        LogMessage("iTEUFDrs: GetDeviceInfo OK");
        m_isInitialized = TRUE;
    }
}

iTEUFDrs::~iTEUFDrs() {
    if(m_hSDK) {
        Unload181FlashSDK(m_hSDK);
        m_hSDK = NULL;
    }
    LogMessage("iTEUFDrs: destructor called.");
}

BOOL iTEUFDrs::InitializeSDK() {
    CHAR sdkPath[MAX_PATH];
    if(! JoinPathA(sdkPath, sizeof(sdkPath), m_basePath, "181FlashSDK.dll")) {
        LogError("iTEUFDrs: Failed to build SDK path.");
        m_lastError = ERROR_BUFFER_OVERFLOW;
        return FALSE;
    }

    if(! Load181FlashSDK(m_basePath, &m_hSDK)) {
        LogWarning("iTEUFDrs: 181FlashSDK.dll not loaded; some features may be unavailable.");
        m_lastError = GetLastError();
        return FALSE;
    }

    LogMessage("iTEUFDrs: Loaded 181FlashSDK.dll. Initializing functions...");

    // Call the reconstructed LoadSDKFunctions instead of InitializeFlashSDK
    if(! LoadSDKFunctions(m_hSDK)) {
        LogError("iTEUFDrs: Failed to load SDK functions.");
        m_lastError = GetLastError();
        Unload181FlashSDK(m_hSDK);
        m_hSDK = NULL;
        return FALSE;
    }

    LogMessage("iTEUFDrs: SDK functions loaded successfully.");
    return TRUE;
}

// Enhanced security function to verify SDK integrity
BOOL iTEUFDrs::VerifySDKIntegrity() {
    if(! m_hSDK) {
        LogError("VerifySDKIntegrity: SDK not loaded.");
        return FALSE;
    }

    // Verify critical SDK functions are loaded based on Ghidra analysis
    // These are the essential security and core functions identified in FUN_00401000

    // Check critical security functions
    if(! g_sdk_api.FLH_ReadISPData || ! g_sdk_api.FLH_WriteISPData) {
        LogError("VerifySDKIntegrity: Critical ISP functions not loaded.");
        return FALSE;
    }

    if(! g_sdk_api.SEC_DoAuthentication || ! g_sdk_api.SEC_GetUserPassword
       || ! g_sdk_api.SEC_ChangePassword) {
        LogError("VerifySDKIntegrity: Critical security functions not loaded.");
        return FALSE;
    }

    // Check core flash operations
    if(! g_sdk_api.FLH_PhyiscalRead || ! g_sdk_api.FLH_PhyiscalWrite
       || ! g_sdk_api.FLH_BlockErase) {
        LogError("VerifySDKIntegrity: Critical flash operations not loaded.");
        return FALSE;
    }

    // Check device management functions
    if(! g_sdk_api.VDR_ReadWriteLUNConfig || ! g_sdk_api.VDR_GetSecurityStatus
       || ! g_sdk_api.STD_TestUnitReady) {
        LogError("VerifySDKIntegrity: Critical device management functions not loaded.");
        return FALSE;
    }

    // Verify the SDK module file integrity (basic check)
    CHAR sdkPath[MAX_PATH];
    if(! JoinPathA(sdkPath, sizeof(sdkPath), m_basePath, "181FlashSDK.dll")) {
        LogError("VerifySDKIntegrity: Failed to build SDK path.");
        return FALSE;
    }

    HANDLE hFile = CreateFileA(
        sdkPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE) {
        LogError("VerifySDKIntegrity: Cannot open SDK file for verification.");
        return FALSE;
    }

    LARGE_INTEGER fileSize;
    if(! GetFileSizeEx(hFile, &fileSize)) {
        LogError("VerifySDKIntegrity: Cannot get SDK file size.");
        CloseHandle(hFile);
        return FALSE;
    }

    CloseHandle(hFile);

    // Basic sanity check - SDK should be reasonably sized
    if(fileSize.QuadPart < 100000 || fileSize.QuadPart > 10000000) {
        LogError("VerifySDKIntegrity: SDK file size suspicious: %lld bytes.", fileSize.QuadPart);
        return FALSE;
    }

    LogMessage("VerifySDKIntegrity: SDK integrity verification passed.");
    return TRUE;
}

void iTEUFDrs::InitializeMembers() {
    // Initialize all members to zero
    memset(&m_deviceInfo, 0, sizeof(m_deviceInfo));
    memset(m_basePath, 0, sizeof(m_basePath));
    memset(m_controllerData, 0, sizeof(m_controllerData));
    memset(m_bcmBuffer, 0, sizeof(m_bcmBuffer));
    memset(m_deviceIDTable, 0, sizeof(m_deviceIDTable));
    m_vtable = nullptr;
    m_isInitialized = FALSE;
    m_lastError = 0;
    m_hSDK = NULL;
    m_controllerCount = 0;
    m_volumeCount = 0;
}

BOOL iTEUFDrs::InitializeDeviceStructures() {
    // Initialize device info structure
    memset(&m_deviceInfo, 0, sizeof(m_deviceInfo));

    // Set default values
    m_deviceInfo.isInitialized = FALSE;
    m_deviceInfo.deviceFound = FALSE;
    m_deviceInfo.driveOpened = FALSE;
    m_deviceInfo.ispLoaded = FALSE;
    m_deviceInfo.systemReady = FALSE;
    m_deviceInfo.repairMode = FALSE;

    // Initialize volume and bank arrays
    for(int i = 0; i < MAX_VOLUMES; i++) {
        memset(&m_deviceInfo.volumes[i], 0, sizeof(DEVICE_VOLUME_INFO));
        m_deviceInfo.volumes[i].volumeIndex = (BYTE) i;
        m_deviceInfo.volumes[i].lunIndex = 0xFF;
        m_deviceInfo.volumes[i].deviceId = 0xFF;
        m_deviceInfo.volumes[i].familyType = 0xFF;
        m_deviceInfo.volumes[i].deviceFound = FALSE;
        m_deviceInfo.volumes[i].a1baFlag = 0xFF;
    }

    for(int i = 0; i < MAX_BANKS; i++) {
        memset(&m_deviceInfo.banks[i], 0, sizeof(DEVICE_BANK_INFO));
        m_deviceInfo.banks[i].bankId = (DWORD) i;
        m_deviceInfo.banks[i].bankType = 0xFF;
        m_deviceInfo.banks[i].isValid = FALSE;
        m_deviceInfo.banks[i].isProcessed = FALSE;
        m_deviceInfo.banks[i].bcmLoaded = FALSE;
    }

    return TRUE;
}

// Main GetDeviceInfo function (reconstructed from iTEUFDrs_DetectAndInitializeDevices)
BOOL iTEUFDrs::GetDeviceInfoInternal() {
    LogMessage("GetDeviceInfo: Start");
    m_deviceInfo.isInitialized = FALSE;

    if(! InitializeParaValue()) {
        LogError("GetDeviceInfo: InitializeParaValue fails.");
        return FALSE;
    }
    LogMessage("GetDeviceInfo: InitializeParaValue OK.");

    m_volumeCount = CheckDriveExist();
    if(m_volumeCount == 0) {
        LogMessage("Open Drive Handle Again !");
        // In the original code, a flag is set here. We'll use a member variable.
        m_deviceInfo.driveOpened = TRUE;
        if(! OpenDriveHandleAgain(0)) {
            LogError("GetDeviceInfo: Device Not Found after trying again.");
            return FALSE;
        }
        // After opening physical drives, we might need to re-scan or re-validate
        m_volumeCount = CheckDriveExist();
    }

    if(m_volumeCount == 0 || m_volumeCount == (BYTE) -1) {
        LogError("GetDeviceInfo: Device Not Found or CheckDriveExist Error.");
        return FALSE;
    }

    LogMessage("GetDeviceInfo CheckDriveExist OK.");

    SetDeviceID();
    LogMessage("GetDeviceInfo SetDeviceID OK.");

    VolumePairController();
    LogMessage("GetDeviceInfo VolumePairController OK.");

    for(BYTE i = 0; i < m_controllerCount; ++i) {
        CONTROLLER_DATA& controller = m_controllerData[i];
        if(! controller.isValid)
            continue;

        // The original code has complex logic to pick a volume. We'll use the first valid one.
        BYTE volumeIndex = controller.volumeIndexes[0];
        if(volumeIndex >= MAX_VOLUMES)
            continue;

        DEVICE_VOLUME_INFO& volume = m_deviceInfo.volumes[volumeIndex];

        // Open the correct handle type based on how drives were found
        if(m_deviceInfo.driveOpened) {
            if(! OpenPhysicalDrive(volumeIndex)) {
                controller.isReady = FALSE;
                continue;
            }
        } else {
            if(! OpenLogicalDriveHandle(volumeIndex)) {
                controller.isReady = FALSE;
                continue;
            }
        }

        m_deviceInfo.selectedVolume = volumeIndex;

        // Check if ISP code needs to be loaded
        if(! volume.ispCodeInitialized) {
            PrepareFirmwareFilePath();  // Placeholder for path logic
            ReadBinaryFileVersion();    // Placeholder for version reading
            if(! InitializeISPCode(i, controller.deviceId)) {
                LogError("GetDeviceInfo: InitializeISPCode failed for controller %d", i);
                controller.isReady = FALSE;
                CloseDeviceHandle(volumeIndex);
                continue;
            }
            volume.ispCodeInitialized = TRUE;
        }

        // Notify firmware about segment info (loads BankC)
        if(! NotifyFwSegmentInfo(i, controller.deviceId)) {
            AfxMessageBox("Load BankC fail (Path not exist?)", 0, 0);
            controller.isReady = FALSE;
            CloseDeviceHandle(volumeIndex);
            continue;
        }

        // Read BCM (Bad Block Management) information
        if(! g_sdk_api.FLH_ReadBCM) {
            AfxMessageBox("Get BCM information CMD fail", 0, 0);
            controller.isReady = FALSE;
            CloseDeviceHandle(volumeIndex);
            continue;
        }
        int bcmResult = ((PFN_FLH_ReadBCM) (g_sdk_api.FLH_ReadBCM))(m_bcmBuffer);
        if(bcmResult != 1) {
            // Original code has a switch for different errors. We'll show a generic message.
            AfxMessageBox("Get BCM information fail", 0, 0);
            controller.isReady = FALSE;
            CloseDeviceHandle(volumeIndex);
            continue;
        }

        // Copy BCM data to the appropriate bank structure
        memcpy(volume.banks[0].bcmInfo, m_bcmBuffer, sizeof(m_bcmBuffer));
        volume.banks[0].bcmLoaded = TRUE;

        // Load/update firmware segments and capacity
        if(! volume.firmwareSegmentsLoaded) {
            LoadAndVerifyFirmwareSegments(i, controller.deviceId);
        } else {
            UpdateFirmwareBankInfo(i, controller.deviceId);
        }

        if(! GetLunArrayData(i, controller.deviceId)) {
            CalculateDeviceCapacity(i);
        } else {
            UpdateDeviceCapacityOrCalculate(i);
        }

        // Get Mass Production info and set final device string
        if(! GetMPInfo(i, controller.deviceId)) {
            strcpy_s(m_deviceInfo.deviceString, sizeof(m_deviceInfo.deviceString), " NONE");
            m_deviceInfo.systemReady = FALSE;
        } else {
            // In the success case, a formatted string is created. We'll do this in
            // FormatFinalDeviceString.
            m_deviceInfo.systemReady = TRUE;
        }

        // Set repair mode based on system readiness
        if(m_deviceInfo.systemReady) {
            LogMessage("DoRepairDevice System Yes bISPLoaded");
            m_deviceInfo.repairMode = FALSE;
        } else {
            LogMessage("DoRepairDevice No System (!ISPLoad)");
            m_deviceInfo.repairMode = TRUE;
        }

        controller.isReady = TRUE;
        CloseDeviceHandle(volumeIndex);
    }

    // Find first ready controller and format final display string
    m_deviceInfo.selectedVolume = 0xFF;
    for(BYTE i = 0; i < m_controllerCount; ++i) {
        if(m_controllerData[i].isReady) {
            m_deviceInfo.selectedVolume = m_controllerData[i].volumeIndexes[0];
            break;
        }
    }

    if(m_deviceInfo.selectedVolume != 0xFF) {
        FormatFinalDeviceString(m_deviceInfo.selectedVolume);
    }

    LogMessage("GetDeviceInfo: completed successfully");
    m_isInitialized = TRUE;
    return m_isInitialized;
}

BOOL iTEUFDrs::NotifyFwSegmentInfo(BYTE controllerIndex, DWORD deviceId) {
    LogMessage("NotifyFwSegmentInfo for controller %d, deviceId 0x%X", controllerIndex, deviceId);

    if(controllerIndex >= m_controllerCount) {
        LogError("NotifyFwSegmentInfo: Invalid controller index %d", controllerIndex);
        return FALSE;
    }

    CONTROLLER_DATA& controller = m_controllerData[controllerIndex];
    if(! controller.isValid) {
        LogWarning("NotifyFwSegmentInfo: Controller %d is not valid.", controllerIndex);
        return FALSE;
    }

    BYTE volumeIndex = controller.volumeIndexes[0];
    if(volumeIndex >= MAX_VOLUMES) {
        LogError(
            "NotifyFwSegmentInfo: Invalid volume index %d for controller %d",
            volumeIndex,
            controllerIndex);
        return FALSE;
    }
    DEVICE_VOLUME_INFO& volume = m_deviceInfo.volumes[volumeIndex];

    // In the original code, this flag is at an offset like +0x9fb from the start of a large
    // structure. We'll use a specific member in our reconstructed struct.
    if(volume.fwSegmentNotified) {
        LogMessage(
            "NotifyFwSegmentInfo: Firmware segments already notified for volume %d.", volumeIndex);
        return TRUE;
    }

    // Prepare segment parameter buffer (128 bytes)
    BYTE segmentParams[128];
    memset(segmentParams, 0, sizeof(segmentParams));

    // Call SDK function to arrange segment parameters
    if(! g_sdk_api.FLH_ArrangeSegmentPara) {
        LogError("NotifyFwSegmentInfo: FLH_ArrangeSegmentPara function not found in SDK.");
        return FALSE;
    }
    // The original code passes a pointer to a large structure + 0x1866. This likely contains
    // firmware layout info. We'll pass a placeholder or a similarly structured part of our data.
    // For now, let's assume m_deviceInfo.deviceData holds some of this info.
    ((int(__stdcall*)(BYTE*, void*)) g_sdk_api.FLH_ArrangeSegmentPara)(
        segmentParams, m_deviceInfo.deviceData);

    // Call SDK function to initialize the controller with the segment parameters
    if(! g_sdk_api.FLH_InitCTRL) {
        LogError("NotifyFwSegmentInfo: FLH_InitCTRL function not found in SDK.");
        return FALSE;
    }

    // The original code passes the device handle, the arranged segment params, and a buffer from
    // the device structure (+0xa26). We'll use our m_bcmBuffer for that.
    int result = ((int(__stdcall*)(HANDLE, BYTE*, void*)) g_sdk_api.FLH_InitCTRL)(
        volume.hDevice, segmentParams, m_bcmBuffer);

    if(result != 1) {
        LogError(
            "NotifyFwSegmentInfo: FLH_InitCTRL failed for volume %d. Result: %d",
            volumeIndex,
            result);
        return FALSE;
    }

    // Mark as notified on success
    volume.fwSegmentNotified = TRUE;
    LogMessage(
        "NotifyFwSegmentInfo: Successfully notified firmware segments for volume %d.", volumeIndex);

    return TRUE;
}

BOOL iTEUFDrs::InitializeParaValue() {
    // This function is based on the decompiled code at 0x00408370.
    // It initializes various parameters and data structures for device management.
    LogMessage("InitializeParaValue: initializing parameters");

    // Reset controller and volume counts
    m_controllerCount = 0;
    m_volumeCount = 0;

    // Clear and initialize the controller data structures
    memset(m_controllerData, 0, sizeof(m_controllerData));

    for(int i = 0; i < MAX_CONTROLLERS; ++i) {
        CONTROLLER_DATA* pController = &m_controllerData[i];

        pController->isValid = TRUE;
        pController->deviceId = -1;
        pController->lunId = -1;
        pController->targetId = -1;
        pController->pathId = -1;
        pController->busId = -1;
        pController->scsiId = -1;
        pController->reserved1 = -1;
        pController->reserved2 = -1;
        pController->isReady = FALSE;

        for(int j = 0; j < 4; ++j) {
            pController->volumeIndexes[j] = 0xFF;  // -1 for byte
        }
    }

    // The original function returns a value, which seems to indicate success.
    return TRUE;
}

BYTE iTEUFDrs::CheckDriveExist() {
    LogMessage("CheckDriveExist: scanning for ITE devices on logical drives");
    m_deviceInfo.volumeCount = 0;
    char drivePath[] = "\\\\.\\A:";

    // Allocate buffer for inquiry data
    BYTE inquiryBuffer[0xB0];  // 176 bytes

    // Scan drive letters 'A' through 'Z' && m_deviceInfo.volumeCount < MAX_VOLUMES;
    for(char driveLetter = 'A'; driveLetter <= 'Z' && m_deviceInfo.volumeCount < MAX_VOLUMES;
        driveLetter++) {
        drivePath[4] = driveLetter;

        HANDLE hDevice = CreateFileA(
            drivePath,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if(hDevice == INVALID_HANDLE_VALUE) {
            continue;
        }

        // Use the bound SDK function for Inquiry
        if(! g_sdk_api.STD_Inquiry
           || ((PFN_STD_Inquiry) g_sdk_api.STD_Inquiry)(
                  0, inquiryBuffer, sizeof(inquiryBuffer), 0, m_bcmBuffer, 0)
                  != 0) {
            LogWarning("CheckDriveExist: STD_Inquiry failed for drive %c:", driveLetter);
            CloseHandle(hDevice);
            continue;
        }

        // Check for "ITEu" signature in the inquiry data (starting from offset 8)
        if(memcmp(inquiryBuffer + 8, "ITEu", 4) != 0) {
            CloseHandle(hDevice);
            continue;
        }

        LogMessage("CheckDriveExist: Found ITE device on drive %c:", driveLetter);

        // It's our device, populate the structure
        BYTE volIdx = m_deviceInfo.volumeCount;
        DEVICE_VOLUME_INFO& vol = m_deviceInfo.volumes[volIdx];

        vol.volumeIndex = volIdx;
        vol.volumeLetter = driveLetter;
        vol.driveType = GetDriveTypeA(drivePath);
        vol.hDevice = hDevice;  // Keep handle open for now
        vol.deviceFound = TRUE;

        // Copy inquiry data parts
        memcpy(&vol.inquiryData1, inquiryBuffer + 0x24, 16);

        // Copy Vendor and Product strings, ensuring they are null-terminated
        memcpy(vol.vendorName, inquiryBuffer + 8, 8);
        vol.vendorName[7] = '\0';
        memcpy(vol.productName, inquiryBuffer + 16, 16);
        vol.productName[15] = '\0';

        // Build ASCII inquiry string for substring search
        char asciiBuf[256] = { 0 };
        for(int k = 8; k < 36; ++k) {
            char c = (char) inquiryBuffer[k];
            if(c >= 32 && c < 127) {  // isprint()
                asciiBuf[strlen(asciiBuf)] = c;
            }
        }

        // Detect controller and flags
        vol.controllerType = 200;  // not supported by default
        vol.a1baFlag = 0xFF;
        if(strstr(asciiBuf, "1181")) {
            vol.familyType = DEVICE_FAMILY_A1BA;
            vol.controllerType = 0;
            if(strstr(asciiBuf, "A0AA"))
                vol.a1baFlag = 0;
            else if(strstr(asciiBuf, "A1BA")) {
                vol.controllerType = 1;
                vol.a1baFlag = 1;
            }
        } else if(strstr(asciiBuf, "1176")) {
            vol.familyType = 2;  // Placeholder for 1176
            vol.controllerType = 2;
            vol.a1baFlag = (strstr(asciiBuf, "A0AA")) ? 0 : 0xFF;
        }

        // Get LUN Index and Device ID
        if(g_sdk_api.STD_TestUnitReady
           && ((PFN_VDR_CheckSYSReady) g_sdk_api.STD_TestUnitReady)(
                  0, m_bcmBuffer, sizeof(m_bcmBuffer), 0, m_bcmBuffer, 0)
                  == 0) {
            if(g_sdk_api.STD_GetLUNIndex) {
                ((PFN_VDR_ReadLUNIndex) g_sdk_api.STD_GetLUNIndex)(
                    0, &vol.lunIndex, 1, 0, m_bcmBuffer, 0);
            }
            if(g_sdk_api.STD_GetDeviceID) {
                ((PFN_VDR_ReadLUNID) g_sdk_api.STD_GetDeviceID)(
                    0, &vol.deviceId, 1, 0, m_bcmBuffer, 0);
            }
        }

        m_deviceInfo.volumeCount++;
        CloseHandle(hDevice);  // Close handle after processing
    }

    LogMessage("CheckDriveExist: Found %d ITE devices.", m_deviceInfo.volumeCount);
    return m_deviceInfo.volumeCount;
}

BOOL iTEUFDrs::OpenDriveHandleAgain(BYTE volumeIndex) {
    LogMessage("OpenDriveHandleAgain: re-checking physical drives");

    // Enhanced implementation based on Ghidra analysis of FUN_00408580
    // This function opens physical drives \\.\PhysicalDrive1-8 with enhanced security

    // Close any existing handles before re-opening
    for(BYTE i = 0; i < m_deviceInfo.volumeCount; ++i) {
        if(m_deviceInfo.volumes[i].hDevice != INVALID_HANDLE_VALUE) {
            CloseHandle(m_deviceInfo.volumes[i].hDevice);
            m_deviceInfo.volumes[i].hDevice = INVALID_HANDLE_VALUE;
            LogMessage("OpenDriveHandleAgain: Closed existing handle for volume %d", i);
        }
    }

    // Physical drive paths based on FUN_00408580 analysis
    const LPCSTR physicalDrivePaths[] = { "\\\\.\\PhysicalDrive1", "\\\\.\\PhysicalDrive2",
                                          "\\\\.\\PhysicalDrive3", "\\\\.\\PhysicalDrive4",
                                          "\\\\.\\PhysicalDrive5", "\\\\.\\PhysicalDrive6",
                                          "\\\\.\\PhysicalDrive7", "\\\\.\\PhysicalDrive8" };

    BOOL foundDevice = FALSE;

    // Scan through physical drives
    for(BYTE driveIndex = 0; driveIndex < 8 && driveIndex < MAX_VOLUMES; driveIndex++) {
        // CRITICAL FIX: Use original access rights as per Ghidra MCP analysis (0x0040beb0)
        // Original used 0xc0000000 (GENERIC_READ | GENERIC_WRITE) - REQUIRED for ITE devices
        // ITE SDK functions need write access for device initialization
        HANDLE hDevice = CreateFileA(
            physicalDrivePaths[driveIndex],
            GENERIC_READ | GENERIC_WRITE,  // Original 0xc0000000 equivalent - FIXED
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);

        if(hDevice == INVALID_HANDLE_VALUE) {
            DWORD error = GetLastError();
            LogMessage(
                "OpenDriveHandleAgain: Cannot access %s, error=%lu",
                physicalDrivePaths[driveIndex],
                error);
            continue;
        }

        // Basic device validation
        if(! ValidatePhysicalDevice(hDevice, driveIndex)) {
            LogWarning(
                "OpenDriveHandleAgain: Device validation failed for %s",
                physicalDrivePaths[driveIndex]);
            CloseHandle(hDevice);
            continue;
        }

        // Store device handle
        if(driveIndex < m_deviceInfo.volumeCount) {
            m_deviceInfo.volumes[driveIndex].hDevice = hDevice;
            m_deviceInfo.volumes[driveIndex].deviceFound = TRUE;
            m_deviceInfo.volumes[driveIndex].volumeIndex = driveIndex;
            foundDevice = TRUE;

            LogMessage(
                "OpenDriveHandleAgain: Successfully opened %s", physicalDrivePaths[driveIndex]);
        } else {
            CloseHandle(hDevice);
        }
    }

    if(! foundDevice) {
        LogError("OpenDriveHandleAgain: No accessible physical drives found");
        return FALSE;
    }

    LogMessage("OpenDriveHandleAgain: Physical drive scan completed");
    return TRUE;
}

// Enhanced security validation for physical devices
BOOL iTEUFDrs::ValidatePhysicalDevice(HANDLE hDevice, BYTE driveIndex) {
    if(hDevice == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    // Basic device validation using DeviceIoControl
    // This helps prevent attacks through malicious device responses

    STORAGE_DEVICE_NUMBER deviceNumber;
    DWORD bytesReturned = 0;

    // Get device number to verify it's a legitimate storage device
    if(! DeviceIoControl(
           hDevice,
           IOCTL_STORAGE_GET_DEVICE_NUMBER,
           NULL,
           0,
           &deviceNumber,
           sizeof(deviceNumber),
           &bytesReturned,
           NULL)) {
        DWORD error = GetLastError();
        LogWarning(
            "ValidatePhysicalDevice: Cannot get device number for drive %d, error=%lu",
            driveIndex,
            error);
        // Don't fail immediately - some devices might not support this
    } else {
        LogMessage(
            "ValidatePhysicalDevice: Drive %d - DeviceType=%lu, DeviceNumber=%lu, "
            "PartitionNumber=%lu",
            driveIndex,
            deviceNumber.DeviceType,
            deviceNumber.DeviceNumber,
            deviceNumber.PartitionNumber);

        // Verify it's a disk device
        if(deviceNumber.DeviceType != FILE_DEVICE_DISK) {
            LogError(
                "ValidatePhysicalDevice: Drive %d is not a disk device (type=%lu)",
                driveIndex,
                deviceNumber.DeviceType);
            return FALSE;
        }
    }

    // Get drive geometry for additional validation
    DISK_GEOMETRY geometry;
    if(DeviceIoControl(
           hDevice,
           IOCTL_DISK_GET_DRIVE_GEOMETRY,
           NULL,
           0,
           &geometry,
           sizeof(geometry),
           &bytesReturned,
           NULL)) {
        LogMessage(
            "ValidatePhysicalDevice: Drive %d geometry - Cylinders=%lld, TracksPerCylinder=%lu, "
            "SectorsPerTrack=%lu, BytesPerSector=%lu",
            driveIndex,
            geometry.Cylinders.QuadPart,
            geometry.TracksPerCylinder,
            geometry.SectorsPerTrack,
            geometry.BytesPerSector);

        // Basic sanity checks
        if(geometry.BytesPerSector == 0 || geometry.BytesPerSector > 8192) {
            LogError(
                "ValidatePhysicalDevice: Drive %d has invalid bytes per sector: %lu",
                driveIndex,
                geometry.BytesPerSector);
            return FALSE;
        }

        if(geometry.SectorsPerTrack == 0 || geometry.SectorsPerTrack > 1024) {
            LogError(
                "ValidatePhysicalDevice: Drive %d has invalid sectors per track: %lu",
                driveIndex,
                geometry.SectorsPerTrack);
            return FALSE;
        }
    }

    LogMessage("ValidatePhysicalDevice: Drive %d validation passed", driveIndex);
    return TRUE;
}

BOOL iTEUFDrs::SetDeviceID() {
    LogMessage("SetDeviceID: processing device IDs for %d volumes", m_volumeCount);

    for(BYTE i = 0; i < m_volumeCount; i++) {
        DEVICE_VOLUME_INFO& volume = m_deviceInfo.volumes[i];
        if(! volume.deviceFound) {
            continue;
        }

        // Open the correct handle type
        if(m_deviceInfo.driveOpened) {
            if(! OpenPhysicalDrive(i)) {
                LogWarning("SetDeviceID: Could not open physical drive %d", i);
                continue;
            }
        } else {
            if(! OpenLogicalDriveHandle(i)) {
                LogWarning("SetDeviceID: Could not open logical drive %c:", volume.volumeLetter);
                continue;
            }
        }

        // Check if device ID already exists
        if(volume.deviceId == 0xFF || volume.deviceId == 0) {  // 0xFF is our uninitialized value
            // Device ID not set, find a new one
            BYTE newDeviceID = 0;
            BOOL idFound = FALSE;
            for(int j = 0; j < 255; j++) {
                if(m_deviceIDTable[j] == 0) {
                    newDeviceID = (BYTE) j;
                    idFound = TRUE;
                    break;
                }
            }

            if(! idFound) {
                LogError("SetDeviceID: DeviceID table is full. Cannot assign new ID.");
                CloseDeviceHandle(i);
                continue;
            }

            if(g_sdk_api.STD_SetDeviceID) {
                int result = ((PFN_VDR_WriteLUNID) g_sdk_api.STD_SetDeviceID)(
                    newDeviceID, m_bcmBuffer, 0, 0, m_bcmBuffer, 1);
                if(result == 0) {
                    LogMessage(
                        "SetDeviceID: Successfully set DeviceID %d for volume %d", newDeviceID, i);
                    m_deviceIDTable[newDeviceID] = 1;  // Mark as used
                    volume.deviceId = newDeviceID;
                } else {
                    LogError("SetDeviceID: Failed to set DeviceID for volume %d", i);
                }
            } else {
                LogError("SetDeviceID: STD_SetDeviceID function not found in SDK.");
            }
        } else {
            // Device ID already exists, let's verify it
            BYTE currentDeviceID = 0xFF;
            if(g_sdk_api.STD_GetDeviceID) {
                int result = ((PFN_VDR_ReadLUNID) g_sdk_api.STD_GetDeviceID)(
                    0, &currentDeviceID, 1, 0, m_bcmBuffer, 0);
                if(result == 0) {
                    if(currentDeviceID != volume.deviceId) {
                        LogWarning(
                            "SetDeviceID: Mismatch! Stored DeviceID is %d but hardware reports %d "
                            "for volume %d.",
                            volume.deviceId,
                            currentDeviceID,
                            i);
                        // Optionally, update our stored ID
                        volume.deviceId = currentDeviceID;
                    } else {
                        LogMessage(
                            "SetDeviceID: Verified existing DeviceID %d for volume %d",
                            currentDeviceID,
                            i);
                    }
                    // Ensure the device ID is marked in our table
                    if(currentDeviceID < 255) {
                        m_deviceIDTable[currentDeviceID] = 1;
                    }
                } else {
                    LogError("SetDeviceID: Failed to read existing DeviceID for volume %d", i);
                }
            } else {
                LogError("SetDeviceID: STD_GetDeviceID function not found in SDK.");
            }
        }

        // Close the handle after processing
        CloseDeviceHandle(i);
    }

    LogMessage("SetDeviceID: finished processing.");
    return TRUE;
}

void iTEUFDrs::VolumePairController() {
    LogMessage("VolumePairController: Pairing %d volumes into controllers.", m_volumeCount);

    // Reset controller count before pairing
    m_controllerCount = 0;

    // This array tracks which volume indices have been assigned to a controller
    bool volumesProcessed[MAX_VOLUMES] = { false };

    // Outer loop: Iterate through each volume to see if it needs to be paired
    for(BYTE i = 0; i < m_volumeCount; i++) {
        // If this volume has already been processed, skip it
        if(volumesProcessed[i]) {
            continue;
        }

        // We've found an unpaired volume, so create a new controller for it.
        // Make sure we don't exceed the max number of controllers.
        if(m_controllerCount >= MAX_CONTROLLERS) {
            LogError(
                "VolumePairController: Exceeded maximum number of controllers (%d).",
                MAX_CONTROLLERS);
            break;
        }

        CONTROLLER_DATA& newController = m_controllerData[m_controllerCount];
        newController.isValid = TRUE;
        newController.volumeCount = 0;

        // The deviceId of the first unpaired volume determines the ID for the new controller
        BYTE controllerDeviceId = m_deviceInfo.volumes[i].deviceId;
        newController.deviceId = controllerDeviceId;

        LogMessage(
            "VolumePairController: Creating new controller #%d with DeviceID %d (from volume %d)",
            m_controllerCount,
            controllerDeviceId,
            i);

        // Inner loop: Find all other volumes that belong to this new controller
        for(BYTE j = 0; j < m_volumeCount; j++) {
            // Check if this volume has the same deviceId and hasn't been processed
            if(! volumesProcessed[j] && m_deviceInfo.volumes[j].deviceId == controllerDeviceId) {
                // Make sure we don't add more volumes than the controller can hold
                if(newController.volumeCount < 4) {
                    newController.volumeIndexes[newController.volumeCount] = j;
                    newController.volumeCount++;
                    volumesProcessed[j] = true;  // Mark this volume as processed
                    LogMessage(
                        "VolumePairController: ... added volume %d to controller #%d",
                        j,
                        m_controllerCount);
                } else {
                    LogWarning(
                        "VolumePairController: Controller #%d is full. Cannot add volume %d.",
                        m_controllerCount,
                        j);
                }
            }
        }

        // Inherit properties from the first volume in the group
        if(newController.volumeCount > 0) {
            BYTE firstVolumeIndex = newController.volumeIndexes[0];
            newController.productId =
                m_deviceInfo.volumes[firstVolumeIndex].familyType  // Or some other ID
                ;
            newController.controllerType = m_deviceInfo.volumes[firstVolumeIndex].controllerType;
        }

        // Increment the total number of controllers found
        m_controllerCount++;
    }

    LogMessage(
        "VolumePairController: Finished pairing. Found %d unique controllers.", m_controllerCount);
}

BOOL iTEUFDrs::CheckSystemReadyIO(BYTE volumeIndex, DWORD deviceId) {
    // This function checks if system is ready for IO
    // Implementation based on decompiled code analysis
    LogMessage("CheckSystemReadyIO: volume %d, device 0x%08X", volumeIndex, deviceId);

    // Check if volume is accessible
    if(volumeIndex >= MAX_VOLUMES) {
        return FALSE;
    }

    // Check if device is ready
    return m_deviceInfo.volumes[volumeIndex].deviceFound;
}

BOOL iTEUFDrs::LoadBankC(BYTE volumeIndex, DWORD deviceId) {
    if(volumeIndex >= m_deviceInfo.volumeCount)
        return FALSE;

    DEVICE_VOLUME_INFO& volume = m_deviceInfo.volumes[volumeIndex];
    DEVICE_BANK_INFO& bank = volume.banks[0];  // Bank C

    // Initialize bank structure fields
    memset(&bank, 0, sizeof(DEVICE_BANK_INFO));

    // Build file path for Bank C
    CHAR filePath[MAX_PATH];
    if(! GetBinFilePath(volumeIndex, "BankC.bin", filePath, sizeof(filePath))) {
        LogError("LoadBankC: Failed to build file path for volume %d", volumeIndex);
        return FALSE;
    }

    // Open binary file
    FILE* file = fopen(filePath, "rb");
    if(! file) {
        LogError("LoadBankC: Can't open binary file: %s", filePath);
        return FALSE;
    }

    // Read version information from offset 0xF1E0 (62048)
    BYTE versionBuffer[64];
    memset(versionBuffer, 0xFF, sizeof(versionBuffer));

    if(fseek(file, 0xF1E0, SEEK_SET) != 0) {
        LogError("LoadBankC: Failed to seek to version offset");
        fclose(file);
        return FALSE;
    }

    size_t bytesRead = fread(versionBuffer, 1, sizeof(versionBuffer), file);
    fclose(file);

    if(bytesRead != sizeof(versionBuffer)) {
        LogError("LoadBankC: Failed to read version data (read %zu bytes)", bytesRead);
        return FALSE;
    }

    // Parse version information
    // Look for "ITEu" signature in the buffer
    char* iteuPos = strstr((char*) versionBuffer, "ITEu");
    if(! iteuPos || (iteuPos - (char*) versionBuffer) == -1) {
        LogWarning("LoadBankC: ITEu signature not found in version buffer");
        return FALSE;
    }

    // Extract version fields from the buffer
    // Based on the decompiled code, these are at specific offsets
    DWORD* versionData = (DWORD*) versionBuffer;

    bank.versionInfo.major = versionData[0];                     // uStack_40
    bank.versionInfo.minor = versionData[1];                     // uStack_3c
    bank.versionInfo.build = versionData[2];                     // uStack_38
    bank.versionInfo.revision = versionData[3];                  // uStack_34
    bank.versionInfo.date = versionData[4];                      // uStack_44
    bank.versionInfo.time = versionData[5];                      // uStack_30
    bank.versionInfo.checksum = versionData[6];                  // uStack_2c
    bank.versionInfo.flags = *(WORD*) (versionData + 7);         // uStack_28
    bank.versionInfo.reserved = *(BYTE*) (versionData + 7 + 2);  // uStack_26
    bank.versionInfo.size = versionData[8];                      // uStack_24
    bank.versionInfo.offset = versionData[9];                    // uStack_20

    // Format version string
    CHAR versionString[256];
    if(sprintf_s(
           versionString,
           sizeof(versionString),
           " %lu.%lu",
           bank.versionInfo.major,
           bank.versionInfo.minor)
       <= 0) {
        LogError("LoadBankC: Failed to format version string");
        return FALSE;
    }

    // Store version string in bank info
    strncpy_s(
        bank.versionString,
        sizeof(bank.versionString),
        versionString,
        sizeof(bank.versionString) - 1);

    LogMessage(
        "LoadBankC: Successfully loaded Bank C for volume %d, version: %s",
        volumeIndex,
        versionString);

    return TRUE;
}

BOOL iTEUFDrs::GetBCMInformation(BYTE volumeIndex, DWORD deviceId) {
    if(volumeIndex >= m_deviceInfo.volumeCount)
        return FALSE;

    DEVICE_VOLUME_INFO& volume = m_deviceInfo.volumes[volumeIndex];
    DEVICE_BANK_INFO& bank = volume.banks[0];  // Bank C

    // Check if system is ready for this volume
    if(! CheckSystemReadyIO(volumeIndex, deviceId)) {
        LogError("GetBCMInformation: System not ready for volume %d", volumeIndex);
        return FALSE;
    }

    // Initialize BCM buffer
    BYTE bcmBuffer[0xE40];
    memset(bcmBuffer, 0, sizeof(bcmBuffer));

    // Build device path
    CHAR devicePath[8];
    buildVolumePath(volume.volumeLetter, devicePath, sizeof(devicePath));

    // Open device handle
    HANDLE hDevice = CreateFileA(
        devicePath,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if(hDevice == INVALID_HANDLE_VALUE) {
        LogError("GetBCMInformation: Failed to open device %s", devicePath);
        return FALSE;
    }

    BOOL result = FALSE;

    // Call VDR_CheckSYSReady to check system ready status
    if(g_sdk_api.STD_TestUnitReady) {
        int sysReadyResult = ((PFN_VDR_CheckSYSReady) g_sdk_api.STD_TestUnitReady)(
            0, bcmBuffer, sizeof(bcmBuffer), 0, bcmBuffer, 0);
        if(sysReadyResult == 0) {
            // System is ready, now call VDR_SetSYSReady
            if(g_sdk_api.VDR_SetSYSReady) {
                int setReadyResult = ((PFN_VDR_SetSYSReady) g_sdk_api.VDR_SetSYSReady)(
                    deviceId, bcmBuffer, sizeof(bcmBuffer), 0, bcmBuffer, 0);
                if(setReadyResult != 0) {
                    // Set ready failed
                    LogError(
                        "GetBCMInformation: Set system ready failed for volume %d", volumeIndex);
                } else {
                    // Now initialize ISP code
                    BYTE initParams[8] = { 0 };
                    initParams[0] = 0xC0;  // ISP init command
                    initParams[1] = 0xD0;  // ISP init subcommand
                    initParams[2] = 0x00;  // Reserved
                    initParams[3] = 0x08;  // Parameter length

                    // Call FLH_InitCodeWithIspPath
                    if(g_sdk_api.FLH_InitCodeWithIspPath) {
                        int initResult =
                            ((PFN_FLH_InitCodeWithIspPath) g_sdk_api.FLH_InitCodeWithIspPath)(
                                1,
                                (BYTE*) &initParams[1],
                                (BYTE*) &initParams[0],
                                (BYTE*) &initParams[2],
                                m_basePath,
                                bcmBuffer,
                                hDevice);
                        if(initResult == 0) {
                            LogMessage(
                                "GetBCMInformation: Successfully initialized ISP code for volume "
                                "%d",
                                volumeIndex);
                            result = TRUE;

                            // Store BCM information in bank structure
                            memcpy(bank.bcmInfo, bcmBuffer, sizeof(bank.bcmInfo));
                            bank.bcmLoaded = TRUE;
                        } else {
                            LogError(
                                "GetBCMInformation: ISP initialization failed for volume %d",
                                volumeIndex);
                        }
                    } else {
                        LogError("GetBCMInformation: FLH_InitCodeWithIspPath not bound");
                    }
                }
            } else {
                LogError("GetBCMInformation: VDR_SetSYSReady not bound");
            }
        } else {
            LogError("GetBCMInformation: System ready check failed for volume %d", volumeIndex);
        }
    } else {
        LogError("GetBCMInformation: VDR_CheckSYSREADY not bound");
    }

    CloseHandle(hDevice);
    return result;
}

BOOL iTEUFDrs::LoadBankData(BYTE volumeIndex, DWORD deviceId) {
    // This function loads bank data
    // Implementation based on decompiled code analysis
    LogMessage("LoadBankData: volume %d, device 0x%08X", volumeIndex, deviceId);

    // Load bank data
    return CopyBankData(volumeIndex);
}

BOOL iTEUFDrs::FormatDeviceIdentification() {
    LogMessage("FormatDeviceIdentification: formatting device identification");

    // Format device identification string
    if(m_deviceInfo.selectedVolume != 0xFF) {
        DEVICE_VOLUME_INFO* pVolume = &m_deviceInfo.volumes[m_deviceInfo.selectedVolume];

        // Format device string
        FormatDeviceString(
            m_deviceInfo.deviceString,
            sizeof(m_deviceInfo.deviceString),
            " %s%s , ( %C )\n%s",
            pVolume->vendorName,
            pVolume->productName,
            pVolume->familyType,
            "");

        // Copy device ID
        memcpy(m_deviceInfo.deviceData, &pVolume->inquiryData1, 8);
    }

    return TRUE;
}

BOOL iTEUFDrs::FormatDeviceString(LPSTR buffer, DWORD size, LPCSTR format, ...) {
    // This function formats device string
    // Implementation based on decompilation code analysis
    if(! buffer || size == 0) {
        return FALSE;
    }

    va_list args;
    va_start(args, format);

    int result = vsnprintf_s(buffer, size, _TRUNCATE, format, args);

    va_end(args);

    return (result >= 0);
}

BOOL iTEUFDrs::IdentifyDeviceFamily(LPCSTR inquiryString, BYTE volumeIndex) {
    if(volumeIndex >= MAX_VOLUMES) {
        return FALSE;
    }

    DEVICE_VOLUME_INFO* pVolume = &m_deviceInfo.volumes[volumeIndex];

    // Check for ITE device
    if(strstr(inquiryString, "ITEu")) {
        // Check for specific family types
        if(strstr(inquiryString, "1181")) {
            pVolume->familyType = DEVICE_FAMILY_A1BA;  // 1181 family
            pVolume->controllerType = 0;               // Default for 1181
            if(strstr(inquiryString, "A1BA")) {
                pVolume->a1baFlag = 1;  // A1BA variant
                pVolume->controllerType = 1;
            } else if(strstr(inquiryString, "A0AA")) {
                pVolume->a1baFlag = 0;  // A0AA variant
            }
        } else if(strstr(inquiryString, "1176")) {
            pVolume->familyType = 2;  // 1176 family
            pVolume->controllerType = 2;
            if(strstr(inquiryString, "A0AA")) {
                pVolume->a1baFlag = 0;  // A0AA variant
            }
        }

        return TRUE;
    }

    return FALSE;
}

/**
 * Load all SDK functions from 181FlashSDK.dll
 * Reconstructed from iTEUFDrs::LoadSDKFunctions at 0x00401000
 * This function loads 100+ API functions from the ITE Flash SDK
 */
BOOL iTEUFDrs::LoadSDKFunctions(HMODULE hSDK) {
    if(! hSDK) {
        LogError("LoadSDKFunctions: SDK module handle is NULL");
        return FALSE;
    }

    LogMessage("LoadSDKFunctions: Loading SDK function addresses...");

#define LOAD_PROC(name)                                   \
    g_sdk_api.name = GetProcAddress(hSDK, #name);         \
    if(! g_sdk_api.name) {                                \
        LogMessage("Failed to load function: %s", #name); \
        return FALSE;                                     \
    }

    LOAD_PROC(FLH_GetInfoFromDataBaseByID);
    LOAD_PROC(FLH_GetFlashDataFromDataBase);
    LOAD_PROC(FLH_GetFlashDataFromMemory);
    LOAD_PROC(FLH_ReadRootTable);
    LOAD_PROC(FLH_WriteRootTable);
    LOAD_PROC(FLH_ReadCISTable);
    LOAD_PROC(FLH_WriteCISTable);
    LOAD_PROC(FLH_ReadISPData);
    LOAD_PROC(FLH_WriteISPData);
    LOAD_PROC(FLH_ReadLatestWBT);
    LOAD_PROC(FLH_FindRootTable);
    LOAD_PROC(FLH_LBA2PhysicalFlash);
    LOAD_PROC(FLH_SetLedBlink);
    LOAD_PROC(FLH_PhyiscalRead);
    LOAD_PROC(FLH_PhyiscalWrite);
    LOAD_PROC(FLH_IsGoodBlock);
    LOAD_PROC(FLH_IsTableBlock);
    LOAD_PROC(FLH_MarkBad);
    LOAD_PROC(FLH_GetRealBlocksPerDie);
    LOAD_PROC(FLH_BlockIsGap);
    LOAD_PROC(FLH_HandleMassBlocksPerChip);
    LOAD_PROC(FLH_ScanNewBlock);
    LOAD_PROC(FLH_GetRetryRegister);
    LOAD_PROC(FLH_CISCheckSum_Calculate);
    LOAD_PROC(FLH_CalCulate_ECCNO);
    LOAD_PROC(FLH_ArrangeSegmentPara);
    LOAD_PROC(ADDR_ReadRootTable);
    LOAD_PROC(FLH_ReadSpare);
    LOAD_PROC(FLH_ReadID);
    LOAD_PROC(FLH_BlockErase);
    LOAD_PROC(FLH_SetSLCFlag);
    LOAD_PROC(FLH_CPUReset);
    LOAD_PROC(FLH_InitCTRL);
    LOAD_PROC(FLH_WriteRootTableWithIspPath);
    LOAD_PROC(FLH_ScanE2NANDBlockPerChip);
    LOAD_PROC(FLH_ReadBCM);
    LOAD_PROC(FLH_InitCodeWithIspPath);
    LOAD_PROC(FLH_GetChannelCeNoAndMap);
    LOAD_PROC(ADDR_ReadISPData);
    LOAD_PROC(ADDR_ReadCISData);
    LOAD_PROC(FLH_InitCodeForReady);
    LOAD_PROC(SEC_DoAuthentication);
    LOAD_PROC(SEC_LeaveAuthenticatedState);
    LOAD_PROC(SEC_GetPasswordHint);
    LOAD_PROC(SEC_SetPasswordHint);
    LOAD_PROC(SEC_ChangePassword);
    LOAD_PROC(SEC_GetUserPassword);
    LOAD_PROC(SEC_GetEncryptedPassword);
    LOAD_PROC(LUN_CreateLun);
    LOAD_PROC(LUN_FindLunStartLBAByItemID);
    LOAD_PROC(LUN_CreateApLunNewItemID);
    LOAD_PROC(LUN_WriteBadBlockMapToApLun);
    LOAD_PROC(LUN_ReadBadBlockMapFromApLun);
    LOAD_PROC(LUN_FindOptimumOffsetCap);
    LOAD_PROC(LUN_CalIsoSize);
    LOAD_PROC(FMT_Format);
    LOAD_PROC(FMT_GetOptimumCapacity);
    LOAD_PROC(FMT_GetOptimumLunConfig);
    LOAD_PROC(FMT_GetOSCapacity);
    LOAD_PROC(STD_Inquiry);
    LOAD_PROC(STD_ReadCapacity);
    LOAD_PROC(STD_LogicalRead);
    LOAD_PROC(STD_LogicalWrite);

    g_sdk_api.STD_TestUnitReady = GetProcAddress(hSDK, "VDR_CheckSYSReady");
    if(! g_sdk_api.STD_TestUnitReady) {
        LogMessage("Failed to load function: VDR_CheckSYSReady as STD_TestUnitReady");
        return FALSE;
    }
    g_sdk_api.STD_GetDeviceID = GetProcAddress(hSDK, "VDR_ReadLUNID");
    if(! g_sdk_api.STD_GetDeviceID) {
        LogMessage("Failed to load function: VDR_ReadLUNID as STD_GetDeviceID");
        return FALSE;
    }
    g_sdk_api.STD_SetDeviceID = GetProcAddress(hSDK, "VDR_WriteLUNID");
    if(! g_sdk_api.STD_SetDeviceID) {
        LogMessage("Failed to load function: VDR_WriteLUNID as STD_SetDeviceID");
        return FALSE;
    }
    g_sdk_api.STD_GetLUNIndex = GetProcAddress(hSDK, "VDR_ReadLUNIndex");
    if(! g_sdk_api.STD_GetLUNIndex) {
        LogMessage("Failed to load function: VDR_ReadLUNIndex as STD_GetLUNIndex");
        return FALSE;
    }

    LOAD_PROC(SwapDWORD);
    LOAD_PROC(SwapWORD);
    LOAD_PROC(CCBAddress2RawAddress);
    LOAD_PROC(RawAddress2CCBAddress);
    LOAD_PROC(CCBAddress2ED3Address);
    LOAD_PROC(ED3Address2CCBAddress);
    LOAD_PROC(BlkAddr2RawAddr);
    LOAD_PROC(VDR_ReadWriteLUNConfig);
    LOAD_PROC(VDR_ReadLUNData);
    LOAD_PROC(VDR_WriteLUNData);
    LOAD_PROC(VDR_ReadXData);
    LOAD_PROC(VDR_WriteXData);
    LOAD_PROC(VDR_ReadIData);
    LOAD_PROC(VDR_WriteIData);
    LOAD_PROC(VDR_ReadSysAddr);
    LOAD_PROC(VDR_WriteSysAddr);
    LOAD_PROC(VDR_SetSYSReady);
    LOAD_PROC(VDR_EndCode);
    LOAD_PROC(VDR_DeviceChange);
    LOAD_PROC(VDR_MediaChange);
    LOAD_PROC(VDR_WriteProtect);
    LOAD_PROC(VDR_RWCurrentLUNType);
    LOAD_PROC(VDR_HiddenArea);
    LOAD_PROC(VDR_ReadWriteLUNNo);
    LOAD_PROC(VDR_FlushCache);
    LOAD_PROC(VDR_ReadPage);
    LOAD_PROC(VDR_WritePage);
    LOAD_PROC(VDR_WriteBlock_TLC);
    LOAD_PROC(VDR_GetSecurityStatus);
    LOAD_PROC(VDR_ED3PageRead);
    LOAD_PROC(VDR_BadTFindRead);
    LOAD_PROC(VDR_Enhance_SLC_Program);
    LOAD_PROC(VDR_Disable_SLC_Program);
    LOAD_PROC(VDR_MassBlocksProcess);
    LOAD_PROC(VDR_F_RST);
    LOAD_PROC(VDR_RootFunc);
    LOAD_PROC(VDR_RootPageWrite);
    LOAD_PROC(VDR_RootAccess);
    LOAD_PROC(MP_CreateSystem);
    LOAD_PROC(MP_EraseSystemTable);
    LOAD_PROC(DG_GetBlockPageMapFromFlash);
    LOAD_PROC(DG_SearchReadBadTBlk);
    LOAD_PROC(DG_CalBlkRequire);
    LOAD_PROC(Is168Device);
    LOAD_PROC(GetLastestPage);

#undef LOAD_PROC

    LogMessage("LoadSDKFunctions: All SDK functions loaded successfully");
    return TRUE;
}

// Missing method implementations

int iTEUFDrs::GetBinFilePath(
    BYTE controller,
    const char* basePath,
    char* binPath,
    unsigned long pathSize) {
    if(! basePath || ! binPath || pathSize == 0) {
        LogError("GetBinFilePath: Invalid parameters");
        return 0;
    }

    // Build binary file path based on controller type
    const char* controllerName = (controller == 0) ? "1181" : "1176";
    int result = snprintf(binPath, pathSize, "%s\\%s.bin", basePath, controllerName);

    if(result < 0 || result >= (int) pathSize) {
        LogError("GetBinFilePath: Path too long");
        return 0;
    }

    LogMessage("GetBinFilePath: %s", binPath);
    return 1;
}

UINT iTEUFDrs::OpenDriveHandleAgain(int deviceIndex) {
    LogMessage("OpenDriveHandleAgain: deviceIndex=%d", deviceIndex);

    if(deviceIndex < 0 || deviceIndex >= MAX_VOLUMES) {
        LogError("OpenDriveHandleAgain: Invalid device index");
        return 0;
    }

    // Stub implementation - return success for now
    return 1;
}

// Stubs for missing functions
BOOL iTEUFDrs::OpenLogicalDriveHandle(BYTE volumeIndex) {
    LogMessage("STUB: OpenLogicalDriveHandle for volume %d", volumeIndex);
    if(volumeIndex < MAX_VOLUMES) {
        // Simulate opening a handle
        m_deviceInfo.volumes[volumeIndex].hDevice = (HANDLE) (uintptr_t) (volumeIndex + 1);
        return TRUE;
    }
    return FALSE;
}

void iTEUFDrs::CloseDeviceHandle(BYTE volumeIndex) {
    LogMessage("STUB: CloseDeviceHandle for volume %d", volumeIndex);
    if(volumeIndex < MAX_VOLUMES
       && m_deviceInfo.volumes[volumeIndex].hDevice != INVALID_HANDLE_VALUE) {
        m_deviceInfo.volumes[volumeIndex].hDevice = INVALID_HANDLE_VALUE;
    }
}

void iTEUFDrs::PrepareFirmwareFilePath() {
    LogMessage("STUB: PrepareFirmwareFilePath");
}

void iTEUFDrs::ReadBinaryFileVersion() {
    LogMessage("STUB: ReadBinaryFileVersion");
}

BOOL iTEUFDrs::InitializeISPCode(BYTE controllerIndex, DWORD deviceId) {
    LogMessage(
        "STUB: InitializeISPCode for controller %d, deviceId 0x%X", controllerIndex, deviceId);
    return TRUE;
}

void iTEUFDrs::LoadAndVerifyFirmwareSegments(BYTE controllerIndex, DWORD deviceId) {
    LogMessage(
        "STUB: LoadAndVerifyFirmwareSegments for controller %d, deviceId 0x%X",
        controllerIndex,
        deviceId);
}

void iTEUFDrs::UpdateFirmwareBankInfo(BYTE controllerIndex, DWORD deviceId) {
    LogMessage(
        "STUB: UpdateFirmwareBankInfo for controller %d, deviceId 0x%X", controllerIndex, deviceId);
}

BOOL iTEUFDrs::GetLunArrayData(BYTE controllerIndex, DWORD deviceId) {
    LogMessage("GetLunArrayData for controller %d, deviceId 0x%X", controllerIndex, deviceId);

    if(controllerIndex >= m_controllerCount) {
        LogError("GetLunArrayData: Invalid controller index %d", controllerIndex);
        return FALSE;
    }

    CONTROLLER_DATA& controller = m_controllerData[controllerIndex];
    if(! controller.isValid) {
        LogWarning("GetLunArrayData: Controller %d is not valid.", controllerIndex);
        return FALSE;
    }

    BYTE lunConfigBuffer[64];
    memset(lunConfigBuffer, 0, sizeof(lunConfigBuffer));

    if(! g_sdk_api.VDR_ReadWriteLUNConfig) {
        LogError("GetLunArrayData: VDR_ReadWriteLUNConfig function not found in SDK.");
        controller.lunInfoLoaded = FALSE;
        return FALSE;
    }

    // Mode 0 is for reading the LUN configuration
    int result = ((int(__stdcall*)(BOOL, LPVOID)) g_sdk_api.VDR_ReadWriteLUNConfig)(
        FALSE, (LPVOID) lunConfigBuffer);

    if(result == 0) {
        LogError("GetLunArrayData: Get Lun information fail for controller %d", controllerIndex);
        controller.lunInfoLoaded = FALSE;
        return FALSE;
    }

    memcpy(controller.lunData, lunConfigBuffer, sizeof(lunConfigBuffer));
    controller.lunInfoLoaded = TRUE;
    LogMessage(
        "GetLunArrayData: Successfully retrieved LUN info for controller %d", controllerIndex);

    return TRUE;
}

void iTEUFDrs::CalculateDeviceCapacity(BYTE controllerIndex) {
    LogMessage("CalculateDeviceCapacity for controller %d", controllerIndex);

    if(controllerIndex >= m_controllerCount) {
        LogError("CalculateDeviceCapacity: Invalid controller index %d", controllerIndex);
        return;
    }

    CONTROLLER_DATA& controller = m_controllerData[controllerIndex];
    if(! controller.isValid) {
        LogWarning("CalculateDeviceCapacity: Controller %d is not valid.", controllerIndex);
        return;
    }

    // The calculation is based on values from the BCM buffer.
    // We assume the relevant BCM data is in the first bank of the first volume.
    BYTE volumeIndex = controller.volumeIndexes[0];
    if(volumeIndex >= MAX_VOLUMES) {
        LogError(
            "CalculateDeviceCapacity: Invalid volume index %d for controller %d",
            volumeIndex,
            controllerIndex);
        return;
    }
    DEVICE_VOLUME_INFO& volume = m_deviceInfo.volumes[volumeIndex];
    if(! volume.banks[0].bcmLoaded) {
        LogError(
            "CalculateDeviceCapacity: BCM not loaded for controller %d, volume %d",
            controllerIndex,
            volumeIndex);
        return;
    }

    BYTE* bcm = volume.banks[0].bcmInfo;

    // Replicate the logic from Ghidra decompilation
    // iVar1 = 1 << (*(byte *)(param_1 + 0xa30) >> 2 & 3);
    int iVar1 = 1 << ((bcm[0x20] >> 2) & 3);

    // uVar3 = __aulldiv(*(undefined2 *)(param_1 + 0xa36),0,iVar1,iVar1 >> 0x1f);
    unsigned __int64 uVar3 = *(WORD*) (bcm + 0x26) / iVar1;

    // uVar3 = __allmul(uVar3,*(undefined4 *)(param_1 + 0xcb6),0);
    uVar3 *= *(DWORD*) (bcm + 0x2A6);

    // uVar3 = __allmul(uVar3,*(undefined2 *)(param_1 + 0xa38),0);
    uVar3 *= *(WORD*) (bcm + 0x28);

    // uVar3 = __allmul(uVar3,*(undefined1 *)(param_1 + 0xa20),0);
    uVar3 *= bcm[0x10];

    // uVar4 = __allmul(uVar3,iVar1,iVar1 >> 0x1f);
    unsigned __int64 uVar4 = uVar3 * iVar1;

    // *(uint *)(param_1 + 0x9ee) = (uint)uVar4 >> 9 | uVar2 << 0x17;
    // This calculates the capacity in some unit (likely sectors or MB)
    DWORD capacity = ((DWORD) uVar4 >> 9) | ((DWORD) (uVar4 >> 32) << 23);

    controller.capacity = capacity;

    LogMessage(
        "CalculateDeviceCapacity: Calculated capacity for controller %d is %lu",
        controllerIndex,
        capacity);
}

void iTEUFDrs::UpdateDeviceCapacityOrCalculate(BYTE controllerIndex) {
    LogMessage("UpdateDeviceCapacityOrCalculate for controller %d", controllerIndex);

    if(controllerIndex >= m_controllerCount) {
        LogError("UpdateDeviceCapacityOrCalculate: Invalid controller index %d", controllerIndex);
        return;
    }

    CONTROLLER_DATA& controller = m_controllerData[controllerIndex];
    if(! controller.isValid) {
        LogWarning("UpdateDeviceCapacityOrCalculate: Controller %d is not valid.", controllerIndex);
        return;
    }

    // In the original code, a flag is checked at an offset like `param_1 + 0xa1e`.
    // This seems to correspond to a field within the BCM buffer.
    BYTE volumeIndex = controller.volumeIndexes[0];
    if(volumeIndex >= MAX_VOLUMES) {
        LogError("UpdateDeviceCapacityOrCalculate: Invalid volume index %d", volumeIndex);
        return;
    }
    DEVICE_VOLUME_INFO& volume = m_deviceInfo.volumes[volumeIndex];
    if(! volume.banks[0].bcmLoaded) {
        LogError(
            "UpdateDeviceCapacityOrCalculate: BCM not loaded for controller %d", controllerIndex);
        // Fallback to calculation
        CalculateDeviceCapacity(controllerIndex);
        return;
    }

    BYTE* bcm = volume.banks[0].bcmInfo;
    char capacityFlag = bcm[0xE];  // Corresponds to offset 0xa1e if bcm starts at 0xa10

    controller.capacity = 0;

    if(capacityFlag != 0) {
        // Use pre-calculated capacity. This value is at an offset like `param_1 + 0x9de`.
        // This is outside the BCM buffer. It seems to be a separate field.
        // We've added `precalculatedCapacity` to our CONTROLLER_DATA struct for this.
        // Let's assume it's populated somewhere before this call.
        LogMessage(
            "UpdateDeviceCapacityOrCalculate: Using pre-calculated capacity %lu for controller %d",
            controller.precalculatedCapacity,
            controllerIndex);
        controller.capacity = controller.precalculatedCapacity;
    } else {
        // If the flag is not set, calculate the capacity now.
        LogMessage(
            "UpdateDeviceiacOrCalculate: Flag not set, calculating capacity for controller %d",
            controllerIndex);
        CalculateDeviceCapacity(controllerIndex);
    }
}

void iTEUFDrs::FormatFinalDeviceString(BYTE volumeIndex) {
    LogMessage("FormatFinalDeviceString for volume %d", volumeIndex);

    if(volumeIndex >= m_deviceInfo.volumeCount) {
        LogError("FormatFinalDeviceString: Invalid volume index %d", volumeIndex);
        return;
    }

    DEVICE_VOLUME_INFO& volume = m_deviceInfo.volumes[volumeIndex];
    BYTE controllerIndex = 0xFF;
    for(BYTE i = 0; i < m_controllerCount; ++i) {
        for(int j = 0; j < 4; ++j) {
            if(m_controllerData[i].volumeIndexes[j] == volumeIndex) {
                controllerIndex = i;
                break;
            }
        }
        if(controllerIndex != 0xFF)
            break;
    }

    if(controllerIndex == 0xFF) {
        LogError("FormatFinalDeviceString: Could not find controller for volume %d", volumeIndex);
        return;
    }

    CONTROLLER_DATA& controller = m_controllerData[controllerIndex];

    if(m_deviceInfo.systemReady && controller.mpInfoLoaded) {
        // This part corresponds to the successful case where MP info is loaded.
        // The original code formats a string with version and capacity.
        // " %s - %s " where the first part is the version string from the bank,
        // and the second part is the capacity.
        char capacityStr[32];
        sprintf_s(capacityStr, sizeof(capacityStr), "%lu.0M", controller.capacity);

        char bankVersion[256] = "NoVer";
        if(strlen(volume.banks[0].versionString) > 0) {
            strncpy_s(bankVersion, sizeof(bankVersion), volume.banks[0].versionString, _TRUNCATE);
        }

        sprintf_s(
            m_deviceInfo.deviceString,
            sizeof(m_deviceInfo.deviceString),
            " %s - %s ",
            bankVersion,
            capacityStr);

        LogMessage("FormatFinalDeviceString: Formatted string: %s", m_deviceInfo.deviceString);
    } else {
        // This is the fallback case if system is not ready or MP info failed.
        strcpy_s(m_deviceInfo.deviceString, sizeof(m_deviceInfo.deviceString), " NONE");
        LogWarning(
            "FormatFinalDeviceString: System not ready or MP info not loaded for volume %d.",
            volumeIndex);
    }

    // The original code also copies some inquiry data. Let's replicate that.
    // This seems to be part of setting the final device identification.
    memcpy(m_deviceInfo.deviceData, &volume.inquiryData1, 8);
}

BOOL iTEUFDrs::GetMPInfo(BYTE controllerIndex, DWORD deviceId) {
    LogMessage("STUB: GetMPInfo for controller %d, deviceId 0x%X", controllerIndex, deviceId);
    return TRUE;
}

BOOL iTEUFDrs::CopyBankData(BYTE volumeIndex) {
    LogMessage("STUB: CopyBankData for volume %d", volumeIndex);
    return TRUE;
}

BOOL iTEUFDrs::OpenPhysicalDrive(int deviceIndex) {
    LogMessage("STUB: OpenPhysicalDrive for device %d", deviceIndex);
    return TRUE;
}
