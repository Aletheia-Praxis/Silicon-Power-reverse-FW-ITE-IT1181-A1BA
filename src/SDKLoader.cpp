#include "SDKLoader.h"

#include <stdio.h>
#include <windows.h>

#include "FlashSDK.h"
#include "Utilities.h"

BOOL Load181FlashSDK(LPCSTR baseDir, HMODULE* outModule) {
    if(! baseDir || ! outModule)
        return FALSE;
    CHAR path[MAX_PATH] = { 0 };
    if(! JoinPathA(path, sizeof(path), baseDir, "181FlashSDK.dll"))
        return FALSE;
    HMODULE h = LoadLibraryA(path);
    if(! h) {
        LogMessage("Failed to load 181FlashSDK.dll from: %s", path);
        return FALSE;
    }
    *outModule = h;
    LogMessage("Loaded 181FlashSDK.dll from: %s", path);
    return TRUE;
}

BOOL InitializeFlashSDK(HMODULE hModule) {
    if(! hModule)
        return FALSE;

#define LOAD_PROC(name)                                   \
    name = (name##_t) GetProcAddress(hModule, #name);     \
    if(! name) {                                          \
        LogMessage("Failed to load function: %s", #name); \
        return FALSE;                                     \
    }

    LOAD_PROC(FLH_GetInfoFromDataBaseByID);
    LOAD_PROC(FLH_GetFlashDataFromDataBase);
    LOAD_PROC(FLH_GetFlashDataFromMemory);
    LOAD_PROC(FLH_ReadRootTable);
    LOAD_PROC(FLH_WriteRootTable);
    LOAD_PROC(FLH_ReadCISTable);
    LOAD_PROC(FLH_WriteCISTable);
    LOAD_PROC(FLH_ReadISPData);
    LOAD_PROC(FLH_WriteISPData);
    LOAD_PROC(FLH_ReadLatestWBT);
    LOAD_PROC(FLH_FindRootTable);
    LOAD_PROC(FLH_LBA2PhysicalFlash);
    LOAD_PROC(FLH_SetLedBlink);
    LOAD_PROC(FLH_PhyiscalRead);
    LOAD_PROC(FLH_PhyiscalWrite);
    LOAD_PROC(FLH_IsGoodBlock);
    LOAD_PROC(FLH_IsTableBlock);
    LOAD_PROC(FLH_MarkBad);
    LOAD_PROC(FLH_GetRealBlocksPerDie);
    LOAD_PROC(FLH_BlockIsGap);

    // VDR (Virtual Device Recognition) functions
    g_VDR_LoadDriver = GetProcAddress(hModule, "VDR_LoadDriver");
    g_VDR_FreeDriver = GetProcAddress(hModule, "VDR_FreeDriver");
    g_VDR_ReadWriteLUNConfig = GetProcAddress(hModule, "VDR_ReadWriteLUNConfig");
    g_VDR_GetSystemAddr = GetProcAddress(hModule, "VDR_GetSystemAddr");
    g_VDR_GetDeviceInquiry = GetProcAddress(hModule, "VDR_GetDeviceInquiry");
    g_VDR_CheckDeviceSupport = GetProcAddress(hModule, "VDR_CheckDeviceSupport");
    g_VDR_GetLunIndex = GetProcAddress(hModule, "VDR_GetLunIndex");
    g_VDR_GetDeviceID = GetProcAddress(hModule, "VDR_GetDeviceID");
    g_VDR_ReadSysAddr = GetProcAddress(hModule, "VDR_ReadSysAddr");

    LOAD_PROC(SEC_DoAuthentication);
    LOAD_PROC(SEC_LeaveAuthenticatedState);
    LOAD_PROC(SEC_GetPasswordHint);
    LOAD_PROC(SEC_SetPasswordHint);
    LOAD_PROC(SEC_ChangePassword);
    LOAD_PROC(LUN_CreateLun);
    LOAD_PROC(LUN_FindLunStartLBAByItemID);
    LOAD_PROC(LUN_CreateApLunNewItemID);
    LOAD_PROC(LUN_WriteBadBlockMapToApLun);
    LOAD_PROC(LUN_ReadBadBlockMapFromApLun);
    LOAD_PROC(LUN_FindOptimumOffsetCap);
    LOAD_PROC(FMT_Format);
    LOAD_PROC(FMT_GetOptimumCapacity);
    LOAD_PROC(FMT_GetOptimumLunConfig);
    LOAD_PROC(FMT_GetOSCapacity);
    LOAD_PROC(STD_Inquiry);
    LOAD_PROC(STD_ReadCapacity);
    LOAD_PROC(STD_LogicalRead);
    LOAD_PROC(STD_LogicalWrite);
    LOAD_PROC(SwapDWORD);
    LOAD_PROC(SwapWORD);
    LOAD_PROC(CCBAddress2RawAddress);
    LOAD_PROC(RawAddress2CCBAddress);
    LOAD_PROC(CCBAddress2ED3Address);
    LOAD_PROC(ED3Address2CCBAddress);
    LOAD_PROC(VDR_ReadWriteLUNConfig);
    LOAD_PROC(VDR_ReadLUNData);
    LOAD_PROC(VDR_WriteLUNData);
    LOAD_PROC(VDR_ReadXData);
    LOAD_PROC(VDR_WriteXData);
    LOAD_PROC(VDR_ReadIData);
    LOAD_PROC(VDR_WriteIData);
    LOAD_PROC(VDR_ReadSysAddr);
    LOAD_PROC(VDR_WriteSysAddr);
    LOAD_PROC(VDR_CheckSYSReady);
    LOAD_PROC(VDR_SetSYSReady);
    LOAD_PROC(VDR_EndCode);
    LOAD_PROC(VDR_DeviceChange);
    LOAD_PROC(VDR_MediaChange);
    LOAD_PROC(VDR_WriteProtect);
    LOAD_PROC(VDR_RWCurrentLUNType);
    LOAD_PROC(VDR_HiddenArea);
    LOAD_PROC(VDR_ReadWriteLUNNo);
    LOAD_PROC(VDR_ReadLUNID);
    LOAD_PROC(VDR_WriteLUNID);
    LOAD_PROC(VDR_ReadLUNIndex);
    LOAD_PROC(VDR_FlushCache);
    LOAD_PROC(VDR_ReadPage);
    LOAD_PROC(VDR_WritePage);
    LOAD_PROC(VDR_WriteBlock_TLC);
    LOAD_PROC(VDR_GetSecurityStatus);
    LOAD_PROC(MP_CreateSystem);
    LOAD_PROC(MP_EraseSystemTable);
    LOAD_PROC(DG_GetBlockPageMapFromFlash);
    LOAD_PROC(Is168Device);
    LOAD_PROC(GetLastestPage);
    LOAD_PROC(SEC_GetUserPassword);
    LOAD_PROC(SEC_GetEncryptedPassword);
    LOAD_PROC(FLH_HandleMassBlocksPerChip);
    LOAD_PROC(FLH_ScanNewBlock);
    LOAD_PROC(FLH_GetRetryRegister);
    LOAD_PROC(VDR_ED3PageRead);
    LOAD_PROC(VDR_BadTFindRead);
    LOAD_PROC(DG_SearchReadBadTBlk);
    LOAD_PROC(DG_CalBlkRequire);
    LOAD_PROC(VDR_Enhance_SLC_Program);
    LOAD_PROC(VDR_Disable_SLC_Program);
    LOAD_PROC(LUN_CalIsoSize);
    LOAD_PROC(FLH_CISCheckSum_Calculate);
    LOAD_PROC(FLH_CalCulate_ECCNO);
    LOAD_PROC(FLH_ArrangeSegmentPara);
    LOAD_PROC(ADDR_ReadRootTable);
    LOAD_PROC(FLH_ReadSpare);
    LOAD_PROC(FLH_ReadID);
    LOAD_PROC(FLH_BlockErase);
    LOAD_PROC(FLH_SetSLCFlag);
    LOAD_PROC(BlkAddr2RawAddr);
    LOAD_PROC(VDR_MassBlocksProcess);
    LOAD_PROC(VDR_F_RST);
    LOAD_PROC(FLH_CPUReset);
    LOAD_PROC(FLH_InitCTRL);
    LOAD_PROC(VDR_RootFunc);
    LOAD_PROC(FLH_WriteRootTableWithIspPath);
    LOAD_PROC(VDR_RootPageWrite);
    LOAD_PROC(FLH_ScanE2NANDBlockPerChip);
    LOAD_PROC(FLH_ReadBCM);
    LOAD_PROC(VDR_RootAccess);
    LOAD_PROC(FLH_InitCodeWithIspPath);
    LOAD_PROC(FLH_GetChannelCeNoAndMap);
    LOAD_PROC(ADDR_ReadISPData);
    LOAD_PROC(ADDR_ReadCISData);
    LOAD_PROC(FLH_InitCodeForReady);

#undef LOAD_PROC

    LogMessage("All Flash SDK functions initialized successfully.");
    return TRUE;
}

void Unload181FlashSDK(HMODULE hModule) {
    if(hModule) {
        FreeLibrary(hModule);
        LogMessage("Unloaded 181FlashSDK.dll");
    }
}
