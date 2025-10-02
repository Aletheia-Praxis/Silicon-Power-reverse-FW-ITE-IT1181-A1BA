#pragma once

// This file contains reconstructed function definitions for the 181FlashSDK.dll
// Based on reverse engineering of URescue_v81D.2.24.2.exe
// Total: 106 functions identified from iTEUFDrs::LoadSDKFunctions analysis

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// Flash Hardware Layer (FLH_*) - Low-level flash operations - 38 functions
// =============================================================================
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
typedef BOOL (
    *PFN_FLH_PhyiscalRead)(LPVOID pAddr, LPVOID pBuffer, DWORD dwSize);  // Note: typo in original
typedef BOOL (
    *PFN_FLH_PhyiscalWrite)(LPVOID pAddr, LPVOID pBuffer, DWORD dwSize);  // Note: typo in original
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
typedef BOOL (*PFN_FLH_ArrangeSegmentPara)(LPVOID pParams);
typedef BOOL (*PFN_FLH_ReadSpare)(DWORD dwAddr, LPVOID pBuffer);
typedef BOOL (*PFN_FLH_ReadID)(LPVOID pBuffer);
typedef BOOL (*PFN_FLH_BlockErase)(DWORD dwBlockAddr);
typedef BOOL (*PFN_FLH_SetSLCFlag)(BOOL bEnable);
typedef BOOL (*PFN_FLH_CPUReset)(void);
typedef BOOL (*PFN_FLH_InitCTRL)(void);
typedef BOOL (*PFN_FLH_WriteRootTableWithIspPath)(LPVOID pBuffer);
typedef BOOL (*PFN_FLH_ScanE2NANDBlockPerChip)(LPVOID pParams);
typedef BOOL (*PFN_FLH_ReadBCM)(LPVOID pBuffer);
typedef BOOL (*PFN_FLH_InitCodeWithIspPath)(LPVOID pParams);
typedef BOOL (*PFN_FLH_GetChannelCeNoAndMap)(LPVOID pParams);
typedef BOOL (*PFN_FLH_InitCodeForReady)(void);

// =============================================================================
// Security (SEC_*) - Authentication and password management - 7 functions
// =============================================================================
typedef BOOL (*PFN_SEC_DoAuthentication)(LPCSTR pszPassword);
typedef BOOL (*PFN_SEC_LeaveAuthenticatedState)(void);
typedef BOOL (*PFN_SEC_GetPasswordHint)(LPSTR pszHint, DWORD dwSize);
typedef BOOL (*PFN_SEC_SetPasswordHint)(LPCSTR pszHint);
typedef BOOL (*PFN_SEC_ChangePassword)(LPCSTR pszOldPass, LPCSTR pszNewPass);
typedef BOOL (*PFN_SEC_GetUserPassword)(LPSTR pszPassword, DWORD dwSize);
typedef BOOL (*PFN_SEC_GetEncryptedPassword)(LPVOID pBuffer, DWORD dwSize);

// =============================================================================
// Logical Unit Number (LUN_*) - Partition management - 7 functions
// =============================================================================
typedef BOOL (*PFN_LUN_CreateLun)(LPVOID pLunConfig);
typedef DWORD (*PFN_LUN_FindLunStartLBAByItemID)(DWORD dwItemID);
typedef BOOL (*PFN_LUN_CreateApLunNewItemID)(LPVOID pParams);
typedef BOOL (*PFN_LUN_WriteBadBlockMapToApLun)(LPVOID pBadBlockMap);
typedef BOOL (*PFN_LUN_ReadBadBlockMapFromApLun)(LPVOID pBadBlockMap);
typedef DWORD (*PFN_LUN_FindOptimumOffsetCap)(LPVOID pParams);
typedef DWORD (*PFN_LUN_CalIsoSize)(LPVOID pParams);

// =============================================================================
// Format (FMT_*) - Device formatting operations - 4 functions
// =============================================================================
typedef BOOL (*PFN_FMT_Format)(LPVOID pFormatParams);
typedef DWORD (*PFN_FMT_GetOptimumCapacity)(void);
typedef BOOL (*PFN_FMT_GetOptimumLunConfig)(LPVOID pLunConfig);
typedef DWORD (*PFN_FMT_GetOSCapacity)(void);

// =============================================================================
// Standard Operations (STD_*) - SCSI-like standard commands - 4 functions
// =============================================================================
typedef BOOL (*PFN_STD_Inquiry)(LPVOID pInquiryData);
typedef BOOL (*PFN_STD_ReadCapacity)(LPVOID pCapacityData);
typedef BOOL (*PFN_STD_LogicalRead)(DWORD dwLBA, DWORD dwSectors, LPVOID pBuffer);
typedef BOOL (*PFN_STD_LogicalWrite)(DWORD dwLBA, DWORD dwSectors, LPVOID pBuffer);

// =============================================================================
// Vendor Specific (VDR_*) - ITE-specific operations - 35 functions
// =============================================================================
typedef BOOL (*PFN_VDR_ReadWriteLUNConfig)(BOOL bWrite, LPVOID pConfig);
typedef BOOL (*PFN_VDR_ReadLUNData)(DWORD dwLUN, LPVOID pBuffer, DWORD dwSize);
typedef BOOL (*PFN_VDR_WriteLUNData)(DWORD dwLUN, LPVOID pBuffer, DWORD dwSize);
typedef BOOL (*PFN_VDR_ReadXData)(LPVOID pBuffer, DWORD dwSize);
typedef BOOL (*PFN_VDR_WriteXData)(LPVOID pBuffer, DWORD dwSize);
typedef BOOL (*PFN_VDR_ReadIData)(LPVOID pBuffer, DWORD dwSize);
typedef BOOL (*PFN_VDR_WriteIData)(LPVOID pBuffer, DWORD dwSize);
typedef BOOL (*PFN_VDR_ReadSysAddr)(DWORD dwAddr, LPVOID pBuffer);
typedef BOOL (*PFN_VDR_WriteSysAddr)(DWORD dwAddr, LPVOID pBuffer);
typedef BOOL (*PFN_VDR_CheckSYSReady)(void);
typedef BOOL (*PFN_VDR_SetSYSReady)(BOOL bReady);
typedef DWORD (*PFN_VDR_EndCode)(void);
typedef BOOL (*PFN_VDR_DeviceChange)(void);
typedef BOOL (*PFN_VDR_MediaChange)(void);
typedef BOOL (*PFN_VDR_WriteProtect)(BOOL bProtect);
typedef BOOL (*PFN_VDR_RWCurrentLUNType)(BOOL bWrite, LPVOID pType);
typedef BOOL (*PFN_VDR_HiddenArea)(BOOL bEnable);
typedef BOOL (*PFN_VDR_ReadWriteLUNNo)(BOOL bWrite, LPVOID pLunNo);
typedef BOOL (*PFN_VDR_ReadLUNID)(DWORD dwLUN, LPVOID pID);
typedef BOOL (*PFN_VDR_WriteLUNID)(DWORD dwLUN, LPVOID pID);
typedef BOOL (*PFN_VDR_ReadLUNIndex)(LPVOID pIndex);
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

// =============================================================================
// Utility Functions - Data conversion and device info - 9 functions
// =============================================================================
typedef DWORD (*PFN_SwapDWORD)(DWORD dwValue);
typedef WORD (*PFN_SwapWORD)(WORD wValue);
typedef DWORD (*PFN_CCBAddress2RawAddress)(DWORD dwCCBAddr);
typedef DWORD (*PFN_RawAddress2CCBAddress)(DWORD dwRawAddr);
typedef DWORD (*PFN_CCBAddress2ED3Address)(DWORD dwCCBAddr);
typedef DWORD (*PFN_ED3Address2CCBAddress)(DWORD dwED3Addr);
typedef DWORD (*PFN_BlkAddr2RawAddr)(DWORD dwBlockAddr);
typedef BOOL (*PFN_Is168Device)(void);
typedef DWORD (*PFN_GetLastestPage)(void);

// =============================================================================
// Mass Production (MP_*) - Factory operations - 2 functions
// =============================================================================
typedef BOOL (*PFN_MP_CreateSystem)(LPVOID pParams);
typedef BOOL (*PFN_MP_EraseSystemTable)(void);

// =============================================================================
// Diagnostics (DG_*) - Debug and diagnostic functions - 3 functions
// =============================================================================
typedef BOOL (*PFN_DG_GetBlockPageMapFromFlash)(LPVOID pMap);
typedef BOOL (*PFN_DG_SearchReadBadTBlk)(LPVOID pParams);
typedef DWORD (*PFN_DG_CalBlkRequire)(LPVOID pParams);

// =============================================================================
// Address Functions (ADDR_*) - Alternative address-based access - 3 functions
// =============================================================================
typedef BOOL (*PFN_ADDR_ReadRootTable)(LPVOID pBuffer);
typedef BOOL (*PFN_ADDR_ReadISPData)(LPVOID pBuffer);
typedef BOOL (*PFN_ADDR_ReadCISData)(LPVOID pBuffer);

// =============================================================================
// Global Function Pointers - Initialized by iTEUFDrs::LoadSDKFunctions
// =============================================================================
extern PFN_FLH_GetInfoFromDataBaseByID g_pFLH_GetInfoFromDataBaseByID;
extern PFN_FLH_GetFlashDataFromDataBase g_pFLH_GetFlashDataFromDataBase;
extern PFN_FLH_GetFlashDataFromMemory g_pFLH_GetFlashDataFromMemory;
extern PFN_FLH_ReadRootTable g_pFLH_ReadRootTable;
extern PFN_FLH_WriteRootTable g_pFLH_WriteRootTable;
extern PFN_FLH_ReadCISTable g_pFLH_ReadCISTable;
extern PFN_FLH_WriteCISTable g_pFLH_WriteCISTable;
extern PFN_FLH_ReadISPData g_pFLH_ReadISPData;
extern PFN_FLH_WriteISPData g_pFLH_WriteISPData;
extern PFN_FLH_ReadLatestWBT g_pFLH_ReadLatestWBT;
extern PFN_FLH_FindRootTable g_pFLH_FindRootTable;
extern PFN_FLH_LBA2PhysicalFlash g_pFLH_LBA2PhysicalFlash;
extern PFN_FLH_SetLedBlink g_pFLH_SetLedBlink;
extern PFN_FLH_PhyiscalRead g_pFLH_PhyiscalRead;
extern PFN_FLH_PhyiscalWrite g_pFLH_PhyiscalWrite;
extern PFN_FLH_IsGoodBlock g_pFLH_IsGoodBlock;
extern PFN_FLH_IsTableBlock g_pFLH_IsTableBlock;
extern PFN_FLH_MarkBad g_pFLH_MarkBad;
extern PFN_FLH_GetRealBlocksPerDie g_pFLH_GetRealBlocksPerDie;
extern PFN_FLH_BlockIsGap g_pFLH_BlockIsGap;

extern PFN_SEC_DoAuthentication g_pSEC_DoAuthentication;
extern PFN_SEC_LeaveAuthenticatedState g_pSEC_LeaveAuthenticatedState;
extern PFN_SEC_GetPasswordHint g_pSEC_GetPasswordHint;
extern PFN_SEC_SetPasswordHint g_pSEC_SetPasswordHint;
extern PFN_SEC_ChangePassword g_pSEC_ChangePassword;
extern PFN_SEC_GetUserPassword g_pSEC_GetUserPassword;
extern PFN_SEC_GetEncryptedPassword g_pSEC_GetEncryptedPassword;

extern PFN_LUN_CreateLun g_pLUN_CreateLun;
extern PFN_LUN_FindLunStartLBAByItemID g_pLUN_FindLunStartLBAByItemID;
extern PFN_LUN_CreateApLunNewItemID g_pLUN_CreateApLunNewItemID;

extern PFN_FMT_Format g_pFMT_Format;
extern PFN_FMT_GetOptimumCapacity g_pFMT_GetOptimumCapacity;
extern PFN_FMT_GetOptimumLunConfig g_pFMT_GetOptimumLunConfig;
extern PFN_FMT_GetOSCapacity g_pFMT_GetOSCapacity;

extern PFN_STD_Inquiry g_pSTD_Inquiry;
extern PFN_STD_ReadCapacity g_pSTD_ReadCapacity;
extern PFN_STD_LogicalRead g_pSTD_LogicalRead;
extern PFN_STD_LogicalWrite g_pSTD_LogicalWrite;

// ... Additional function pointers for VDR, MP, DG, ADDR categories

// Helper function for loading SDK functions
BOOL LoadSDKFunctions(HMODULE hSDK);

#ifdef __cplusplus
}
#endif
