#pragma once
#include "WindowsHeaders.h"

// =============================================================================
// SDK Function Typedefs
//
// This section provides the correct, full signatures for the function pointers
// used by the 181FlashSDK.dll. This is now the single source of truth.
// =============================================================================

// Standard & Vendor-Specific Command Typedefs (with full 6-parameter signature)
typedef int(__stdcall* PFN_STD_Inquiry)(
    DWORD deviceId,
    BYTE* buffer,
    DWORD bufferSize,
    BYTE lunIndex,
    BYTE* bcmInfo,
    BYTE mode);
typedef int(__stdcall* PFN_VDR_CheckSYSReady)(
    DWORD deviceId,
    BYTE* buffer,
    DWORD bufferSize,
    BYTE lunIndex,
    BYTE* bcmInfo,
    BYTE mode);
typedef int(__stdcall* PFN_VDR_ReadLUNIndex)(
    DWORD deviceId,
    BYTE* buffer,
    DWORD bufferSize,
    BYTE lunIndex,
    BYTE* bcmInfo,
    BYTE mode);
typedef int(__stdcall* PFN_VDR_ReadLUNID)(
    DWORD deviceId,
    BYTE* buffer,
    DWORD bufferSize,
    BYTE lunIndex,
    BYTE* bcmInfo,
    BYTE mode);
typedef int(__stdcall* PFN_VDR_WriteLUNID)(
    DWORD deviceId,
    BYTE* buffer,
    DWORD bufferSize,
    BYTE lunIndex,
    BYTE* bcmInfo,
    BYTE mode);
typedef int(__stdcall* PFN_VDR_SetSYSReady)(
    DWORD deviceId,
    BYTE* buffer,
    DWORD bufferSize,
    BYTE lunIndex,
    BYTE* bcmInfo,
    BYTE mode);

// Other SDK Function Typedefs (reconstructed)
typedef BOOL (*PFN_FLH_GetInfoFromDataBaseByID)(DWORD dwID, LPVOID pInfo);
typedef BOOL (*PFN_FLH_GetFlashDataFromDataBase)(LPVOID pData);
typedef BOOL (*PFN_FLH_GetFlashDataFromMemory)(LPVOID pData);
typedef BOOL (*PFN_FLH_ReadRootTable)(LPVOID pBuffer);
typedef BOOL (*PFN_FLH_WriteRootTable)(LPVOID pBuffer);
typedef BOOL (*PFN_FLH_ReadCISTable)(LPVOID pBuffer);
typedef BOOL (*PFN_FLH_WriteCISTable)(LPVOID pBuffer);
typedef BOOL (*PFN_FLH_ReadISPData)(LPVOID pBuffer);
typedef BOOL (*PFN_FLH_WriteISPData)(LPVOID pBuffer);
typedef BOOL (*PFN_FLH_ReadLatestWBT)(LPVOID pBuffer);
typedef BOOL (*PFN_FLH_FindRootTable)(LPVOID pBuffer);
typedef BOOL (*PFN_FLH_LBA2PhysicalFlash)(DWORD dwLBA, LPVOID pPhysAddr);
typedef BOOL (*PFN_FLH_SetLedBlink)(BOOL bBlink);
typedef BOOL (*PFN_FLH_PhyiscalRead)(LPVOID pAddr, LPVOID pBuffer, DWORD dwSize);
typedef BOOL (*PFN_FLH_PhyiscalWrite)(LPVOID pAddr, LPVOID pBuffer, DWORD dwSize);
typedef BOOL (*PFN_FLH_IsGoodBlock)(DWORD dwBlockAddr);
typedef BOOL (*PFN_FLH_IsTableBlock)(DWORD dwBlockAddr);
typedef BOOL (*PFN_FLH_MarkBad)(DWORD dwBlockAddr);
typedef DWORD (*PFN_FLH_GetRealBlocksPerDie)(void);
typedef BOOL (*PFN_FLH_BlockIsGap)(DWORD dwBlockAddr);
typedef BOOL (*PFN_FLH_HandleMassBlocksPerChip)(LPVOID pParams);
typedef BOOL (*PFN_FLH_ScanNewBlock)(LPVOID pParams);
typedef DWORD (*PFN_FLH_GetRetryRegister)(void);
typedef DWORD (*PFN_FLH_CISCheckSum_Calculate)(LPVOID pData, DWORD dwSize);
typedef DWORD (*PFN_FLH_CalCulate_ECCNO)(LPVOID pData);
typedef BOOL (*PFN_FLH_ArrangeSegmentPara)(LPVOID pParams, LPVOID pData);
typedef BOOL (*PFN_FLH_ReadSpare)(DWORD dwAddr, LPVOID pBuffer);
typedef BOOL (*PFN_FLH_ReadID)(LPVOID pBuffer);
typedef BOOL (*PFN_FLH_BlockErase)(DWORD dwBlockAddr);
typedef BOOL (*PFN_FLH_SetSLCFlag)(BOOL bEnable);
typedef BOOL (*PFN_FLH_CPUReset)(void);
typedef int (*PFN_FLH_InitCTRL)(HANDLE hDevice, BYTE* pSegmentParams, LPVOID pBcmBuffer);
typedef BOOL (*PFN_FLH_WriteRootTableWithIspPath)(LPVOID pBuffer);
typedef BOOL (*PFN_FLH_ScanE2NANDBlockPerChip)(LPVOID pParams);
typedef BOOL (*PFN_FLH_ReadBCM)(LPVOID pBuffer);
typedef int (*PFN_FLH_ReadBCM_Alt)(LPVOID pBuffer, HANDLE hDevice);
typedef BOOL (*PFN_FLH_InitCodeWithIspPath)(
    DWORD deviceId,
    BYTE* p1,
    BYTE* p2,
    BYTE* p3,
    LPCSTR basePath,
    BYTE* bcm,
    HANDLE hDevice);
typedef BOOL (*PFN_FLH_GetChannelCeNoAndMap)(LPVOID pParams);
typedef BOOL (*PFN_FLH_InitCodeForReady)(void);

typedef BOOL (*PFN_SEC_DoAuthentication)(LPCSTR pszPassword);
typedef BOOL (*PFN_SEC_LeaveAuthenticatedState)(void);
typedef BOOL (*PFN_SEC_GetPasswordHint)(LPSTR pszHint, DWORD dwSize);
typedef BOOL (*PFN_SEC_SetPasswordHint)(LPCSTR pszHint);
typedef BOOL (*PFN_SEC_ChangePassword)(LPCSTR pszOldPass, LPCSTR pszNewPass);
typedef BOOL (*PFN_SEC_GetUserPassword)(LPSTR pszPassword, DWORD dwSize);
typedef BOOL (*PFN_SEC_GetEncryptedPassword)(LPVOID pBuffer, DWORD dwSize);

typedef BOOL (*PFN_LUN_CreateLun)(LPVOID pLunConfig);
typedef DWORD (*PFN_LUN_FindLunStartLBAByItemID)(DWORD dwItemID);
typedef BOOL (*PFN_LUN_CreateApLunNewItemID)(LPVOID pParams);
typedef BOOL (*PFN_LUN_WriteBadBlockMapToApLun)(LPVOID pBadBlockMap);
typedef BOOL (*PFN_LUN_ReadBadBlockMapFromApLun)(LPVOID pBadBlockMap);
typedef DWORD (*PFN_LUN_FindOptimumOffsetCap)(LPVOID pParams);
typedef DWORD (*PFN_LUN_CalIsoSize)(LPVOID pParams);

typedef BOOL (*PFN_FMT_Format)(LPVOID pFormatParams);
typedef DWORD (*PFN_FMT_GetOptimumCapacity)(void);
typedef BOOL (*PFN_FMT_GetOptimumLunConfig)(LPVOID pLunConfig);
typedef DWORD (*PFN_FMT_GetOSCapacity)(void);

typedef BOOL (*PFN_STD_ReadCapacity)(LPVOID pCapacityData);
typedef BOOL (*PFN_STD_LogicalRead)(DWORD dwLBA, DWORD dwSectors, LPVOID pBuffer);
typedef BOOL (*PFN_STD_LogicalWrite)(DWORD dwLBA, DWORD dwSectors, LPVOID pBuffer);

typedef BOOL (*PFN_VDR_LoadDriver)(LPVOID pParams);
typedef BOOL (*PFN_VDR_FreeDriver)(void);
typedef BOOL (*PFN_VDR_ReadWriteLUNConfig)(BOOL bWrite, LPVOID pConfig);
typedef BOOL (*PFN_VDR_GetSystemAddr)(LPVOID pAddr);
typedef BOOL (*PFN_VDR_ReadSysAddr)(DWORD dwAddr, LPVOID pBuffer);
typedef BOOL (*PFN_VDR_WriteSysAddr)(DWORD dwAddr, LPVOID pBuffer);
typedef BOOL (*PFN_VDR_ReadLUNData)(DWORD dwLUN, LPVOID pBuffer, DWORD dwSize);
typedef BOOL (*PFN_VDR_WriteLUNData)(DWORD dwLUN, LPVOID pBuffer, DWORD dwSize);
typedef BOOL (*PFN_VDR_ReadXData)(LPVOID pBuffer, DWORD dwSize);
typedef BOOL (*PFN_VDR_WriteXData)(LPVOID pBuffer, DWORD dwSize);
typedef BOOL (*PFN_VDR_ReadIData)(LPVOID pBuffer, DWORD dwSize);
typedef BOOL (*PFN_VDR_WriteIData)(LPVOID pBuffer, DWORD dwSize);
typedef DWORD (*PFN_VDR_EndCode)(void);
typedef BOOL (*PFN_VDR_DeviceChange)(void);
typedef BOOL (*PFN_VDR_MediaChange)(void);
typedef BOOL (*PFN_VDR_WriteProtect)(BOOL bProtect);
typedef BOOL (*PFN_VDR_RWCurrentLUNType)(BOOL bWrite, LPVOID pType);
typedef BOOL (*PFN_VDR_HiddenArea)(BOOL bEnable);
typedef BOOL (*PFN_VDR_ReadWriteLUNNo)(BOOL bWrite, LPVOID pLunNo);
typedef BOOL (*PFN_VDR_FlushCache)(void);
typedef BOOL (*PFN_VDR_ReadPage)(DWORD dwPageAddr, LPVOID pBuffer);
typedef BOOL (*PFN_VDR_WritePage)(DWORD dwPageAddr, LPVOID pBuffer);
typedef BOOL (*PFN_VDR_WriteBlock_TLC)(DWORD dwBlockAddr, LPVOID pBuffer);
typedef DWORD (*PFN_VDR_GetSecurityStatus)(void);
typedef BOOL (*PFN_VDR_ED3PageRead)(DWORD dwPageAddr, LPVOID pBuffer);
typedef BOOL (*PFN_VDR_BadTFindRead)(LPVOID pParams);
typedef BOOL (*PFN_VDR_Enhance_SLC_Program)(BOOL bEnable);
typedef BOOL (*PFN_VDR_Disable_SLC_Program)(void);
typedef BOOL (*PFN_VDR_MassBlocksProcess)(LPVOID pParams);
typedef BOOL (*PFN_VDR_F_RST)(void);
typedef BOOL (*PFN_VDR_RootFunc)(LPVOID pParams);
typedef BOOL (*PFN_VDR_RootPageWrite)(DWORD dwPageAddr, LPVOID pBuffer);
typedef BOOL (*PFN_VDR_RootAccess)(LPVOID pParams);

typedef DWORD (*PFN_SwapDWORD)(DWORD dwValue);
typedef WORD (*PFN_SwapWORD)(WORD wValue);
typedef DWORD (*PFN_CCBAddress2RawAddress)(DWORD dwCCBAddr);
typedef DWORD (*PFN_RawAddress2CCBAddress)(DWORD dwRawAddr);
typedef DWORD (*PFN_CCBAddress2ED3Address)(DWORD dwCCBAddr);
typedef DWORD (*PFN_ED3Address2CCBAddress)(DWORD dwED3Addr);
typedef DWORD (*PFN_BlkAddr2RawAddr)(DWORD dwBlockAddr);
typedef BOOL (*PFN_Is168Device)(void);
typedef DWORD (*PFN_GetLastestPage)(void);

typedef BOOL (*PFN_MP_CreateSystem)(LPVOID pParams);
typedef BOOL (*PFN_MP_EraseSystemTable)(void);

typedef BOOL (*PFN_DG_GetBlockPageMapFromFlash)(LPVOID pMap);
typedef BOOL (*PFN_DG_SearchReadBadTBlk)(LPVOID pParams);
typedef DWORD (*PFN_DG_CalBlkRequire)(LPVOID pParams);

typedef BOOL (*PFN_ADDR_ReadRootTable)(LPVOID pBuffer);
typedef BOOL (*PFN_ADDR_ReadISPData)(LPVOID pBuffer);
typedef BOOL (*PFN_ADDR_ReadCISData)(LPVOID pBuffer);

// =============================================================================
// Global FARPROC variables
//
// These are used by the original SDK loading mechanism.
// They are kept for reference but should be replaced by the typed pointers.
// =============================================================================

// Virtual Device Recognition (VDR) APIs
extern FARPROC g_VDR_LoadDriver;
extern FARPROC g_VDR_FreeDriver;
// ... (rest of g_ variables)

// =============================================================================
// SDK_API Structure
//
// This structure holds the function pointers, loaded from the DLL.
// It uses the generic FARPROC type.
// =============================================================================

// Main SDK API structure
typedef struct _SDK_API {
    // Flash information and database operations
    FARPROC FLH_GetInfoFromDataBaseByID;
    FARPROC FLH_GetFlashDataFromDataBase;
    FARPROC FLH_GetFlashDataFromMemory;
    FARPROC FLH_ReadRootTable;
    FARPROC FLH_WriteRootTable;
    FARPROC FLH_ReadCISTable;
    FARPROC FLH_WriteCISTable;
    FARPROC FLH_ReadISPData;
    FARPROC FLH_WriteISPData;
    FARPROC FLH_ReadLatestWBT;
    FARPROC FLH_FindRootTable;
    FARPROC FLH_LBA2PhysicalFlash;
    FARPROC FLH_SetLedBlink;
    FARPROC FLH_PhyiscalRead;
    FARPROC FLH_PhyiscalWrite;
    FARPROC FLH_IsGoodBlock;
    FARPROC FLH_IsTableBlock;
    FARPROC FLH_MarkBad;
    FARPROC FLH_GetRealBlocksPerDie;
    FARPROC FLH_BlockIsGap;
    FARPROC FLH_HandleMassBlocksPerChip;
    FARPROC FLH_ScanNewBlock;
    FARPROC FLH_GetRetryRegister;
    FARPROC FLH_CISCheckSum_Calculate;
    FARPROC FLH_CalCulate_ECCNO;
    FARPROC FLH_ArrangeSegmentPara;
    FARPROC ADDR_ReadRootTable;
    FARPROC FLH_ReadSpare;
    FARPROC FLH_ReadID;
    FARPROC FLH_BlockErase;
    FARPROC FLH_SetSLCFlag;
    FARPROC FLH_CPUReset;
    FARPROC FLH_InitCTRL;
    FARPROC FLH_WriteRootTableWithIspPath;
    FARPROC FLH_ScanE2NANDBlockPerChip;
    FARPROC FLH_ReadBCM;
    FARPROC FLH_InitCodeWithIspPath;
    FARPROC FLH_GetChannelCeNoAndMap;
    FARPROC ADDR_ReadISPData;
    FARPROC ADDR_ReadCISData;
    FARPROC FLH_InitCodeForReady;

    // Security functions
    FARPROC SEC_DoAuthentication;
    FARPROC SEC_LeaveAuthenticatedState;
    FARPROC SEC_GetPasswordHint;
    FARPROC SEC_SetPasswordHint;
    FARPROC SEC_ChangePassword;
    FARPROC SEC_GetUserPassword;
    FARPROC SEC_GetEncryptedPassword;

    // LUN management functions
    FARPROC LUN_CreateLun;
    FARPROC LUN_FindLunStartLBAByItemID;
    FARPROC LUN_CreateApLunNewItemID;
    FARPROC LUN_WriteBadBlockMapToApLun;
    FARPROC LUN_ReadBadBlockMapFromApLun;
    FARPROC LUN_FindOptimumOffsetCap;
    FARPROC LUN_CalIsoSize;

    // Formatting functions
    FARPROC FMT_Format;
    FARPROC FMT_GetOptimumCapacity;
    FARPROC FMT_GetOptimumLunConfig;
    FARPROC FMT_GetOSCapacity;

    // Standard SCSI commands
    FARPROC STD_Inquiry;
    FARPROC STD_ReadCapacity;
    FARPROC STD_LogicalRead;
    FARPROC STD_LogicalWrite;
    FARPROC STD_TestUnitReady;  // Actually VDR_CheckSYSReady
    FARPROC STD_GetDeviceID;    // Actually VDR_ReadLUNID
    FARPROC STD_SetDeviceID;    // Actually VDR_WriteLUNID
    FARPROC STD_GetLUNIndex;    // Actually VDR_ReadLUNIndex

    // Data conversion utilities
    FARPROC SwapDWORD;
    FARPROC SwapWORD;
    FARPROC CCBAddress2RawAddress;
    FARPROC RawAddress2CCBAddress;
    FARPROC CCBAddress2ED3Address;
    FARPROC ED3Address2CCBAddress;
    FARPROC BlkAddr2RawAddr;

    // Vendor-specific commands (VDR)
    FARPROC VDR_ReadWriteLUNConfig;
    FARPROC VDR_ReadLUNData;
    FARPROC VDR_WriteLUNData;
    FARPROC VDR_ReadXData;
    FARPROC VDR_WriteXData;
    FARPROC VDR_ReadIData;
    FARPROC VDR_WriteIData;
    FARPROC VDR_ReadSysAddr;
    FARPROC VDR_WriteSysAddr;
    FARPROC VDR_SetSYSReady;
    FARPROC VDR_EndCode;
    FARPROC VDR_DeviceChange;
    FARPROC VDR_MediaChange;
    FARPROC VDR_WriteProtect;
    FARPROC VDR_RWCurrentLUNType;
    FARPROC VDR_HiddenArea;
    FARPROC VDR_ReadWriteLUNNo;
    FARPROC VDR_FlushCache;
    FARPROC VDR_ReadPage;
    FARPROC VDR_WritePage;
    FARPROC VDR_WriteBlock_TLC;
    FARPROC VDR_GetSecurityStatus;
    FARPROC VDR_ED3PageRead;
    FARPROC VDR_BadTFindRead;
    FARPROC VDR_Enhance_SLC_Program;
    FARPROC VDR_Disable_SLC_Program;
    FARPROC VDR_MassBlocksProcess;
    FARPROC VDR_F_RST;
    FARPROC VDR_RootFunc;
    FARPROC VDR_RootPageWrite;
    FARPROC VDR_RootAccess;

    // Diagnostic and Mass Production (MP) functions
    FARPROC MP_CreateSystem;
    FARPROC MP_EraseSystemTable;
    FARPROC DG_GetBlockPageMapFromFlash;
    FARPROC DG_SearchReadBadTBlk;
    FARPROC DG_CalBlkRequire;

    // Miscellaneous
    FARPROC Is168Device;
    FARPROC GetLastestPage;

} SDK_API, *PSDK_API;

extern SDK_API g_sdk_api;

// =============================================================================
// API Binding Functions
// =============================================================================

// Function to bind all SDK APIs (decompiled from FUN_00401000)
BOOL BindSDKAPIs(HMODULE hSDK);

// Function to clear all API pointers
void ClearSDKAPIs();
