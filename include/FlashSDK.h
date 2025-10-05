#pragma once

// This file contains reconstructed function definitions for the 181FlashSDK.dll
// Based on reverse engineering of URescue_v81D.2.24.2.exe
// All typedefs have been moved to SDKAPIs.h to prevent redefinition errors.

#include "SDKAPIs.h"  // Include the single source of truth for typedefs

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// Global Function Pointers - Initialized by InitializeFlashSDK
// =============================================================================

// Direct function pointer declarations used by SDKLoader.cpp
extern PFN_FLH_GetInfoFromDataBaseByID FLH_GetInfoFromDataBaseByID;
extern PFN_FLH_GetFlashDataFromDataBase FLH_GetFlashDataFromDataBase;
extern PFN_FLH_GetFlashDataFromMemory FLH_GetFlashDataFromMemory;
extern PFN_FLH_ReadRootTable FLH_ReadRootTable;
extern PFN_FLH_WriteRootTable FLH_WriteRootTable;
extern PFN_FLH_ReadCISTable FLH_ReadCISTable;
extern PFN_FLH_WriteCISTable FLH_WriteCISTable;
extern PFN_FLH_ReadISPData FLH_ReadISPData;
extern PFN_FLH_WriteISPData FLH_WriteISPData;
extern PFN_FLH_ReadLatestWBT FLH_ReadLatestWBT;
extern PFN_FLH_FindRootTable FLH_FindRootTable;
extern PFN_FLH_LBA2PhysicalFlash FLH_LBA2PhysicalFlash;
extern PFN_FLH_SetLedBlink FLH_SetLedBlink;
extern PFN_FLH_PhyiscalRead FLH_PhyiscalRead;
extern PFN_FLH_PhyiscalWrite FLH_PhyiscalWrite;
extern PFN_FLH_IsGoodBlock FLH_IsGoodBlock;
extern PFN_FLH_IsTableBlock FLH_IsTableBlock;
extern PFN_FLH_MarkBad FLH_MarkBad;
extern PFN_FLH_GetRealBlocksPerDie FLH_GetRealBlocksPerDie;
extern PFN_FLH_BlockIsGap FLH_BlockIsGap;
extern PFN_FLH_HandleMassBlocksPerChip FLH_HandleMassBlocksPerChip;
extern PFN_FLH_ScanNewBlock FLH_ScanNewBlock;
extern PFN_FLH_GetRetryRegister FLH_GetRetryRegister;
extern PFN_FLH_CISCheckSum_Calculate FLH_CISCheckSum_Calculate;
extern PFN_FLH_CalCulate_ECCNO FLH_CalCulate_ECCNO;
extern PFN_FLH_ArrangeSegmentPara FLH_ArrangeSegmentPara;
extern PFN_FLH_ReadSpare FLH_ReadSpare;
extern PFN_FLH_ReadID FLH_ReadID;
extern PFN_FLH_BlockErase FLH_BlockErase;
extern PFN_FLH_SetSLCFlag FLH_SetSLCFlag;
extern PFN_FLH_CPUReset FLH_CPUReset;
extern PFN_FLH_InitCTRL FLH_InitCTRL;
extern PFN_FLH_WriteRootTableWithIspPath FLH_WriteRootTableWithIspPath;
extern PFN_FLH_ScanE2NANDBlockPerChip FLH_ScanE2NANDBlockPerChip;
extern PFN_FLH_ReadBCM FLH_ReadBCM;
extern PFN_FLH_InitCodeWithIspPath FLH_InitCodeWithIspPath;
extern PFN_FLH_GetChannelCeNoAndMap FLH_GetChannelCeNoAndMap;
extern PFN_FLH_InitCodeForReady FLH_InitCodeForReady;

// VDR functions
extern PFN_VDR_LoadDriver VDR_LoadDriver;
extern PFN_VDR_FreeDriver VDR_FreeDriver;
extern PFN_VDR_ReadWriteLUNConfig VDR_ReadWriteLUNConfig;
extern PFN_VDR_GetSystemAddr VDR_GetSystemAddr;
// Note: VDR_GetDeviceInquiry, VDR_CheckDeviceSupport, VDR_GetLunIndex, VDR_GetDeviceID
// are defined elsewhere with different signatures
extern PFN_VDR_ReadSysAddr VDR_ReadSysAddr;
extern PFN_VDR_ReadLUNData VDR_ReadLUNData;
extern PFN_VDR_WriteLUNData VDR_WriteLUNData;
extern PFN_VDR_ReadXData VDR_ReadXData;
extern PFN_VDR_WriteXData VDR_WriteXData;
extern PFN_VDR_ReadIData VDR_ReadIData;
extern PFN_VDR_WriteIData VDR_WriteIData;
extern PFN_VDR_WriteSysAddr VDR_WriteSysAddr;
extern PFN_VDR_CheckSYSReady VDR_CheckSYSReady;
extern PFN_VDR_SetSYSReady VDR_SetSYSReady;
extern PFN_VDR_EndCode VDR_EndCode;
extern PFN_VDR_DeviceChange VDR_DeviceChange;
extern PFN_VDR_MediaChange VDR_MediaChange;
extern PFN_VDR_WriteProtect VDR_WriteProtect;
extern PFN_VDR_RWCurrentLUNType VDR_RWCurrentLUNType;
extern PFN_VDR_HiddenArea VDR_HiddenArea;
extern PFN_VDR_ReadWriteLUNNo VDR_ReadWriteLUNNo;
extern PFN_VDR_ReadLUNID VDR_ReadLUNID;
extern PFN_VDR_WriteLUNID VDR_WriteLUNID;
extern PFN_VDR_ReadLUNIndex VDR_ReadLUNIndex;
extern PFN_VDR_FlushCache VDR_FlushCache;
extern PFN_VDR_ReadPage VDR_ReadPage;
extern PFN_VDR_WritePage VDR_WritePage;
extern PFN_VDR_WriteBlock_TLC VDR_WriteBlock_TLC;
extern PFN_VDR_GetSecurityStatus VDR_GetSecurityStatus;
extern PFN_VDR_ED3PageRead VDR_ED3PageRead;
extern PFN_VDR_BadTFindRead VDR_BadTFindRead;
extern PFN_VDR_Enhance_SLC_Program VDR_Enhance_SLC_Program;
extern PFN_VDR_Disable_SLC_Program VDR_Disable_SLC_Program;
extern PFN_VDR_MassBlocksProcess VDR_MassBlocksProcess;
extern PFN_VDR_F_RST VDR_F_RST;
extern PFN_VDR_RootFunc VDR_RootFunc;
extern PFN_VDR_RootPageWrite VDR_RootPageWrite;
extern PFN_VDR_RootAccess VDR_RootAccess;

// Security functions
extern PFN_SEC_DoAuthentication SEC_DoAuthentication;
extern PFN_SEC_LeaveAuthenticatedState SEC_LeaveAuthenticatedState;
extern PFN_SEC_GetPasswordHint SEC_GetPasswordHint;
extern PFN_SEC_SetPasswordHint SEC_SetPasswordHint;
extern PFN_SEC_ChangePassword SEC_ChangePassword;
extern PFN_SEC_GetUserPassword SEC_GetUserPassword;
extern PFN_SEC_GetEncryptedPassword SEC_GetEncryptedPassword;

// LUN functions
extern PFN_LUN_CreateLun LUN_CreateLun;
extern PFN_LUN_FindLunStartLBAByItemID LUN_FindLunStartLBAByItemID;
extern PFN_LUN_CreateApLunNewItemID LUN_CreateApLunNewItemID;
extern PFN_LUN_WriteBadBlockMapToApLun LUN_WriteBadBlockMapToApLun;
extern PFN_LUN_ReadBadBlockMapFromApLun LUN_ReadBadBlockMapFromApLun;
extern PFN_LUN_FindOptimumOffsetCap LUN_FindOptimumOffsetCap;
extern PFN_LUN_CalIsoSize LUN_CalIsoSize;

// Format functions
extern PFN_FMT_Format FMT_Format;
extern PFN_FMT_GetOptimumCapacity FMT_GetOptimumCapacity;
extern PFN_FMT_GetOptimumLunConfig FMT_GetOptimumLunConfig;
extern PFN_FMT_GetOSCapacity FMT_GetOSCapacity;

// Standard functions
extern PFN_STD_Inquiry STD_Inquiry;
extern PFN_STD_ReadCapacity STD_ReadCapacity;
extern PFN_STD_LogicalRead STD_LogicalRead;
extern PFN_STD_LogicalWrite STD_LogicalWrite;

// Utility functions
extern PFN_SwapDWORD SwapDWORD;
extern PFN_SwapWORD SwapWORD;
extern PFN_CCBAddress2RawAddress CCBAddress2RawAddress;
extern PFN_RawAddress2CCBAddress RawAddress2CCBAddress;
extern PFN_CCBAddress2ED3Address CCBAddress2ED3Address;
extern PFN_ED3Address2CCBAddress ED3Address2CCBAddress;
extern PFN_BlkAddr2RawAddr BlkAddr2RawAddr;
extern PFN_Is168Device Is168Device;
extern PFN_GetLastestPage GetLastestPage;

// Mass Production functions
extern PFN_MP_CreateSystem MP_CreateSystem;
extern PFN_MP_EraseSystemTable MP_EraseSystemTable;

// Diagnostics functions
extern PFN_DG_GetBlockPageMapFromFlash DG_GetBlockPageMapFromFlash;
extern PFN_DG_SearchReadBadTBlk DG_SearchReadBadTBlk;
extern PFN_DG_CalBlkRequire DG_CalBlkRequire;

// Address functions
extern PFN_ADDR_ReadRootTable ADDR_ReadRootTable;
extern PFN_ADDR_ReadISPData ADDR_ReadISPData;
extern PFN_ADDR_ReadCISData ADDR_ReadCISData;

// Helper function for loading SDK functions
BOOL LoadSDKFunctions(HMODULE hSDK);

#ifdef __cplusplus
}
#endif
