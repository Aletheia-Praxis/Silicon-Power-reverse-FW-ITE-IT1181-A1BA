#pragma once
#include <windows.h>
#include "SDKAPIs.h"
#include "DeviceStructures.h"
#include "CryptoManager.h"

// Maximum number of volumes and controllers
#define MAX_VOLUMES 8
#define MAX_CONTROLLERS 3

// SDK function pointer types for device management
typedef int (__stdcall *PFN_VDR_GetDeviceInquiry)(void* buffer, HANDLE handle);
typedef int (__stdcall *PFN_VDR_CheckDeviceSupport)(void* buffer, HANDLE handle);
typedef int (__stdcall *PFN_VDR_GetLunIndex)(BYTE* lunIndex, void* buffer, HANDLE handle);
typedef int (__stdcall *PFN_VDR_GetDeviceID)(BYTE* deviceId, void* buffer, HANDLE handle);

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

// iTEUFDrs class (represents the main application logic)
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

    // Enhanced device information structure based on Ghidra analysis
struct DeviceInfo {
    WORD controllerType;     // 0x1181, 0x1176, etc.
    UCHAR firmwareType;      // Firmware type identifier
    UCHAR revisionType;      // Revision identifier (A0AA=0, A1BA=1, etc.)
    UCHAR lunIndex;          // LUN index
    UCHAR deviceId;          // Device ID
    UINT driveType;          // Drive type from GetDriveTypeA
    char vendorId[16];       // Vendor identification
    char productId[32];      // Product identification
    DWORD capabilities;      // Device capabilities flags
};

// Constants for enhanced device management
#define MAX_DEVICES 256
#define DEVICE_STRUCT_SIZE 0x57  // Size per device structure (from Ghidra analysis)

// Enhanced iTEFlashDevice structure based on Ghidra decompilation
    UINT EnhancedOpenDriveHandleAgain();
    BOOL PerformSecureDeviceInquiry(void* inquiryBuffer, HANDLE driveHandle, DWORD driveIndex);
    BOOL AnalyzeAndValidateDevice(void* inquiryBuffer, void* extensionBuffer, 
                                HANDLE driveHandle, DWORD driveIndex, DWORD structOffset);
    BOOL DetermineControllerType(const char* inquiryString, DeviceInfo* deviceInfo);
    BOOL ExtractEnhancedDeviceCapabilities(void* extensionBuffer, HANDLE driveHandle, 
                                         DeviceInfo* deviceInfo, DWORD driveIndex);
    
    // Security utility functions
    BOOL ValidateSystemState();
    BOOL SecureMemoryAllocation(void** buffer, SIZE_T size, const char* purpose);
    void CleanupSecureBuffers(void** buffer1, void** buffer2);
    BOOL ValidateInquiryResponse(void* buffer, SIZE_T size);
    BOOL ValidateITEDeviceSignature(const char* inquiryString);
    BOOL CopyInquiryString(char* dest, SIZE_T destSize, const char* src);

private:
    // Members based on decompiled structure
    void* m_vtable; // 0x00
    BOOL m_isInitialized; // 0x04 (param_1 + 5)
    DWORD m_lastError; // 0x08 (param_1 + 1)
    CHAR m_basePath[MAX_PATH]; // 0x11b
    HMODULE m_hSDK; // 0x21f
    
    // Device management data
    BYTE m_volumeCount;
    BYTE m_controllerCount;
    BOOL m_forcedMode;
    DEVICE_VOLUME_INFO m_deviceVolumeInfo[MAX_VOLUMES];
    CONTROLLER_DATA m_controllerData[MAX_CONTROLLERS];
    
    // Enhanced device management structures (based on Ghidra MCP analysis)
    ITE_DEVICE_INFO_ENHANCED device_array[MAX_DEVICE_COUNT_ENHANCED];
    CONTROLLER_PAIR_INFO controller_pairs[MAX_CONTROLLERS];
    int device_count;                    // Number of detected devices
    int controller_count;                // Number of created controllers

    // Internal initialization functions
    BOOL InitializeSDK();
    BOOL VerifySDKIntegrity();
    void InitializeMembers();
    
    // Device information functions (decompiled from various FUN_* functions)
    BOOL GetDeviceInfoInternal();
    BOOL InitializeParaValue();
    BYTE CheckDriveExist();
    BOOL CheckDriveExistInternal(BYTE volumeIndex);
    BOOL OpenDriveHandleAgain(BYTE volumeIndex);
    BOOL ValidatePhysicalDevice(HANDLE hDevice, BYTE driveIndex);
    BOOL ValidateControllerConfiguration(CONTROLLER_DATA* controller);
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

    // Helpers mirrored from FUN_00408710 and FUN_004087b0
    void UpdateFlagsAfterFlashParse(BYTE volumeIndex);
    void UpdateHighBitFlag(BYTE volumeIndex);

    // SDK wrappers (names inferred from usage in 0x0040c640)
    BOOL flhScanMassBlocksPerChip(DWORD ctx, BYTE ce, BYTE ch, int rtPtr, void* outBuf, BYTE mode, BYTE* outFlag, int* outRet);
    BOOL flhGetScanResult(DWORD ctx, BYTE ce, BYTE ch, int rtPtr, void* outBuf);
    BOOL flhReadSpare(BYTE ch, BYTE ce, int rtPtr, void* spare, DWORD spareSize, DWORD ctx);
    BOOL flhBlockErase(DWORD ctx, DWORD handle, int rtPtr);
    void flhCPUReset(int rtPtr, DWORD handle, DWORD ctx);
    
    // Device management functions based on Ghidra analysis
    UINT OpenDriveHandleAgain(int deviceIndex);
    void CloseDriveHandle(int driveIndex);
    BOOL OpenPhysicalDrive(int driveIndex);
    void FormatInquiryString(void* destination, const void* format, const void* source);
    void* GetManager();
    
    // Helper functions for OpenDriveHandleAgain (based on Ghidra MCP decompilation)
    char OpenPhysicalDriveByIndex(int deviceIndex);
    int GetControllerDataBase();
    int CallSDKInquiry(void* buffer, HANDLE deviceHandle);
    void FormatInquiryString(uchar** resultStr, int sourceOffset);
    uchar* FindSubstring(uchar* haystack, const char* needle);
    char CallSDKDeviceSupport(void* buffer, HANDLE deviceHandle);
    int CallSDKGetLunIndex(unsigned char* lunIndex, void* buffer, HANDLE deviceHandle);
    int CallSDKGetDeviceID(unsigned char* deviceId, void* buffer, HANDLE deviceHandle);
    void CloseAndReopenDriveHandle(int deviceIndex);
    
    // Enhanced Ghidra MCP based functions
    UINT InitializeParaValue();          // Based on 0x00408370 (180 bytes)
    UINT VolumePairController();         // Based on 0x00408430 (335 bytes)  
    UINT DeviceManagementWorkflow();     // Based on 0x0040d022 (1483 bytes)
    
    // Supporting workflow functions for new implementation
    BOOL CheckSystemReadyIO(int device_idx);
    BOOL InitializeController(int controller_idx);
    BOOL LoadBankC(int controller_idx);
    BOOL GetBCMInformation(int controller_idx);
    BOOL CopyBankData(int controller_idx);
    BOOL LoadBankData(int controller_idx);
    BOOL FormatFinalDeviceString(int controller_idx);
    
    // Enhanced validation functions with security controls
    bool VerifySDKIntegrity();
    bool ValidatePhysicalDevice();
    bool ValidateControllerConfiguration();
    
    // SDK function pointers for device management
    PFN_VDR_GetDeviceInquiry m_pVDR_GetDeviceInquiry;
    PFN_VDR_CheckDeviceSupport m_pVDR_CheckDeviceSupport;
    PFN_VDR_GetLunIndex m_pVDR_GetLunIndex;
    PFN_VDR_GetDeviceID m_pVDR_GetDeviceID;
    
    // Cryptographic manager for hash functions (from Ghidra analysis)
    CryptoManager m_cryptoManager;
};

// Error codes for iTEUFDrs
#define ITEUFDRS_ERROR_NONE          0
#define ITEUFDRS_ERROR_SDK_LOAD      1  // Failed to load 181FlashSDK.dll
#define ITEUFDRS_ERROR_API_BIND      2  // Failed to bind API functions 
#define ITEUFDRS_ERROR_DEVICE_INFO   3  // Failed to get device information
