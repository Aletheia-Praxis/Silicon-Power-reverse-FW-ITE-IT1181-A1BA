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
    BOOL ProcessDeviceInquiry(HANDLE hDevice, BYTE volumeIndex);
    BOOL IdentifyDeviceFamily(LPCSTR inquiryString, BYTE volumeIndex);
    BOOL setupDatabasePaths(); // Added for FUN_004098f0
    BOOL scanMassBlocks(BYTE mode); // Added for FUN_0040c640
    BOOL fetchInquiryData(HANDLE hDevice, BYTE volumeIndex, LPVOID inquiryBuffer); // Added for STD_Inquiry
    BOOL readCapacity(BYTE volumeIndex, LPDWORD lpCapacity); // Added for STD_ReadCapacity
    BOOL logicalRead(BYTE volumeIndex, LPVOID lpBuffer, DWORD lba, DWORD sectors); // Added for STD_LogicalRead
    BOOL GetBinFilePath(BYTE volumeIndex, LPCSTR fileName, LPSTR filePath, DWORD pathSize);

    BOOL FormatDeviceString(LPSTR buffer, DWORD size, LPCSTR format, ...);

    void updateCISBuffer();

    // New functions based on decompilation analysis
    BOOL NotifyFwSegmentInfo(BYTE volumeIndex, DWORD deviceId);
    BOOL ScanMassBlocks(BYTE volumeIndex, DWORD deviceId, BYTE mode);
    BOOL IsCEChannelEnabled(BYTE volumeIndex, BYTE ce, BYTE ch);
    void UpdateDeviceStatusFromScan(BYTE volumeIndex, BYTE ce, BYTE ch, BYTE* scanData);
    void ProcessBadBlocks(BYTE volumeIndex, BYTE ce, BYTE ch, DWORD deviceId, BYTE mode);
    
    // Additional functions from decompilation analysis
    BOOL GetDeviceInfoMain(BYTE volumeIndex, DWORD deviceId);
    BOOL GetFlashMethod(BYTE volumeIndex, DWORD deviceId);
    BOOL CheckNeedLoadBank(BYTE volumeIndex, DWORD deviceId);
    void UpdateDeviceParameters(DWORD deviceId);
    void UpdateDeviceStatus(DWORD deviceId, BYTE volumeLetter);
    BOOL LoadBankData(BYTE volumeIndex, DWORD deviceId);
    BOOL LoadBankData2(BYTE volumeIndex, DWORD deviceId);
    BOOL LoadBankData3(BYTE volumeIndex, DWORD deviceId);
    BOOL GetBinFileVersion(BYTE volumeIndex, DWORD deviceId);

    // SDK wrappers (names inferred from usage in 0x0040c640)
    BOOL flhScanMassBlocksPerChip(DWORD ctx, BYTE ce, BYTE ch, int rtPtr, void* outBuf, BYTE mode, BYTE* outFlag, int* outRet);
    BOOL flhGetScanResult(DWORD ctx, BYTE ce, BYTE ch, int rtPtr, void* outBuf);
    BOOL flhReadSpare(BYTE ch, BYTE ce, int rtPtr, void* spare, DWORD spareSize, DWORD ctx);
    BOOL flhBlockErase(DWORD ctx, DWORD handle, int rtPtr);
    void flhCPUReset(int rtPtr, DWORD handle, DWORD ctx);
};

// Error codes for iTEUFDrs
#define ITEUFDRS_ERROR_NONE          0
#define ITEUFDRS_ERROR_SDK_LOAD      1  // Failed to load 181FlashSDK.dll
#define ITEUFDRS_ERROR_API_BIND      2  // Failed to bind API functions 
#define ITEUFDRS_ERROR_DEVICE_INFO   3  // Failed to get device information
