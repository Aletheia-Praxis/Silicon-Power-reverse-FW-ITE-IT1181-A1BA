#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ResourceManager.h"
#include "Utilities.h"

// Resource finding function (decompiled from FUN_0046bc3d)
HRSRC FindResourceWrapper(HMODULE hModule, LPCSTR lpName, LPCSTR lpType)
{
    LogMessage("Finding resource: %s, type: %s", lpName ? lpName : "NULL", lpType ? lpType : "NULL");
    
    HRSRC hResource = FindResourceA(hModule, lpName, lpType);
    
    if (!hResource) {
        DWORD error = GetLastError();
        LogError("FindResource failed with error: %lu", error);
        return NULL;
    }
    
    LogMessage("Resource found successfully");
    return hResource;
}

// Resource loading function (decompiled from FUN_0046ba6c)
HGLOBAL LoadResourceWrapper(HMODULE hModule, HRSRC hResInfo)
{
    if (!hResInfo) {
        LogError("Invalid resource handle");
        return NULL;
    }
    
    LogMessage("Loading resource");
    
    HGLOBAL hResource = LoadResource(hModule, hResInfo);
    
    if (!hResource) {
        DWORD error = GetLastError();
        LogError("LoadResource failed with error: %lu", error);
        return NULL;
    }
    
    LogMessage("Resource loaded successfully");
    return hResource;
}

// Resource locking function (decompiled from FUN_0045719c)
LPVOID LockResourceWrapper(HGLOBAL hResData)
{
    if (!hResData) {
        LogError("Invalid resource data handle");
        return NULL;
    }
    
    LogMessage("Locking resource");
    
    LPVOID pResource = LockResource(hResData);
    
    if (!pResource) {
        LogError("LockResource failed");
        return NULL;
    }
    
    LogMessage("Resource locked successfully");
    return pResource;
}

// Resource size retrieval function (decompiled from FUN_00457171)
DWORD SizeofResourceWrapper(HMODULE hModule, HRSRC hResInfo)
{
    if (!hResInfo) {
        LogError("Invalid resource handle for SizeofResource");
        return 0;
    }
    
    LogMessage("Getting resource size");
    
    DWORD resourceSize = SizeofResource(hModule, hResInfo);
    
    if (resourceSize == 0) {
        DWORD error = GetLastError();
        LogError("SizeofResource failed with error: %lu", error);
        return 0;
    }
    
    LogMessage("Resource size: %lu bytes", resourceSize);
    return resourceSize;
}

// Function to load firmware from resources
BOOL LoadFirmwareFromResource(LPCSTR resourceName, LPVOID* ppBuffer, DWORD* pSize)
{
    LogMessage("Loading firmware from resource: %s", resourceName);
    
    // Search for the firmware resource
    HRSRC hResInfo = FindResourceWrapper(NULL, resourceName, RT_RCDATA);
    if (!hResInfo) {
        LogError("Firmware resource not found: %s", resourceName);
        return FALSE;
    }
    
    // Get the resource size
    DWORD resourceSize = SizeofResourceWrapper(NULL, hResInfo);
    if (resourceSize == 0) {
        LogError("Failed to get firmware resource size");
        return FALSE;
    }
    
    // Load the resource
    HGLOBAL hResource = LoadResourceWrapper(NULL, hResInfo);
    if (!hResource) {
        LogError("Failed to load firmware resource");
        return FALSE;
    }
    
    // Lock the resource
    LPVOID pResource = LockResourceWrapper(hResource);
    if (!pResource) {
        LogError("Failed to lock firmware resource");
        return FALSE;
    }
    
    // Allocate memory for the firmware copy
    LPVOID pBuffer = malloc(resourceSize);
    if (!pBuffer) {
        LogError("Failed to allocate memory for firmware");
        return FALSE;
    }
    
    // Copy the firmware
    memcpy(pBuffer, pResource, resourceSize);
    
    *ppBuffer = pBuffer;
    *pSize = resourceSize;
    
    LogMessage("Firmware loaded successfully: %lu bytes", resourceSize);
    return TRUE;
}

// Function to load BootCode from resources
BOOL LoadBootCodeFromResource(LPVOID* ppBuffer, DWORD* pSize)
{
    return LoadFirmwareFromResource("BootCode", ppBuffer, pSize);
}

// Function to load SDK from resources
BOOL LoadSDKFromResource(LPVOID* ppBuffer, DWORD* pSize)
{
    return LoadFirmwareFromResource("FlashSDK", ppBuffer, pSize);
}

// Function to check for resource existence
BOOL ResourceExists(LPCSTR resourceName, LPCSTR resourceType)
{
    HRSRC hResInfo = FindResourceA(NULL, resourceName, resourceType);
    return (hResInfo != NULL);
}

// Function to get a list of resources
DWORD GetResourceList(LPCSTR resourceType, LPCSTR* resourceNames, DWORD maxCount)
{
    DWORD count = 0;
    
    // Iterate through resources of the specified type
    HRSRC hResInfo = FindResourceA(NULL, MAKEINTRESOURCE(1), resourceType);
    if (hResInfo) {
        // You can add logic to iterate through resources here
        // This is a simplified version
        LogMessage("Resource enumeration not fully implemented");
    }
    
    return count;
}

// Function to save a resource to a file
BOOL SaveResourceToFile(LPCSTR resourceName, LPCSTR resourceType, LPCSTR fileName)
{
    LogMessage("Saving resource %s to file: %s", resourceName, fileName);
    
    // Load the resource
    HRSRC hResInfo = FindResourceWrapper(NULL, resourceName, resourceType);
    if (!hResInfo) {
        return FALSE;
    }
    
    DWORD resourceSize = SizeofResourceWrapper(NULL, hResInfo);
    if (resourceSize == 0) {
        return FALSE;
    }
    
    HGLOBAL hResource = LoadResourceWrapper(NULL, hResInfo);
    if (!hResource) {
        return FALSE;
    }
    
    LPVOID pResource = LockResourceWrapper(hResource);
    if (!pResource) {
        return FALSE;
    }
    
    // Create the file
    HANDLE hFile = CreateFileA(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 
                              FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        LogError("Failed to create file: %s", fileName);
        return FALSE;
    }
    
    // Write the resource to the file
    DWORD bytesWritten;
    BOOL result = WriteFile(hFile, pResource, resourceSize, &bytesWritten, NULL);
    
    CloseHandle(hFile);
    
    if (!result || bytesWritten != resourceSize) {
        LogError("Failed to write resource to file");
        return FALSE;
    }
    
    LogMessage("Resource saved to file successfully");
    return TRUE;
}

// Function to load a resource from a file
BOOL LoadResourceFromFile(LPCSTR fileName, LPVOID* ppBuffer, DWORD* pSize)
{
    LogMessage("Loading resource from file: %s", fileName);
    
    // Open the file
    HANDLE hFile = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, 
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        LogError("Failed to open file: %s", fileName);
        return FALSE;
    }
    
    // Get the file size
    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE) {
        LogError("Failed to get file size: %s", fileName);
        CloseHandle(hFile);
        return FALSE;
    }
    
    // Allocate memory
    LPVOID pBuffer = malloc(fileSize);
    if (!pBuffer) {
        LogError("Failed to allocate memory for file content");
        CloseHandle(hFile);
        return FALSE;
    }
    
    // Read the file
    DWORD bytesRead;
    BOOL result = ReadFile(hFile, pBuffer, fileSize, &bytesRead, NULL);
    
    CloseHandle(hFile);
    
    if (!result || bytesRead != fileSize) {
        LogError("Failed to read file: %s", fileName);
        free(pBuffer);
        return FALSE;
    }
    
    *ppBuffer = pBuffer;
    *pSize = fileSize;
    
    LogMessage("Resource loaded from file successfully: %lu bytes", fileSize);
    return TRUE;
}
