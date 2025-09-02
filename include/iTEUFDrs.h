#pragma once
#include <windows.h>
#include "SDKAPIs.h"
#include "DeviceStructures.h"

// Maximum number of volumes and controllers
#define MAX_VOLUMES 8
#define MAX_CONTROLLERS 3

// Controller data structure
typedef struct _CONTROLLER_DATA {
    BOOL isValid;
    DWORD deviceId;
    DWORD lunId;
    DWORD targetId;
    DWORD pathId;
    DWORD busId;
    DWORD scsiId;
    DWORD reserved1;
    DWORD reserved2;
    BYTE inquiryData[64];
    // Additional controller-specific data
    BYTE volumeIndexes[4];
    BYTE volumeCount;
    WORD productId;
    BYTE controllerType;
    BOOL isReady;
    BOOL bcmAvailable;
    BOOL bankDataLoaded;
} CONTROLLER_DATA, *PCONTROLLER_DATA;

// iTEUFDrs class (decompiled from FUN_0040d690)
class iTEUFDrs
{
public:
    iTEUFDrs(LPCSTR basePath);
    ~iTEUFDrs();

    BOOL IsInitialized() const { return m_isInitialized; }
    DWORD GetLastError() const { return m_lastError; }
    
    // Device information access
    BYTE GetVolumeCount() const { return m_volumeCount; }
    BYTE GetControllerCount() const { return m_controllerCount; }
    const DEVICE_VOLUME_INFO* GetVolumeInfo(BYTE index) const;
    const CONTROLLER_DATA* GetControllerData(BYTE index) const;

private:
    // Members based on decompiled structure
    void* m_vtable; // 0x00
    BOOL m_isInitialized; // 0x04 (param_1 + 5)
    DWORD m_lastError; // 0x08 (param_1 + 1)
    CHAR m_basePath[MAX_PATH]; // 0x11b
    HMODULE m_hSDK; // 0x21f
    SDK_APIS m_sdkApis; // Structure to hold all bound SDK function pointers
    
    // Device management data
    BYTE m_volumeCount;
    BYTE m_controllerCount;
    BOOL m_forcedMode;
    DEVICE_VOLUME_INFO m_deviceVolumeInfo[MAX_VOLUMES];
    CONTROLLER_DATA m_controllerData[MAX_CONTROLLERS];

    // Internal initialization functions
    BOOL InitializeSDK();
    void InitializeMembers();
    
    // Device information functions (decompiled from various FUN_* functions)
    BOOL GetDeviceInfoInternal();
    BOOL InitializeParaValue();
    BYTE CheckDriveExist();
    BOOL CheckDriveExistInternal(BYTE volumeIndex);
    BOOL OpenDriveHandleAgain(BYTE volumeIndex);
    BOOL SetDeviceID();
    void VolumePairController();
    
    // Controller processing functions
    BYTE GetControllerCount() const { return m_controllerCount; }
    BYTE GetVolumeIndexForController(BYTE controllerIndex) const;
    DWORD GetDeviceIdForController(BYTE controllerIndex) const;
    
    // Device operation functions
    BOOL CheckSystemReadyIO(BYTE volumeIndex, DWORD deviceId);
    BOOL LoadBankC(BYTE volumeIndex, DWORD deviceId);
    BOOL GetBCMInformation(BYTE volumeIndex, DWORD deviceId);
    BOOL LoadBankData(BYTE volumeIndex, DWORD deviceId);
    
    // Additional device functions
    BOOL LoadBankC2(BYTE volumeIndex, DWORD deviceId);
    BOOL GetBCMInformation2(BYTE volumeIndex, DWORD deviceId);
    BOOL LoadBankData2(BYTE volumeIndex, DWORD deviceId);
    BOOL LoadBankData3(BYTE volumeIndex, DWORD deviceId);
    BOOL GetMPInfo(BYTE volumeIndex, DWORD deviceId);
    BOOL GetLunArrayData(BYTE volumeIndex, DWORD deviceId);
    void CalculateCapacity(BYTE volumeIndex);
    void CalculateRealCapacity(BYTE volumeIndex);
    
    // Helper functions
    BOOL DetectPhysicalDrives();
    BOOL DetectLogicalVolumes();
    BOOL FormatDeviceString(LPSTR buffer, DWORD size, LPCSTR format, ...);
};

// Error codes for iTEUFDrs
#define ITEUFDRS_ERROR_NONE          0
#define ITEUFDRS_ERROR_SDK_LOAD      1  // Failed to load 181FlashSDK.dll
#define ITEUFDRS_ERROR_API_BIND      2  // Failed to bind API functions 
#define ITEUFDRS_ERROR_DEVICE_INFO   3  // Failed to get device information
