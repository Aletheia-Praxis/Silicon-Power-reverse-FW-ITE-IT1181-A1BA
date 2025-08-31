#pragma once

#include <windows.h>

// Error handling functions (decompiled from Ghidra)
DWORD GetLastErrorWrapper();

void SetLastErrorWrapper(DWORD dwErrCode);

void HandleCriticalError(DWORD errorCode, LPCSTR errorMessage);

void HandleIOError(DWORD errorCode, LPCSTR operation);

void HandleMemoryError(DWORD errorCode, LPCSTR operation);

void HandleFirmwareError(DWORD errorCode, LPCSTR operation);

// Additional error handling functions
BOOL CheckAndHandleError(DWORD errorCode, LPCSTR operation);

LPCSTR GetErrorDescription(DWORD errorCode);

// Error handling constants
#define ERROR_CRITICAL 0xFFFFFFFF
#define ERROR_IO 0x00000001
#define ERROR_MEMORY 0x00000002
#define ERROR_FIRMWARE 0x00000003
