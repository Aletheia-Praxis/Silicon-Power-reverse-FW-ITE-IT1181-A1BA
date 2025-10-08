#pragma once

#include <windows.h>

// SDK function pointer type (moved from iTEUFDrs_Ghidra.h)
typedef FARPROC SDK_FUNCTION_PTR;

// Global SDK function pointers used by LoadSDKFunctions
// These are defined in Globals.cpp

// Flash Layer Helper (FLH) function pointers
extern SDK_FUNCTION_PTR g_pFLH_GetInfoFromDataBaseByID;
extern SDK_FUNCTION_PTR g_pFLH_GetFlashDataFromDataBase;
extern SDK_FUNCTION_PTR g_pFLH_GetFlashDataFromMemory;
extern SDK_FUNCTION_PTR g_pFLH_ReadRootTable;
extern SDK_FUNCTION_PTR g_pFLH_WriteRootTable;
extern SDK_FUNCTION_PTR g_pFLH_ReadCISTable;
extern SDK_FUNCTION_PTR g_pFLH_WriteCISTable;
extern SDK_FUNCTION_PTR g_pFLH_ReadISPData;
extern SDK_FUNCTION_PTR g_pFLH_WriteISPData;
extern SDK_FUNCTION_PTR g_pFLH_ReadLatestWBT;
extern SDK_FUNCTION_PTR g_pFLH_FindRootTable;
extern SDK_FUNCTION_PTR g_pFLH_LBA2PhysicalFlash;
extern SDK_FUNCTION_PTR g_pFLH_SetLedBlink;
extern SDK_FUNCTION_PTR g_pFLH_PhyiscalRead;
extern SDK_FUNCTION_PTR g_pFLH_PhyiscalWrite;
extern SDK_FUNCTION_PTR g_pFLH_IsGoodBlock;
extern SDK_FUNCTION_PTR g_pFLH_IsTableBlock;
extern SDK_FUNCTION_PTR g_pFLH_MarkBad;
extern SDK_FUNCTION_PTR g_pFLH_GetRealBlocksPerDie;
extern SDK_FUNCTION_PTR g_pFLH_BlockIsGap;
extern SDK_FUNCTION_PTR g_pFLH_HandleMassBlocksPerChip;
extern SDK_FUNCTION_PTR g_pFLH_ScanNewBlock;
extern SDK_FUNCTION_PTR g_pFLH_GetRetryRegister;
extern SDK_FUNCTION_PTR g_pFLH_CISCheckSum_Calculate;
extern SDK_FUNCTION_PTR g_pFLH_CalCulate_ECCNO;
extern SDK_FUNCTION_PTR g_pFLH_ArrangeSegmentPara;
extern SDK_FUNCTION_PTR g_pFLH_ReadSpare;
extern SDK_FUNCTION_PTR g_pFLH_ReadID;
extern SDK_FUNCTION_PTR g_pFLH_BlockErase;
extern SDK_FUNCTION_PTR g_pFLH_SetSLCFlag;
extern SDK_FUNCTION_PTR g_pFLH_CPUReset;
extern SDK_FUNCTION_PTR g_pFLH_InitCTRL;

// Vendor Driver (VDR) function pointers
extern SDK_FUNCTION_PTR g_pVDR_ReadWriteLUNConfig;
extern SDK_FUNCTION_PTR g_pVDR_ReadLUNData;
extern SDK_FUNCTION_PTR g_pVDR_WriteLUNData;
extern SDK_FUNCTION_PTR g_pVDR_ReadXData;
extern SDK_FUNCTION_PTR g_pVDR_WriteXData;
extern SDK_FUNCTION_PTR g_pVDR_ReadIData;
extern SDK_FUNCTION_PTR g_pVDR_WriteIData;
extern SDK_FUNCTION_PTR g_pVDR_ReadSysAddr;
extern SDK_FUNCTION_PTR g_pVDR_WriteSysAddr;
extern SDK_FUNCTION_PTR g_pVDR_SetSYSReady;
extern SDK_FUNCTION_PTR g_pVDR_EndCode;
extern SDK_FUNCTION_PTR g_pVDR_DeviceChange;
extern SDK_FUNCTION_PTR g_pVDR_MediaChange;
extern SDK_FUNCTION_PTR g_pVDR_WriteProtect;
extern SDK_FUNCTION_PTR g_pVDR_RWCurrentLUNType;
extern SDK_FUNCTION_PTR g_pVDR_HiddenArea;
extern SDK_FUNCTION_PTR g_pVDR_ReadWriteLUNNo;
extern SDK_FUNCTION_PTR g_pVDR_FlushCache;
extern SDK_FUNCTION_PTR g_pVDR_ReadPage;
extern SDK_FUNCTION_PTR g_pVDR_WritePage;
extern SDK_FUNCTION_PTR g_pVDR_WriteBlock_TLC;
extern SDK_FUNCTION_PTR g_pVDR_GetSecurityStatus;
extern SDK_FUNCTION_PTR g_pVDR_ED3PageRead;
extern SDK_FUNCTION_PTR g_pVDR_BadTFindRead;
extern SDK_FUNCTION_PTR g_pVDR_Enhance_SLC_Program;
extern SDK_FUNCTION_PTR g_pVDR_Disable_SLC_Program;
extern SDK_FUNCTION_PTR g_pVDR_MassBlocksProcess;
extern SDK_FUNCTION_PTR g_pVDR_F_RST;
extern SDK_FUNCTION_PTR g_pVDR_RootFunc;
extern SDK_FUNCTION_PTR g_pVDR_RootPageWrite;
extern SDK_FUNCTION_PTR g_pVDR_RootAccess;

// Additional STD function pointers (used with VDR functions)
extern SDK_FUNCTION_PTR g_pSTD_GetDeviceID;
extern SDK_FUNCTION_PTR g_pSTD_SetDeviceID;
extern SDK_FUNCTION_PTR g_pSTD_GetLUNIndex;

// Security (SEC) function pointers
extern SDK_FUNCTION_PTR g_pSEC_DoAuthentication;
extern SDK_FUNCTION_PTR g_pSEC_LeaveAuthenticatedState;
extern SDK_FUNCTION_PTR g_pSEC_GetPasswordHint;
extern SDK_FUNCTION_PTR g_pSEC_SetPasswordHint;
extern SDK_FUNCTION_PTR g_pSEC_ChangePassword;
extern SDK_FUNCTION_PTR g_pSEC_GetUserPassword;
extern SDK_FUNCTION_PTR g_pSEC_GetEncryptedPassword;

// LUN function pointers
extern SDK_FUNCTION_PTR g_pLUN_CreateLun;
extern SDK_FUNCTION_PTR g_pLUN_FindLunStartLBAByItemID;
extern SDK_FUNCTION_PTR g_pLUN_CreateApLunNewItemID;
extern SDK_FUNCTION_PTR g_pLUN_WriteBadBlockMapToApLun;
extern SDK_FUNCTION_PTR g_pLUN_ReadBadBlockMapFromApLun;
extern SDK_FUNCTION_PTR g_pLUN_FindOptimumOffsetCap;
extern SDK_FUNCTION_PTR g_pLUN_CalIsoSize;

// Format (FMT) function pointers
extern SDK_FUNCTION_PTR g_pFMT_Format;
extern SDK_FUNCTION_PTR g_pFMT_GetOptimumCapacity;
extern SDK_FUNCTION_PTR g_pFMT_GetOptimumLunConfig;
extern SDK_FUNCTION_PTR g_pFMT_GetOSCapacity;

// Standard (STD) function pointers
extern SDK_FUNCTION_PTR g_pSTD_Read_ID_New;
extern SDK_FUNCTION_PTR g_pSTD_Read_ID;
extern SDK_FUNCTION_PTR g_pSTD_Inquiry;
extern SDK_FUNCTION_PTR g_pSTD_ReadCapacity;
extern SDK_FUNCTION_PTR g_pSTD_LogicalRead;
extern SDK_FUNCTION_PTR g_pSTD_LogicalWrite;
extern SDK_FUNCTION_PTR g_pSTD_TestUnitReady;

// Utility function pointers
extern SDK_FUNCTION_PTR g_pSwapDWORD;
extern SDK_FUNCTION_PTR g_pSwapWORD;
extern SDK_FUNCTION_PTR g_pCCBAddress2RawAddress;
extern SDK_FUNCTION_PTR g_pRawAddress2CCBAddress;
extern SDK_FUNCTION_PTR g_pCCBAddress2ED3Address;
extern SDK_FUNCTION_PTR g_pED3Address2CCBAddress;
extern SDK_FUNCTION_PTR g_pBlkAddr2RawAddr;
extern SDK_FUNCTION_PTR g_pIs168Device;
extern SDK_FUNCTION_PTR g_pGetLastestPage;
extern SDK_FUNCTION_PTR g_pRawAddr2BlkAddr;
extern SDK_FUNCTION_PTR g_pGetLastestBlockPerChannel;
extern SDK_FUNCTION_PTR g_pGetECCLevelFromFlashDBID;
extern SDK_FUNCTION_PTR g_pIs3DTLC;
extern SDK_FUNCTION_PTR g_pReadRetry;
extern SDK_FUNCTION_PTR g_pWriteRetry;

// Additional FLH function pointers (continued)
extern SDK_FUNCTION_PTR g_pFLH_WriteRootTableWithIspPath;
extern SDK_FUNCTION_PTR g_pFLH_ScanE2NANDBlockPerChip;
extern SDK_FUNCTION_PTR g_pFLH_ReadBCM;
extern SDK_FUNCTION_PTR g_pFLH_InitCodeWithIspPath;
extern SDK_FUNCTION_PTR g_pFLH_GetChannelCeNoAndMap;
extern SDK_FUNCTION_PTR g_pFLH_InitCodeForReady;

// Mass Production (MP) function pointers
extern SDK_FUNCTION_PTR g_pMP_CreateSystem;
extern SDK_FUNCTION_PTR g_pMP_EraseSystemTable;

// Diagnostic (DG) function pointers
extern SDK_FUNCTION_PTR g_pDG_GetBlockPageMapFromFlash;
extern SDK_FUNCTION_PTR g_pDG_SearchReadBadTBlk;
extern SDK_FUNCTION_PTR g_pDG_CalBlkRequire;

// Address (ADDR) function pointers
extern SDK_FUNCTION_PTR g_pADDR_ReadRootTable;
extern SDK_FUNCTION_PTR g_pADDR_ReadISPData;
extern SDK_FUNCTION_PTR g_pADDR_ReadCISData;
