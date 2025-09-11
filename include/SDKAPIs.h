#pragma once
#include "WindowsHeaders.h"

// 181FlashSDK.dll API function pointers (decompiled from FUN_00401000)
// These correspond to the global DAT_ variables in the decompiled code

// Virtual Device Recognition (VDR) APIs
extern FARPROC g_VDR_LoadDriver;
extern FARPROC g_VDR_FreeDriver;
extern FARPROC g_VDR_ReadWriteLUNConfig;
extern FARPROC g_VDR_GetSystemAddr;
extern FARPROC g_VDR_GetDeviceInquiry;
extern FARPROC g_VDR_CheckDeviceSupport;
extern FARPROC g_VDR_GetLunIndex;
extern FARPROC g_VDR_GetDeviceID;
extern FARPROC g_VDR_ReadSysAddr;

// Flash Layer Helper (FLH) APIs
extern FARPROC g_FLH_GetInfoFromDataBaseByID;
extern FARPROC g_FLH_GetFlashDataFromDataBase;
extern FARPROC g_FLH_GetFlashDataFromMemory;
extern FARPROC g_FLH_ReadRootTable;
extern FARPROC g_FLH_WriteRootTable;
extern FARPROC g_FLH_ReadCISTable;
extern FARPROC g_FLH_WriteCISTable;
extern FARPROC g_FLH_ReadISPData;
extern FARPROC g_FLH_WriteISPData;
extern FARPROC g_FLH_ReadLatestWBT;
extern FARPROC g_FLH_FindRootTable;
extern FARPROC g_FLH_LBA2PhysicalFlash;
extern FARPROC g_FLH_SetLedBlink;
extern FARPROC g_FLH_PhyiscalRead;
extern FARPROC g_FLH_PhyiscalWrite;
extern FARPROC g_FLH_IsGoodBlock;
extern FARPROC g_FLH_IsTableBlock;
extern FARPROC g_FLH_MarkBad;
extern FARPROC g_FLH_GetRealBlocksPerDie;
extern FARPROC g_FLH_BlockIsGap;
extern FARPROC g_FLH_HandleMassBlocksPerChip;
extern FARPROC g_FLH_ScanNewBlock;
extern FARPROC g_FLH_GetRetryRegister;
extern FARPROC g_FLH_CISCheckSum_Calculate;
extern FARPROC g_FLH_CalCulate_ECCNO;
extern FARPROC g_FLH_ArrangeSegmentPara;
extern FARPROC g_FLH_ReadSpare;
extern FARPROC g_FLH_ReadID;
extern FARPROC g_FLH_BlockErase;
extern FARPROC g_FLH_SetSLCFlag;
extern FARPROC g_FLH_CPUReset;
extern FARPROC g_FLH_InitCTRL;
extern FARPROC g_FLH_WriteRootTableWithIspPath;
extern FARPROC g_FLH_ScanE2NANDBlockPerChip;
extern FARPROC g_FLH_ReadBCM;
extern FARPROC g_FLH_InitCodeWithIspPath;
extern FARPROC g_FLH_GetChannelCeNoAndMap;
extern FARPROC g_FLH_InitCodeForReady;

// Security (SEC) APIs
extern FARPROC g_SEC_DoAuthentication;
extern FARPROC g_SEC_LeaveAuthenticatedState;
extern FARPROC g_SEC_GetPasswordHint;
extern FARPROC g_SEC_SetPasswordHint;
extern FARPROC g_SEC_ChangePassword;
extern FARPROC g_SEC_GetUserPassword;
extern FARPROC g_SEC_GetEncryptedPassword;

// LUN (Logical Unit Number) APIs
extern FARPROC g_LUN_CreateLun;
extern FARPROC g_LUN_FindLunStartLBAByItemID;
extern FARPROC g_LUN_CreateApLunNewItemID;
extern FARPROC g_LUN_WriteBadBlockMapToApLun;
extern FARPROC g_LUN_ReadBadBlockMapFromApLun;
extern FARPROC g_LUN_FindOptimumOffsetCap;
extern FARPROC g_LUN_CalIsoSize;

// Format (FMT) APIs
extern FARPROC g_FMT_Format;
extern FARPROC g_FMT_GetOptimumCapacity;
extern FARPROC g_FMT_GetOptimumLunConfig;
extern FARPROC g_FMT_GetOSCapacity;

// Standard (STD) APIs
extern FARPROC g_STD_Inquiry;
extern FARPROC g_STD_ReadCapacity;
extern FARPROC g_STD_LogicalRead;
extern FARPROC g_STD_LogicalWrite;

// Utility APIs
extern FARPROC g_SwapDWORD;
extern FARPROC g_SwapWORD;
extern FARPROC g_CCBAddress2RawAddress;
extern FARPROC g_RawAddress2CCBAddress;
extern FARPROC g_CCBAddress2ED3Address;
extern FARPROC g_ED3Address2CCBAddress;
extern FARPROC g_BlkAddr2RawAddr;

// Vendor Data Record (VDR) APIs
extern FARPROC g_VDR_ReadWriteLUNConfig;
extern FARPROC g_VDR_ReadLUNData;
extern FARPROC g_VDR_WriteLUNData;
extern FARPROC g_VDR_ReadXData;
extern FARPROC g_VDR_WriteXData;
extern FARPROC g_VDR_ReadIData;
extern FARPROC g_VDR_WriteIData;
extern FARPROC g_VDR_ReadSysAddr;
extern FARPROC g_VDR_WriteSysAddr;
extern FARPROC g_VDR_CheckSYSReady;
extern FARPROC g_VDR_SetSYSReady;
extern FARPROC g_VDR_EndCode;
extern FARPROC g_VDR_DeviceChange;
extern FARPROC g_VDR_MediaChange;
extern FARPROC g_VDR_WriteProtect;
extern FARPROC g_VDR_RWCurrentLUNType;
extern FARPROC g_VDR_HiddenArea;
extern FARPROC g_VDR_ReadWriteLUNNo;
extern FARPROC g_VDR_ReadLUNID;
extern FARPROC g_VDR_WriteLUNID;
extern FARPROC g_VDR_ReadLUNIndex;
extern FARPROC g_VDR_FlushCache;
extern FARPROC g_VDR_ReadPage;
extern FARPROC g_VDR_WritePage;
extern FARPROC g_VDR_WriteBlock_TLC;
extern FARPROC g_VDR_GetSecurityStatus;
extern FARPROC g_VDR_ED3PageRead;
extern FARPROC g_VDR_BadTFindRead;
extern FARPROC g_VDR_Enhance_SLC_Program;
extern FARPROC g_VDR_Disable_SLC_Program;
extern FARPROC g_VDR_MassBlocksProcess;
extern FARPROC g_VDR_F_RST;
extern FARPROC g_VDR_RootFunc;
extern FARPROC g_VDR_RootPageWrite;
extern FARPROC g_VDR_RootAccess;

// Mass Production (MP) APIs
extern FARPROC g_MP_CreateSystem;
extern FARPROC g_MP_EraseSystemTable;

// Data Gathering (DG) APIs
extern FARPROC g_DG_GetBlockPageMapFromFlash;
extern FARPROC g_DG_SearchReadBadTBlk;
extern FARPROC g_DG_CalBlkRequire;

// Address (ADDR) APIs
extern FARPROC g_ADDR_ReadRootTable;
extern FARPROC g_ADDR_ReadISPData;
extern FARPROC g_ADDR_ReadCISData;

// Device check APIs
extern FARPROC g_Is168Device;
extern FARPROC g_GetLastestPage;

// Function to bind all SDK APIs (decompiled from FUN_00401000)
BOOL BindSDKAPIs(HMODULE hSDK);

// Function to clear all API pointers
void ClearSDKAPIs();
