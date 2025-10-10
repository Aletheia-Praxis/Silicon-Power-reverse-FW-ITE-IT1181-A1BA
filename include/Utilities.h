#pragma once

#include <cstdint>

#include "WindowsHeaders.h"

// Logging functions
void LogMessage(LPCSTR format, ...);
void LogError(const char* format, ...);
void LogWarning(const char* format, ...);

// Formatting functions
void FormatBytes(DWORD bytes, char* buffer, size_t bufferSize);
void BytesToHexString(const BYTE* pBytes, DWORD byteCount, char* buffer, size_t bufferSize);
BOOL HexStringToBytes(LPCSTR hexString, BYTE* pBytes, DWORD* pByteCount);

// Calculation functions
DWORD CalculateCRC32(const BYTE* pData, DWORD dataSize);

// Validation functions
BOOL ValidateFirmwareFile(LPCSTR filePath);

// File operation functions
BOOL CreateBackup(LPCSTR sourcePath, LPCSTR backupPath);

// Path utilities
BOOL GetModuleDirectoryA(LPSTR buffer, DWORD size);
BOOL GetTempLongPathA(LPSTR buffer, DWORD size);
BOOL JoinPathA(LPSTR outBuffer, DWORD size, LPCSTR a, LPCSTR b);
void BuildDatabasePathsA(
    const char* moduleDir,
    char* flashDbPath,
    size_t flashDbPathSize,
    char* ctrlDbPath,
    size_t ctrlDbPathSize);
void BuildBinPathA(
    const char* moduleDir,
    uint8_t familyHint,
    uint8_t binIndex,
    bool isA1BA,
    char* outBinPath,
    size_t outBinPathSize);

// Logging macros (if not already defined in stdafx.h)
#ifndef LOG_INFO
    #define LOG_INFO(format, ...) LogMessage("[INFO] " format, ##__VA_ARGS__)
#endif

#ifndef LOG_WARNING
    #define LOG_WARNING(format, ...) LogMessage("[WARNING] " format, ##__VA_ARGS__)
#endif

#ifndef LOG_ERROR
    #define LOG_ERROR(format, ...) LogMessage("[ERROR] " format, ##__VA_ARGS__)
#endif

#ifndef LOG_DEBUG
    #define LOG_DEBUG(format, ...) LogMessage("[DEBUG] " format, ##__VA_ARGS__)
#endif

// Helpers decompiled and re-implemented
void SafeCloseHandle(HANDLE& handle);
const char* FormatCapacityLabelMB(uint32_t valueMB);
uint32_t swap32_mixed(uint32_t value);
uint16_t swap16(uint16_t value);
void* alignedAllocCustom(size_t size, uint32_t alignmentMask, void** rawOut);
