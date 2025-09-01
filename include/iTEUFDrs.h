#pragma once
#include <windows.h>
#include "SDKAPIs.h"
#include "DeviceStructures.h"

// Forward declarations
BOOL InitializeParaValue();
BOOL CheckDriveExist();
BOOL OpenDriveHandleAgain();
BOOL SetDeviceID();
BOOL VolumePairController();
BOOL CheckSystemReadyIO(BYTE volumeIndex, DWORD deviceId);
BOOL LoadBankC(BYTE volumeIndex, DWORD deviceId);
BOOL GetBCMInformation(BYTE volumeIndex, DWORD deviceId);
BOOL LoadBankData(BYTE volumeIndex, DWORD deviceId);
BOOL FormatDeviceString(LPSTR buffer, DWORD size, LPCSTR format, ...);

// iTEUFDrs class (decompiled from FUN_0040d690)
class iTEUFDrs
{
public:
    iTEUFDrs(LPCSTR basePath);
    ~iTEUFDrs();

    BOOL IsInitialized() const { return m_isInitialized; }
    DWORD GetLastError() const { return m_lastError; }
    
    // Device information methods
    BOOL GetDeviceInfo();
    BOOL InitializeParaValue();
    BOOL CheckDriveExist();
    BOOL OpenDriveHandleAgain();
    BOOL SetDeviceID();
    BOOL VolumePairController();
    
    // Device access methods
    BOOL CheckSystemReadyIO(BYTE volumeIndex, DWORD deviceId);
    BOOL LoadBankC(BYTE volumeIndex, DWORD deviceId);
    BOOL GetBCMInformation(BYTE volumeIndex, DWORD deviceId);
    BOOL LoadBankData(BYTE volumeIndex, DWORD deviceId);
    
    // Utility methods
    BOOL FormatDeviceString(LPSTR buffer, DWORD size, LPCSTR format, ...);

private:
    // Members based on decompiled structure
    void* m_vtable; // 0x00
    BOOL m_isInitialized; // 0x04
    DWORD m_lastError; // 0x08
    CHAR m_basePath[MAX_PATH]; // 0x0C
    HMODULE m_hSDK; // 0x110
    SDK_APIS m_sdkApis; // Structure to hold all bound SDK function pointers
    
    // Device information structure
    DEVICE_INFO m_deviceInfo;

    // Internal initialization functions
    BOOL InitializeSDK();
    void InitializeMembers();
    BOOL InitializeDeviceStructures();
    
    // Device detection functions
    BOOL DetectPhysicalDrives();
    BOOL DetectLogicalVolumes();
    BOOL ProcessDeviceInquiry(HANDLE hDevice, BYTE volumeIndex);
    BOOL IdentifyDeviceFamily(LPCSTR inquiryString, BYTE volumeIndex);
    
    // Bank management functions
    BOOL InitializeBankStructures();
    BOOL LoadBankInformation(BYTE volumeIndex);
    BOOL CopyBankData(BYTE volumeIndex);
    
    // String formatting functions
    BOOL FormatDeviceIdentification();
    BOOL ConcatenateBankInfo();
};

// Error codes for iTEUFDrs
#define ITEUFDRS_ERROR_NONE          0
#define ITEUFDRS_ERROR_SDK_LOAD      1  // Failed to load 181FlashSDK.dll
#define ITEUFDRS_ERROR_API_BIND      2  // Failed to bind API functions 
#define ITEUFDRS_ERROR_DEVICE_INFO   3  // Failed to get device information
