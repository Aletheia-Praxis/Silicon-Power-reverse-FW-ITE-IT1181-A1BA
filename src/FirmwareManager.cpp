#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FirmwareManager.h"
#include "USBDevice.h"

// Firmware loading function
BOOL LoadFirmware(LPCSTR firmwarePath, LPVOID* ppBuffer, DWORD* pSize)
{
    // Opening the firmware file
    HANDLE hFile = CreateFileA(firmwarePath,
                              GENERIC_READ,
                              FILE_SHARE_READ,
                              NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL);
    
    if (hFile == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
    
    // Getting the file size
    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE) {
        CloseHandle(hFile);
        return FALSE;
    }
    
    // Allocating memory for the firmware
    LPVOID pBuffer = malloc(fileSize);
    if (!pBuffer) {
        CloseHandle(hFile);
        return FALSE;
    }
    
    // Reading the firmware file
    DWORD bytesRead;
    BOOL result = ReadFile(hFile, pBuffer, fileSize, &bytesRead, NULL);
    
    CloseHandle(hFile);
    
    if (!result || bytesRead != fileSize) {
        free(pBuffer);
        return FALSE;
    }
    
    *ppBuffer = pBuffer;
    *pSize = fileSize;
    return TRUE;
}

// Firmware writing function to the device
BOOL WriteFirmware(HANDLE hDevice, LPCVOID pFirmware, DWORD firmwareSize)
{
    // Parameter check
    if (!hDevice || !pFirmware || firmwareSize == 0) {
        return FALSE;
    }
    
    // Preparing the buffer for writing
    DWORD bytesWritten = 0;
    DWORD totalWritten = 0;
    const BYTE* pData = (const BYTE*)pFirmware;
    
    // Writing firmware in blocks
    while (totalWritten < firmwareSize) {
        DWORD blockSize = min(USB_BUFFER_SIZE, firmwareSize - totalWritten);
        
        BOOL result = Write(hDevice, pData + totalWritten, blockSize, &bytesWritten, NULL);
        if (!result) {
            return FALSE;
        }
        
        totalWritten += bytesWritten;
        
        // Delay between blocks
        Sleep(10);
    }
    
    return TRUE;
}

// Firmware verification function
BOOL VerifyFirmware(HANDLE hDevice, LPCVOID pFirmware, DWORD firmwareSize)
{
    // Parameter check
    if (!hDevice || !pFirmware || firmwareSize == 0) {
        return FALSE;
    }
    
    // Allocating a buffer for reading
    BYTE* pReadBuffer = (BYTE*)malloc(firmwareSize);
    if (!pReadBuffer) {
        return FALSE;
    }
    
    // Reading firmware from the device
    DWORD bytesRead = 0;
    DWORD totalRead = 0;
    
    while (totalRead < firmwareSize) {
        DWORD blockSize = min(USB_BUFFER_SIZE, firmwareSize - totalRead);
        
        BOOL result = Read(hDevice, pReadBuffer + totalRead, blockSize, &bytesRead, NULL);
        if (!result) {
            free(pReadBuffer);
            return FALSE;
        }
        
        totalRead += bytesRead;
    }
    
    // Comparing firmwares
    BOOL isMatch = (memcmp(pFirmware, pReadBuffer, firmwareSize) == 0);
    
    free(pReadBuffer);
    return isMatch;
}

// Firmware cleanup function
void CleanupFirmware(LPVOID pBuffer)
{
    if (pBuffer) {
        free(pBuffer);
    }
}
