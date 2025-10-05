#pragma once
#include "CryptoManager.h"
#include "DeviceStructures.h"
#include "SDKAPIs.h"
#include "Utilities.h"
#include "WindowsHeaders.h"

// Maximum number of volumes and controllers
#define MAX_VOLUMES               8
#define MAX_CONTROLLERS           3
#define MAX_DEVICE_COUNT_ENHANCED 256

// iTEUFDrs class (represents the main application logic)
class iTEUFDrs {
public:
    iTEUFDrs(LPCSTR basePath);
    ~iTEUFDrs();

    BOOL IsInitialized() const { return m_isInitialized; }
    DWORD GetLastError() const { return m_lastError; }
    const DEVICE_INFO& GetDeviceInfo() const { return m_deviceInfo; }

private:
    // VTable (placeholder)
    void* m_vtable;

    // Member variables
    BOOL m_isInitialized;
    DWORD m_lastError;
    CHAR m_basePath[MAX_PATH];
    DEVICE_INFO m_deviceInfo;
    CONTROLLER_DATA m_controllerData[MAX_CONTROLLERS];
    BYTE m_controllerCount;
    BYTE m_volumeCount;
    BYTE m_bcmBuffer[0xE40];    // Buffer for BCM (Bad Block Management) information
    BYTE m_deviceIDTable[255];  // Table to track used device IDs

    // SDK handles and functions
    HMODULE m_hSDK;

    // Internal initialization functions
    BOOL InitializeSDK();
    BOOL LoadSDKFunctions(HMODULE hSDK);
    BOOL VerifySDKIntegrity();
    void InitializeMembers();
    BOOL InitializeDeviceStructures();

    // Device information functions (decompiled from various FUN_* functions)
    BOOL GetDeviceInfoInternal();
    BOOL InitializeParaValue();
    BYTE CheckDriveExist();
    BOOL OpenDriveHandleAgain(BYTE volumeIndex);
    BOOL ValidatePhysicalDevice(HANDLE hDevice, BYTE driveIndex);
    BOOL SetDeviceID();
    void VolumePairController();

    // Device operation functions
    BOOL CheckSystemReadyIO(BYTE volumeIndex, DWORD deviceId);
    BOOL LoadBankC(BYTE volumeIndex, DWORD deviceId);
    BOOL GetBCMInformation(BYTE volumeIndex, DWORD deviceId);
    BOOL LoadBankData(BYTE volumeIndex, DWORD deviceId);
    BOOL GetMPInfo(BYTE controllerIndex, DWORD deviceId);
    BOOL GetLunArrayData(BYTE controllerIndex, DWORD deviceId);

    // Helper functions
    BOOL GetBinFilePath(BYTE volumeIndex, LPCSTR fileName, LPSTR filePath, DWORD pathSize);
    BOOL FormatDeviceString(LPSTR buffer, DWORD size, LPCSTR format, ...);
    BOOL IdentifyDeviceFamily(LPCSTR inquiryString, BYTE volumeIndex);
    BOOL CopyBankData(BYTE volumeIndex);
    BOOL FormatDeviceIdentification();

    // New functions based on decompilation analysis
    BOOL NotifyFwSegmentInfo(BYTE controllerIndex, DWORD deviceId);

    // Device management functions based on Ghidra analysis
    UINT OpenDriveHandleAgain(int deviceIndex);
    BOOL OpenPhysicalDrive(int driveIndex);
    BOOL OpenLogicalDriveHandle(BYTE volumeIndex);
    void CloseDeviceHandle(BYTE volumeIndex);
    void PrepareFirmwareFilePath();
    void ReadBinaryFileVersion();
    BOOL InitializeISPCode(BYTE controllerIndex, DWORD deviceId, HANDLE hDevice);
    void LoadAndVerifyFirmwareSegments(BYTE controllerIndex, DWORD deviceId);
    void UpdateFirmwareBankInfo(BYTE controllerIndex, DWORD deviceId);
    void FormatFinalDeviceString(BYTE volumeIndex);
    void CalculateDeviceCapacity(BYTE controllerIndex);
    void UpdateDeviceCapacityOrCalculate(BYTE controllerIndex);

    // SDK function pointers for device management
    PFN_VDR_ReadLUNIndex m_pVDR_GetLunIndex;
    PFN_VDR_ReadLUNID m_pVDR_GetDeviceID;

    // Cryptographic manager for hash functions (from Ghidra analysis)
    CryptoManager m_cryptoManager;

    // Internal helper functions
    DWORD AnalyzeSpareAreaAndClassifyBlock(DWORD blockIndex, DWORD unknown, BYTE* spareBuffer);
    BOOL CheckDeviceTypeAndFlag(BYTE* spareBuffer, BYTE flag);
};

// Error codes for iTEUFDrs
#define ITEUFDRS_ERROR_NONE        0
#define ITEUFDRS_ERROR_SDK_LOAD    1  // Failed to load 181FlashSDK.dll
#define ITEUFDRS_ERROR_API_BIND    2  // Failed to bind API functions
#define ITEUFDRS_ERROR_DEVICE_INFO 3  // Failed to get device information
