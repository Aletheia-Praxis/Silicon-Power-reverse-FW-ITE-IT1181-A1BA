#pragma once

#include <windows.h>

// Memory management functions (decompiled from Ghidra)
LPVOID AllocateMemory(DWORD size);

void FreeMemory(LPVOID pMemory);

LPVOID ReallocateMemory(LPVOID pMemory, DWORD newSize);

LPVOID AllocateAlignedMemory(DWORD size, DWORD alignment);

void FreeAlignedMemory(LPVOID pMemory);

// Memory copy and manipulation functions
LPVOID CopyMemory(LPVOID pDestination, LPCVOID pSource, DWORD size);

LPVOID MoveMemory(LPVOID pDestination, LPCVOID pSource, DWORD size);

LPVOID FillMemory(LPVOID pDestination, DWORD size, BYTE value);

int CompareMemory(LPCVOID pBuffer1, LPCVOID pBuffer2, DWORD size);

LPVOID FindByteInMemory(LPCVOID pBuffer, DWORD size, BYTE value);

// Specialized functions for firmware management
LPVOID AllocateFirmwareMemory(DWORD firmwareSize);

void FreeFirmwareMemory(LPVOID pFirmware);

BOOL CopyFirmware(LPVOID pDestination, LPCVOID pSource, DWORD firmwareSize);

BOOL VerifyFirmware(LPCVOID pFirmware1, LPCVOID pFirmware2, DWORD firmwareSize);

void ClearFirmwareMemory(LPVOID pFirmware, DWORD firmwareSize);

// Memory constants
#define FIRMWARE_MEMORY_ALIGNMENT 4096
#define DEFAULT_MEMORY_ALIGNMENT 16
#define MAX_MEMORY_SIZE 0x7FFFFFFF
#define MIN_MEMORY_SIZE 1
