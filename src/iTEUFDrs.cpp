#include "../include/iTEUFDrs.h"

#include <ntddstor.h>
#include <stdio.h>
#include <string.h>
#include <winioctl.h>

#include "../include/DeviceStructures.h"
#include "../include/FlashSDK.h"
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

// Typedefs for SDK calls (best-effort based on decompilation patterns)
typedef int(__stdcall* PFN_VDR_SYSREADY)(void* bcmBuffer, HANDLE hDevice);
typedef int(__stdcall* PFN_VDR_SETSYSREADY)(DWORD mode, void* bcmBuffer, HANDLE hDevice);
typedef int(__stdcall* PFN_FLH_INITCODE)(
    DWORD param1,
    void* param2,
    void* param3,
    void* param4,
    LPCSTR basePath,
    void* bcmBuffer,
    HANDLE hDevice);
typedef int(__stdcall* PFN_STD_INQUIRY)(void* outBuffer, HANDLE deviceHandle);

// Typedefs for SDK calls (best-effort based on decompilation patterns)
typedef int(__stdcall* PFN_FLH_SCAN_MASS_BLOCKS_PER_CHIP)(
    DWORD ctx,
    BYTE ce,
    BYTE ch,
    int rtPtr,
    void* outBuf,
    BYTE mode,
    BYTE* outFlag,
    int* outRet);
typedef int(__stdcall* PFN_FLH_SCAN_E2NAND)(DWORD ctx, BYTE ce, BYTE ch, int rtPtr, void* outBuf);
typedef int(__stdcall* PFN_MP_ERASE_SYSTEM_TABLE)(DWORD ctx, int rtPtr, void* blockMap);
typedef int(__stdcall* PFN_FLH_ARRANGE_SEGMENT_PARA)(BYTE* outBuf, void* segmentInfo);
typedef int(__stdcall* PFN_FLH_INIT_CTRL)(DWORD ctx, BYTE* segmentParams, void* bankInfo);
typedef int(__stdcall* PFN_FLH_BLOCK_ERASE)(DWORD ctx, DWORD handle, int rtPtr);
typedef int(__stdcall* PFN_FLH_READISP)(
    DWORD deviceId,
    BYTE* buffer,
    DWORD bufferSize,
    BYTE lunIndex,
    BYTE* bcmInfo,
    BYTE mode);
typedef int(__stdcall* PFN_ADDR_READCIS)(DWORD deviceId, DWORD* buffer, DWORD lunId, BYTE* bcmInfo);
typedef int(__stdcall* PFN_FLH_GETFLASHDATAFROMDATABASE)(
    DWORD deviceId,
    BYTE* flashData,
    BYTE* deviceData,
    CHAR* devicePath);
typedef int(__stdcall* PFN_FLH_GETFLASHDATAFROMMEMORY)(DWORD deviceId, BYTE* flashData);
typedef int(__stdcall* PFN_VDR_READWRITELUNCONFIG)(
    DWORD mode,
    DWORD* buffer,
    BYTE* bcmInfo,
    DWORD deviceId);
typedef int(__stdcall* PFN_FLH_FINDROOTTABLE)(
    DWORD deviceId,
    DWORD* rootTableEntries,
    BYTE* bcmInfo,
    DWORD mode);
typedef int(__stdcall* PFN_VDR_ROOTFUNC)(
    DWORD address,
    DWORD mode,
    DWORD param1,
    DWORD param2,
    DWORD param3,
    BYTE* buffer,
    BYTE* bcmInfo,
    DWORD deviceId);
typedef int(__stdcall* PFN_VDR_READSYSADDR)(DWORD* sysAddrData, BYTE* bcmInfo, DWORD deviceId);

// Constructor implementation (equivalent to the original FUN_0040d690)
iTEUFDrs::iTEUFDrs(LPCSTR basePath)
    : m_vtable(nullptr), m_isInitialized(FALSE), m_lastError(0), m_hSDK(NULL),
      m_pVDR_GetDeviceInquiry(nullptr), m_pVDR_CheckDeviceSupport(nullptr),
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
    if(! InitializeFlashSDK(m_hSDK)) {
        LogError("iTEUFDrs: Failed to initialize SDK functions.");
        m_lastError = GetLastError();
        Unload181FlashSDK(m_hSDK);
        m_hSDK = NULL;
        return FALSE;
    }

    // Initialize device management SDK function pointers
    m_pVDR_GetDeviceInquiry = (PFN_VDR_GetDeviceInquiry) g_VDR_GetDeviceInquiry;
    m_pVDR_CheckDeviceSupport = (PFN_VDR_CheckDeviceSupport) g_VDR_CheckDeviceSupport;
    m_pVDR_GetLunIndex = (PFN_VDR_GetLunIndex) g_VDR_GetLunIndex;
    m_pVDR_GetDeviceID = (PFN_VDR_GetDeviceID) g_VDR_GetDeviceID;

    LogMessage("iTEUFDrs: SDK functions initialized successfully.");
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
    if(! FLH_ReadISPData || ! FLH_WriteISPData) {
        LogError("VerifySDKIntegrity: Critical ISP functions not loaded.");
        return FALSE;
    }

    if(! SEC_DoAuthentication || ! SEC_GetUserPassword || ! SEC_ChangePassword) {
        LogError("VerifySDKIntegrity: Critical security functions not loaded.");
        return FALSE;
    }

    // Check core flash operations
    if(! FLH_PhyiscalRead || ! FLH_PhyiscalWrite || ! FLH_BlockErase) {
        LogError("VerifySDKIntegrity: Critical flash operations not loaded.");
        return FALSE;
    }

    // Check device management functions
    if(! VDR_ReadWriteLUNConfig || ! VDR_GetSecurityStatus || ! VDR_CheckSYSReady) {
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
    m_vtable = nullptr;
    m_isInitialized = FALSE;
    m_lastError = 0;
    m_hSDK = NULL;
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

// Main GetDeviceInfo function (equivalent to the original FUN_0040cf30)
BOOL iTEUFDrs::GetDeviceInfoInternal() {
    LogMessage("GetDeviceInfo: Start");

    // Initialize parameter values
    if(! InitializeParaValue()) {
        LogError("GetDeviceInfo: InitializeParaValue fails.");
        return FALSE;
    }
    LogMessage("GetDeviceInfo: InitializeParaValue OK.");

    // Check if drives exist
    if(! CheckDriveExist()) {
        LogMessage("Open Drive Handle Again !");
        if(! OpenDriveHandleAgain(0)) {
            LogError("GetDeviceInfo: Device Not Found.");
            return FALSE;
        }
        m_deviceInfo.driveOpened = TRUE;
    }

    if(! CheckDriveExist()) {
        LogError("GetDeviceInfo: Device Not Found.");
        return FALSE;
    }

    LogMessage("GetDeviceInfo CheckDriveExist OK.");

    // Set device ID
    SetDeviceID();
    LogMessage("GetDeviceInfo SetDeviceID OK.");

    // Initialize volume pair controller
    VolumePairController();
    LogMessage("GetDeviceInfo VolumePairController OK.");

    // Process each volume
    for(BYTE i = 0; i < m_deviceInfo.volumeCount; i++) {
        if(m_deviceInfo.volumes[i].deviceFound) {
            BYTE volumeIndex = i;
            DWORD deviceId = m_deviceInfo.volumes[i].inquiryData1;

            // Check system ready IO
            if(! CheckSystemReadyIO(volumeIndex, deviceId)) {
                LogError("Check system ready IO fail ....");
                continue;
            }

            // Set current volume
            m_deviceInfo.selectedVolume = volumeIndex;

            // Load bank C
            if(! LoadBankC(volumeIndex, deviceId)) {
                LogError("Load BankC fail (Path not exist?)");
                continue;
            }

            // Get BCM information
            if(! GetBCMInformation(volumeIndex, deviceId)) {
                LogError("Get BCM information failed");
                continue;
            }

            // Copy bank data
            CopyBankData(volumeIndex);

            // Load bank data
            LoadBankData(volumeIndex, deviceId);

            // Format device string
            FormatDeviceIdentification();

            // Set system ready flag
            if(m_deviceInfo.ispLoaded && m_deviceInfo.deviceFound) {
                LogMessage("DoRepairDevice System Yes bISPLoaded");
                m_deviceInfo.systemReady = TRUE;
            } else {
                LogMessage("DoRepairDevice No System (!ISPLoad)");
                m_deviceInfo.repairMode = TRUE;
            }

            // Set bank as processed
            m_deviceInfo.banks[volumeIndex].isProcessed = TRUE;
        }
    }

    // Find first available volume
    m_deviceInfo.selectedVolume = 0xFF;
    for(BYTE i = 0; i < m_deviceInfo.volumeCount; i++) {
        if(m_deviceInfo.banks[i].isProcessed) {
            m_deviceInfo.selectedVolume = i;
            break;
        }
    }

    // Format final device string
    if(m_deviceInfo.selectedVolume != 0xFF) {
        DEVICE_VOLUME_INFO* pVolume = &m_deviceInfo.volumes[m_deviceInfo.selectedVolume];
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

    LogMessage("GetDeviceInfo: completed successfully");
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

        pController->isValid = TRUE;  // Based on loop structure, seems it's set to valid
        pController->deviceId = -1;
        pController->lunId = -1;
        pController->targetId = -1;
        pController->pathId = -1;
        pController->busId = -1;
        pController->scsiId = -1;
        pController->reserved1 = -1;
        pController->reserved2 = -1;

        // The original code had complex loops initializing parts of a larger structure.
        // This is a simplified interpretation based on the available structure definitions.
        for(int j = 0; j < 4; ++j) {
            pController->volumeIndexes[j] = 0xFF;  // -1 for byte
        }
    }

    // The original function returns a value, which seems to indicate success.
    return TRUE;
}

BYTE iTEUFDrs::CheckDriveExist() {
    LogMessage("CheckDriveExist: checking for physical drives");
    m_deviceInfo.volumeCount = 0;

    // Allocate buffer for inquiry data
    BYTE inquiryBuffer[176];  // 0xB0 bytes

    // Scan PhysicalDrive1-8 (as per Ghidra analysis - original binary behavior)
    for(int i = 1; i <= 8 && m_deviceInfo.volumeCount < MAX_VOLUMES; ++i) {
        char physicalPath[32];
        wsprintfA(physicalPath, "\\\\.\\PhysicalDrive%d", i);

        HANDLE hDevice = CreateFileA(
            physicalPath,
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
        PFN_STD_INQUIRY pInquiry = (PFN_STD_INQUIRY) g_STD_Inquiry;
        if(! pInquiry || pInquiry(inquiryBuffer, hDevice) == 0) {
            LogWarning("CheckDriveExist: STD_Inquiry failed for drive %d", i);
            CloseHandle(hDevice);
            continue;
        }

        // Check for "ITEu" signature
        char* inquiryString = (char*) inquiryBuffer;
        if(strstr(inquiryString + 8, "ITEu") == NULL) {
            LogMessage("CheckDriveExist: Not an ITE device on drive %d", i);
            CloseHandle(hDevice);
            continue;
        }

        // It's our device, populate the structure
        BYTE volIdx = m_deviceInfo.volumeCount;
        DEVICE_VOLUME_INFO& vol = m_deviceInfo.volumes[volIdx];

        vol.volumeIndex = volIdx;
        vol.volumeLetter = (char) ('C' + i - 1);  // Placeholder letter
        vol.driveType = GetDriveTypeA(NULL);      // Placeholder
        vol.hDevice = hDevice;                    // Keep handle open for now
        vol.deviceFound = TRUE;

        // Copy inquiry data parts
        memcpy(&vol.inquiryData1, inquiryBuffer + 0x24, 16);

        // Copy Vendor and Product strings
        for(int j = 0; j < 8; ++j)
            vol.vendorName[j] = (inquiryBuffer[8 + j] == 0) ? ' ' : inquiryBuffer[8 + j];
        vol.vendorName[7] = '\0';
        for(int j = 0; j < 16; ++j)
            vol.productName[j] = (inquiryBuffer[16 + j] == 0) ? ' ' : inquiryBuffer[16 + j];
        vol.productName[15] = '\0';
        // Build ASCII inquiry string for substring search
        char asciiBuf[256];
        int off = 0;
        for(int k = 8; k < 36 && off < 200; ++k) {
            char c = (char) inquiryBuffer[k];
            asciiBuf[off++] = (c == 0) ? ' ' : c;
        }
        asciiBuf[off] = '\0';

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
        // Copy raw inquiry
        memcpy(&vol.inquiryData1, inquiryBuffer, sizeof(vol.inquiryData1));
        SafeCloseHandle(vol.hDevice);
    }
    return (m_deviceInfo.volumeCount > 0);
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
    // This function sets device ID
    // Implementation based on decompiled code analysis
    LogMessage("SetDeviceID: setting device identification");

    // Process device inquiry for each volume
    for(BYTE i = 0; i < m_deviceInfo.volumeCount; i++) {
        if(m_deviceInfo.volumes[i].deviceFound) {
            // Set device ID based on inquiry data
            m_deviceInfo.volumes[i].inquiryData1 = 0x12345678;  // Placeholder
        }
    }

    return TRUE;
}

void iTEUFDrs::VolumePairController() {
    // This function initializes volume pair controller
    // Implementation based on decompiled code analysis
    LogMessage("VolumePairController: initializing volume controller");

    // Initialize bank structures
    InitializeDeviceStructures();
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
           " %s%s",
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
    PFN_VDR_SYSREADY pCheckSYSReady = (PFN_VDR_SYSREADY) g_VDR_CheckSYSReady;
    if(pCheckSYSReady) {
        int sysReadyResult = pCheckSYSReady(bcmBuffer, hDevice);
        if(sysReadyResult == 0) {
            // System is ready, now call VDR_SetSYSReady
            PFN_VDR_SETSYSREADY pSetSYSReady = (PFN_VDR_SETSYSREADY) g_VDR_SetSYSReady;
            if(pSetSYSReady) {
                int setReadyResult = pSetSYSReady(0, bcmBuffer, hDevice);
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
                    PFN_FLH_INITCODE pInitCode = (PFN_FLH_INITCODE) g_FLH_InitCodeWithIspPath;
                    if(pInitCode) {
                        int initResult = pInitCode(
                            1,
                            &initParams[1],
                            &initParams[0],
                            &initParams[2],
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

BOOL iTEUFDrs::CopyBankData(BYTE volumeIndex) {
    // Dummy implementation
    return TRUE;
}
