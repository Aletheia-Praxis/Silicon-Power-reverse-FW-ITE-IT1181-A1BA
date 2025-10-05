// clang-format off
#include "../include/iTEUFDrs.h"

#include <ntddstor.h>
#include <winioctl.h>

#include <cstdio>
#include <cstring>
#include "../include/SDKLoader.h"
#include "../include/Utilities.h"
#include "../include/WindowsHeaders.h"
// clang-format on

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
    : m_vtable(nullptr), m_isInitialized(FALSE), m_lastError(ITEUFDRS_ERROR_NONE), m_hSDK(NULL),
      m_pVDR_GetLunIndex(nullptr), m_pVDR_GetDeviceID(nullptr), m_pParentDlg(nullptr) {
    LogMessage("iTEUFDrs: constructor called with basePath: %s", basePath);

    // Initialize all member variables to a known state
    InitializeMembers();

    if(! basePath || *basePath == '\0') {
        LogError("iTEUFDrs: basePath is NULL or empty.");
        m_lastError = ITEUFDRS_ERROR_DEVICE_INFO;  // A generic init error
        return;
    }

    strncpy_s(m_basePath, sizeof(m_basePath), basePath, _TRUNCATE);

    // Load the SDK DLL
    CHAR sdkPath[MAX_PATH];
    sprintf_s(sdkPath, sizeof(sdkPath), "%s\\181FlashSDK.dll", m_basePath);
    m_hSDK = LoadLibraryA(sdkPath);

    if(m_hSDK == NULL) {
        LogError("iTEUFDrs: Failed to load 181FlashSDK.dll from %s", sdkPath);
        m_lastError = ITEUFDRS_ERROR_SDK_LOAD;
        return;
    }
    LogMessage("iTEUFDrs: Load 181FlashSDK.dll succeed.");

    // Load function pointers from the SDK
    if(! LoadSDKFunctions(m_hSDK)) {
        LogError("iTEUFDrs: Failed to get API addresses from SDK.");
        m_lastError = ITEUFDRS_ERROR_API_BIND;
        FreeLibrary(m_hSDK);
        m_hSDK = NULL;
        return;
    }
    LogMessage("iTEUFDrs: Get API address succeed in SDK.");

    // Detect and initialize devices
    if(! GetDeviceInfoInternal()) {
        LogError("iTEUFDrs: GetDeviceInfo failed.");
        m_lastError = ITEUFDRS_ERROR_DEVICE_INFO;
        // Don't return, allow partial initialization
    } else {
        LogMessage("iTEUFDrs: GetDeviceInfo OK");
        m_isInitialized = TRUE;
    }
}

// Default constructor
iTEUFDrs::iTEUFDrs()
    : m_vtable(nullptr), m_isInitialized(FALSE), m_lastError(ITEUFDRS_ERROR_NONE), m_hSDK(NULL),
      m_pVDR_GetLunIndex(nullptr), m_pVDR_GetDeviceID(nullptr), m_pParentDlg(nullptr) {
    LogMessage("iTEUFDrs: default constructor called");
    InitializeMembers();

    // Get the directory of the current executable to find the SDK
    char currentModulePath[MAX_PATH];
    char currentModuleDir[MAX_PATH];
    GetModuleFileNameA(NULL, currentModulePath, sizeof(currentModulePath));
    strcpy_s(currentModuleDir, sizeof(currentModuleDir), currentModulePath);
    char* lastBackslash = strrchr(currentModuleDir, '\\');
    if(lastBackslash) {
        *lastBackslash = '\0';
    }

    strncpy_s(m_basePath, sizeof(m_basePath), currentModuleDir, _TRUNCATE);

    // Load the SDK DLL
    CHAR sdkPath[MAX_PATH];
    sprintf_s(sdkPath, sizeof(sdkPath), "%s\\181FlashSDK.dll", m_basePath);
    m_hSDK = LoadLibraryA(sdkPath);

    if(m_hSDK == NULL) {
        LogError("iTEUFDrs: Failed to load 181FlashSDK.dll from %s", sdkPath);
        m_lastError = ITEUFDRS_ERROR_SDK_LOAD;
        return;
    }
    LogMessage("iTEUFDrs: Load 181FlashSDK.dll succeed.");

    // Load function pointers from the SDK
    if(! LoadSDKFunctions(m_hSDK)) {
        LogError("iTEUFDrs: Failed to get API addresses from SDK.");
        m_lastError = ITEUFDRS_ERROR_API_BIND;
        FreeLibrary(m_hSDK);
        m_hSDK = NULL;
        return;
    }
    LogMessage("iTEUFDrs: Get API address succeed in SDK.");

    // Detect and initialize devices
    if(! GetDeviceInfoInternal()) {
        LogError("iTEUFDrs: GetDeviceInfo failed.");
        m_lastError = ITEUFDRS_ERROR_DEVICE_INFO;
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

// This is the main entry point for device detection and initialization
// It is a reconstruction of the original iTEUFDrs_DetectAndInitializeDevices function
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
            if(! this->OpenPhysicalDrive(volumeIndex)) {
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
            if(! InitializeISPCode(i, controller, volume.hDevice)) {
                LogError("GetDeviceInfo: InitializeISPCode failed for controller %d", i);
                controller.isReady = FALSE;
                CloseDeviceHandle(volumeIndex);
                continue;
            }
            volume.ispCodeInitialized = TRUE;
        }

        // Notify firmware about segment info (loads BankC)
        if(! NotifyFwSegmentInfo(i, controller, volume.hDevice)) {
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
        // The original function takes the buffer and the handle
        int bcmResult =
            ((PFN_FLH_ReadBCM_Alt) g_sdk_api.FLH_ReadBCM)(controller.bcm, volume.hDevice);
        if(bcmResult != 1) {
            // Original code has a switch for different errors. We'll show a generic message.
            AfxMessageBox("Get BCM information fail", 0, 0);
            controller.isReady = FALSE;
            CloseDeviceHandle(volumeIndex);
            continue;
        }

        // Copy BCM data to the appropriate bank structure
        memcpy(volume.banks[0].bcmInfo, controller.bcm, sizeof(controller.bcm));
        volume.banks[0].bcmLoaded = TRUE;

        // Load/update firmware segments and capacity
        if(! volume.firmwareSegmentsLoaded) {
            LoadAndVerifyFirmwareSegments(i, controller, volume.hDevice);
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

BOOL iTEUFDrs::InitializeISPCode(
    BYTE controllerIndex,
    CONTROLLER_DATA& controller,
    HANDLE hDevice) {
    // This function appears to load the initial ISP code into the device.
    // The original implementation calls FLH_InitCodeWithIspPath with several parameters.
    // We will replicate that call here.

    if(! g_sdk_api.FLH_InitCodeWithIspPath) {
        // Log error
        return FALSE;
    }

    // The parameters from the Ghidra decompilation are complex.
    // For now, we pass NULLs and placeholders. These will need to be updated
    // as we understand the data structures better.
    // The path is likely the directory containing the firmware files.
    char currentDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, currentDir);

    return ((PFN_FLH_InitCodeWithIspPath) g_sdk_api.FLH_InitCodeWithIspPath)(
        controller.deviceId,
        NULL,        // p1
        NULL,        // p2
        NULL,        // p3
        currentDir,  // Base path
        controller.bcm,
        hDevice);
}

BOOL iTEUFDrs::NotifyFwSegmentInfo(
    BYTE controllerIndex,
    CONTROLLER_DATA& controller,
    HANDLE hDevice) {
    // This function notifies the SDK about the firmware segment information.
    // It calls FLH_ReadBCM_Alt in the original code.
    if(! g_sdk_api.FLH_ReadBCM) {
        return FALSE;
    }

    // The original function passes a buffer from the _DEVICE_INFO struct.
    // We'll pass our equivalent member from _CONTROLLER_DATA
    int result =
        ((PFN_FLH_ReadBCM_Alt) g_sdk_api.FLH_ReadBCM)(m_deviceInfo.firmwareLayout, hDevice);

    return result == 0;
}

BOOL iTEUFDrs::LoadAndVerifyFirmwareSegments(
    BYTE controllerIndex,
    CONTROLLER_DATA& controller,
    HANDLE hDevice) {
    if(! g_sdk_api.FLH_FindRootTable || ! g_sdk_api.VDR_RootFunc) {
        return FALSE;
    }

    BYTE findRootTableResult = ((PFN_FLH_FindRootTable_Alt) g_sdk_api.FLH_FindRootTable)(
        hDevice,
        controller.segmentIds,
        controller.bcm,
        1  // Mode
    );

    if(findRootTableResult != 1) {
        return FALSE;
    }

    for(int i = 0; i < 4; ++i) {
        if(controller.segmentIds[i] != 0 && controller.segmentIds[i] != 0xFFFFFFFF) {
            controller.segmentPresent[i] = TRUE;

            int rootFuncResult = ((PFN_VDR_RootFunc_Alt) g_sdk_api.VDR_RootFunc)(
                controller.segmentIds[i],
                0,
                0,
                0,
                0,  // Placeholder params
                controller.firmwareSegments[i],
                controller.bcm,
                hDevice);

            if(rootFuncResult != 0) {
                // Error loading segment
                return FALSE;
            }
        }
    }

    // Now, perform verification logic similar to the original binary
    // This involves iterating through blocks and analyzing their spare area.
    int numBlocks = controller.numBlocks;
    if(numBlocks > 0) {
        BYTE spareBuffer[16];
        for(int blockIndex = 0; blockIndex < numBlocks; ++blockIndex) {
            DWORD blockType =
                AnalyzeSpareAreaAndClassifyBlock(controller, blockIndex, 0, spareBuffer);

            // TODO: Add logic to handle the classified block type.
            if(blockType == 0x19 || blockType == 0x00) {
                // Handle bad block
            }
        }
    }

    return TRUE;
}

DWORD iTEUFDrs::AnalyzeSpareAreaAndClassifyBlock(
    CONTROLLER_DATA& controller,
    DWORD blockIndex,
    DWORD unknown,
    BYTE* spareBuffer) {
    if(! g_sdk_api.FLH_ReadSpare || ! g_sdk_api.FLH_LBA2PhysicalFlash) {
        return 0x19;  // General bad block error
    }

    memset(spareBuffer, 0xFF, 16);

    BYTE originalDeviceIndex = controller.currentDeviceIndex;
    // The original logic was complex: *(undefined1 *)((uint)*(byte *)(param_1 + 0x9a2) * 0x1daa +
    // 0x9ff + param_1); This needs careful reconstruction. For now, we use a simplified assignment.
    // controller.currentDeviceIndex = m_deviceInfo.currentVolume; // Example
    // simplification

    DWORD physicalAddress[2] = { 0 };

    ((PFN_FLH_CalculateBlockAddress) g_sdk_api.FLH_LBA2PhysicalFlash)(
        &controller, blockIndex, &physicalAddress);

    int readResult = ((PFN_FLH_ReadSpare_Alt) g_sdk_api.FLH_ReadSpare)(
        &physicalAddress, 1, &controller, spareBuffer, 16, unknown);

    DWORD blockType = 1;

    if(readResult != 0) {
        int ffCount = 0;
        for(int i = 0; i < 6; ++i) {
            if(spareBuffer[i] == 0xFF) {
                ffCount++;
            }
        }
        if(ffCount > 2) {
            blockType = 4;
        }
    }

    if(! CheckDeviceTypeAndFlag(spareBuffer, 0x11)) {
        if(! CheckDeviceTypeAndFlag(spareBuffer, 0x12)) {
            if(! CheckDeviceTypeAndFlag(spareBuffer, 0x13)) {
                if(! CheckDeviceTypeAndFlag(spareBuffer, 0x14)) {
                    if(! CheckDeviceTypeAndFlag(spareBuffer, 0x16)) {
                        if(! CheckDeviceTypeAndFlag(spareBuffer, 0x1B)) {
                            if(*(DWORD*) spareBuffer == 0x42415442)  // "BATB"
                                blockType = 6;
                            else if(spareBuffer[5] == 'U') {
                                blockType = 0;
                            } else {
                                switch(spareBuffer[5]) {
                                case 0x11: blockType = 0x48; break;
                                case 0x22: blockType = 0x49; break;
                                case 0x33: blockType = 0x4A; break;
                                case 0x00: blockType = 1; break;
                                default: blockType = 0x19; break;
                                }
                            }
                        } else {
                            blockType = 0x1B;
                        }
                    } else {
                        blockType = 0x46;
                    }
                } else {
                    blockType = 0x44;
                }
            } else {
                blockType = 0x43;
            }
        } else {
            blockType = 0x42;
        }
    } else {
        blockType = 0x41;
    }

    controller.currentDeviceIndex = originalDeviceIndex;

    return blockType;
}

BOOL iTEUFDrs::CheckDeviceTypeAndFlag(BYTE* spareBuffer, BYTE flag) {
    char charToCheck;
    switch(flag) {
    case 0x11: charToCheck = 0x10; break;
    case 0x12: charToCheck = 0x20; break;
    case 0x13: charToCheck = 0x30; break;
    case 0x14: charToCheck = 0x40; break;
    case 0x15: charToCheck = 0x50; break;
    case 0x16: charToCheck = 0x60; break;
    case 0x1B: charToCheck = 0x80; break;
    default: return FALSE;
    }

    if(spareBuffer[4] == charToCheck && spareBuffer[5] == 'h') {
        return TRUE;
    }

    return FALSE;
}

// Stubs for unresolved external symbols
BOOL iTEUFDrs::LoadSDKFunctions(HMODULE hSDK) {
    return FALSE;
}
BOOL iTEUFDrs::InitializeParaValue() {
    // This function is the equivalent of FUN_00408370 (InitializeDeviceParameters)
    // It clears and initializes the main device and controller data structures.

    LogMessage("InitializeParaValue: Initializing device parameters.");

    // Corresponds to:
    // *(undefined1 *)(param_1 + 0x8a1) = 0; -> m_deviceInfo.deviceFound = FALSE;
    // *(undefined1 *)(param_1 + 0x8a2) = 0; -> m_deviceInfo.driveOpened = FALSE;
    m_deviceInfo.deviceFound = FALSE;
    m_deviceInfo.driveOpened = FALSE;

    // Corresponds to: _memset((void *)(param_1 + 0x8a3),0,0xff);
    // This clears the deviceString.
    memset(m_deviceInfo.deviceString, 0, sizeof(m_deviceInfo.deviceString));

    // Corresponds to: _memset((void *)(param_1 + 0x62a2),0,0x828);
    // This clears the entire volumes array within m_deviceInfo.
    memset(m_deviceInfo.volumes, 0, sizeof(m_deviceInfo.volumes));

    // This section corresponds to the loop that initializes the 3 controller structures.
    // puVar4 = (undefined1 *)(param_1 + 0x9a5); -> start of m_controllerData array
    for(int i = 0; i < MAX_CONTROLLERS; ++i) {
        CONTROLLER_DATA& controller = m_controllerData[i];

        // Corresponds to: _memset(puVar4 + -1,0,0x1daa);
        // We clear the entire controller structure.
        memset(&controller, 0, sizeof(CONTROLLER_DATA));

        // Corresponds to setting various fields to 0xFFFFFFFF
        controller.isValid = TRUE;  // *puVar4 = 1;
        controller.deviceId = 0xFFFFFFFF;
        // The following fields are mapped from the Ghidra decompilation offsets
        // puVar4 + 0x5c -> lunId
        // puVar4 + 0x60 -> targetId
        // puVar4 + 0x64 -> pathId
        // puVar4 + 0x68 -> busId
        // puVar4 + 0x6c -> scsiId
        // puVar4 + 0x70 -> reserved1
        // puVar4 + 0x74 -> reserved2
        // puVar4 + 0x78 -> productId (based on pattern)
        controller.lunId = 0xFFFFFFFF;
        controller.targetId = 0xFFFFFFFF;
        controller.pathId = 0xFFFFFFFF;
        controller.busId = 0xFFFFFFFF;
        controller.scsiId = 0xFFFFFFFF;
        controller.reserved1 = 0xFFFFFFFF;
        controller.reserved2 = 0xFFFFFFFF;
        // controller.productId = 0xFFFFFFFF; // This was incorrect, productId is a char array
        memset(controller.productId, 0, sizeof(controller.productId));

        // This part of the original struct is not yet fully defined in our C++ version.
        // Commenting out for now to allow compilation.
        // for(int j = 0; j < 2; ++j) {
        //     controller.lunInfo[j].field_0x0 = 0xFFFFFFFF;
        //     controller.lunInfo[j].field_0x4 = 0xFFFFFFFF;
        //     controller.lunInfo[j].field_0x8 = 0xFFFFFFFF;
        // }
        // controller.field_0x1d3a = 0xFFFFFFFF;
    }

    m_controllerCount = 0;
    m_volumeCount = 0;

    LogMessage("InitializeParaValue: Device parameters initialized successfully.");
    return TRUE;  // Return TRUE as the operation is successful.
}
BYTE iTEUFDrs::CheckDriveExist() {
    // This function is a reimplementation of FUN_0040b940 (ScanForITEUSBDevices)
    LogMessage("CheckDriveExist: Scanning for ITE USB devices...");

    char drivePath[] = "A:\\";
    BYTE foundDevices = 0;

    for(char driveLetter = 'A'; driveLetter <= 'Z'; ++driveLetter) {
        drivePath[0] = driveLetter;
        UINT driveType = GetDriveTypeA(drivePath);

        if(driveType == DRIVE_REMOVABLE || driveType == DRIVE_FIXED) {
            char volumePath[8];
            buildVolumePath(driveLetter, volumePath, sizeof(volumePath));

            HANDLE hDevice = CreateFileA(
                volumePath,
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL);

            if(hDevice == INVALID_HANDLE_VALUE) {
                continue;
            }

            // Use STORAGE_PROPERTY_QUERY to get device descriptor
            STORAGE_PROPERTY_QUERY query;
            query.PropertyId = StorageDeviceProperty;
            query.QueryType = PropertyStandardQuery;

            STORAGE_DEVICE_DESCRIPTOR devDescriptor;
            DWORD bytesReturned = 0;

            if(DeviceIoControl(
                   hDevice,
                   IOCTL_STORAGE_QUERY_PROPERTY,
                   &query,
                   sizeof(query),
                   &devDescriptor,
                   sizeof(devDescriptor),
                   &bytesReturned,
                   NULL)
               && bytesReturned > 0) {
                char* vendorId = (char*) &devDescriptor + devDescriptor.VendorIdOffset;
                char* productId = (char*) &devDescriptor + devDescriptor.ProductIdOffset;

                if(vendorId && strstr(vendorId, "ITE") != NULL) {
                    LogMessage("Found ITE device on drive %c:", driveLetter);
                    LogMessage("  Vendor: %s, Product: %s", vendorId, productId);

                    // Find an empty volume slot
                    int volumeIndex = -1;
                    for(int i = 0; i < MAX_VOLUMES; ++i) {
                        if(m_deviceInfo.volumes[i].volumeLetter == 0) {
                            volumeIndex = i;
                            break;
                        }
                    }

                    if(volumeIndex != -1) {
                        DEVICE_VOLUME_INFO& volume = m_deviceInfo.volumes[volumeIndex];
                        volume.volumeLetter = driveLetter;
                        volume.driveType = (BYTE) driveType;
                        strncpy_s(
                            volume.vendorName, sizeof(volume.vendorName), vendorId, _TRUNCATE);
                        strncpy_s(
                            volume.productName, sizeof(volume.productName), productId, _TRUNCATE);
                        volume.deviceFound = TRUE;

                        // Simplified logic from original binary to identify device family
                        if(strstr(productId, "1181")) {
                            volume.familyType = 1;  // A1BA family
                            if(strstr(productId, "A1BA")) {
                                volume.a1baFlag = 1;
                            } else {
                                volume.a1baFlag = 0;
                            }
                        } else if(strstr(productId, "1176")) {
                            volume.familyType = 2;  // 1176 family
                        } else {
                            volume.familyType = 0;  // Default/unknown
                        }

                        // Get LUN and Device ID using SDK functions
                        if(g_sdk_api.STD_GetLUNIndex && g_sdk_api.STD_GetDeviceID) {
                            BYTE lunIndex = 0xFF;
                            BYTE deviceId = 0xFF;
                            // The original functions are VDR_ReadLUNIndex and VDR_ReadLUNID
                            // which have a 6-parameter signature. We pass NULL for the unused ones.
                            if(((PFN_VDR_ReadLUNIndex) g_sdk_api.STD_GetLUNIndex)(
                                   0, &lunIndex, 1, 0, NULL, 0)) {
                                volume.lunIndex = lunIndex;
                            }
                            if(((PFN_VDR_ReadLUNID) g_sdk_api.STD_GetDeviceID)(
                                   0, &deviceId, 1, 0, NULL, 0)) {
                                volume.deviceId = deviceId;
                            }
                        }

                        foundDevices++;
                    }
                }
            }
            CloseHandle(hDevice);
        }
    }

    LogMessage("CheckDriveExist: Found %d ITE devices.", foundDevices);
    return foundDevices;
}
BOOL iTEUFDrs::SetDeviceID() {
    // This function is a reimplementation of FUN_0040ae40
    LogMessage("SetDeviceID: Assigning device IDs...");

    if(m_volumeCount == 0) {
        LogWarning("SetDeviceID: No volumes found to assign IDs.");
        return TRUE;  // Not an error, just nothing to do.
    }

    for(BYTE i = 0; i < m_volumeCount; ++i) {
        DEVICE_VOLUME_INFO& volume = m_deviceInfo.volumes[i];
        if(! volume.deviceFound) {
            continue;
        }

        HANDLE hDevice = INVALID_HANDLE_VALUE;
        if(m_deviceInfo.driveOpened) {
            // This logic needs to be more robust, mapping volume index to physical drive
            // For now, we assume a simple mapping.
            char physicalDrivePath[32];
            sprintf_s(physicalDrivePath, "\\\\.\\PhysicalDrive%d", i);
            hDevice = CreateFileA(
                physicalDrivePath,
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL);
        } else {
            char volumePath[8];
            buildVolumePath(volume.volumeLetter, volumePath, sizeof(volumePath));
            hDevice = CreateFileA(
                volumePath,
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL);
        }

        if(hDevice == INVALID_HANDLE_VALUE) {
            LogWarning("SetDeviceID: Could not open handle for volume %c:", volume.volumeLetter);
            continue;
        }

        BYTE currentDeviceId = 0xFF;
        BOOL success = ((PFN_VDR_ReadLUNID) g_sdk_api.STD_GetDeviceID)(
            0, &currentDeviceId, 1, 0, m_bcmBuffer, 0);

        if(success && currentDeviceId != 0xFF) {
            volume.deviceId = currentDeviceId;
            // Mark this ID as used in our table
            if(currentDeviceId < MAX_CONTROLLERS) {
                m_deviceIDTable[currentDeviceId] = 1;
            }
        } else {
            // Find an available device ID
            BYTE newDeviceId = 0xFF;
            for(BYTE j = 0; j < MAX_CONTROLLERS; ++j) {
                if(m_deviceIDTable[j] == 0) {
                    newDeviceId = j;
                    break;
                }
            }

            if(newDeviceId == 0xFF) {
                LogError("SetDeviceID: No available device IDs left.");
                CloseHandle(hDevice);
                continue;
            }

            // Assign the new device ID
            success = ((PFN_VDR_WriteLUNID) g_sdk_api.STD_SetDeviceID)(
                0, &newDeviceId, 1, 0, m_bcmBuffer, 0);
            if(success) {
                volume.deviceId = newDeviceId;
                m_deviceIDTable[newDeviceId] = 1;
                LogMessage(
                    "SetDeviceID: Assigned new device ID %d to volume %c:",
                    newDeviceId,
                    volume.volumeLetter);
            } else {
                LogError(
                    "SetDeviceID: Failed to set new device ID for volume %c:", volume.volumeLetter);
            }
        }

        CloseHandle(hDevice);
    }

    LogMessage("SetDeviceID: Finished assigning device IDs.");
    return TRUE;
}
void iTEUFDrs::VolumePairController() {
    // This function is a reimplementation of FUN_00408430
    LogMessage("VolumePairController: Pairing volumes to controllers...");

    if(m_volumeCount == 0) {
        LogWarning("VolumePairController: No volumes to pair.");
        return;
    }

    BYTE processedVolumes[MAX_VOLUMES] = { 0 };
    m_controllerCount = 0;

    for(BYTE i = 0; i < m_volumeCount; ++i) {
        if(processedVolumes[i] == 0) {
            if(m_controllerCount >= MAX_CONTROLLERS) {
                LogError("VolumePairController: Exceeded maximum number of controllers.");
                break;
            }

            DEVICE_VOLUME_INFO& baseVolume = m_deviceInfo.volumes[i];
            CONTROLLER_DATA& controller = m_controllerData[m_controllerCount];

            controller.deviceId = baseVolume.deviceId;
            controller.volumeIndexes[0] = i;
            controller.volumeCount = 1;
            processedVolumes[i] = 1;

            for(BYTE j = i + 1; j < m_volumeCount; ++j) {
                if(processedVolumes[j] == 0
                   && m_deviceInfo.volumes[j].deviceId == baseVolume.deviceId) {
                    if(controller.volumeCount
                       < 4) {  // Max 4 volumes per controller in original code
                        controller.volumeIndexes[controller.volumeCount] = j;
                        controller.volumeCount++;
                    }
                    processedVolumes[j] = 1;
                }
            }

            // Copy common properties from the first volume
            controller.controllerType = baseVolume.familyType;
            controller.isValid = TRUE;

            m_controllerCount++;
        }
    }

    LogMessage(
        "VolumePairController: Paired %d volumes into %d controllers.",
        m_volumeCount,
        m_controllerCount);
}
BOOL iTEUFDrs::GetMPInfo(BYTE controllerIndex, DWORD deviceId) {
    // This function is a reimplementation of FUN_0040b720
    LogMessage("GetMPInfo: Retrieving MP info for controller %d...", controllerIndex);

    CONTROLLER_DATA& controller = m_controllerData[controllerIndex];
    if(! controller.isValid) {
        LogError("GetMPInfo: Controller %d is not valid.", controllerIndex);
        return FALSE;
    }

    // The original code uses a function pointer g_pMP_ReadISPData.
    // Based on context, this is likely a vendor-specific command.
    // We'll assume it's aliased to VDR_ReadIData for now.
    if(! g_sdk_api.VDR_ReadIData) {
        LogError("GetMPInfo: VDR_ReadIData function not available.");
        return FALSE;
    }

    BYTE* mpBuffer = new BYTE[0x10000];
    if(! mpBuffer) {
        LogError("GetMPInfo: Failed to allocate memory for MP buffer.");
        return FALSE;
    }
    memset(mpBuffer, 0, 0x10000);

    // The original code tries reading from two different locations (mode 0 and 1).
    BOOL success = ((PFN_VDR_ReadIData) g_sdk_api.VDR_ReadIData)(mpBuffer, 0x10000);
    if(! success) {
        LogWarning("GetMPInfo: Read 1st ISP data fail. Trying 2nd...");
        // The second attempt in the original code seems to be a fallback.
        // The signature for VDR_ReadIData doesn't have a mode, so we can't replicate it exactly.
        // We'll just log the failure for now.
        delete[] mpBuffer;
        return FALSE;
    }

    // Extract MP info from the buffer based on Ghidra offsets
    // param_1 + 0x887 -> m_deviceInfo.volumes[x].mpInfo
    DEVICE_VOLUME_INFO& volume = m_deviceInfo.volumes[controller.volumeIndexes[0]];
    MP_INFO& mpInfo = volume.mpInfo;

    mpInfo.majorVersion = mpBuffer[0xf1fc];
    mpInfo.minorVersion = mpBuffer[0xf1fd];
    memcpy(mpInfo.vendorInfo, &mpBuffer[0xf1f0], 4);
    memcpy(mpInfo.productInfo, &mpBuffer[0xf1f4], 12);
    mpInfo.isLoaded = TRUE;

    LogMessage(
        "GetMPInfo: MP Info loaded: v%d.%d, Vendor: %c%c%c%c, Product: %s",
        mpInfo.majorVersion,
        mpInfo.minorVersion,
        mpInfo.vendorInfo[0],
        mpInfo.vendorInfo[1],
        mpInfo.vendorInfo[2],
        mpInfo.vendorInfo[3],
        mpInfo.productInfo);

    delete[] mpBuffer;
    return TRUE;
}
BOOL iTEUFDrs::GetLunArrayData(BYTE controllerIndex, DWORD deviceId) {
    // This function is a reimplementation of FUN_004088d0
    LogMessage("GetLunArrayData: Retrieving LUN array for controller %d...", controllerIndex);

    CONTROLLER_DATA& controller = m_controllerData[controllerIndex];
    if(! controller.isValid) {
        LogError("GetLunArrayData: Controller %d is not valid.", controllerIndex);
        return FALSE;
    }

    // The original code calls a function at DAT_004ad64c, which is likely VDR_ReadLUNData.
    if(! g_sdk_api.VDR_ReadLUNData) {
        LogError("GetLunArrayData: VDR_ReadLUNData function not available.");
        return FALSE;
    }

    // The original function reads into a 64-byte buffer.
    BOOL success = ((PFN_VDR_ReadLUNData) g_sdk_api.VDR_ReadLUNData)(0, controller.lunData, 64);

    if(! success) {
        LogError(" (GetLunArrayData) Get Lun information fail");
        controller.lunInfoLoaded = FALSE;
        return FALSE;
    }

    controller.lunInfoLoaded = TRUE;
    LogMessage("GetLunArrayData: LUN data loaded successfully for controller %d.", controllerIndex);
    return TRUE;
}

UINT iTEUFDrs::OpenDriveHandleAgain(int deviceIndex) {
    // This function is a reimplementation of FUN_0040d330
    LogMessage("OpenDriveHandleAgain: Attempting to open physical drive handles...");
    this->m_controllerCount = 0;  // Reset controller count before scanning

    for(int i = 0; i < 8; ++i) {  // Iterate through potential physical drives
        if(this->m_controllerCount >= MAX_CONTROLLERS) {
            LogWarning("OpenDriveHandleAgain: Reached max controllers.");
            break;
        }

        // OpenPhysicalDrive will attempt to open drive 'i', and if it's a valid
        // ITE device, it will populate a controller structure and increment m_controllerCount.
        this->OpenPhysicalDrive(i);
    }

    // The original code closes the handles after detection.
    // OpenPhysicalDrive now leaves the handle open on success, so we close them here.
    for(int i = 0; i < this->m_controllerCount; ++i) {
        if(this->m_controllerData[i].hDevice != INVALID_HANDLE_VALUE) {
            CloseHandle(this->m_controllerData[i].hDevice);
            this->m_controllerData[i].hDevice = INVALID_HANDLE_VALUE;
        }
    }

    LogMessage("OpenDriveHandleAgain: Found %d potential devices.", this->m_controllerCount);
    return this->m_controllerCount;
}

// This function attempts to open a single physical drive and, if it's a supported ITE device,
// populates the next available controller slot. It increments m_controllerCount on success.
BOOL iTEUFDrs::OpenPhysicalDrive(int driveIndex) {
    char drivePath[32];
    BYTE inquiryBuffer[0xB0];
    BYTE commandBuffer[0xE40];

    if(this->m_controllerCount >= MAX_CONTROLLERS) {
        return FALSE;  // No space for new controllers
    }

    sprintf_s(drivePath, sizeof(drivePath), "\\\\.\\PhysicalDrive%d", driveIndex);

    HANDLE hDevice = CreateFileA(
        drivePath,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if(hDevice == INVALID_HANDLE_VALUE) {
        return FALSE;  // Drive doesn't exist or can't be opened.
    }

    memset(inquiryBuffer, 0, sizeof(inquiryBuffer));
    // The device ID for SDK calls is the drive index.
    if(! ((PFN_STD_Inquiry) g_sdk_api.STD_Inquiry)(
           driveIndex, inquiryBuffer, sizeof(inquiryBuffer), 0, NULL, 0)) {
        CloseHandle(hDevice);
        return FALSE;
    }

    char* inquiryString = (char*) (inquiryBuffer + 8);
    if(strstr(inquiryString, "ITE") == NULL) {
        CloseHandle(hDevice);
        return FALSE;  // Not an ITE device.
    }

    // It is an ITE device. Get the next available controller slot.
    CONTROLLER_DATA& controller = this->m_controllerData[this->m_controllerCount];
    controller.hDevice = hDevice;  // Store the handle.
    LOG_INFO(" (OpenPhysicalDrive) Vol = %d, Inquiry = %s", driveIndex, inquiryString);

    controller.controllerType = 0;
    controller.controllerVersion = 0xFF;
    controller.controllerSubVersion = 0xFF;

    if(strstr(inquiryString, "1181")) {
        controller.controllerType = 0x1181;
        if(strstr(inquiryString, "A1BA")) {
            controller.controllerVersion = 1;
            controller.controllerSubVersion = 1;
        } else if(strstr(inquiryString, "A0AA")) {
            controller.controllerVersion = 0;
            controller.controllerSubVersion = 0;
        }
    } else if(strstr(inquiryString, "1176")) {
        controller.controllerType = 0x1176;
        if(strstr(inquiryString, "A0AA")) {
            controller.controllerVersion = 2;
            controller.controllerSubVersion = 0;
        }
    } else {
        controller.controllerVersion = 200;  // Unsupported
    }

    if(controller.controllerVersion == 200) {
        CloseHandle(hDevice);
        controller.hDevice = INVALID_HANDLE_VALUE;
        return FALSE;
    }

    memcpy(controller.vendorId, inquiryBuffer + 8, 8);
    controller.vendorId[8] = '\0';
    memcpy(controller.productId, inquiryBuffer + 16, 16);
    controller.productId[16] = '\0';

    if(((PFN_VDR_CheckSYSReady) g_sdk_api.STD_TestUnitReady)(
           driveIndex, commandBuffer, sizeof(commandBuffer), 0, NULL, 0)) {
        BYTE lunIndex = 0xFF;
        if(((PFN_VDR_ReadLUNIndex) g_sdk_api.STD_GetLUNIndex)(
               driveIndex, &lunIndex, 1, 0, NULL, 0)) {
            controller.lunId = lunIndex;
        }
        BYTE deviceId = 0xFF;
        if(((PFN_VDR_ReadLUNID) g_sdk_api.STD_GetDeviceID)(driveIndex, &deviceId, 1, 0, NULL, 0)) {
            controller.deviceId = deviceId;
            if(deviceId != 0xFF) {
                this->m_deviceIDTable[deviceId] = TRUE;
            }
        }
    }

    // Success. Increment the controller count.
    this->m_controllerCount++;
    return TRUE;
}

BOOL iTEUFDrs::OpenLogicalDriveHandle(BYTE param_1) {
    //... existing code...
    return FALSE;
}
void iTEUFDrs::CloseDeviceHandle(BYTE volumeIndex) {}
void iTEUFDrs::PrepareFirmwareFilePath() {}
void iTEUFDrs::ReadBinaryFileVersion() {}
void iTEUFDrs::UpdateFirmwareBankInfo(BYTE controllerIndex, DWORD deviceId) {}
void iTEUFDrs::FormatFinalDeviceString(BYTE volumeIndex) {}
void iTEUFDrs::CalculateDeviceCapacity(BYTE controllerIndex) {}
void iTEUFDrs::UpdateDeviceCapacityOrCalculate(BYTE controllerIndex) {}
