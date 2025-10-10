#pragma once

#include "WindowsHeaders.h"

// Forward declarations
class iTEUFDrs;

// FirmwareOperations namespace with firmware helper functions
namespace FirmwareOperations {

// PrepareFirmwareFilePath - construct firmware file path for current controller
void PrepareFirmwareFilePath(iTEUFDrs* pInstance);

// ReadBinaryFileVersion - read firmware version block from firmware binary
void ReadBinaryFileVersion(iTEUFDrs* pInstance);

// InitializeISPCode - initialize ISP mode using firmware path
BYTE InitializeISPCode(int deviceIndex, DWORD deviceHandle);

// GetFlashMethod - load and prepare flash method and configuration
char GetFlashMethod(int deviceIndex, DWORD deviceHandle);

// LoadAndVerifyFirmwareSegments - load firmware segments and verify integrity
void LoadAndVerifyFirmwareSegments(int deviceIndex, DWORD deviceHandle);

// NotifyFwSegmentInfo - initialize or notify firmware segment metadata
char NotifyFwSegmentInfo(int deviceIndex, DWORD deviceHandle);

// UpdateFirmwareBankInfo - update firmware bank metadata for device
void UpdateFirmwareBankInfo(int deviceIndex, DWORD deviceHandle);

// BuildDatabasePathsA - build full paths for SDK database files
BOOL BuildDatabasePathsA(
    LPCSTR baseDir,
    LPSTR outFlashFdb,
    DWORD outFlashFdbSize,
    LPSTR outCtrlCdb,
    DWORD outCtrlCdbSize);

// BuildBinPathA - build firmware binary file path
BOOL BuildBinPathA(
    LPCSTR baseDir,
    BYTE familyHint,
    BYTE binIndex,
    BOOL isA1BA,
    LPSTR outBin,
    DWORD outBinSize);

// LoadFirmwareFile - load firmware file into allocated memory
BOOL LoadFirmwareFile(LPCSTR firmwarePath, LPVOID* ppBuffer, DWORD* pSize);

// CleanupFirmwareResources - free firmware memory allocated by LoadFirmwareFile
void CleanupFirmwareResources(LPVOID pFirmware);

}  // namespace FirmwareOperations

// Constants

// DEVICE_STRIDE - bytes between device entries
#define DEVICE_STRIDE 0x1DAA

// CONTROLLER_STRIDE - bytes between controller entries
#define CONTROLLER_STRIDE 0x57

// FIRMWARE_PATH_BUFFER_SIZE - firmware file path buffer size in bytes
#define FIRMWARE_PATH_BUFFER_SIZE 0x104 /* 260 */

// FIRMWARE_VERSION_OFFSET - firmware version offset in binary file
#define FIRMWARE_VERSION_OFFSET 0xF1E0

// FIRMWARE_VERSION_SIZE - firmware version data size in bytes
#define FIRMWARE_VERSION_SIZE 0x40 /* 64 */

// ISP_COMM_BUFFER_SIZE - ISP communication buffer size in bytes
#define ISP_COMM_BUFFER_SIZE 0xE40 /* 3648 */

// VERSION_STRING_BUFFER_SIZE - version string buffer size in bytes
#define VERSION_STRING_BUFFER_SIZE 0x40 /* 64 */

// iTEUFDrs structure offsets relative to instance base
#define OFFSET_BASE_DIRECTORY       0x46C
#define OFFSET_FIRMWARE_PATH        0x570
#define OFFSET_CURRENT_DEVICE_INDEX 0x9A2
#define OFFSET_DEVICE_ISP_FLAG      0x9F9
#define OFFSET_DEVICE_CONTROLLER_ID 0x9A6
#define OFFSET_DEVICE_FW_VARIANT    0xA86
#define OFFSET_DEVICE_CHIP_TYPE     0xE13
#define OFFSET_CONTROLLER_CHIP_ID   0x62F6
#define OFFSET_CONTROLLER_REVISION  0x62F8
#define OFFSET_VERSION_DATA_START   0x6B0A
#define OFFSET_VERSION_COMPONENT1   0x6B17
#define OFFSET_VERSION_STRING       0x6ACA

// Firmware operation result codes
#define FW_OP_SUCCESS                0
#define FW_OP_ERROR_INVALID_PARAM    1
#define FW_OP_ERROR_FILE_NOT_FOUND   2
#define FW_OP_ERROR_FILE_READ        3
#define FW_OP_ERROR_DEVICE_NOT_READY 4
#define FW_OP_ERROR_ISP_INIT_FAILED  5
#define FW_OP_ERROR_VERSION_INVALID  6
