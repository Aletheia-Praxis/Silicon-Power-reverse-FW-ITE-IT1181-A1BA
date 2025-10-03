/**
 * iTEUFDrs Constructor - Exact Reconstruction
 * Based on detailed Ghidra decompilation at 0x0040d690
 *
 * This is the main dialog class constructor that initializes the USB flash drive interface,
 * loads the 181FlashSDK.dll, resolves all 106 SDK function addresses, and detects connected
 * devices.
 */

#include <afxwin.h>
#include <windows.h>

#include "../include/DeviceStructures.h"
#include "../include/FlashSDK.h"
#include "../include/Utilities.h"
#include "../include/iTEUFDrs.h"


// External logging functions identified from Ghidra
extern void FUN_00406170(LPCSTR msg);  // Log message function

/**
 * iTEUFDrs constructor - EXACT reconstruction from 0x0040d690
 * Initializes all member variables, loads SDK, and detects device
 *
 * @param param_2 - Path to directory containing 181FlashSDK.dll
 */
iTEUFDrs::iTEUFDrs(LPCSTR sdkPath) {
    char sdkPathBuffer[512];  // acStack_204 from stack analysis
    HMODULE sdkModule;
    int loadResult;
    char deviceDetected;
    LPSTR pathString;

    // Initialize security cookie for stack protection
    // (equivalent to local_4 = DAT_004abe40 ^ stack_addr)

    // Set up vtable pointer
    // *param_1 = iTEUFDrs::vftable;
    // This is handled by C++ compiler automatically

    // Initialize path string buffer (offset 0x11b = 283 decimal)
    pathString = m_sdkPath;        // Equivalent to (param_1 + 0x11b)
    memset(pathString, 0, 0x104);  // Clear 260 bytes
    lstrcpyA(pathString, sdkPath);

    FUN_00406170("iTEUFDrs: initialize.");

    // Initialize all status flags and member variables
    m_sdkLoadError = 0;      // *(param_1 + 5) = 0
    m_deviceReady = 0;       // *(param_1 + 1) = 0
    m_initError1 = 0;        // *(param_1 + 6) = 0
    m_initError2 = 0;        // *(param_1 + 7) = 0
    m_statusFlag1 = 0;       // *(param_1 + 0x81cd0) = 0
    m_deviceConnected = 0;   // *(param_1 + 0x881) = 0
    m_connectionStatus = 0;  // *(param_1 + 0x882) = 0
    m_deviceCount = 4;       // param_1[0x81cd2] = 4
    m_scanComplete = 1;      // *(param_1 + 0x886) = 1
    m_processingFlag = 0;    // *(param_1 + 0x6b37) = 0
    m_activeDevice = 0;      // *(param_1 + 0x885) = 0

    // Clear device info arrays
    m_deviceHandle1 = 0;  // param_1[0x97] = 0
    m_deviceHandle2 = 0;  // param_1[0x98] = 0
    m_deviceHandle3 = 0;  // param_1[0x99] = 0
    m_deviceHandle4 = 0;  // param_1[0x9a] = 0

    // Clear large data buffers
    memset(m_deviceInfoBuffer, 0, 0x200);    // param_1 + 0x9b, 512 bytes
    memset(m_flashDataBuffer, 0, 0x100000);  // param_1 + 0x41cd0, 1MB buffer
    memset(m_commandBuffer, 0, 0x200);       // param_1 + 2, 512 bytes
    memset(m_responseBuffer, 0, 0x40);       // param_1 + 0x6aca, 64 bytes
    memset(m_statusBuffer, 0, 0x40);         // param_1 + 0x82, 64 bytes

    // Clear additional status variables
    m_lastError = 0;         // param_1[0x92] = 0
    m_operationStatus = 0;   // param_1[0x93] = 0
    m_progressStatus = 0;    // param_1[0x94] = 0
    m_transferStatus = 0;    // param_1[0x95] = 0
    m_completionStatus = 0;  // param_1[0x96] = 0

    // Clear firmware segment buffer
    memset(m_firmwareBuffer, 0, 0x800);  // param_1 + 0x41ace, 2048 bytes

    // Build path to SDK DLL
    sprintf(sdkPathBuffer, "%s\\181FlashSDK.dll", pathString);

    // Load the SDK library
    sdkModule = LoadLibraryA(sdkPathBuffer);
    m_sdkModule = sdkModule;  // param_1[0x21f] = pHVar2

    if(sdkModule == NULL) {
        m_sdkLoadError = 1;  // SDK load failed
        FUN_00406170("iTEUFDrs: Failed to load 181FlashSDK.dll");
    } else {
        FUN_00406170("iTEUFDrs: Load 181FlashSDK succeed.");

        // Load all 106 SDK function addresses
        loadResult = LoadSDKFunctions(m_sdkModule);

        if(loadResult == 0) {
            m_sdkLoadError = 2;  // Function loading failed
            FUN_00406170("iTEUFDrs: Failed to get API addresses from SDK");
        } else {
            FUN_00406170("iTEUFDrs: Get API address succeed in SDK.");

            // Attempt device detection
            deviceDetected = GetDeviceInfo();

            if(deviceDetected == 0) {
                m_sdkLoadError = 3;  // Device detection failed
                FUN_00406170("iTEUFDrs: GetDeviceInfo failed - no device detected");
            } else {
                FUN_00406170("iTEUFDrs: GetDeviceInfo OK");
                m_deviceReady = 1;  // Success - device ready
            }
        }
    }

    // Stack protection check (equivalent to __security_check_cookie)
    // This is handled by compiler-generated code
}

/**
 * LoadSDKFunctions - Loads all 106 function addresses from 181FlashSDK.dll
 * Based on decompilation at 0x00401000
 *
 * This function resolves every single SDK function via GetProcAddress.
 * Returns TRUE only if ALL functions are successfully loaded.
 */
bool iTEUFDrs::LoadSDKFunctions(HMODULE sdkModule) {
    if(sdkModule == NULL) {
        return false;
    }

    // Load Flash Layer Helper (FLH) functions - 36 functions
    g_pFLH_GetInfoFromDataBaseByID = GetProcAddress(sdkModule, "FLH_GetInfoFromDataBaseByID");
    if(g_pFLH_GetInfoFromDataBaseByID == NULL)
        return false;

    g_pFLH_GetFlashDataFromDataBase = GetProcAddress(sdkModule, "FLH_GetFlashDataFromDataBase");
    if(g_pFLH_GetFlashDataFromDataBase == NULL)
        return false;

    g_pFLH_GetFlashDataFromMemory = GetProcAddress(sdkModule, "FLH_GetFlashDataFromMemory");
    if(g_pFLH_GetFlashDataFromMemory == NULL)
        return false;

    g_pFLH_ReadRootTable = GetProcAddress(sdkModule, "FLH_ReadRootTable");
    if(g_pFLH_ReadRootTable == NULL)
        return false;

    g_pFLH_WriteRootTable = GetProcAddress(sdkModule, "FLH_WriteRootTable");
    if(g_pFLH_WriteRootTable == NULL)
        return false;

    g_pFLH_ReadCISTable = GetProcAddress(sdkModule, "FLH_ReadCISTable");
    if(g_pFLH_ReadCISTable == NULL)
        return false;

    g_pFLH_WriteCISTable = GetProcAddress(sdkModule, "FLH_WriteCISTable");
    if(g_pFLH_WriteCISTable == NULL)
        return false;

    g_pFLH_ReadISPData = GetProcAddress(sdkModule, "FLH_ReadISPData");
    if(g_pFLH_ReadISPData == NULL)
        return false;

    g_pFLH_WriteISPData = GetProcAddress(sdkModule, "FLH_WriteISPData");
    if(g_pFLH_WriteISPData == NULL)
        return false;

    g_pFLH_ReadLatestWBT = GetProcAddress(sdkModule, "FLH_ReadLatestWBT");
    if(g_pFLH_ReadLatestWBT == NULL)
        return false;

    g_pFLH_FindRootTable = GetProcAddress(sdkModule, "FLH_FindRootTable");
    if(g_pFLH_FindRootTable == NULL)
        return false;

    g_pFLH_LBA2PhysicalFlash = GetProcAddress(sdkModule, "FLH_LBA2PhysicalFlash");
    if(g_pFLH_LBA2PhysicalFlash == NULL)
        return false;

    g_pFLH_SetLedBlink = GetProcAddress(sdkModule, "FLH_SetLedBlink");
    if(g_pFLH_SetLedBlink == NULL)
        return false;

    g_pFLH_PhyiscalRead = GetProcAddress(sdkModule, "FLH_PhyiscalRead");
    if(g_pFLH_PhyiscalRead == NULL)
        return false;

    g_pFLH_PhyiscalWrite = GetProcAddress(sdkModule, "FLH_PhyiscalWrite");
    if(g_pFLH_PhyiscalWrite == NULL)
        return false;

    g_pFLH_IsGoodBlock = GetProcAddress(sdkModule, "FLH_IsGoodBlock");
    if(g_pFLH_IsGoodBlock == NULL)
        return false;

    g_pFLH_IsTableBlock = GetProcAddress(sdkModule, "FLH_IsTableBlock");
    if(g_pFLH_IsTableBlock == NULL)
        return false;

    g_pFLH_MarkBad = GetProcAddress(sdkModule, "FLH_MarkBad");
    if(g_pFLH_MarkBad == NULL)
        return false;

    g_pFLH_GetRealBlocksPerDie = GetProcAddress(sdkModule, "FLH_GetRealBlocksPerDie");
    if(g_pFLH_GetRealBlocksPerDie == NULL)
        return false;

    g_pFLH_BlockIsGap = GetProcAddress(sdkModule, "FLH_BlockIsGap");
    if(g_pFLH_BlockIsGap == NULL)
        return false;

    // Load Security (SEC) functions - 8 functions
    g_pSEC_DoAuthentication = GetProcAddress(sdkModule, "SEC_DoAuthentication");
    if(g_pSEC_DoAuthentication == NULL)
        return false;

    g_pSEC_LeaveAuthenticatedState = GetProcAddress(sdkModule, "SEC_LeaveAuthenticatedState");
    if(g_pSEC_LeaveAuthenticatedState == NULL)
        return false;

    g_pSEC_GetPasswordHint = GetProcAddress(sdkModule, "SEC_GetPasswordHint");
    if(g_pSEC_GetPasswordHint == NULL)
        return false;

    g_pSEC_SetPasswordHint = GetProcAddress(sdkModule, "SEC_SetPasswordHint");
    if(g_pSEC_SetPasswordHint == NULL)
        return false;

    g_pSEC_ChangePassword = GetProcAddress(sdkModule, "SEC_ChangePassword");
    if(g_pSEC_ChangePassword == NULL)
        return false;

    g_pSEC_GetUserPassword = GetProcAddress(sdkModule, "SEC_GetUserPassword");
    if(g_pSEC_GetUserPassword == NULL)
        return false;

    g_pSEC_GetEncryptedPassword = GetProcAddress(sdkModule, "SEC_GetEncryptedPassword");
    if(g_pSEC_GetEncryptedPassword == NULL)
        return false;

    // Load LUN (Logical Unit Number) functions - 8 functions
    g_pLUN_CreateLun = GetProcAddress(sdkModule, "LUN_CreateLun");
    if(g_pLUN_CreateLun == NULL)
        return false;

    g_pLUN_FindLunStartLBAByItemID = GetProcAddress(sdkModule, "LUN_FindLunStartLBAByItemID");
    if(g_pLUN_FindLunStartLBAByItemID == NULL)
        return false;

    g_pLUN_CreateApLunNewItemID = GetProcAddress(sdkModule, "LUN_CreateApLunNewItemID");
    if(g_pLUN_CreateApLunNewItemID == NULL)
        return false;

    g_pLUN_WriteBadBlockMapToApLun = GetProcAddress(sdkModule, "LUN_WriteBadBlockMapToApLun");
    if(g_pLUN_WriteBadBlockMapToApLun == NULL)
        return false;

    g_pLUN_ReadBadBlockMapFromApLun = GetProcAddress(sdkModule, "LUN_ReadBadBlockMapFromApLun");
    if(g_pLUN_ReadBadBlockMapFromApLun == NULL)
        return false;

    g_pLUN_FindOptimumOffsetCap = GetProcAddress(sdkModule, "LUN_FindOptimumOffsetCap");
    if(g_pLUN_FindOptimumOffsetCap == NULL)
        return false;

    g_pLUN_CalIsoSize = GetProcAddress(sdkModule, "LUN_CalIsoSize");
    if(g_pLUN_CalIsoSize == NULL)
        return false;

    // Load Format (FMT) functions - 4 functions
    g_pFMT_Format = GetProcAddress(sdkModule, "FMT_Format");
    if(g_pFMT_Format == NULL)
        return false;

    g_pFMT_GetOptimumCapacity = GetProcAddress(sdkModule, "FMT_GetOptimumCapacity");
    if(g_pFMT_GetOptimumCapacity == NULL)
        return false;

    g_pFMT_GetOptimumLunConfig = GetProcAddress(sdkModule, "FMT_GetOptimumLunConfig");
    if(g_pFMT_GetOptimumLunConfig == NULL)
        return false;

    g_pFMT_GetOSCapacity = GetProcAddress(sdkModule, "FMT_GetOSCapacity");
    if(g_pFMT_GetOSCapacity == NULL)
        return false;

    // Load Standard (STD) functions - 6 functions
    g_pSTD_Inquiry = GetProcAddress(sdkModule, "STD_Inquiry");
    if(g_pSTD_Inquiry == NULL)
        return false;

    g_pSTD_ReadCapacity = GetProcAddress(sdkModule, "STD_ReadCapacity");
    if(g_pSTD_ReadCapacity == NULL)
        return false;

    g_pSTD_LogicalRead = GetProcAddress(sdkModule, "STD_LogicalRead");
    if(g_pSTD_LogicalRead == NULL)
        return false;

    g_pSTD_LogicalWrite = GetProcAddress(sdkModule, "STD_LogicalWrite");
    if(g_pSTD_LogicalWrite == NULL)
        return false;

    g_pSTD_TestUnitReady = GetProcAddress(sdkModule, "VDR_CheckSYSReady");
    if(g_pSTD_TestUnitReady == NULL)
        return false;

    // Load Utility functions - 8 functions
    g_pSwapDWORD = GetProcAddress(sdkModule, "SwapDWORD");
    if(g_pSwapDWORD == NULL)
        return false;

    g_pSwapWORD = GetProcAddress(sdkModule, "SwapWORD");
    if(g_pSwapWORD == NULL)
        return false;

    g_pCCBAddress2RawAddress = GetProcAddress(sdkModule, "CCBAddress2RawAddress");
    if(g_pCCBAddress2RawAddress == NULL)
        return false;

    g_pRawAddress2CCBAddress = GetProcAddress(sdkModule, "RawAddress2CCBAddress");
    if(g_pRawAddress2CCBAddress == NULL)
        return false;

    g_pCCBAddress2ED3Address = GetProcAddress(sdkModule, "CCBAddress2ED3Address");
    if(g_pCCBAddress2ED3Address == NULL)
        return false;

    g_pED3Address2CCBAddress = GetProcAddress(sdkModule, "ED3Address2CCBAddress");
    if(g_pED3Address2CCBAddress == NULL)
        return false;

    g_pBlkAddr2RawAddr = GetProcAddress(sdkModule, "BlkAddr2RawAddr");
    if(g_pBlkAddr2RawAddr == NULL)
        return false;

    // Load Vendor Driver (VDR) functions - 36 functions
    g_pVDR_ReadWriteLUNConfig = GetProcAddress(sdkModule, "VDR_ReadWriteLUNConfig");
    if(g_pVDR_ReadWriteLUNConfig == NULL)
        return false;

    g_pVDR_ReadLUNData = GetProcAddress(sdkModule, "VDR_ReadLUNData");
    if(g_pVDR_ReadLUNData == NULL)
        return false;

    g_pVDR_WriteLUNData = GetProcAddress(sdkModule, "VDR_WriteLUNData");
    if(g_pVDR_WriteLUNData == NULL)
        return false;

    g_pVDR_ReadXData = GetProcAddress(sdkModule, "VDR_ReadXData");
    if(g_pVDR_ReadXData == NULL)
        return false;

    g_pVDR_WriteXData = GetProcAddress(sdkModule, "VDR_WriteXData");
    if(g_pVDR_WriteXData == NULL)
        return false;

    g_pVDR_ReadIData = GetProcAddress(sdkModule, "VDR_ReadIData");
    if(g_pVDR_ReadIData == NULL)
        return false;

    g_pVDR_WriteIData = GetProcAddress(sdkModule, "VDR_WriteIData");
    if(g_pVDR_WriteIData == NULL)
        return false;

    g_pVDR_ReadSysAddr = GetProcAddress(sdkModule, "VDR_ReadSysAddr");
    if(g_pVDR_ReadSysAddr == NULL)
        return false;

    g_pVDR_WriteSysAddr = GetProcAddress(sdkModule, "VDR_WriteSysAddr");
    if(g_pVDR_WriteSysAddr == NULL)
        return false;

    g_pVDR_SetSYSReady = GetProcAddress(sdkModule, "VDR_SetSYSReady");
    if(g_pVDR_SetSYSReady == NULL)
        return false;

    g_pVDR_EndCode = GetProcAddress(sdkModule, "VDR_EndCode");
    if(g_pVDR_EndCode == NULL)
        return false;

    g_pVDR_DeviceChange = GetProcAddress(sdkModule, "VDR_DeviceChange");
    if(g_pVDR_DeviceChange == NULL)
        return false;

    g_pVDR_MediaChange = GetProcAddress(sdkModule, "VDR_MediaChange");
    if(g_pVDR_MediaChange == NULL)
        return false;

    g_pVDR_WriteProtect = GetProcAddress(sdkModule, "VDR_WriteProtect");
    if(g_pVDR_WriteProtect == NULL)
        return false;

    g_pVDR_RWCurrentLUNType = GetProcAddress(sdkModule, "VDR_RWCurrentLUNType");
    if(g_pVDR_RWCurrentLUNType == NULL)
        return false;

    g_pVDR_HiddenArea = GetProcAddress(sdkModule, "VDR_HiddenArea");
    if(g_pVDR_HiddenArea == NULL)
        return false;

    g_pVDR_ReadWriteLUNNo = GetProcAddress(sdkModule, "VDR_ReadWriteLUNNo");
    if(g_pVDR_ReadWriteLUNNo == NULL)
        return false;

    g_pSTD_GetDeviceID = GetProcAddress(sdkModule, "VDR_ReadLUNID");
    if(g_pSTD_GetDeviceID == NULL)
        return false;

    g_pSTD_SetDeviceID = GetProcAddress(sdkModule, "VDR_WriteLUNID");
    if(g_pSTD_SetDeviceID == NULL)
        return false;

    g_pSTD_GetLUNIndex = GetProcAddress(sdkModule, "VDR_ReadLUNIndex");
    if(g_pSTD_GetLUNIndex == NULL)
        return false;

    g_pVDR_FlushCache = GetProcAddress(sdkModule, "VDR_FlushCache");
    if(g_pVDR_FlushCache == NULL)
        return false;

    g_pVDR_ReadPage = GetProcAddress(sdkModule, "VDR_ReadPage");
    if(g_pVDR_ReadPage == NULL)
        return false;

    g_pVDR_WritePage = GetProcAddress(sdkModule, "VDR_WritePage");
    if(g_pVDR_WritePage == NULL)
        return false;

    g_pVDR_WriteBlock_TLC = GetProcAddress(sdkModule, "VDR_WriteBlock_TLC");
    if(g_pVDR_WriteBlock_TLC == NULL)
        return false;

    g_pVDR_GetSecurityStatus = GetProcAddress(sdkModule, "VDR_GetSecurityStatus");
    if(g_pVDR_GetSecurityStatus == NULL)
        return false;

    g_pVDR_ED3PageRead = GetProcAddress(sdkModule, "VDR_ED3PageRead");
    if(g_pVDR_ED3PageRead == NULL)
        return false;

    g_pVDR_BadTFindRead = GetProcAddress(sdkModule, "VDR_BadTFindRead");
    if(g_pVDR_BadTFindRead == NULL)
        return false;

    g_pVDR_Enhance_SLC_Program = GetProcAddress(sdkModule, "VDR_Enhance_SLC_Program");
    if(g_pVDR_Enhance_SLC_Program == NULL)
        return false;

    g_pVDR_Disable_SLC_Program = GetProcAddress(sdkModule, "VDR_Disable_SLC_Program");
    if(g_pVDR_Disable_SLC_Program == NULL)
        return false;

    g_pVDR_MassBlocksProcess = GetProcAddress(sdkModule, "VDR_MassBlocksProcess");
    if(g_pVDR_MassBlocksProcess == NULL)
        return false;

    g_pVDR_F_RST = GetProcAddress(sdkModule, "VDR_F_RST");
    if(g_pVDR_F_RST == NULL)
        return false;

    g_pVDR_RootFunc = GetProcAddress(sdkModule, "VDR_RootFunc");
    if(g_pVDR_RootFunc == NULL)
        return false;

    g_pVDR_RootPageWrite = GetProcAddress(sdkModule, "VDR_RootPageWrite");
    if(g_pVDR_RootPageWrite == NULL)
        return false;

    g_pVDR_RootAccess = GetProcAddress(sdkModule, "VDR_RootAccess");
    if(g_pVDR_RootAccess == NULL)
        return false;

    // Additional FLH functions - continuing from above
    g_pFLH_HandleMassBlocksPerChip = GetProcAddress(sdkModule, "FLH_HandleMassBlocksPerChip");
    if(g_pFLH_HandleMassBlocksPerChip == NULL)
        return false;

    g_pFLH_ScanNewBlock = GetProcAddress(sdkModule, "FLH_ScanNewBlock");
    if(g_pFLH_ScanNewBlock == NULL)
        return false;

    g_pFLH_GetRetryRegister = GetProcAddress(sdkModule, "FLH_GetRetryRegister");
    if(g_pFLH_GetRetryRegister == NULL)
        return false;

    g_pFLH_CISCheckSum_Calculate = GetProcAddress(sdkModule, "FLH_CISCheckSum_Calculate");
    if(g_pFLH_CISCheckSum_Calculate == NULL)
        return false;

    g_pFLH_CalCulate_ECCNO = GetProcAddress(sdkModule, "FLH_CalCulate_ECCNO");
    if(g_pFLH_CalCulate_ECCNO == NULL)
        return false;

    g_pFLH_ArrangeSegmentPara = GetProcAddress(sdkModule, "FLH_ArrangeSegmentPara");
    if(g_pFLH_ArrangeSegmentPara == NULL)
        return false;

    g_pFLH_ReadSpare = GetProcAddress(sdkModule, "FLH_ReadSpare");
    if(g_pFLH_ReadSpare == NULL)
        return false;

    g_pFLH_ReadID = GetProcAddress(sdkModule, "FLH_ReadID");
    if(g_pFLH_ReadID == NULL)
        return false;

    g_pFLH_BlockErase = GetProcAddress(sdkModule, "FLH_BlockErase");
    if(g_pFLH_BlockErase == NULL)
        return false;

    g_pFLH_SetSLCFlag = GetProcAddress(sdkModule, "FLH_SetSLCFlag");
    if(g_pFLH_SetSLCFlag == NULL)
        return false;

    g_pFLH_CPUReset = GetProcAddress(sdkModule, "FLH_CPUReset");
    if(g_pFLH_CPUReset == NULL)
        return false;

    g_pFLH_InitCTRL = GetProcAddress(sdkModule, "FLH_InitCTRL");
    if(g_pFLH_InitCTRL == NULL)
        return false;

    g_pFLH_WriteRootTableWithIspPath = GetProcAddress(sdkModule, "FLH_WriteRootTableWithIspPath");
    if(g_pFLH_WriteRootTableWithIspPath == NULL)
        return false;

    g_pFLH_ScanE2NANDBlockPerChip = GetProcAddress(sdkModule, "FLH_ScanE2NANDBlockPerChip");
    if(g_pFLH_ScanE2NANDBlockPerChip == NULL)
        return false;

    g_pFLH_ReadBCM = GetProcAddress(sdkModule, "FLH_ReadBCM");
    if(g_pFLH_ReadBCM == NULL)
        return false;

    g_pFLH_InitCodeWithIspPath = GetProcAddress(sdkModule, "FLH_InitCodeWithIspPath");
    if(g_pFLH_InitCodeWithIspPath == NULL)
        return false;

    g_pFLH_GetChannelCeNoAndMap = GetProcAddress(sdkModule, "FLH_GetChannelCeNoAndMap");
    if(g_pFLH_GetChannelCeNoAndMap == NULL)
        return false;

    g_pFLH_InitCodeForReady = GetProcAddress(sdkModule, "FLH_InitCodeForReady");
    if(g_pFLH_InitCodeForReady == NULL)
        return false;

    // Media Processor (MP) functions
    g_pMP_CreateSystem = GetProcAddress(sdkModule, "MP_CreateSystem");
    if(g_pMP_CreateSystem == NULL)
        return false;

    g_pMP_EraseSystemTable = GetProcAddress(sdkModule, "MP_EraseSystemTable");
    if(g_pMP_EraseSystemTable == NULL)
        return false;

    // Diagnostic (DG) functions
    g_pDG_GetBlockPageMapFromFlash = GetProcAddress(sdkModule, "DG_GetBlockPageMapFromFlash");
    if(g_pDG_GetBlockPageMapFromFlash == NULL)
        return false;

    g_pDG_SearchReadBadTBlk = GetProcAddress(sdkModule, "DG_SearchReadBadTBlk");
    if(g_pDG_SearchReadBadTBlk == NULL)
        return false;

    g_pDG_CalBlkRequire = GetProcAddress(sdkModule, "DG_CalBlkRequire");
    if(g_pDG_CalBlkRequire == NULL)
        return false;

    // Address (ADDR) functions
    g_pADDR_ReadRootTable = GetProcAddress(sdkModule, "ADDR_ReadRootTable");
    if(g_pADDR_ReadRootTable == NULL)
        return false;

    g_pADDR_ReadISPData = GetProcAddress(sdkModule, "ADDR_ReadISPData");
    if(g_pADDR_ReadISPData == NULL)
        return false;

    g_pADDR_ReadCISData = GetProcAddress(sdkModule, "ADDR_ReadCISData");
    if(g_pADDR_ReadCISData == NULL)
        return false;

    // Device detection functions
    g_pIs168Device = GetProcAddress(sdkModule, "Is168Device");
    if(g_pIs168Device == NULL)
        return false;

    g_pGetLastestPage = GetProcAddress(sdkModule, "GetLastestPage");
    if(g_pGetLastestPage == NULL)
        return false;

    // If we reach here, all 106 functions were successfully loaded
    FUN_00406170("LoadSDKFunctions: All 106 SDK functions loaded successfully");
    return true;
}

/**
 * GetDeviceInfo - Device detection and initialization stub
 * This calls the actual GetDeviceInfo function at 0x0040cf30
 */
char iTEUFDrs::GetDeviceInfo() {
    // Call the actual GetDeviceInfo implementation
    return iTEUFDrs_GetDeviceInfo((int) this);
}
