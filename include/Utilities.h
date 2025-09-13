#pragma once

// clang-format off
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <afxwin.h>
#include <windows.h>

#include <cstdint>
// clang-format on

// Logging functions
void LogMessage(LPCSTR format, ...);
void LogError(const char* format, ...);
void LogWarning(const char* format, ...);

// Formatting functions
CString FormatBytes(DWORD bytes);
CString BytesToHexString(const BYTE* pBytes, DWORD byteCount);
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

// Logging macros
#define LOG_INFO(format, ...)    LogMessage("[INFO] " format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) LogMessage("[WARNING] " format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...)   LogMessage("[ERROR] " format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...)   LogMessage("[DEBUG] " format, ##__VA_ARGS__)

// Helpers decompiled and re-implemented
void SafeCloseHandle(HANDLE& handle);
const char* FormatCapacityLabelMB(uint32_t valueMB);
uint32_t swap32_mixed(uint32_t value);
uint16_t swap16(uint16_t value);
void* alignedAllocCustom(size_t size, uint32_t alignmentMask, void** rawOut);
