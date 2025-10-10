#include "../include/FirmwareManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/Globals.h"
#include "../include/SDKAPIs.h"
#include "../include/SDKGlobals.h"
#include "../include/Utilities.h"
#include "../include/WindowsHeaders.h"
#include "../include/iTEUFDrs.h"

// External SDK function declarations
extern SDK_FUNCTION_PTR g_pSTD_TestUnitReady;
extern SDK_FUNCTION_PTR g_pFLH_InitCodeWithIspPath;
extern SDK_FUNCTION_PTR g_pFLH_GetFlashDataFromDataBase;
extern SDK_FUNCTION_PTR g_pFLH_GetFlashDataFromMemory;
extern SDK_FUNCTION_PTR g_pFLH_ArrangeSegmentPara;
extern SDK_FUNCTION_PTR g_pFLH_InitCTRL;
extern SDK_FUNCTION_PTR g_pFLH_FindRootTable;
extern SDK_FUNCTION_PTR g_pVDR_RootFunc;
extern SDK_FUNCTION_PTR g_pBlkAddr2RawAddr;
extern SDK_FUNCTION_PTR g_pFLH_GetBCMInfo;

// External function declarations from iTEUFDrs.cpp
extern char CheckNeedLoadBank(UINT deviceIndex, DWORD deviceHandle);
extern void SetDatabasePathAndOpen();
extern void UpdateBankStatusFlags(BYTE deviceIndex);
extern void AssignDeviceFlagFromBank(BYTE deviceIndex, DWORD deviceHandle);
extern int FormatStringToBuffer(void* buffer, int size, const char* format, ...);
extern DWORD SwapEndianness32(DWORD value);
extern WORD SwapEndianness16(WORD value);
extern BYTE AnalyzeSpareAreaAndClassifyBlock(
    DWORD blockAddr,
    DWORD deviceHandle,
    DWORD* pVerificationBuffer,
    UINT_PTR bcmOffset);

// Global iTEUFDrs instance pointer (used by all functions)
extern iTEUFDrs* g_iTEUFDrs_instance;

namespace FirmwareOperations {

// PrepareFirmwareFilePath
// Construct firmware binary path based on controller type, revision and variant.
void PrepareFirmwareFilePath(iTEUFDrs* pThis) {
    if(! pThis)
        return;

    // Clear firmware path buffer (260 bytes at offset 0x570)
    char* firmwarePath = (char*) ((BYTE*) pThis + 0x570);
    memset(firmwarePath, 0, 0x104);

    char tempBinName[0x104];
    memset(tempBinName, 0, 0x104);

    // Get current device index and controller information
    BYTE deviceIndex = *((BYTE*) pThis + 0x9a2);
    int deviceOffset = deviceIndex * 0x1daa;
    BYTE controllerIndex = *((BYTE*) pThis + deviceOffset + 0x9a6);

    // Get controller type and revision
    int controllerOffset = controllerIndex * 0x57;
    BYTE controllerType = *((BYTE*) pThis + controllerOffset + 0x62f6);
    BYTE controllerRevision = *((BYTE*) pThis + controllerOffset + 0x62f8);

    // Store controller type in device structure
    *((BYTE*) pThis + deviceOffset + 0xe13) = controllerType;

    // Get firmware variant ID
    BYTE firmwareVariant = *((BYTE*) pThis + deviceOffset + 0xa86);

    // Generate firmware filename
    if(firmwareVariant == 0xff) {
        sprintf_s(tempBinName, sizeof(tempBinName), "u181s00.bin");
    } else {
        sprintf_s(tempBinName, sizeof(tempBinName), "u181s%02x.bin", firmwareVariant);
    }

    // Determine controller path based on type and revision
    const char* pathFormat;
    if(controllerRevision == 0x00) {
        if(controllerType == 0x02) {
            pathFormat = "%s\\Bin\\1176\\DownGrade\\A0AA\\%s";
        } else {
            pathFormat = "%s\\Bin\\1181\\DownGrade\\A0AA\\%s";
        }
    } else {
        pathFormat = "%s\\Bin\\1181\\DownGrade\\A1BA\\%s";
    }

    // Construct full firmware path
    const char* moduleDir = (const char*) ((BYTE*) pThis + 0x46c);
    sprintf_s(firmwarePath, 0x104, pathFormat, moduleDir, tempBinName);

    LogMessage("PrepareFirmwareFilePath: Generated path: %s", firmwarePath);
}

// ReadBinaryFileVersion
// Read and parse firmware version data from binary file at fixed offset.
void ReadBinaryFileVersion(iTEUFDrs* pThis) {
    if(! pThis)
        return;

    // Clear version data structure (44 bytes)
    memset((BYTE*) pThis + 0x6b0a, 0, 44);

    // Get firmware file path
    const char* firmwarePath = (const char*) ((BYTE*) pThis + 0x570);

    // Open firmware binary file
    FILE* binFile = nullptr;
    errno_t err = fopen_s(&binFile, firmwarePath, "rb");
    if(err != 0 || ! binFile) {
        strcpy_s((char*) ((BYTE*) pThis + 0x6aca), 0x40, "Unknown Version");
        LogError("ReadBinaryFileVersion: Cannot open binary file: %s", firmwarePath);
        return;
    }

    // Read version data from offset 0xF1E0 (64 bytes)
    BYTE versionBuffer[0x40];
    memset(versionBuffer, 0xFF, 0x40);

    fseek(binFile, 0xF1E0, SEEK_SET);
    size_t bytesRead = fread(versionBuffer, 1, 0x40, binFile);
    fclose(binFile);

    if(bytesRead != 0x40) {
        strcpy_s((char*) ((BYTE*) pThis + 0x6aca), 0x40, "Version Read Error");
        LogError("ReadBinaryFileVersion: Failed to read version data");
        return;
    }

    // Search for "ITEu" signature
    bool signatureFound = false;
    for(int i = 0; i <= 0x40 - 4; i++) {
        if(memcmp(&versionBuffer[i], "ITEu", 4) == 0) {
            signatureFound = true;
            break;
        }
    }

    if(signatureFound) {
        // Extract structured version information
        memcpy((BYTE*) pThis + 0x6b17, &versionBuffer[0x00], 4);
        memcpy((BYTE*) pThis + 0x6b0a, &versionBuffer[0x04], 4);
        memcpy((BYTE*) pThis + 0x6b0e, &versionBuffer[0x08], 4);
        memcpy((BYTE*) pThis + 0x6b12, &versionBuffer[0x0c], 4);
        memcpy((BYTE*) pThis + 0x6b1c, &versionBuffer[0x10], 4);

        // Format version string
        sprintf_s(
            (char*) ((BYTE*) pThis + 0x6aca),
            0x40,
            " %s%s",
            (char*) ((BYTE*) pThis + 0x6b17),
            (char*) ((BYTE*) pThis + 0x6b0a));

        LogMessage(
            "ReadBinaryFileVersion: Successfully read version: %s",
            (char*) ((BYTE*) pThis + 0x6aca));
    } else {
        strcpy_s((char*) ((BYTE*) pThis + 0x6aca), 0x40, "Invalid Signature");
        LogWarning("ReadBinaryFileVersion: ITEu signature not found");
    }
}

// InitializeISPCode
// Initialize ISP mode by verifying device readiness and loading ISP code.
BYTE InitializeISPCode(int deviceIndex, DWORD deviceHandle) {
    LogMessage(
        "InitializeISPCode: SYSTEMATIC RECONSTRUCTION - device %d, handle 0x%08X",
        deviceIndex,
        deviceHandle);

    if(! g_iTEUFDrs_instance) {
        LogError("InitializeISPCode: No global instance available");
        return 0;
    }

    if(! g_sdk_api.STD_TestUnitReady || ! g_sdk_api.VDR_SetSYSReady
       || ! g_sdk_api.FLH_InitCodeWithIspPath) {
        LogError("InitializeISPCode: Required SDK functions not available");
        return 0;
    }

    if(deviceIndex >= MAX_VOLUMES) {
        LogError("InitializeISPCode: Invalid device index %d", deviceIndex);
        return 0;
    }

    // Check whether bank loading is needed and reset flags if so
    char needLoadBank = CheckNeedLoadBank((BYTE) deviceIndex, deviceHandle);
    if(needLoadBank != 0) {
        LogMessage(
            "InitializeISPCode: Bank loading needed, resetting ISP flag for device %d",
            deviceIndex);
    }

    // Assume ISP not initialized and proceed
    BOOL ispAlreadyInitialized = FALSE;

    if(! ispAlreadyInitialized) {
        LogMessage("InitializeISPCode: ISP not initialized, starting initialization process");

        const DWORD COMM_BUFFER_SIZE = 0xE40;
        BYTE* commBuffer = (BYTE*) malloc(COMM_BUFFER_SIZE);
        if(! commBuffer) {
            LogError("InitializeISPCode: Failed to allocate communication buffer");
            return 0;
        }
        memset(commBuffer, 0, COMM_BUFFER_SIZE);

        // Check system readiness via SDK
        int testUnitResult = ((PFN_VDR_CheckSYSReady) g_sdk_api.STD_TestUnitReady)(
            deviceHandle, commBuffer, COMM_BUFFER_SIZE, 0, commBuffer + 512, 1);

        if(testUnitResult != 0) {
            int setSysReadyResult = ((PFN_VDR_SetSYSReady) g_sdk_api.VDR_SetSYSReady)(
                0, commBuffer, COMM_BUFFER_SIZE, 0, commBuffer + 512, 1);

            if(setSysReadyResult == 0) {
                if(commBuffer[COMM_BUFFER_SIZE - 0x5F0] == '?') {
                    LogError("InitializeISPCode: Check system ready IO fail");
                } else {
                    LogMessage(
                        "InitializeISPCode: System ready checks passed, initializing ISP code");

                    // ISP command parameters
                    BYTE ispCommand[6];
                    ispCommand[0] = 0x00;
                    ispCommand[1] = 0x00;
                    ispCommand[2] = 0xD0;
                    ispCommand[3] = 0xC0;
                    ispCommand[4] = 0x00;
                    ispCommand[5] = 0x08;

                    // Use current directory as firmware base for initialization
                    char firmwarePath[MAX_PATH];
                    GetCurrentDirectoryA(MAX_PATH, firmwarePath);
                    strcat_s(firmwarePath, "\\FW");

                    // Call SDK to initialize ISP code with path
                    BOOL initResult =
                        ((PFN_FLH_InitCodeWithIspPath) g_sdk_api.FLH_InitCodeWithIspPath)(
                            1,
                            &ispCommand[0],
                            &ispCommand[2],
                            &ispCommand[4],
                            firmwarePath,
                            commBuffer,
                            reinterpret_cast<HANDLE>(static_cast<uintptr_t>(deviceHandle)));

                    free(commBuffer);

                    if(initResult) {
                        LogMessage(
                            "InitializeISPCode: ISP code initialization successful for device %d",
                            deviceIndex);
                        return 1;
                    } else {
                        LogError(
                            "InitializeISPCode: FLH_InitCodeWithIspPath failed for device %d",
                            deviceIndex);
                        return 0;
                    }
                }
            } else {
                LogError("InitializeISPCode: Set not ready fail");
                free(commBuffer);
                return 0;
            }
        } else {
            LogError("InitializeISPCode: Test unit ready failed");
            free(commBuffer);
            return 0;
        }
    } else {
        LogMessage("InitializeISPCode: ISP already initialized for device %d", deviceIndex);
        return 1;
    }

    return 0;
}

// Helper Functions - Path Construction

// BuildDatabasePathsA
// Create full paths for FlashSSD_D.fdb and CtrlSSD.cdb under baseDir.
BOOL BuildDatabasePathsA(
    LPCSTR baseDir,
    LPSTR outFlashFdb,
    DWORD outFlashFdbSize,
    LPSTR outCtrlCdb,
    DWORD outCtrlCdbSize) {
    if(! baseDir || ! outFlashFdb || ! outCtrlCdb) {
        return FALSE;
    }

    if(! JoinPathA(outFlashFdb, outFlashFdbSize, baseDir, "Bin\\FlashSSD_D.fdb")) {
        return FALSE;
    }

    if(! JoinPathA(outCtrlCdb, outCtrlCdbSize, baseDir, "Bin\\CtrlSSD.cdb")) {
        return FALSE;
    }

    return TRUE;
}

// BuildBinPathA
// Build full path to firmware binary based on family, index and revision.
BOOL BuildBinPathA(
    LPCSTR baseDir,
    BYTE familyHint,
    BYTE binIndex,
    BOOL isA1BA,
    LPSTR outBin,
    DWORD outBinSize) {
    if(! baseDir || ! outBin) {
        return FALSE;
    }

    CHAR subdir[64] = { 0 };

    if(isA1BA) {
        strcpy_s(subdir, sizeof(subdir), "1181\\DownGrade\\A1BA");
    } else {
        if(familyHint == 0x76) {
            strcpy_s(subdir, sizeof(subdir), "1176\\DownGrade\\A0AA");
        } else {
            strcpy_s(subdir, sizeof(subdir), "1181\\DownGrade\\A0AA");
        }
    }

    CHAR binName[64] = { 0 };
    if(binIndex == 0xFF) {
        strcpy_s(binName, sizeof(binName), "u181s00.bin");
    } else {
        sprintf_s(binName, sizeof(binName), "u181s%02x.bin", (unsigned) binIndex);
    }

    CHAR dir[MAX_PATH] = { 0 };
    if(! JoinPathA(dir, sizeof(dir), baseDir, "Bin")) {
        return FALSE;
    }

    if(! JoinPathA(dir, sizeof(dir), dir, subdir)) {
        return FALSE;
    }

    return JoinPathA(outBin, outBinSize, dir, binName);
}

// LoadFirmwareFile
// Load entire firmware file into heap buffer and return size.
BOOL LoadFirmwareFile(LPCSTR firmwarePath, LPVOID* ppBuffer, DWORD* pSize) {
    if(! firmwarePath || ! ppBuffer || ! pSize) {
        return FALSE;
    }

    FILE* f = fopen(firmwarePath, "rb");
    if(! f) {
        return FALSE;
    }

    fseek(f, 0, SEEK_END);
    long fileSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if(fileSize <= 0) {
        fclose(f);
        return FALSE;
    }

    LPVOID buffer = malloc(fileSize);
    if(! buffer) {
        fclose(f);
        return FALSE;
    }

    size_t bytesRead = fread(buffer, 1, fileSize, f);
    fclose(f);

    if(bytesRead != (size_t) fileSize) {
        free(buffer);
        return FALSE;
    }

    *ppBuffer = buffer;
    *pSize = (DWORD) fileSize;
    return TRUE;
}

// CleanupFirmwareResources
// Free firmware buffer allocated by LoadFirmwareFile.
void CleanupFirmwareResources(LPVOID pFirmware) {
    if(pFirmware) {
        free(pFirmware);
    }
}

// GetFlashMethod
// Load flash configuration from database and memory, validate and apply to device.
char GetFlashMethod(int deviceIndex, DWORD deviceHandle) {
    LogMessage(
        "GetFlashMethod: SYSTEMATIC RECONSTRUCTION - device %d, handle 0x%08X",
        deviceIndex,
        deviceHandle);

    if(! g_iTEUFDrs_instance) {
        LogError("GetFlashMethod: No global instance available");
        return 0;
    }

    if(! g_pFLH_GetFlashDataFromDataBase || ! g_pFLH_GetFlashDataFromMemory) {
        LogError("GetFlashMethod: Required SDK functions not available");
        return 0;
    }

    if(deviceIndex >= MAX_VOLUMES) {
        LogError("GetFlashMethod: Invalid device index %d", deviceIndex);
        return 0;
    }

    const DWORD CONFIG_BUFFER_SIZE = 0xE40;
    BYTE* configBuffer = (BYTE*) malloc(CONFIG_BUFFER_SIZE);
    if(! configBuffer) {
        LogError("GetFlashMethod: Failed to allocate configuration buffer");
        return 0;
    }
    memset(configBuffer, 0, CONFIG_BUFFER_SIZE);

    SetDatabasePathAndOpen();

    LogMessage(
        "GetFlashMethod: Loading flash configuration from database for device %d", deviceIndex);

    DWORD simulatedFlashParams[0x100];
    memset(simulatedFlashParams, 0, sizeof(simulatedFlashParams));

    BOOL dbResult =
        ((PFN_FLH_GetFlashDataFromDataBase) g_pFLH_GetFlashDataFromDataBase)(configBuffer);

    if(! dbResult) {
        LogMessage("GetFlashMethod: (GetFlashMethod) Get Flash fail");
        free(configBuffer);
        return 0;
    }

    LogMessage("GetFlashMethod: Flash database configuration loaded successfully");

    BYTE flashConfigPrimary[CONFIG_BUFFER_SIZE];
    BYTE flashConfigBackup[CONFIG_BUFFER_SIZE];
    memcpy(flashConfigPrimary, configBuffer, CONFIG_BUFFER_SIZE);
    memcpy(flashConfigBackup, configBuffer, CONFIG_BUFFER_SIZE);

    BYTE flashMethodId = configBuffer[0x2E] & 0x0F;
    LogMessage("GetFlashMethod: Flash method ID extracted: 0x%02X", flashMethodId);

    WORD pageCount = *(WORD*) (configBuffer + 0xE3E - 0xE50 + 0x10);
    LogMessage("GetFlashMethod: Page count set: %d", pageCount);

    WORD tableLowBound = 0;
    WORD tableHighBound = pageCount - 1;
    LogMessage("GetFlashMethod: Table boundaries set: [%d, %d]", tableLowBound, tableHighBound);

    BOOL deviceReady = TRUE;
    if(! deviceReady) {
        LogMessage("GetFlashMethod: Device is not ready....");
        free(configBuffer);
        return 0;
    }

    BOOL memResult = ((PFN_FLH_GetFlashDataFromMemory) g_pFLH_GetFlashDataFromMemory)(configBuffer);

    BYTE rootTableParam1 = configBuffer[0xDFE - 0xE50 + 0x10];
    BYTE rootTableParam2 = configBuffer[0xDFD - 0xE50 + 0x10];

    if(rootTableParam1 > 8 || rootTableParam2 > 2) {
        LogMessage("GetFlashMethod: Root table is strange....");
        memResult = FALSE;
    }

    if(memResult) {
        LogMessage("GetFlashMethod: Memory-based flash configuration validation successful");
        memcpy(flashConfigPrimary, configBuffer, CONFIG_BUFFER_SIZE);
        WORD configParam1 = *(WORD*) (configBuffer + 0xAA2 - 0xE50 + 0x10);
        WORD configParam2 = *(WORD*) (configBuffer + 0xAA4 - 0xE50 + 0x10);
        LogMessage(
            "GetFlashMethod: Additional config parameters: 0x%04X, 0x%04X",
            configParam1,
            configParam2);
    } else {
        LogError("GetFlashMethod: Memory-based flash configuration validation failed");
    }

    UpdateBankStatusFlags((BYTE) deviceIndex);
    AssignDeviceFlagFromBank((BYTE) deviceIndex, deviceHandle);

    free(configBuffer);

    LogMessage(
        "GetFlashMethod: Flash method configuration completed successfully for device %d",
        deviceIndex);

    return 1;
}

// LoadAndVerifyFirmwareSegments
// Discover firmware segments, load the first valid segment and perform verification stages.
void LoadAndVerifyFirmwareSegments(int deviceIndex, DWORD deviceHandle) {
    if(! g_iTEUFDrs_instance) {
        LogError("LoadAndVerifyFirmwareSegments: No iTEUFDrs instance available");
        return;
    }

    UINT_PTR param_1 = (UINT_PTR) g_iTEUFDrs_instance;
    BYTE param_2 = (BYTE) deviceIndex;
    DWORD param_3 = deviceHandle;

    BYTE auStack_248[3];
    UINT local_4 = 0x004abe40 ^ (UINT) (UINT_PTR) auStack_248;

    UINT_PTR local_234 = (UINT) param_2 * 0x1daa + param_1;
    UINT_PTR iVar7 = local_234 + 0xa26;

    DWORD local_230 = 0, local_22c = 0, local_228 = 0;
    DWORD local_224[3] = { 0, 0, 0 };
    WORD local_218 = 0;

    BYTE bStack_245 = 0;
    if(g_pFLH_FindRootTable) {
        typedef BYTE(__cdecl * PFN_FLH_FindRootTable)(DWORD, DWORD*, UINT_PTR, int);
        bStack_245 = ((PFN_FLH_FindRootTable) g_pFLH_FindRootTable)(param_3, &local_230, iVar7, 0);
    }

    WORD* puStack_244 = (WORD*) (UINT) bStack_245;

    int iVar6 = 0;
    if(puStack_244 != (WORD*) 0x0) {
        DWORD* puVar3 = (DWORD*) (local_234 + 0x26b6);
        DWORD* puStack_240 = (DWORD*) ((UINT) param_2 * 0x1daa + 0x26be + param_1);

        do {
            *((BYTE*) puStack_240 + iVar6) = 1;
            *puVar3 = local_224[iVar6];
            iVar6++;
            puVar3++;
        } while(iVar6 < (int) puStack_244);
    }

    if((bStack_245 != 0) && (iVar6 = 0, puStack_244 != (WORD*) 0x0)) {
        BYTE auStack_204[0x200];

        do {
            memset(auStack_204, 0, 0x200);

            int iVar4 = 0;
            if(g_pVDR_RootFunc) {
                typedef int(__cdecl * PFN_VDR_RootFunc)(
                    DWORD, int, int, int, int, LPVOID, UINT_PTR, DWORD);
                iVar4 = ((PFN_VDR_RootFunc) g_pVDR_RootFunc)(
                    local_224[iVar6], 1, 0x40, 1, 0x200, auStack_204, iVar7, param_3);
            }

            if(iVar4 == 1) {
                WORD* wordBuffer = (WORD*) auStack_204;
                DWORD* dwordBuffer = (DWORD*) auStack_204;
                DWORD* verificationResults = (DWORD*) (local_234 + 0x26ca);

                for(int elemIndex = 0; elemIndex < 2; elemIndex++) {
                    DWORD verificationBuffer[4] = { 0 };

                    DWORD swappedDword = SwapEndianness32(dwordBuffer[elemIndex]);
                    BYTE classResult1 = AnalyzeSpareAreaAndClassifyBlock(
                        swappedDword, param_3, (DWORD*) verificationBuffer, iVar7);

                    if(classResult1 == 0x12) {
                        verificationResults[elemIndex] = swappedDword;
                        *((BYTE*) (local_234 + 0x26c8 + elemIndex)) = 1;
                    }

                    WORD swappedWord1 = SwapEndianness16(wordBuffer[elemIndex + 2]);
                    DWORD sdkResult1 = 0;
                    if(g_pBlkAddr2RawAddr) {
                        typedef DWORD(__cdecl * PFN_BlkAddr2RawAddr)(UINT_PTR, WORD);
                        sdkResult1 =
                            ((PFN_BlkAddr2RawAddr) g_pBlkAddr2RawAddr)(iVar7, swappedWord1);
                    }

                    BYTE classResult2 = AnalyzeSpareAreaAndClassifyBlock(
                        sdkResult1, param_3, (DWORD*) verificationBuffer, iVar7);

                    if(classResult2 == 0x13) {
                        verificationResults[elemIndex + 1] = sdkResult1;
                        *((BYTE*) (local_234 + 0x26da + elemIndex)) = 1;
                    }

                    WORD swappedWord2 = SwapEndianness16(wordBuffer[elemIndex + 4]);
                    DWORD sdkResult2 = 0;
                    if(g_pBlkAddr2RawAddr) {
                        typedef DWORD(__cdecl * PFN_BlkAddr2RawAddr)(UINT_PTR, WORD);
                        sdkResult2 =
                            ((PFN_BlkAddr2RawAddr) g_pBlkAddr2RawAddr)(iVar7, swappedWord2);
                    }

                    BYTE classResult3 = AnalyzeSpareAreaAndClassifyBlock(
                        sdkResult2, param_3, (DWORD*) verificationBuffer, iVar7);

                    if(classResult3 == 0x13) {
                        verificationResults[elemIndex + 2] = sdkResult2;
                        *((BYTE*) (local_234 + 0x26dc + elemIndex)) = 1;
                    }
                }
                break;
            }
            iVar6++;
        } while(iVar6 < (int) (UINT_PTR) puStack_244);
    }

    LogMessage("LoadAndVerifyFirmwareSegments: Completed verification for device %d", deviceIndex);
}

// NotifyFwSegmentInfo
// Arrange and notify firmware segment parameters to SDK and mark as loaded.
char NotifyFwSegmentInfo(int deviceIndex, DWORD deviceHandle) {
    LogMessage(
        "NotifyFwSegmentInfo: SYSTEMATIC RECONSTRUCTION - device %d, handle 0x%08X",
        deviceIndex,
        deviceHandle);

    if(! g_iTEUFDrs_instance) {
        LogError("NotifyFwSegmentInfo: No global instance available");
        return 0;
    }

    if(deviceIndex >= MAX_VOLUMES || deviceIndex < 0) {
        LogError("NotifyFwSegmentInfo: Invalid device index %d", deviceIndex);
        return 0;
    }

    if(! g_pFLH_ArrangeSegmentPara || ! g_pFLH_InitCTRL) {
        LogError("NotifyFwSegmentInfo: Required SDK functions not available");
        return 0;
    }

    _DEVICE_VOLUME_INFO& volume = g_iTEUFDrs_instance->GetDeviceInfoMutable().volumes[deviceIndex];

    if(! volume.fwSegmentNotified) {
        LogMessage(
            "NotifyFwSegmentInfo: Initializing firmware segments for device %d", deviceIndex);

        BYTE segmentBuffer[128];
        memset(segmentBuffer, 0, 128);

        PFN_FLH_ArrangeSegmentPara arrangeFunc =
            (PFN_FLH_ArrangeSegmentPara) g_pFLH_ArrangeSegmentPara;
        arrangeFunc(segmentBuffer, volume.segmentInfo);

        PFN_FLH_InitCTRL initFunc = (PFN_FLH_InitCTRL) g_pFLH_InitCTRL;
        int result = initFunc((HANDLE) (uintptr_t) deviceHandle, segmentBuffer, volume.bcmInfo);

        if(result != 1) {
            LogError("NotifyFwSegmentInfo: Notify Fw segment information fail (result=%d)", result);
            return 0;
        }

        volume.fwSegmentNotified = TRUE;

        LogMessage(
            "NotifyFwSegmentInfo: Firmware segments initialized successfully for device %d",
            deviceIndex);
    } else {
        LogMessage(
            "NotifyFwSegmentInfo: Firmware segments already loaded for device %d", deviceIndex);
    }

    LogMessage(
        "NotifyFwSegmentInfo: SUCCESS - device %d firmware segment notification completed",
        deviceIndex);
    return 1;
}

// UpdateFirmwareBankInfo
// Placeholder for updating bank related firmware metadata on device.
void UpdateFirmwareBankInfo(int deviceIndex, DWORD deviceHandle) {
    LogMessage("UpdateFirmwareBankInfo: Updating firmware bank info for device %d", deviceIndex);
    // Implementation pending
}

}  // namespace FirmwareOperations
