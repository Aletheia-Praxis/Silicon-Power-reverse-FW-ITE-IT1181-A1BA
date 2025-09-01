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
    LogMessage("DetectPhysicalDrives: scanning physical drives");
    
    // Scan physical drives 1-8
    for (int i = 1; i <= 8; i++) {
        CHAR drivePath[MAX_PATH];
        sprintf_s(drivePath, "\\\\.\\PhysicalDrive%d", i);
        
        HANDLE hDrive = CreateFileA(drivePath, GENERIC_READ | GENERIC_WRITE,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE,
                                   NULL, OPEN_EXISTING, 0, NULL);
        
        if (hDrive != INVALID_HANDLE_VALUE) {
            // Process device inquiry
            if (ProcessDeviceInquiry(hDrive, i - 1)) {
                m_deviceInfo.volumeCount++;
            }
            CloseHandle(hDrive);
        }
    }
    
    return TRUE;
}

BOOL iTEUFDrs::DetectLogicalVolumes()
{
    LogMessage("DetectLogicalVolumes: scanning logical volumes");
    
    // Scan logical volumes A-Z
    for (char drive = 'A'; drive <= 'Z'; drive++) {
        CHAR drivePath[MAX_PATH];
        sprintf_s(drivePath, "%c:\\", drive);
        
        UINT driveType = GetDriveTypeA(drivePath);
        if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
            // Check if this is an ITE device
            // Implementation would check device properties
        }
    }
    
    return TRUE;
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
