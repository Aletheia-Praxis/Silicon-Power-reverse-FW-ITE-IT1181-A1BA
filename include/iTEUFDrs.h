#pragma once
#include "CryptoManager.h"
#include "DeviceStructures.h"
#include "SDKAPIs.h"
#include "Utilities.h"
#include "WindowsHeaders.h"

// Forward declaration
class CUrescueDlg;

// iTEUFDrs class (represents the main application logic)
class iTEUFDrs {
public:
    iTEUFDrs();  // Default constructor
    iTEUFDrs(LPCSTR basePath);
    ~iTEUFDrs();

    void SetParentDialog(CUrescueDlg* pDlg) { m_pParentDlg = pDlg; }

    BOOL IsInitialized() const { return m_isInitialized; }
    DWORD GetLastError() const { return m_lastError; }
    const _DEVICE_INFO& GetDeviceInfo() const { return m_deviceInfo; }
    char GetDeviceInfo();  // Returns success/failure status (from constructor)

    // Friend functions for global function access
    friend char InitializeDeviceParameters();
    friend char ScanForITEUSBDevices();
    friend BYTE OpenDriveHandleAgain();
    friend void SetDeviceID();
    friend void VolumePairController();
    friend char NotifyFwSegmentInfo(int deviceIndex, DWORD deviceHandle);

private:
    // VTable (placeholder)
    void* m_vtable;

    // Member variables
    BOOL m_isInitialized;
    DWORD m_lastError;
    CHAR m_basePath[MAX_PATH];
    _DEVICE_INFO m_deviceInfo;
    CONTROLLER_DATA m_controllerData[MAX_CONTROLLERS];
    BYTE m_bcmBuffer[0xE40];    // Buffer for BCM (Bad Block Management) information
    BYTE m_deviceIDTable[255];  // Table to track used device IDs
    BYTE m_controllerCount;
    BYTE m_volumeCount;

    CUrescueDlg* m_pParentDlg;  // Pointer to the main dialog

    // SDK handles and functions
    HMODULE m_hSDK;

    // Internal initialization functions
    BOOL InitializeSDK();
    BOOL LoadSDKFunctions(HMODULE hSDK);
    BOOL VerifySDKIntegrity();
    void InitializeMembers();
    BOOL InitializeDeviceStructures();

    // Device information functions (decompiled from various FUN_* functions)
    BOOL InitializeParaValue(void*);
    BYTE CheckDriveExist(void*);
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

    // Device processing (internal)
    void ProcessDetectedDevices();

    // Helper functions for device processing
    BYTE InitializeDeviceParameters();
    BYTE ScanForITEUSBDevices();
    BYTE OpenDriveHandleAgain();
    BYTE OpenLogicalDriveHandle(BYTE driveIndex);
    BYTE OpenPhysicalDriveHandle(BYTE driveIndex);
    // GetFlashMethod removed - using global function as per Ghidra analysis
    // InitializeISPCode removed - using global function as per Ghidra analysis
    // NotifyFwSegmentInfo removed - using global function as per Ghidra analysis
    INT CallSDKGetBCMInfo(BYTE* bufferPtr, DWORD deviceParam);
    void HandleBCMError(INT errorCode);
    void CopyDeviceStructures(INT deviceOffset);
    void UpdateFirmwareBankInfo(INT deviceIndex, DWORD deviceParam);
    void GetMPInfoAndUpdateBuffers(INT deviceIndex, DWORD deviceParam);
    void CalculateDeviceCapacity(INT deviceIndex);
    void UpdateDeviceCapacityOrCalculate(INT deviceIndex);
    BYTE GetLunArrayData(INT deviceIndex, DWORD deviceParam);
    BYTE GetMPInfo(INT deviceIndex, DWORD deviceParam);
    void FormatDeviceDisplayString(CHAR mpResult, INT deviceIndex);
    void SetDeviceFlags(INT deviceIndex);
    void FindActiveDeviceAndBuildStrings();
    void BuildMultiControllerString(UINT activeDevice);
    void AssignDeviceSizeString(CHAR* sizeString);
    void PrepareFirmwareFilePath();
    void ReadBinaryVersionInfo();
    void CloseITEDeviceHandle(UINT deviceSlot);

    // Helper functions
    BOOL GetBinFilePath(BYTE volumeIndex, LPCSTR fileName, LPSTR filePath, DWORD pathSize);
    BOOL FormatDeviceString(LPSTR buffer, DWORD size, LPCSTR format, ...);
    BOOL IdentifyDeviceFamily(LPCSTR inquiryString, BYTE volumeIndex);
    BOOL CopyBankData(BYTE volumeIndex);
    BOOL FormatDeviceIdentification();

    // New functions based on decompilation analysis
    // NotifyFwSegmentInfo removed - using global function as per Ghidra analysis

    // Legacy device management functions (deprecated - use new ones above)

    // SDK function pointers for device management
    PFN_VDR_ReadLUNIndex m_pVDR_GetLunIndex;
    PFN_VDR_ReadLUNID m_pVDR_GetDeviceID;

    // Cryptographic manager for hash functions (from Ghidra analysis)
    CryptoManager m_cryptoManager;

    // Internal helper functions
    DWORD AnalyzeSpareAreaAndClassifyBlock(
        CONTROLLER_DATA& controller,
        DWORD blockIndex,
        DWORD unknown,
        BYTE* spareBuffer);
    BOOL CheckDeviceTypeAndFlag(BYTE* spareBuffer, BYTE flag);

    // Additional state members from Ghidra analysis
    char m_sdkPath[260];      // SDK DLL path
    BOOL m_sdkLoadError;      // SDK load failure flag
    BOOL m_deviceReady;       // Device ready status
    BOOL m_initError1;        // Initialization error flag 1
    BOOL m_initError2;        // Initialization error flag 2
    BOOL m_statusFlag1;       // General status flag
    BOOL m_deviceConnected;   // Device connection status
    BOOL m_connectionStatus;  // Connection state
    BYTE m_deviceCount;       // Number of detected devices
    BOOL m_scanComplete;      // Device scan completion flag
    BOOL m_processingFlag;    // Processing operation flag
    BYTE m_activeDevice;      // Currently active device index

    // Device handles for multiple devices
    HANDLE m_deviceHandle1;
    HANDLE m_deviceHandle2;
    HANDLE m_deviceHandle3;
    HANDLE m_deviceHandle4;

    // Large data buffers for operations
    BYTE m_deviceBuffer[0x200000];  // Main device buffer (2 MB) - matches original layout
    BYTE m_commandBuffer[0x200];    // Command buffer (512 bytes)
    BYTE m_responseBuffer[0x40];    // Response buffer (64 bytes)
    BYTE m_statusBuffer[0x40];      // Status buffer (64 bytes)

    // Operation status tracking
    DWORD m_operationStatus;
    DWORD m_progressStatus;
    DWORD m_transferStatus;
    DWORD m_completionStatus;

    // Additional buffers
    BYTE m_firmwareBuffer[0x800];  // Firmware buffer (2 KB)
    HMODULE m_sdkModule;           // Handle to loaded SDK DLL
};

// Error codes for iTEUFDrs
#define ITEUFDRS_ERROR_NONE        0
#define ITEUFDRS_ERROR_SDK_LOAD    1  // Failed to load 181FlashSDK.dll
#define ITEUFDRS_ERROR_API_BIND    2  // Failed to bind API functions
#define ITEUFDRS_ERROR_DEVICE_INFO 3  // Failed to get device information

// Function declarations for helper functions
int GetBCMInfo(int deviceStructBase, DWORD deviceHandle);

// Global function declarations - reconstructed from Ghidra analysis
extern "C" {
// Global function that performs complete device detection and initialization - EXACT Ghidra
// signature: returns char, no parameters (uses global instance access)
char iTEUFDrs_DetectAndInitializeDevices();

// C wrapper functions for backward compatibility with Ghidra-reconstructed code
void PrepareFirmwareFilePath();
char GetMPInfo(int deviceIndex, DWORD deviceHandle);
}
