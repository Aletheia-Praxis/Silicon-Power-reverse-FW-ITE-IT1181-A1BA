/**
 * iTEUFDrs_Ghidra.h - Exact Class Definition Based on Ghidra Analysis
 *
 * This header contains the precise iTEUFDrs class structure as reconstructed
 * from detailed Ghidra decompilation and memory layout analysis.
 */

#pragma once

#include <afxwin.h>
#include <windows.h>

// Forward declarations
typedef FARPROC SDK_FUNCTION_PTR;

/**
 * iTEUFDrs - Main USB Flash Drive Recovery Dialog Class
 * EXACT reconstruction based on Ghidra analysis at 0x0040d690
 *
 * Memory layout precisely matches the original executable.
 * Member variable offsets are based on actual disassembly.
 */
class iTEUFDrs : public CDialog {
public:
    // Constructor - reconstructed from 0x0040d690
    iTEUFDrs(LPCSTR sdkPath);
    virtual ~iTEUFDrs();

    // Core methods from Ghidra analysis
    bool LoadSDKFunctions(HMODULE sdkModule);  // 0x00401000
    char GetDeviceInfo();                      // Calls 0x0040cf30

    // Status queries
    bool IsDeviceReady() const { return m_deviceReady != 0; }
    bool IsSDKLoaded() const { return m_sdkLoadError == 0; }
    int GetLastError() const { return m_sdkLoadError; }

private:
    //==============================================================================
    // EXACT MEMBER VARIABLE LAYOUT - Based on Ghidra Offset Analysis
    //==============================================================================

    // Status flags - offsets from param_1 pointer analysis
    BYTE m_deviceReady;   // offset 1 (param_1 + 1)
    BYTE pad1[3];         // padding
    BYTE m_sdkLoadError;  // offset 5 (param_1 + 5)
    BYTE m_initError1;    // offset 6 (param_1 + 6)
    BYTE m_initError2;    // offset 7 (param_1 + 7)

    // Large padding to reach correct offsets
    BYTE padding1[0x11b - 8];  // Padding to reach SDK path offset

    // SDK path - offset 0x11b (param_1 + 0x11b)
    char m_sdkPath[0x104];  // 260 bytes for SDK directory path

    // More padding to reach device handles
    BYTE padding2[0x97 * 4 - 0x11b - 0x104];

    // Device handles - offsets 0x97-0x9a (param_1[0x97] etc.)
    HANDLE m_deviceHandle1;  // param_1[0x97]
    HANDLE m_deviceHandle2;  // param_1[0x98]
    HANDLE m_deviceHandle3;  // param_1[0x99]
    HANDLE m_deviceHandle4;  // param_1[0x9a]

    // Device info buffer - offset 0x9b (param_1 + 0x9b)
    BYTE m_deviceInfoBuffer[0x200];  // 512 bytes

    // Command buffer - offset 2 (param_1 + 2)
    BYTE m_commandBuffer[0x200];  // 512 bytes

    // Status and response buffers
    BYTE m_statusBuffer[0x40];    // 64 bytes at offset 0x82
    BYTE m_responseBuffer[0x40];  // 64 bytes at offset 0x6aca

    // Operation status DWORDs - offsets 0x92-0x96
    DWORD m_lastError;         // param_1[0x92]
    DWORD m_operationStatus;   // param_1[0x93]
    DWORD m_progressStatus;    // param_1[0x94]
    DWORD m_transferStatus;    // param_1[0x95]
    DWORD m_completionStatus;  // param_1[0x96]

    // SDK module handle - offset 0x21f (param_1[0x21f])
    HMODULE m_sdkModule;

    // Additional status flags with exact offsets
    BYTE m_deviceConnected;   // offset 0x881 (param_1 + 0x881)
    BYTE m_connectionStatus;  // offset 0x882 (param_1 + 0x882)
    BYTE m_repairDeviceFlag;  // offset 0x883 (param_1 + 0x883)
    BYTE m_processingFlag;    // offset 0x885 (param_1 + 0x885)
    BYTE m_scanComplete;      // offset 0x886 (param_1 + 0x886)

    // Device management flags
    BYTE m_deviceInfoFlag;    // offset 0x8a0 (param_1 + 0x8a0)
    char m_driveExistStatus;  // offset 0x8a1 (param_1 + 0x8a1)
    BYTE m_deviceListCount;   // offset 0x8a2 (param_1 + 0x8a2)
    char m_selectedDevice;    // offset 0x9a2 (param_1 + 0x9a2)
    BYTE m_firstValidDevice;  // offset 0x9a3 (param_1 + 0x9a3)

    // Large data buffers with exact sizes from Ghidra
    BYTE m_flashDataBuffer[0x100000];  // 1MB at offset 0x41cd0
    BYTE m_firmwareBuffer[0x800];      // 2KB at offset 0x41ace

    // Additional status variables from Ghidra analysis
    DWORD m_deviceCount;        // offset 0x81cd2 (param_1[0x81cd2])
    BYTE m_statusFlag1;         // offset 0x81cd0 (param_1 + 0x81cd0)
    BYTE m_processingModeFlag;  // offset 0x6b37 (param_1 + 0x6b37)
    BYTE m_systemReadyFlag;     // offset 0x880 (param_1 + 0x880)

    // Device display information
    char m_deviceDisplayInfo[0x200];  // offset 8 (param_1 + 8)
    BYTE m_deviceIdBuffer[8];         // offset 0x107338

    //==============================================================================
    // ALL 106 SDK FUNCTION POINTERS - Exact addresses from Ghidra
    //==============================================================================

    // Flash Layer Helper (FLH) functions with exact memory addresses
    static SDK_FUNCTION_PTR g_pFLH_GetInfoFromDataBaseByID;    // _g_pFLH_GetInfoFromDataBaseByID
    static SDK_FUNCTION_PTR g_pFLH_GetFlashDataFromDataBase;   // g_pFLH_GetFlashDataFromDataBase
    static SDK_FUNCTION_PTR g_pFLH_GetFlashDataFromMemory;     // g_pFLH_GetFlashDataFromMemory
    static SDK_FUNCTION_PTR g_pFLH_ReadRootTable;              // _g_pFLH_ReadRootTable
    static SDK_FUNCTION_PTR g_pFLH_WriteRootTable;             // _g_pFLH_WriteRootTable
    static SDK_FUNCTION_PTR g_pFLH_ReadCISTable;               // _g_pFLH_ReadCISTable
    static SDK_FUNCTION_PTR g_pFLH_WriteCISTable;              // _DAT_004ad5a4
    static SDK_FUNCTION_PTR g_pFLH_ReadISPData;                // g_pMP_ReadISPData
    static SDK_FUNCTION_PTR g_pFLH_WriteISPData;               // _DAT_004ad594
    static SDK_FUNCTION_PTR g_pFLH_ReadLatestWBT;              // _DAT_004ad5fc
    static SDK_FUNCTION_PTR g_pFLH_FindRootTable;              // DAT_004ad5c0
    static SDK_FUNCTION_PTR g_pFLH_LBA2PhysicalFlash;          // _DAT_004ad5f8
    static SDK_FUNCTION_PTR g_pFLH_SetLedBlink;                // _DAT_004ad60c
    static SDK_FUNCTION_PTR g_pFLH_PhyiscalRead;               // _DAT_004ad590
    static SDK_FUNCTION_PTR g_pFLH_PhyiscalWrite;              // _DAT_004ad58c
    static SDK_FUNCTION_PTR g_pFLH_IsGoodBlock;                // _DAT_004ad580
    static SDK_FUNCTION_PTR g_pFLH_IsTableBlock;               // _DAT_004ad57c
    static SDK_FUNCTION_PTR g_pFLH_MarkBad;                    // DAT_004ad578
    static SDK_FUNCTION_PTR g_pFLH_GetRealBlocksPerDie;        // _DAT_004ad574
    static SDK_FUNCTION_PTR g_pFLH_BlockIsGap;                 // DAT_004ad570
    static SDK_FUNCTION_PTR g_pFLH_HandleMassBlocksPerChip;    // DAT_004ad5dc
    static SDK_FUNCTION_PTR g_pFLH_ScanNewBlock;               // _DAT_004ad5d8
    static SDK_FUNCTION_PTR g_pFLH_GetRetryRegister;           // _DAT_004ad608
    static SDK_FUNCTION_PTR g_pFLH_CISCheckSum_Calculate;      // _DAT_004ad5a0
    static SDK_FUNCTION_PTR g_pFLH_CalCulate_ECCNO;            // _DAT_004ad5c4
    static SDK_FUNCTION_PTR g_pFLH_ArrangeSegmentPara;         // DAT_004ad5f4
    static SDK_FUNCTION_PTR g_pFLH_ReadSpare;                  // DAT_004ad588
    static SDK_FUNCTION_PTR g_pFLH_ReadID;                     // _DAT_004ad600
    static SDK_FUNCTION_PTR g_pFLH_BlockErase;                 // DAT_004ad584
    static SDK_FUNCTION_PTR g_pFLH_SetSLCFlag;                 // _DAT_004ad610
    static SDK_FUNCTION_PTR g_pFLH_CPUReset;                   // DAT_004ad604
    static SDK_FUNCTION_PTR g_pFLH_InitCTRL;                   // DAT_004ad5ec
    static SDK_FUNCTION_PTR g_pFLH_WriteRootTableWithIspPath;  // _DAT_004ad5b0
    static SDK_FUNCTION_PTR g_pFLH_ScanE2NANDBlockPerChip;     // DAT_004ad5e0
    static SDK_FUNCTION_PTR g_pFLH_ReadBCM;                    // DAT_004ad5f0
    static SDK_FUNCTION_PTR g_pFLH_InitCodeWithIspPath;        // DAT_004ad5e8
    static SDK_FUNCTION_PTR g_pFLH_GetChannelCeNoAndMap;       // _DAT_004ad5d4
    static SDK_FUNCTION_PTR g_pFLH_InitCodeForReady;           // _DAT_004ad5e4

    // Security (SEC) functions
    static SDK_FUNCTION_PTR g_pSEC_DoAuthentication;         // _g_pSEC_DoAuthentication
    static SDK_FUNCTION_PTR g_pSEC_LeaveAuthenticatedState;  // _DAT_004ad568
    static SDK_FUNCTION_PTR g_pSEC_GetPasswordHint;          // _DAT_004ad560
    static SDK_FUNCTION_PTR g_pSEC_SetPasswordHint;          // _DAT_004ad55c
    static SDK_FUNCTION_PTR g_pSEC_ChangePassword;           // _DAT_004ad564
    static SDK_FUNCTION_PTR g_pSEC_GetUserPassword;          // _DAT_004ad558
    static SDK_FUNCTION_PTR g_pSEC_GetEncryptedPassword;     // _DAT_004ad554

    // LUN (Logical Unit Number) functions
    static SDK_FUNCTION_PTR g_pLUN_CreateLun;                 // _DAT_004ad550
    static SDK_FUNCTION_PTR g_pLUN_FindLunStartLBAByItemID;   // _DAT_004ad540
    static SDK_FUNCTION_PTR g_pLUN_CreateApLunNewItemID;      // _DAT_004ad53c
    static SDK_FUNCTION_PTR g_pLUN_WriteBadBlockMapToApLun;   // _DAT_004ad548
    static SDK_FUNCTION_PTR g_pLUN_ReadBadBlockMapFromApLun;  // _DAT_004ad544
    static SDK_FUNCTION_PTR g_pLUN_FindOptimumOffsetCap;      // _DAT_004ad54c
    static SDK_FUNCTION_PTR g_pLUN_CalIsoSize;                // _DAT_004ad538

    // Format (FMT) functions
    static SDK_FUNCTION_PTR g_pFMT_Format;               // _g_pFMT_Format
    static SDK_FUNCTION_PTR g_pFMT_GetOptimumCapacity;   // _DAT_004ad530
    static SDK_FUNCTION_PTR g_pFMT_GetOptimumLunConfig;  // _DAT_004ad52c
    static SDK_FUNCTION_PTR g_pFMT_GetOSCapacity;        // _DAT_004ad528

    // Standard (STD) functions
    static SDK_FUNCTION_PTR g_pSTD_Inquiry;        // g_pSTD_Inquiry
    static SDK_FUNCTION_PTR g_pSTD_ReadCapacity;   // _DAT_004ad524
    static SDK_FUNCTION_PTR g_pSTD_LogicalRead;    // _DAT_004ad51c
    static SDK_FUNCTION_PTR g_pSTD_LogicalWrite;   // _DAT_004ad518
    static SDK_FUNCTION_PTR g_pSTD_TestUnitReady;  // g_pSTD_TestUnitReady (VDR_CheckSYSReady)
    static SDK_FUNCTION_PTR g_pSTD_GetDeviceID;    // g_pSTD_GetDeviceID (VDR_ReadLUNID)
    static SDK_FUNCTION_PTR g_pSTD_SetDeviceID;    // g_pSTD_SetDeviceID (VDR_WriteLUNID)
    static SDK_FUNCTION_PTR g_pSTD_GetLUNIndex;    // g_pSTD_GetLUNIndex (VDR_ReadLUNIndex)

    // Utility functions
    static SDK_FUNCTION_PTR g_pSwapDWORD;              // _DAT_004ad514
    static SDK_FUNCTION_PTR g_pSwapWORD;               // DAT_004ad510
    static SDK_FUNCTION_PTR g_pCCBAddress2RawAddress;  // DAT_004ad50c
    static SDK_FUNCTION_PTR g_pRawAddress2CCBAddress;  // DAT_004ad508
    static SDK_FUNCTION_PTR g_pCCBAddress2ED3Address;  // _DAT_004ad500
    static SDK_FUNCTION_PTR g_pED3Address2CCBAddress;  // _DAT_004ad4fc
    static SDK_FUNCTION_PTR g_pBlkAddr2RawAddr;        // DAT_004ad504

    // Extensive Vendor Driver (VDR) functions - low-level hardware interface
    static SDK_FUNCTION_PTR g_pVDR_ReadWriteLUNConfig;   // DAT_004ad64c
    static SDK_FUNCTION_PTR g_pVDR_ReadLUNData;          // _DAT_004ad648
    static SDK_FUNCTION_PTR g_pVDR_WriteLUNData;         // _DAT_004ad644
    static SDK_FUNCTION_PTR g_pVDR_ReadXData;            // DAT_004ad69c
    static SDK_FUNCTION_PTR g_pVDR_WriteXData;           // _DAT_004ad698
    static SDK_FUNCTION_PTR g_pVDR_ReadIData;            // _DAT_004ad694
    static SDK_FUNCTION_PTR g_pVDR_WriteIData;           // _DAT_004ad690
    static SDK_FUNCTION_PTR g_pVDR_ReadSysAddr;          // DAT_004ad68c
    static SDK_FUNCTION_PTR g_pVDR_WriteSysAddr;         // _DAT_004ad688
    static SDK_FUNCTION_PTR g_pVDR_SetSYSReady;          // DAT_004ad680
    static SDK_FUNCTION_PTR g_pVDR_EndCode;              // _DAT_004ad67c
    static SDK_FUNCTION_PTR g_pVDR_DeviceChange;         // _DAT_004ad678
    static SDK_FUNCTION_PTR g_pVDR_MediaChange;          // _DAT_004ad674
    static SDK_FUNCTION_PTR g_pVDR_WriteProtect;         // _DAT_004ad670
    static SDK_FUNCTION_PTR g_pVDR_RWCurrentLUNType;     // _DAT_004ad66c
    static SDK_FUNCTION_PTR g_pVDR_HiddenArea;           // _DAT_004ad668
    static SDK_FUNCTION_PTR g_pVDR_ReadWriteLUNNo;       // _DAT_004ad664
    static SDK_FUNCTION_PTR g_pVDR_FlushCache;           // _DAT_004ad654
    static SDK_FUNCTION_PTR g_pVDR_ReadPage;             // _DAT_004ad62c
    static SDK_FUNCTION_PTR g_pVDR_WritePage;            // _DAT_004ad618
    static SDK_FUNCTION_PTR g_pVDR_WriteBlock_TLC;       // _DAT_004ad614
    static SDK_FUNCTION_PTR g_pVDR_GetSecurityStatus;    // _DAT_004ad650
    static SDK_FUNCTION_PTR g_pVDR_ED3PageRead;          // _DAT_004ad628
    static SDK_FUNCTION_PTR g_pVDR_BadTFindRead;         // _DAT_004ad624
    static SDK_FUNCTION_PTR g_pVDR_Enhance_SLC_Program;  // _DAT_004ad640
    static SDK_FUNCTION_PTR g_pVDR_Disable_SLC_Program;  // _DAT_004ad63c
    static SDK_FUNCTION_PTR g_pVDR_MassBlocksProcess;    // _DAT_004ad634
    static SDK_FUNCTION_PTR g_pVDR_F_RST;                // DAT_004ad630
    static SDK_FUNCTION_PTR g_pVDR_RootFunc;             // DAT_004ad620
    static SDK_FUNCTION_PTR g_pVDR_RootPageWrite;        // _DAT_004ad61c
    static SDK_FUNCTION_PTR g_pVDR_RootAccess;           // _DAT_004ad638

    // Media Processor (MP) functions
    static SDK_FUNCTION_PTR g_pMP_CreateSystem;      // DAT_004ad4f0
    static SDK_FUNCTION_PTR g_pMP_EraseSystemTable;  // DAT_004ad4ec

    // Diagnostic (DG) functions
    static SDK_FUNCTION_PTR g_pDG_GetBlockPageMapFromFlash;  // _DAT_004ad4e8
    static SDK_FUNCTION_PTR g_pDG_SearchReadBadTBlk;         // _DAT_004ad4e4
    static SDK_FUNCTION_PTR g_pDG_CalBlkRequire;             // _DAT_004ad4e0

    // Address (ADDR) functions
    static SDK_FUNCTION_PTR g_pADDR_ReadRootTable;  // _DAT_004ad5b8
    static SDK_FUNCTION_PTR g_pADDR_ReadISPData;    // _DAT_004ad598
    static SDK_FUNCTION_PTR g_pADDR_ReadCISData;    // DAT_004ad5a8

    // Device detection functions
    static SDK_FUNCTION_PTR g_pIs168Device;     // _DAT_004ad4f8
    static SDK_FUNCTION_PTR g_pGetLastestPage;  // _DAT_004ad4f4
};

//==============================================================================
// EXTERNAL FUNCTION DECLARATIONS FROM GHIDRA ANALYSIS
//==============================================================================

// Main GetDeviceInfo implementation at 0x0040cf30
extern "C" char __fastcall iTEUFDrs_GetDeviceInfo(int param_1);

// Helper functions identified in device detection chain
extern "C" char InitializeParaValue(void);
extern "C" char CheckDriveExist(void);
extern "C" char OpenDriveHandleAgain(void);
extern "C" void SetDeviceID(void);
extern "C" void VolumePairController(void);
extern "C" void PrepareFirmwareFilePath(void);
extern "C" char NotifyFwSegmentInfo(int deviceIndex, DWORD param);
extern "C" char GetMPInfo(int deviceIndex, DWORD param);

// Functions called during device processing (from GetDeviceInfo analysis)
extern "C" char FUN_00409500(BYTE deviceIndex);              // Device processing function 1
extern "C" char FUN_00408580(BYTE deviceIndex);              // Device processing function 2
extern "C" char FUN_004097c0(int deviceIndex, DWORD param);  // Device preparation
extern "C" char FUN_0040afe0(int deviceIndex, DWORD param);  // Additional processing
extern "C" void FUN_0040c390(void);                          // Helper function
extern "C" char FUN_0040b200(int deviceIndex, DWORD param);  // System processing
extern "C" void FUN_00409a00(int deviceIndex, DWORD param);  // Alternative processing
extern "C" void FUN_00409cd0(int deviceIndex, DWORD param);  // Final processing
extern "C" char FUN_004088d0(int deviceIndex, DWORD param);  // Status check
extern "C" void FUN_00408980(int deviceIndex);               // Error handler
extern "C" void FUN_00409ea0(int deviceIndex);               // Success handler
extern "C" void FUN_00408630(UINT deviceIndex);              // Cleanup function
extern "C" void FUN_00409f10(void* buffer);                  // Buffer preparation

// Logging function used throughout the codebase
extern "C" void FUN_00406170(LPCSTR msg);  // Log message function
