#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MemoryManager.h"
#include "Utilities.h"

// Memory allocation function (decompiled from FUN_004625cc)
LPVOID AllocateMemory(DWORD size)
{
    LogMessage("Allocating memory: %lu bytes", size);
    
    if (size == 0) {
        LogError("Invalid memory allocation size: 0");
        return NULL;
    }
    
    LPVOID pMemory = malloc(size);
    
    if (!pMemory) {
        LogError("Memory allocation failed for size: %lu", size);
        return NULL;
    }
    
    // Clearing allocated memory
    memset(pMemory, 0, size);
    
    LogMessage("Memory allocated successfully: %p", pMemory);
    return pMemory;
}

// Memory deallocation function (decompiled from FUN_0045461f)
void FreeMemory(LPVOID pMemory)
{
    if (!pMemory) {
        LogWarning("Attempting to free NULL pointer");
        return;
    }
    
    LogMessage("Freeing memory: %p", pMemory);
    free(pMemory);
    LogMessage("Memory freed successfully");
}

// Memory reallocation function (decompiled from FUN_00454556)
LPVOID ReallocateMemory(LPVOID pMemory, DWORD newSize)
{
    LogMessage("Reallocating memory: %p, new size: %lu bytes", pMemory, newSize);
    
    if (newSize == 0) {
        LogError("Invalid reallocation size: 0");
        if (pMemory) {
            FreeMemory(pMemory);
        }
        return NULL;
    }
    
    LPVOID pNewMemory = realloc(pMemory, newSize);
    
    if (!pNewMemory) {
        LogError("Memory reallocation failed for size: %lu", newSize);
        return NULL;
    }
    
    LogMessage("Memory reallocated successfully: %p", pNewMemory);
    return pNewMemory;
}

// Aligned memory allocation function (decompiled from FUN_0045459c)
LPVOID AllocateAlignedMemory(DWORD size, DWORD alignment)
{
    LogMessage("Allocating aligned memory: %lu bytes, alignment: %lu", size, alignment);
    
    if (size == 0 || alignment == 0) {
        LogError("Invalid parameters for aligned allocation");
        return NULL;
    }
    
    // Using _aligned_malloc for aligned allocation
    LPVOID pMemory = _aligned_malloc(size, alignment);
    
    if (!pMemory) {
        LogError("Aligned memory allocation failed");
        return NULL;
    }
    
    // Clearing allocated memory
    memset(pMemory, 0, size);
    
    LogMessage("Aligned memory allocated successfully: %p", pMemory);
    return pMemory;
}

// Aligned memory deallocation function
void FreeAlignedMemory(LPVOID pMemory)
{
    if (!pMemory) {
        LogWarning("Attempting to free NULL aligned pointer");
        return;
    }
    
    LogMessage("Freeing aligned memory: %p", pMemory);
    _aligned_free(pMemory);
    LogMessage("Aligned memory freed successfully");
}

// Memory copy function (decompiled from FUN_0046bc3d)
LPVOID CopyMemory(LPVOID pDestination, LPCVOID pSource, DWORD size)
{
    if (!pDestination || !pSource || size == 0) {
        LogError("Invalid parameters for memory copy");
        return NULL;
    }
    
    LogMessage("Copying memory: %lu bytes from %p to %p", size, pSource, pDestination);
    
    LPVOID result = memcpy(pDestination, pSource, size);
    
    LogMessage("Memory copy completed successfully");
    return result;
}

// Memory move function (decompiled from FUN_0046ba6c)
LPVOID MoveMemory(LPVOID pDestination, LPCVOID pSource, DWORD size)
{
    if (!pDestination || !pSource || size == 0) {
        LogError("Invalid parameters for memory move");
        return NULL;
    }
    
    LogMessage("Moving memory: %lu bytes from %p to %p", size, pSource, pDestination);
    
    LPVOID result = memmove(pDestination, pSource, size);
    
    LogMessage("Memory move completed successfully");
    return result;
}

// Memory fill function (decompiled from FUN_0045719c)
LPVOID FillMemory(LPVOID pDestination, DWORD size, BYTE value)
{
    if (!pDestination || size == 0) {
        LogError("Invalid parameters for memory fill");
        return NULL;
    }
    
    LogMessage("Filling memory: %lu bytes with value 0x%02X at %p", size, value, pDestination);
    
    LPVOID result = memset(pDestination, value, size);
    
    LogMessage("Memory fill completed successfully");
    return result;
}

// Memory comparison function (decompiled from FUN_00457171)
int CompareMemory(LPCVOID pBuffer1, LPCVOID pBuffer2, DWORD size)
{
    if (!pBuffer1 || !pBuffer2 || size == 0) {
        LogError("Invalid parameters for memory comparison");
        return -1;
    }
    
    LogMessage("Comparing memory: %lu bytes between %p and %p", size, pBuffer1, pBuffer2);
    
    int result = memcmp(pBuffer1, pBuffer2, size);
    
    if (result == 0) {
        LogMessage("Memory comparison: buffers are identical");
    } else if (result < 0) {
        LogMessage("Memory comparison: buffer1 is less than buffer2");
    } else {
        LogMessage("Memory comparison: buffer1 is greater than buffer2");
    }
    
    return result;
}

// Byte search function in memory (decompiled from FUN_0045f280)
LPVOID FindByteInMemory(LPCVOID pBuffer, DWORD size, BYTE value)
{
    if (!pBuffer || size == 0) {
        LogError("Invalid parameters for byte search");
        return NULL;
    }
    
    LogMessage("Searching for byte 0x%02X in memory: %lu bytes at %p", value, size, pBuffer);
    
    LPVOID result = memchr(pBuffer, value, size);
    
    if (result) {
        LogMessage("Byte found at offset: %lu", (BYTE*)result - (BYTE*)pBuffer);
    } else {
        LogMessage("Byte not found in memory");
    }
    
    return result;
}

// Firmware memory allocation function
LPVOID AllocateFirmwareMemory(DWORD firmwareSize)
{
    LogMessage("Allocating memory for firmware: %lu bytes", firmwareSize);
    
    // Allocating aligned memory for firmware
    LPVOID pFirmware = AllocateAlignedMemory(firmwareSize, 4096); // 4KB alignment
    
    if (!pFirmware) {
        LogError("Failed to allocate firmware memory");
        return NULL;
    }
    
    LogMessage("Firmware memory allocated successfully: %p", pFirmware);
    return pFirmware;
}

// Firmware memory deallocation function
void FreeFirmwareMemory(LPVOID pFirmware)
{
    if (!pFirmware) {
        LogWarning("Attempting to free NULL firmware pointer");
        return;
    }
    
    LogMessage("Freeing firmware memory: %p", pFirmware);
    FreeAlignedMemory(pFirmware);
    LogMessage("Firmware memory freed successfully");
}

// Firmware copy function
BOOL CopyFirmware(LPVOID pDestination, LPCVOID pSource, DWORD firmwareSize)
{
    if (!pDestination || !pSource || firmwareSize == 0) {
        LogError("Invalid parameters for firmware copy");
        return FALSE;
    }
    
    LogMessage("Copying firmware: %lu bytes", firmwareSize);
    
    LPVOID result = CopyMemory(pDestination, pSource, firmwareSize);
    
    if (!result) {
        LogError("Firmware copy failed");
        return FALSE;
    }
    
    LogMessage("Firmware copy completed successfully");
    return TRUE;
}

// Firmware verification function
BOOL VerifyFirmware(LPCVOID pFirmware1, LPCVOID pFirmware2, DWORD firmwareSize)
{
    if (!pFirmware1 || !pFirmware2 || firmwareSize == 0) {
        LogError("Invalid parameters for firmware verification");
        return FALSE;
    }
    
    LogMessage("Verifying firmware: %lu bytes", firmwareSize);
    
    int result = CompareMemory(pFirmware1, pFirmware2, firmwareSize);
    
    if (result == 0) {
        LogMessage("Firmware verification successful");
        return TRUE;
    } else {
        LogMessage("Firmware verification failed");
        return FALSE;
    }
}

// Firmware memory clear function
void ClearFirmwareMemory(LPVOID pFirmware, DWORD firmwareSize)
{
    if (!pFirmware || firmwareSize == 0) {
        LogWarning("Invalid parameters for firmware memory clear");
        return;
    }
    
    LogMessage("Clearing firmware memory: %lu bytes at %p", firmwareSize, pFirmware);
    
    FillMemory(pFirmware, firmwareSize, 0xFF); // Filling with 0xFF
    
    LogMessage("Firmware memory cleared successfully");
}
