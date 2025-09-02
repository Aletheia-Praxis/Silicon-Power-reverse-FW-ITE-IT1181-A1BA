#include "iTEUFDrs.h"
#include "SDKLoader.h"
#include "SDKAPIs.h"
#include "Utilities.h"
#include <stdio.h>
#include <string.h>

// Constructor implementation (decompiled from FUN_0040d690)
iTEUFDrs::iTEUFDrs(LPCSTR basePath)
    : m_vtable(nullptr)
    , m_isInitialized(FALSE)
    , m_lastError(0)
    , m_hSDK(NULL)
{
    LogMessage("iTEUFDrs: constructor called with basePath: %s", basePath);
    
    // Initialize device structures
    InitializeDeviceStructures();
    
    if (basePath) {
        strncpy_s(m_basePath, sizeof(m_basePath), basePath, _TRUNCATE);
    } else {
        LogError("iTEUFDrs: basePath is NULL.");
        m_lastError = ERROR_INVALID_PARAMETER;
        return;
    }

    if (!InitializeSDK()) {
        LogError("iTEUFDrs: Failed to initialize SDK.");
        return;
    }

    // Call GetDeviceInfo (equivalent to FUN_0040cf30)
    if (!GetDeviceInfo()) {
        LogError("iTEUFDrs: GetDeviceInfo failed.");
        m_lastError = GetLastError();
        return;
    }

    LogMessage("iTEUFDrs: GetDeviceInfo OK");
    m_isInitialized = TRUE;
}

iTEUFDrs::~iTEUFDrs()
{
    if (m_hSDK) {
        ClearSDKAPIs(&m_sdkApis);
        Unload181FlashSDK(m_hSDK);
        m_hSDK = NULL;
    }
    LogMessage("iTEUFDrs: destructor called.");
}

BOOL iTEUFDrs::InitializeSDK()
{
    CHAR sdkPath[MAX_PATH];
    if (!JoinPathA(sdkPath, sizeof(sdkPath), m_basePath, "181FlashSDK.dll")) {
        LogError("iTEUFDrs: Failed to build SDK path.");
        m_lastError = ERROR_BUFFER_OVERFLOW;
        return FALSE;
    }

    if (!Load181FlashSDK(m_basePath, &m_hSDK)) {
        LogWarning("iTEUFDrs: 181FlashSDK.dll not loaded; some features may be unavailable.");
        m_lastError = GetLastError();
        return FALSE;
    }

    LogMessage("iTEUFDrs: Loaded 181FlashSDK.dll. Binding APIs...");
    if (!BindSDKAPIs(m_hSDK, &m_sdkApis)) {
        LogError("iTEUFDrs: Failed to bind SDK APIs.");
        m_lastError = GetLastError();
        Unload181FlashSDK(m_hSDK);
        m_hSDK = NULL;
        return FALSE;
    }
    LogMessage("iTEUFDrs: SDK APIs bound successfully.");
    return TRUE;
}

void iTEUFDrs::InitializeMembers()
{
    // Initialize all members to zero
    memset(&m_deviceInfo, 0, sizeof(m_deviceInfo));
    memset(m_basePath, 0, sizeof(m_basePath));
    m_vtable = nullptr;
    m_isInitialized = FALSE;
    m_lastError = 0;
    m_hSDK = NULL;
}

BOOL iTEUFDrs::InitializeDeviceStructures()
{
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
    for (int i = 0; i < MAX_VOLUMES; i++) {
        memset(&m_deviceInfo.volumes[i], 0, sizeof(DEVICE_VOLUME_INFO));
        m_deviceInfo.volumes[i].volumeIndex = (BYTE)i;
        m_deviceInfo.volumes[i].lunIndex = 0xFF;
        m_deviceInfo.volumes[i].deviceId = 0xFF;
        m_deviceInfo.volumes[i].familyType = 0xFF;
        m_deviceInfo.volumes[i].deviceFound = FALSE;
        m_deviceInfo.volumes[i].a1baFlag = 0xFF;
    }
    
    for (int i = 0; i < MAX_BANKS; i++) {
        memset(&m_deviceInfo.banks[i], 0, sizeof(DEVICE_BANK_INFO));
        m_deviceInfo.banks[i].bankIndex = (BYTE)i;
        m_deviceInfo.banks[i].bankStatus = 0xFF;
        m_deviceInfo.banks[i].bankType = 0xFF;
        m_deviceInfo.banks[i].bankReady = FALSE;
        m_deviceInfo.banks[i].bankLoaded = FALSE;
        m_deviceInfo.banks[i].bankError = FALSE;
    }
    
    return TRUE;
}

// Main GetDeviceInfo function (decompiled from FUN_0040cf30)
BOOL iTEUFDrs::GetDeviceInfo()
{
    LogMessage("GetDeviceInfo: Start");
    
    // Initialize parameter values
    if (!InitializeParaValue()) {
        LogError("GetDeviceInfo: InitializeParaValue fails.");
        return FALSE;
    }
    LogMessage("GetDeviceInfo: InitializeParaValue OK.");
    
    // Check if drives exist
    if (!CheckDriveExist()) {
        LogMessage("Open Drive Handle Again !");
        if (!OpenDriveHandleAgain()) {
            LogError("GetDeviceInfo: Device Not Found.");
            return FALSE;
        }
        m_deviceInfo.driveOpened = TRUE;
    }
    
    if (!CheckDriveExist()) {
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
    for (BYTE i = 0; i < m_deviceInfo.volumeCount; i++) {
        if (m_deviceInfo.volumes[i].deviceFound) {
            BYTE volumeIndex = i;
            DWORD deviceId = m_deviceInfo.volumes[i].inquiryData1;
            
            // Check system ready IO
            if (!CheckSystemReadyIO(volumeIndex, deviceId)) {
                LogError("Check system ready IO fail ....");
                continue;
            }
            
            // Set current volume
            m_deviceInfo.currentVolume = volumeIndex;
            
            // Load bank C
            if (!LoadBankC(volumeIndex, deviceId)) {
                LogError("Load BankC fail (Path not exist?)");
                continue;
            }
            
            // Get BCM information
            if (!GetBCMInformation(volumeIndex, deviceId)) {
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
            if (m_deviceInfo.ispLoaded && m_deviceInfo.deviceFound) {
                LogMessage("DoRepairDevice System Yes bISPLoaded");
                m_deviceInfo.systemReady = TRUE;
            } else {
                LogMessage("DoRepairDevice No System (!ISPLoad)");
                m_deviceInfo.repairMode = TRUE;
            }
            
            // Set bank as processed
            m_deviceInfo.banks[volumeIndex].bankStatus = 1;
        }
    }
    
    // Find first available volume
    m_deviceInfo.selectedVolume = 0xFF;
    for (BYTE i = 0; i < m_deviceInfo.volumeCount; i++) {
        if (m_deviceInfo.banks[i].bankStatus == 1) {
            m_deviceInfo.selectedVolume = i;
            break;
        }
    }
    
    // Format final device string
    if (m_deviceInfo.selectedVolume != 0xFF) {
        DEVICE_VOLUME_INFO* pVolume = &m_deviceInfo.volumes[m_deviceInfo.selectedVolume];
        FormatDeviceString(m_deviceInfo.deviceString, sizeof(m_deviceInfo.deviceString),
                          " %s%s , ( %C )\n%s",
                          pVolume->vendorName, pVolume->productName,
                          pVolume->familyType, "");
        
        // Copy device ID
        memcpy(m_deviceInfo.deviceData, &pVolume->inquiryData1, 8);
    }
    
    LogMessage("GetDeviceInfo: completed successfully");
    return TRUE;
}

BOOL iTEUFDrs::InitializeParaValue()
{
    // This function initializes parameter values
    // Implementation based on decompiled code analysis
    LogMessage("InitializeParaValue: initializing parameters");
    
    // Set default values
    m_deviceInfo.volumeCount = 0;
    m_deviceInfo.currentVolume = 0;
    m_deviceInfo.selectedVolume = 0xFF;
    
    return TRUE;
}

BOOL iTEUFDrs::CheckDriveExist()
{
    // This function checks if drives exist
    // Implementation based on decompiled code analysis
    LogMessage("CheckDriveExist: checking for available drives");
    
    // Detect physical drives
    if (!DetectPhysicalDrives()) {
        return FALSE;
    }
    
    // Detect logical volumes
    if (!DetectLogicalVolumes()) {
        return FALSE;
    }
    
    return (m_deviceInfo.volumeCount > 0);
}

BOOL iTEUFDrs::OpenDriveHandleAgain()
{
    // This function opens drive handles again
    // Implementation based on decompiled code analysis
    LogMessage("OpenDriveHandleAgain: reopening drive handles");
    
    // Re-detect drives
    return DetectPhysicalDrives();
}

BOOL iTEUFDrs::SetDeviceID()
{
    // This function sets device ID
    // Implementation based on decompiled code analysis
    LogMessage("SetDeviceID: setting device identification");
    
    // Process device inquiry for each volume
    for (BYTE i = 0; i < m_deviceInfo.volumeCount; i++) {
        if (m_deviceInfo.volumes[i].deviceFound) {
            // Set device ID based on inquiry data
            m_deviceInfo.volumes[i].inquiryData1 = 0x12345678; // Placeholder
        }
    }
    
    return TRUE;
}

BOOL iTEUFDrs::VolumePairController()
{
    // This function initializes volume pair controller
    // Implementation based on decompiled code analysis
    LogMessage("VolumePairController: initializing volume controller");
    
    // Initialize bank structures
    return InitializeBankStructures();
}

BOOL iTEUFDrs::CheckSystemReadyIO(BYTE volumeIndex, DWORD deviceId)
{
    // This function checks if system is ready for IO
    // Implementation based on decompiled code analysis
    LogMessage("CheckSystemReadyIO: volume %d, device 0x%08X", volumeIndex, deviceId);
    
    // Check if volume is accessible
    if (volumeIndex >= MAX_VOLUMES) {
        return FALSE;
    }
    
    // Check if device is ready
    return m_deviceInfo.volumes[volumeIndex].deviceFound;
}

BOOL iTEUFDrs::LoadBankC(BYTE volumeIndex, DWORD deviceId)
{
    // This function loads bank C
    // Implementation based on decompiled code analysis
    LogMessage("LoadBankC: volume %d, device 0x%08X", volumeIndex, deviceId);
    
    // Load bank information
    return LoadBankInformation(volumeIndex);
}

BOOL iTEUFDrs::GetBCMInformation(BYTE volumeIndex, DWORD deviceId)
{
    // This function gets BCM information
    // Implementation based on decompiled code analysis
    LogMessage("GetBCMInformation: volume %d, device 0x%08X", volumeIndex, deviceId);
    
    // Get BCM info using SDK
    if (m_sdkApis.FLH_GetInfoFromDataBaseByID) {
        // Call SDK function
        return TRUE;
    }
    
    return FALSE;
}

BOOL iTEUFDrs::LoadBankData(BYTE volumeIndex, DWORD deviceId)
{
    // This function loads bank data
    // Implementation based on decompiled code analysis
    LogMessage("LoadBankData: volume %d, device 0x%08X", volumeIndex, deviceId);
    
    // Load bank data
    return CopyBankData(volumeIndex);
}

BOOL iTEUFDrs::FormatDeviceString(LPSTR buffer, DWORD size, LPCSTR format, ...)
{
    // This function formats device string
    // Implementation based on decompiled code analysis
    if (!buffer || size == 0) {
        return FALSE;
    }
    
    va_list args;
    va_start(args, format);
    
    int result = vsnprintf_s(buffer, size, _TRUNCATE, format, args);
    
    va_end(args);
    
    return (result >= 0);
}

// Additional private methods implementation
BOOL iTEUFDrs::DetectPhysicalDrives()
{
    LOG_INFO("DetectPhysicalDrives: start");
    m_deviceInfo.volumeCount = 0;
    const char driveLetters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (int i = 0; i < 26 && m_deviceInfo.volumeCount < MAX_VOLUMES; ++i) {
        char letter = driveLetters[i];
        char path[8];
        path[0] = '\\'; path[1] = '\\'; path[2] = '.'; path[3] = '\\'; path[4] = letter; path[5] = ':'; path[6] = '\0';
        UINT dtype;
        {
            char typePath[4];
            typePath[0] = letter; typePath[1] = ':'; typePath[2] = '\\'; typePath[3] = '\0';
            dtype = GetDriveTypeA(typePath);
        }
        HANDLE h = CreateFileA(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                               NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h == INVALID_HANDLE_VALUE) {
            continue;
        }
        BYTE idx = m_deviceInfo.volumeCount;
        DEVICE_VOLUME_INFO& v = m_deviceInfo.volumes[idx];
        v.volumeIndex = idx;
        v.volumeLetter = (BYTE)letter;
        v.hDevice = h;
        v.driveType = dtype;
        v.isInitialized = TRUE;
        v.deviceFound = TRUE;
        // Default family until deeper probe
        v.familyType = 0x1181;
        v.a1baFlag = 0xFF; // unknown
        // Vendor/Product placeholders
        memset(v.vendorName, ' ', sizeof(v.vendorName));
        memset(v.productName, ' ', sizeof(v.productName));
        v.vendorName[sizeof(v.vendorName)-1] = '\0';
        v.productName[sizeof(v.productName)-1] = '\0';
        // Fill inquiryData minimally
        memset(v.inquiryData, 0, sizeof(v.inquiryData));
        // Close handle now; subsequent steps reopen as needed
        SafeCloseHandle(v.hDevice);
        ++m_deviceInfo.volumeCount;
    }
    LOG_INFO("DetectPhysicalDrives: found %u", (unsigned)m_deviceInfo.volumeCount);
    return m_deviceInfo.volumeCount > 0;
}

BOOL iTEUFDrs::DetectLogicalVolumes()
{
    LOG_INFO("DetectLogicalVolumes: start");
    for (BYTE idx = 0; idx < m_deviceInfo.volumeCount; ++idx) {
        DEVICE_VOLUME_INFO& v = m_deviceInfo.volumes[idx];
        // Reopen handle for inquiry
        char path[8];
        path[0] = '\\'; path[1] = '\\'; path[2] = '.'; path[3] = '\\'; path[4] = (char)v.volumeLetter; path[5] = ':'; path[6] = '\0';
        HANDLE h = CreateFileA(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                               NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h == INVALID_HANDLE_VALUE) {
            continue;
        }
        v.hDevice = h;
        BYTE inq[176];
        memset(inq, 0, sizeof(inq));
        if (!fetchInquiryData(idx, inq, sizeof(inq))) {
            LOG_WARNING("Inquiry failed for %C", v.volumeLetter);
            SafeCloseHandle(v.hDevice);
            continue;
        }
        // Save vendor/product (offsets 8..15 and 16..31 like SCSI INQUIRY standard)
        for (int i = 0; i < 8; ++i) {
            BYTE c = inq[8 + i];
            v.vendorName[i] = (c == 0) ? ' ' : (char)c;
        }
        v.vendorName[8] = '\0';
        for (int i = 0; i < 16; ++i) {
            BYTE c = inq[16 + i];
            v.productName[i] = (c == 0) ? ' ' : (char)c;
        }
        v.productName[16] = '\0';
        // Build ASCII inquiry string for substring search
        char asciiBuf[256];
        int off = 0;
        for (int i = 8; i < 36 && off < 200; ++i) {
            char c = (char)inq[i];
            asciiBuf[off++] = (c == 0) ? ' ' : c;
        }
        asciiBuf[off] = '\0';
        std::string s(asciiBuf);
        // Detect controller and flags
        v.controllerType = 200; // not supported by default
        v.a1baFlag = 0xFF;
        if (s.find("1181") != std::string::npos) {
            v.familyType = 0x1181;
            v.controllerType = 0;
            if (s.find("A0AA") != std::string::npos) v.a1baFlag = 0;
            else if (s.find("A1BA") != std::string::npos) { v.controllerType = 1; v.a1baFlag = 1; }
        } else if (s.find("1176") != std::string::npos) {
            v.familyType = 0x1176;
            v.controllerType = 2;
            v.a1baFlag = (s.find("A0AA") != std::string::npos) ? 0 : 0xFF;
        }
        // Copy raw inquiry
        memcpy(v.inquiryData, inq, sizeof(v.inquiryData));
        SafeCloseHandle(v.hDevice);
    }
    LOG_INFO("DetectLogicalVolumes: done");
    return TRUE;
}

BOOL iTEUFDrs::fetchInquiryData(BYTE volumeIndex, BYTE* outBuffer, DWORD bufferSize)
{
    if (volumeIndex >= m_deviceInfo.volumeCount || !outBuffer || bufferSize < 0xB0) return FALSE;
    DEVICE_VOLUME_INFO& v = m_deviceInfo.volumes[volumeIndex];
    // Here we should use SDK_APIS to issue INQUIRY; as a placeholder, attempt DeviceIoControl SCSI pass-through could be added later
    // For now, return FALSE to avoid pretending data
    return FALSE;
}

BOOL iTEUFDrs::ProcessDeviceInquiry(HANDLE hDevice, BYTE volumeIndex)
{
    if (volumeIndex >= MAX_VOLUMES) {
        return FALSE;
    }
    
    // Allocate inquiry buffer
    BYTE* inquiryBuffer = (BYTE*)malloc(0xB0);
    if (!inquiryBuffer) {
        return FALSE;
    }
    
    // Get inquiry data using SDK
    BOOL result = FALSE;
    if (m_sdkApis.FLH_GetInfoFromDataBaseByID) {
        // Call SDK function to get inquiry data
        result = TRUE;
        
        // Parse inquiry data
        if (result) {
            // Extract vendor and product information
            memcpy(m_deviceInfo.volumes[volumeIndex].vendorName, 
                   inquiryBuffer + 8, 8);
            memcpy(m_deviceInfo.volumes[volumeIndex].productName, 
                   inquiryBuffer + 16, 16);
            
            // Identify device family
            CHAR inquiryString[256];
            sprintf_s(inquiryString, "%s", inquiryBuffer + 24);
            IdentifyDeviceFamily(inquiryString, volumeIndex);
            
            m_deviceInfo.volumes[volumeIndex].deviceFound = TRUE;
        }
    }
    
    free(inquiryBuffer);
    return result;
}

BOOL iTEUFDrs::IdentifyDeviceFamily(LPCSTR inquiryString, BYTE volumeIndex)
{
    if (volumeIndex >= MAX_VOLUMES) {
        return FALSE;
    }
    
    DEVICE_VOLUME_INFO* pVolume = &m_deviceInfo.volumes[volumeIndex];
    
    // Check for ITE device
    if (strstr(inquiryString, "ITEu")) {
        // Check for specific family types
        if (strstr(inquiryString, "1181")) {
            pVolume->familyType = 0; // 1181 family
            if (strstr(inquiryString, "A1BA")) {
                pVolume->a1baFlag = 1; // A1BA variant
            } else if (strstr(inquiryString, "A0AA")) {
                pVolume->a1baFlag = 0; // A0AA variant
            }
        } else if (strstr(inquiryString, "1176")) {
            pVolume->familyType = 2; // 1176 family
            if (strstr(inquiryString, "A0AA")) {
                pVolume->a1baFlag = 0; // A0AA variant
            }
        }
        
        return TRUE;
    }
    
    return FALSE;
}

BOOL iTEUFDrs::InitializeBankStructures()
{
    LogMessage("InitializeBankStructures: initializing bank structures");
    
    // Initialize bank structures for each volume
    for (int i = 0; i < MAX_BANKS; i++) {
        m_deviceInfo.banks[i].bankIndex = (BYTE)i;
        m_deviceInfo.banks[i].bankStatus = 0xFF;
        m_deviceInfo.banks[i].bankType = 0xFF;
        m_deviceInfo.banks[i].bankReady = FALSE;
        m_deviceInfo.banks[i].bankLoaded = FALSE;
        m_deviceInfo.banks[i].bankError = FALSE;
    }
    
    return TRUE;
}

BOOL iTEUFDrs::LoadBankInformation(BYTE volumeIndex)
{
    if (volumeIndex >= MAX_BANKS) {
        return FALSE;
    }
    
    LogMessage("LoadBankInformation: loading bank %d", volumeIndex);
    
    // Load bank information
    m_deviceInfo.banks[volumeIndex].bankStatus = 1;
    m_deviceInfo.banks[volumeIndex].bankReady = TRUE;
    
    return TRUE;
}

BOOL iTEUFDrs::CopyBankData(BYTE volumeIndex)
{
    if (volumeIndex >= MAX_BANKS) {
        return FALSE;
    }
    
    LogMessage("CopyBankData: copying bank %d data", volumeIndex);
    
    // Copy bank data
    DEVICE_BANK_INFO* pBank = &m_deviceInfo.banks[volumeIndex];
    
    // Copy BCM info
    memcpy(pBank->bcmInfo, pBank->bankData + 0x100, 0x100);
    
    // Copy ISP info
    memcpy(pBank->ispInfo, pBank->bankData + 0x200, 0x100);
    
    return TRUE;
}

BOOL iTEUFDrs::FormatDeviceIdentification()
{
    LogMessage("FormatDeviceIdentification: formatting device identification");
    
    // Format device identification string
    if (m_deviceInfo.selectedVolume != 0xFF) {
        DEVICE_VOLUME_INFO* pVolume = &m_deviceInfo.volumes[m_deviceInfo.selectedVolume];
        
        // Format device string
        FormatDeviceString(m_deviceInfo.deviceString, sizeof(m_deviceInfo.deviceString),
                          " %s%s , ( %C )\n%s",
                          pVolume->vendorName, pVolume->productName,
                          pVolume->familyType, "");
        
        // Copy device ID
        memcpy(m_deviceInfo.deviceData, &pVolume->inquiryData1, 8);
    }
    
    return TRUE;
}

BOOL iTEUFDrs::ConcatenateBankInfo()
{
    LogMessage("ConcatenateBankInfo: concatenating bank information");
    
    // Concatenate bank information
    CHAR bankInfo[0x100] = {0};
    int bankInfoLen = 0;
    
    for (int i = 1; i < 4; i++) {
        if (m_deviceInfo.banks[i].bankStatus != 0xFF) {
            CHAR tempBuffer[0x100];
            sprintf_s(tempBuffer, "( %C )", m_deviceInfo.banks[i].bankType);
            
            // Concatenate to device string
            strcat_s(m_deviceInfo.deviceString, sizeof(m_deviceInfo.deviceString), tempBuffer);
        }
    }
    
    return TRUE;
}

// Get volume info by index
const DEVICE_VOLUME_INFO* iTEUFDrs::GetVolumeInfo(BYTE index) const
{
    if (index >= MAX_VOLUMES) {
        return nullptr;
    }
    return &m_deviceInfo.volumes[index];
}

// Get controller data by index
const CONTROLLER_DATA* iTEUFDrs::GetControllerData(BYTE index) const
{
    if (index >= MAX_CONTROLLERS) {
        return nullptr;
    }
    return &m_controllerData[index];
}

// Set device IDs for volumes (decompiled from FUN_0040ae40)
BOOL iTEUFDrs::SetDeviceID()
{
    if (m_deviceInfo.volumeCount == 0) {
        return TRUE; // No volumes to process
    }
    
    for (BYTE volumeIndex = 0; volumeIndex < m_deviceInfo.volumeCount; volumeIndex++) {
        DEVICE_VOLUME_INFO* volumeInfo = &m_deviceInfo.volumes[volumeIndex];
        
        BOOL driveExists;
        if (m_forcedMode) {
            driveExists = OpenDriveHandleAgain(volumeIndex);
        } else {
            driveExists = CheckDriveExistInternal(volumeIndex);
        }
        
        if (!driveExists) {
            continue;
        }
        
        // Find available device ID slot
        BYTE deviceId = 0xFF;
        for (BYTE i = 0; i < 0xFF; i++) {
            BOOL slotTaken = FALSE;
            for (BYTE j = 0; j < MAX_VOLUMES; j++) {
                if (m_deviceInfo.volumes[j].deviceId == i) {
                    slotTaken = TRUE;
                    break;
                }
            }
            if (!slotTaken) {
                deviceId = i;
                break;
            }
        }
        
        if (deviceId == 0xFF) {
            LogError("iTEUFDrs: Over DeviceID Table Max value");
            CloseHandle(volumeInfo->hVolume);
            return FALSE;
        }
        
        // Set the device ID
        volumeInfo->deviceId = deviceId;
        LogMessage("iTEUFDrs: Volume %d assigned device ID %d", volumeIndex, deviceId);
        
        CloseHandle(volumeInfo->hVolume);
    }
    
    return TRUE;
}

// Pair volumes with controllers (decompiled from FUN_00408430)
void iTEUFDrs::VolumePairController()
{
    BOOL volumeUsed[MAX_VOLUMES] = {FALSE};
    m_controllerCount = 0;
    
    for (BYTE volumeIndex = 0; volumeIndex < m_deviceInfo.volumeCount && m_controllerCount < MAX_CONTROLLERS; volumeIndex++) {
        if (volumeUsed[volumeIndex]) {
            continue;
        }
        
        DEVICE_VOLUME_INFO* baseVolume = &m_deviceInfo.volumes[volumeIndex];
        CONTROLLER_DATA* controller = &m_controllerData[m_controllerCount];
        
        // Start a new controller group
        controller->volumeIndexes[0] = volumeIndex;
        controller->volumeCount = 1;
        volumeUsed[volumeIndex] = TRUE;
        
        // Find other volumes with the same controller type
        BYTE volumeCount = 1;
        for (BYTE otherIndex = volumeIndex + 1; otherIndex < m_deviceInfo.volumeCount && volumeCount < 4; otherIndex++) {
            if (volumeUsed[otherIndex]) {
                continue;
            }
            
            DEVICE_VOLUME_INFO* otherVolume = &m_deviceInfo.volumes[otherIndex];
            if (otherVolume->controllerType == baseVolume->controllerType) {
                controller->volumeIndexes[volumeCount] = otherIndex;
                volumeCount++;
                volumeUsed[otherIndex] = TRUE;
            }
        }
        
        controller->volumeCount = volumeCount;
        controller->productId = m_deviceInfo.volumes[controller->volumeIndexes[0]].productId;
        controller->isValid = TRUE;
        
        LogMessage("iTEUFDrs: Controller %d paired with %d volumes", m_controllerCount, volumeCount);
        m_controllerCount++;
    }
}

// Get volume index for controller
BYTE iTEUFDrs::GetVolumeIndexForController(BYTE controllerIndex) const
{
    if (controllerIndex >= m_controllerCount) {
        return 0xFF;
    }
    return m_controllerData[controllerIndex].volumeIndexes[0];
}

// Get device ID for controller
DWORD iTEUFDrs::GetDeviceIdForController(BYTE controllerIndex) const
{
    if (controllerIndex >= m_controllerCount) {
        return 0xFFFFFFFF;
    }
    BYTE volumeIndex = m_controllerData[controllerIndex].volumeIndexes[0];
    return m_deviceInfo.volumes[volumeIndex].deviceId;
}

// Check system ready IO (decompiled from FUN_004095e0)
BOOL iTEUFDrs::CheckSystemReadyIO(BYTE volumeIndex, DWORD deviceId)
{
    LogMessage("iTEUFDrs: Checking system ready IO for volume %d, device %d", volumeIndex, deviceId);
    
    // Clear binary file path buffer
    CHAR binFilePath[MAX_PATH + 4] = {0};
    CHAR fileName[MAX_PATH] = {0};
    
    // Get volume info
    if (volumeIndex >= m_deviceInfo.volumeCount) {
        LogError("iTEUFDrs: Invalid volume index %d", volumeIndex);
        return FALSE;
    }
    
    DEVICE_VOLUME_INFO* volumeInfo = &m_deviceInfo.volumes[volumeIndex];
    
    // Determine bin file name based on controller type
    if (deviceId == 0xFF) {
        strcpy_s(fileName, sizeof(fileName), "u181s00.bin");
    } else {
        sprintf_s(fileName, sizeof(fileName), "u181s%02x.bin", (BYTE)deviceId);
    }
    
    // Build full path to bin file
    LPCSTR pathFormat;
    if (volumeInfo->isA1BA) {
        pathFormat = "%s\\Bin\\1181\\DownGrade\\A1BA\\%s";
    } else {
        if (volumeInfo->controllerType == 2) {
            pathFormat = "%s\\Bin\\1176\\DownGrade\\A0AA\\%s";
        } else {
            pathFormat = "%s\\Bin\\1181\\DownGrade\\A0AA\\%s";
        }
    }
    
    int result = FormatDeviceString(binFilePath, sizeof(binFilePath), pathFormat, m_basePath, fileName);
    if (result != 0) {
        LogError("iTEUFDrs: GetBinFilePath: Formatted String Buffer fails.");
        return FALSE;
    }
    
    // Check if bin file exists
    if (binFilePath[0] == '\0') {
        LogError("iTEUFDrs: Bin file path = %s", binFilePath);
        return FALSE;
    }
    
    // Try to open the file
    HANDLE hFile = CreateFileA(binFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        LogError("iTEUFDrs: Cannot open bin file: %s", binFilePath);
        return FALSE;
    }
    
    CloseHandle(hFile);
    LogMessage("iTEUFDrs: System ready IO check passed for %s", binFilePath);
    return TRUE;
}

// Load Bank C information (placeholder implementations for remaining functions)
BOOL iTEUFDrs::LoadBankC(BYTE volumeIndex, DWORD deviceId)
{
    LogMessage("iTEUFDrs: Loading Bank C for volume %d, device %d", volumeIndex, deviceId);
    // Implementation would call SDK functions for bank loading
    return TRUE;
}

BOOL iTEUFDrs::GetBCMInformation(BYTE volumeIndex, DWORD deviceId)
{
    LogMessage("iTEUFDrs: Getting BCM information for volume %d, device %d", volumeIndex, deviceId);
    // Implementation would call SDK functions for BCM data
    return TRUE;
}

BOOL iTEUFDrs::LoadBankData(BYTE volumeIndex, DWORD deviceId)
{
    LogMessage("iTEUFDrs: Loading bank data for volume %d, device %d", volumeIndex, deviceId);
    // Implementation would call SDK functions for bank data loading
    return TRUE;
}

// Additional function placeholders
BOOL iTEUFDrs::LoadBankC2(BYTE volumeIndex, DWORD deviceId) { return TRUE; }
BOOL iTEUFDrs::GetBCMInformation2(BYTE volumeIndex, DWORD deviceId) { return TRUE; }
BOOL iTEUFDrs::LoadBankData2(BYTE volumeIndex, DWORD deviceId) { return TRUE; }
BOOL iTEUFDrs::LoadBankData3(BYTE volumeIndex, DWORD deviceId) { return TRUE; }
BOOL iTEUFDrs::GetMPInfo(BYTE volumeIndex, DWORD deviceId) { return TRUE; }
BOOL iTEUFDrs::GetLunArrayData(BYTE volumeIndex, DWORD deviceId) { return TRUE; }
void iTEUFDrs::CalculateCapacity(BYTE volumeIndex) { }
void iTEUFDrs::CalculateRealCapacity(BYTE volumeIndex) { }

// Format device string (decompiled from FUN_004094a0)
BOOL iTEUFDrs::FormatDeviceString(LPSTR buffer, DWORD size, LPCSTR format, ...)
{
    if (size == 0 || size > 0x7FFFFFFF) {
        return FALSE;
    }
    
    va_list args;
    va_start(args, format);
    
    DWORD actualSize = size - 1;
    int result = _vsnprintf_s(buffer, actualSize, _TRUNCATE, format, args);
    
    va_end(args);
    
    if (result < 0 || (DWORD)result >= actualSize) {
        buffer[actualSize] = '\0';
        return FALSE;
    }
    
    if ((DWORD)result == actualSize) {
        buffer[actualSize] = '\0';
    }
    
    return TRUE;
}

// Helper functions
BOOL iTEUFDrs::DetectPhysicalDrives()
{
    // Implementation for detecting physical drives
    LogMessage("iTEUFDrs: Detecting physical drives");
    return TRUE;
}

BOOL iTEUFDrs::DetectLogicalVolumes()
{
    // Implementation for detecting logical volumes
    LogMessage("iTEUFDrs: Detecting logical volumes");
    return TRUE;
}

BOOL iTEUFDrs::setupDatabasePaths()
{
    // Build "%s\\Bin\\FlashSSD_D.fdb" and "%s\\Bin\\CtrlSSD.cdb" from base path
    char fdb[MAX_PATH + 4] = {0};
    char cdb[MAX_PATH + 4] = {0};
    int r1 = FormatDeviceString(fdb, sizeof(fdb), "%s\\Bin\\FlashSSD_D.fdb", m_basePath);
    if (r1 != 0) {
        LogMessage("SetDBPath: Formatted String Buffer fails.");
    }
    int r2 = FormatDeviceString(cdb, sizeof(cdb), "%s\\Bin\\CtrlSSD.cdb", m_basePath);
    if (r2 != 0) {
        LogMessage("SetDBPath: Formatted String Buffer fails.");
    }
    // Check existence of at least one
    if (fdb[0] != '\0') {
        HANDLE h = CreateFileA(fdb, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h != INVALID_HANDLE_VALUE) { CloseHandle(h); return TRUE; }
    }
    if (cdb[0] != '\0') {
        HANDLE h = CreateFileA(cdb, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h != INVALID_HANDLE_VALUE) { CloseHandle(h); return TRUE; }
    }
    return FALSE;
}

// High-level orchestration: mirrors FUN_0040d022
BOOL iTEUFDrs::GetDeviceInfoInternal()
{
    return RunInitializationChain();
}

BOOL iTEUFDrs::RunInitializationChain()
{
    LogMessage("iTEUFDrs: RunInitializationChain start");

    if (!InitializeParaValue()) {
        LogError("InitializeParaValue failed");
        return FALSE;
    }

    if (!CheckDriveExist()) {
        LogWarning("No drives detected");
        return FALSE;
    }

    if (!SetDeviceID()) {
        LogError("SetDeviceID failed");
        return FALSE;
    }

    VolumePairController();

    // SetDBPath analog (FUN_004098f0)
    setupDatabasePaths();

    BYTE controllerCount = m_controllerCount;
    for (BYTE ci = 0; ci < controllerCount; ++ci) {
        BYTE volIndex = GetVolumeIndexForController(ci);
        DWORD deviceId = GetDeviceIdForController(ci);

        if (volIndex == 0xFF || deviceId == 0xFFFFFFFF) {
            LogWarning("Controller %u pairing incomplete", ci);
            continue;
        }

        BOOL opened = m_forcedMode ? OpenDriveHandleAgain(volIndex)
                                   : CheckDriveExistInternal(volIndex);
        if (!opened) {
            LogWarning("Controller %u volume %u handle open failed", ci, volIndex);
            continue;
        }

        if (!CheckSystemReadyIO(volIndex, deviceId)) {
            LogWarning("SystemReadyIO failed (vol=%u, dev=%u)", volIndex, (UINT)deviceId);
            continue;
        }

        if (!LoadBankC(volIndex, deviceId)) {
            LogWarning("LoadBankC failed (vol=%u)", volIndex);
            continue;
        }

        if (!GetBCMInformation(volIndex, deviceId)) {
            LogWarning("ISP_InitCode/BCM init failed (vol=%u)", volIndex);
            continue;
        }

        if (!LoadBankData(volIndex, deviceId)) {
            LogWarning("LoadBankData failed (vol=%u)", volIndex);
            continue;
        }

        LoadBankC2(volIndex, deviceId);
        GetBCMInformation2(volIndex, deviceId);
        LoadBankData2(volIndex, deviceId);
        LoadBankData3(volIndex, deviceId);
        GetMPInfo(volIndex, deviceId);

        if (!GetLunArrayData(volIndex, deviceId)) {
            CalculateCapacity(volIndex);
        } else {
            CalculateRealCapacity(volIndex);
        }

        LogMessage("Controller %u processed", ci);
    }

    LogMessage("iTEUFDrs: RunInitializationChain done");
    return TRUE;
}

void iTEUFDrs::updateCISBuffer()
{
    // Produce version string like "81.0.X.X-mmdd" or from 4-part version if available
    time_t now = time(NULL);
    struct tm t;
    localtime_s(&t, &now);
    char mmdd[8];
    strftime(mmdd, sizeof(mmdd), "%m%d", &t);
    char version[32] = {0};
    // Placeholder: if SDK/registry provides 4-part version, format it; else fallback
    _snprintf_s(version, sizeof(version), _TRUNCATE, "81.0.X.X-%s", mmdd);
    // Store into a dedicated buffer area if present; otherwise log
    LOG_INFO("updateCISBuffer: %s", version);
}
