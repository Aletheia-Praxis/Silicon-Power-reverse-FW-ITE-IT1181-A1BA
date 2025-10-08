// clang-format off
// #include "../include/iTEUFDrs_Ghidra.h"  // Not needed - functionality moved to iTEUFDrs class
#include "../include/Globals.h"
#include "../include/FlashSDK.h"
#include "../include/iTEUFDrs.h"
#include "../include/SDKGlobals.h"  // For SDK_FUNCTION_PTR type definition
// clang-format on

// Define global variables
DWORD g_windowsVersion = 0;              // DAT_004ad748
DWORD g_windowsVersionBuild = 0;         // Additional Windows version build
char g_cmdLineArgs[128] = { 0 };         // DAT_004ad6c0
BOOL g_calledFromURescueM = FALSE;       // DAT_004ad740
BOOL g_systemReadyFlag = FALSE;          // DAT_004ad744
char g_tempDirectory[MAX_PATH] = { 0 };  // DAT_004ad74c

// clang-format off
#ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0601  // Windows 7+
#endif

#include <windows.h>
#include <setupapi.h>
// #include <winusb.h>  // Temporarily excluded to avoid usbspec.h conflicts
// clang-format on

// Global GUIDs for USB devices
const GUID GUID_DEVCLASS_USB = { 0x36fc9e60,
                                 0xc465,
                                 0x11cf,
                                 { 0x80, 0x56, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 } };
const GUID GUID_DEVINTERFACE_USB_DEVICE = { 0xa5dcbfa1,
                                            0x6530,
                                            0x11d2,
                                            { 0x90, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x51, 0xed } };

// Constants for ITE controller
const BYTE ITE_COMMAND_PREFIX[] = { 0x55, 0xAA };
const BYTE ITE_RESPONSE_PREFIX[] = { 0xAA, 0x55 };

// ITE controller commands
const BYTE ITE_CMD_INIT = 0x01;
const BYTE ITE_CMD_GET_INFO = 0x02;
const BYTE ITE_CMD_SET_MODE = 0x03;
const BYTE ITE_CMD_GET_STATUS = 0x04;
const BYTE ITE_CMD_RESET = 0xFF;

// ITE controller operating modes
const BYTE ITE_MODE_NORMAL = 0x00;
const BYTE ITE_MODE_FLASH = 0x01;
const BYTE ITE_MODE_RECOVERY = 0x02;

// ITE controller states
const BYTE ITE_STATE_READY = 0x00;
const BYTE ITE_STATE_BUSY = 0x01;
const BYTE ITE_STATE_ERROR = 0xFF;

// Operation timeouts
const DWORD ITE_TIMEOUT_INIT = 100;     // ms
const DWORD ITE_TIMEOUT_COMMAND = 100;  // ms
const DWORD ITE_TIMEOUT_RESET = 1000;   // ms
const DWORD ITE_TIMEOUT_FLASH = 5000;   // ms

// Buffer sizes
const DWORD ITE_BUFFER_SIZE = 4096;
const DWORD ITE_MAX_FIRMWARE_SIZE = 16 * 1024 * 1024;  // 16 MB
const DWORD ITE_HEADER_SIZE = 512;

// Signatures and identifiers
const char ITE_SIGNATURE[] = "ITE_FW";
const char ITE_MODEL[] = "IT1181";
const char ITE_VERSION_PREFIX[] = "v81D.2.24.2";

// Error codes
const DWORD ITE_ERROR_NONE = 0x00000000;
const DWORD ITE_ERROR_INIT_FAILED = 0x00000001;
const DWORD ITE_ERROR_INVALID_COMMAND = 0x00000002;
const DWORD ITE_ERROR_DEVICE_BUSY = 0x00000003;
const DWORD ITE_ERROR_FLASH_FAILED = 0x00000004;
const DWORD ITE_ERROR_VERIFY_FAILED = 0x00000005;
const DWORD ITE_ERROR_TIMEOUT = 0x00000006;
const DWORD ITE_ERROR_UNKNOWN = 0xFFFFFFFF;

// Global pointer to the main iTEUFDrs object instance
void* g_iTEUFDrs = nullptr;

// Global variables for SDK function pointers (for LoadSDKFunctions)
SDK_FUNCTION_PTR g_pSTD_Read_ID_New = NULL;
SDK_FUNCTION_PTR g_pSTD_Read_ID = NULL;

// Flash Layer Helper (FLH) function pointers
SDK_FUNCTION_PTR g_pFLH_GetInfoFromDataBaseByID = NULL;
SDK_FUNCTION_PTR g_pFLH_GetFlashDataFromDataBase = NULL;
SDK_FUNCTION_PTR g_pFLH_GetFlashDataFromMemory = NULL;
SDK_FUNCTION_PTR g_pFLH_ReadRootTable = NULL;
SDK_FUNCTION_PTR g_pFLH_WriteRootTable = NULL;
SDK_FUNCTION_PTR g_pFLH_ReadCISTable = NULL;
SDK_FUNCTION_PTR g_pFLH_WriteCISTable = NULL;
SDK_FUNCTION_PTR g_pFLH_ReadISPData = NULL;
SDK_FUNCTION_PTR g_pFLH_WriteISPData = NULL;
SDK_FUNCTION_PTR g_pFLH_ReadLatestWBT = NULL;
SDK_FUNCTION_PTR g_pFLH_FindRootTable = NULL;
SDK_FUNCTION_PTR g_pFLH_LBA2PhysicalFlash = NULL;
SDK_FUNCTION_PTR g_pFLH_SetLedBlink = NULL;
SDK_FUNCTION_PTR g_pFLH_PhyiscalRead = NULL;
SDK_FUNCTION_PTR g_pFLH_PhyiscalWrite = NULL;
SDK_FUNCTION_PTR g_pFLH_IsGoodBlock = NULL;
SDK_FUNCTION_PTR g_pFLH_IsTableBlock = NULL;
SDK_FUNCTION_PTR g_pFLH_MarkBad = NULL;
SDK_FUNCTION_PTR g_pFLH_GetRealBlocksPerDie = NULL;
SDK_FUNCTION_PTR g_pFLH_BlockIsGap = NULL;
SDK_FUNCTION_PTR g_pFLH_HandleMassBlocksPerChip = NULL;
SDK_FUNCTION_PTR g_pFLH_ScanNewBlock = NULL;
SDK_FUNCTION_PTR g_pFLH_GetRetryRegister = NULL;
SDK_FUNCTION_PTR g_pFLH_CISCheckSum_Calculate = NULL;
SDK_FUNCTION_PTR g_pFLH_CalCulate_ECCNO = NULL;
SDK_FUNCTION_PTR g_pFLH_ArrangeSegmentPara = NULL;
SDK_FUNCTION_PTR g_pFLH_ReadSpare = NULL;
SDK_FUNCTION_PTR g_pFLH_ReadID = NULL;
SDK_FUNCTION_PTR g_pFLH_BlockErase = NULL;
SDK_FUNCTION_PTR g_pFLH_SetSLCFlag = NULL;
SDK_FUNCTION_PTR g_pFLH_CPUReset = NULL;
SDK_FUNCTION_PTR g_pFLH_InitCTRL = NULL;

// Vendor Driver (VDR) function pointers
SDK_FUNCTION_PTR g_pVDR_ReadWriteLUNConfig = NULL;
SDK_FUNCTION_PTR g_pVDR_ReadLUNData = NULL;
SDK_FUNCTION_PTR g_pVDR_WriteLUNData = NULL;
SDK_FUNCTION_PTR g_pVDR_ReadXData = NULL;
SDK_FUNCTION_PTR g_pVDR_WriteXData = NULL;
SDK_FUNCTION_PTR g_pVDR_ReadIData = NULL;
SDK_FUNCTION_PTR g_pVDR_WriteIData = NULL;
SDK_FUNCTION_PTR g_pVDR_ReadSysAddr = NULL;
SDK_FUNCTION_PTR g_pVDR_WriteSysAddr = NULL;
SDK_FUNCTION_PTR g_pVDR_SetSYSReady = NULL;
SDK_FUNCTION_PTR g_pVDR_EndCode = NULL;
SDK_FUNCTION_PTR g_pVDR_DeviceChange = NULL;
SDK_FUNCTION_PTR g_pVDR_MediaChange = NULL;
SDK_FUNCTION_PTR g_pVDR_WriteProtect = NULL;
SDK_FUNCTION_PTR g_pVDR_RWCurrentLUNType = NULL;
SDK_FUNCTION_PTR g_pVDR_HiddenArea = NULL;
SDK_FUNCTION_PTR g_pVDR_ReadWriteLUNNo = NULL;
SDK_FUNCTION_PTR g_pVDR_FlushCache = NULL;
SDK_FUNCTION_PTR g_pVDR_ReadPage = NULL;
SDK_FUNCTION_PTR g_pVDR_WritePage = NULL;
SDK_FUNCTION_PTR g_pVDR_WriteBlock_TLC = NULL;
SDK_FUNCTION_PTR g_pVDR_GetSecurityStatus = NULL;
SDK_FUNCTION_PTR g_pVDR_ED3PageRead = NULL;
SDK_FUNCTION_PTR g_pVDR_BadTFindRead = NULL;
SDK_FUNCTION_PTR g_pVDR_Enhance_SLC_Program = NULL;
SDK_FUNCTION_PTR g_pVDR_Disable_SLC_Program = NULL;
SDK_FUNCTION_PTR g_pVDR_MassBlocksProcess = NULL;
SDK_FUNCTION_PTR g_pVDR_F_RST = NULL;
SDK_FUNCTION_PTR g_pVDR_RootFunc = NULL;
SDK_FUNCTION_PTR g_pVDR_RootPageWrite = NULL;
SDK_FUNCTION_PTR g_pVDR_RootAccess = NULL;

// Additional STD function pointers (used with VDR functions)
SDK_FUNCTION_PTR g_pSTD_GetDeviceID = NULL;
SDK_FUNCTION_PTR g_pSTD_SetDeviceID = NULL;
SDK_FUNCTION_PTR g_pSTD_GetLUNIndex = NULL;

// Security (SEC) function pointers
SDK_FUNCTION_PTR g_pSEC_DoAuthentication = NULL;
SDK_FUNCTION_PTR g_pSEC_LeaveAuthenticatedState = NULL;
SDK_FUNCTION_PTR g_pSEC_GetPasswordHint = NULL;
SDK_FUNCTION_PTR g_pSEC_SetPasswordHint = NULL;
SDK_FUNCTION_PTR g_pSEC_ChangePassword = NULL;
SDK_FUNCTION_PTR g_pSEC_GetUserPassword = NULL;
SDK_FUNCTION_PTR g_pSEC_GetEncryptedPassword = NULL;

// LUN function pointers
SDK_FUNCTION_PTR g_pLUN_CreateLun = NULL;
SDK_FUNCTION_PTR g_pLUN_FindLunStartLBAByItemID = NULL;
SDK_FUNCTION_PTR g_pLUN_CreateApLunNewItemID = NULL;
SDK_FUNCTION_PTR g_pLUN_WriteBadBlockMapToApLun = NULL;
SDK_FUNCTION_PTR g_pLUN_ReadBadBlockMapFromApLun = NULL;
SDK_FUNCTION_PTR g_pLUN_FindOptimumOffsetCap = NULL;
SDK_FUNCTION_PTR g_pLUN_CalIsoSize = NULL;

// Format (FMT) function pointers
SDK_FUNCTION_PTR g_pFMT_Format = NULL;
SDK_FUNCTION_PTR g_pFMT_GetOptimumCapacity = NULL;
SDK_FUNCTION_PTR g_pFMT_GetOptimumLunConfig = NULL;
SDK_FUNCTION_PTR g_pFMT_GetOSCapacity = NULL;

// Standard (STD) function pointers
SDK_FUNCTION_PTR g_pSTD_Inquiry = NULL;
SDK_FUNCTION_PTR g_pSTD_ReadCapacity = NULL;
SDK_FUNCTION_PTR g_pSTD_LogicalRead = NULL;
SDK_FUNCTION_PTR g_pSTD_LogicalWrite = NULL;
SDK_FUNCTION_PTR g_pSTD_TestUnitReady = NULL;

// Utility function pointers
SDK_FUNCTION_PTR g_pSwapDWORD = NULL;
SDK_FUNCTION_PTR g_pSwapWORD = NULL;
SDK_FUNCTION_PTR g_pCCBAddress2RawAddress = NULL;
SDK_FUNCTION_PTR g_pRawAddress2CCBAddress = NULL;
SDK_FUNCTION_PTR g_pCCBAddress2ED3Address = NULL;
SDK_FUNCTION_PTR g_pED3Address2CCBAddress = NULL;
SDK_FUNCTION_PTR g_pBlkAddr2RawAddr = NULL;
SDK_FUNCTION_PTR g_pIs168Device = NULL;
SDK_FUNCTION_PTR g_pGetLastestPage = NULL;
SDK_FUNCTION_PTR g_pRawAddr2BlkAddr = NULL;
SDK_FUNCTION_PTR g_pGetLastestBlockPerChannel = NULL;
SDK_FUNCTION_PTR g_pGetECCLevelFromFlashDBID = NULL;
SDK_FUNCTION_PTR g_pIs3DTLC = NULL;
SDK_FUNCTION_PTR g_pReadRetry = NULL;
SDK_FUNCTION_PTR g_pWriteRetry = NULL;

// Additional FLH function pointers (continued)
SDK_FUNCTION_PTR g_pFLH_WriteRootTableWithIspPath = NULL;
SDK_FUNCTION_PTR g_pFLH_ScanE2NANDBlockPerChip = NULL;
SDK_FUNCTION_PTR g_pFLH_ReadBCM = NULL;
SDK_FUNCTION_PTR g_pFLH_InitCodeWithIspPath = NULL;
SDK_FUNCTION_PTR g_pFLH_GetChannelCeNoAndMap = NULL;
SDK_FUNCTION_PTR g_pFLH_InitCodeForReady = NULL;

// Mass Production (MP) function pointers
SDK_FUNCTION_PTR g_pMP_CreateSystem = NULL;
SDK_FUNCTION_PTR g_pMP_EraseSystemTable = NULL;

// Diagnostic (DG) function pointers
SDK_FUNCTION_PTR g_pDG_GetBlockPageMapFromFlash = NULL;
SDK_FUNCTION_PTR g_pDG_SearchReadBadTBlk = NULL;
SDK_FUNCTION_PTR g_pDG_CalBlkRequire = NULL;

// Address (ADDR) function pointers
SDK_FUNCTION_PTR g_pADDR_ReadRootTable = NULL;
SDK_FUNCTION_PTR g_pADDR_ReadISPData = NULL;
SDK_FUNCTION_PTR g_pADDR_ReadCISData = NULL;
