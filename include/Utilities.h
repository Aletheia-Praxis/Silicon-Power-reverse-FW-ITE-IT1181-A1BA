#pragma once

#include <windows.h>
#include <afxwin.h>

// Logging functions
void LogMessage(LPCSTR format, ...);

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
#define LOG_INFO(format, ...) LogMessage("[INFO] " format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) LogMessage("[WARNING] " format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) LogMessage("[ERROR] " format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) LogMessage("[DEBUG] " format, ##__VA_ARGS__)
