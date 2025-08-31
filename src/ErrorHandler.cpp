#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ErrorHandler.h"
#include "Utilities.h"

// Function to get the last error (decompiled from FUN_0046bc3d)
DWORD GetLastErrorWrapper()
{
    DWORD error = GetLastError();
    LogMessage("Last error code: %lu", error);
    return error;
}

// Function to set the last error (decompiled from FUN_0046ba6c)
void SetLastErrorWrapper(DWORD dwErrCode)
{
    LogMessage("Setting last error code: %lu", dwErrCode);
    SetLastError(dwErrCode);
}

// Function to handle a critical error (decompiled from FUN_0045719c)
void HandleCriticalError(DWORD errorCode, LPCSTR errorMessage)
{
    LogError("Critical error occurred: %lu - %s", errorCode, errorMessage ? errorMessage : "Unknown error");
    
    // Display error message
    MessageBoxA(NULL, errorMessage, "Critical Error", MB_OK | MB_ICONERROR);
    
    // Terminate the program
    ExitProcess(errorCode);
}

// Function to handle an IO error (decompiled from FUN_00457171)
void HandleIOError(DWORD errorCode, LPCSTR operation)
{
    char errorMessage[512];
    sprintf(errorMessage, "IO Error during %s: %lu", operation ? operation : "operation", errorCode);
    
    LogError("%s", errorMessage);
    
    // Display error message
    MessageBoxA(NULL, errorMessage, "IO Error", MB_OK | MB_ICONWARNING);
}

// Function to handle a memory error (decompiled from FUN_0045f280)
void HandleMemoryError(DWORD errorCode, LPCSTR operation)
{
    char errorMessage[512];
    sprintf(errorMessage, "Memory error during %s: %lu", operation ? operation : "operation", errorCode);
    
    LogError("%s", errorMessage);
    
    // Display error message
    MessageBoxA(NULL, errorMessage, "Memory Error", MB_OK | MB_ICONWARNING);
}

// Function to handle a firmware error (decompiled from FUN_0045f2b0)
void HandleFirmwareError(DWORD errorCode, LPCSTR operation)
{
    char errorMessage[512];
    sprintf(errorMessage, "Firmware error during %s: %lu", operation ? operation : "operation", errorCode);
    
    LogError("%s", errorMessage);
    
    // Display error message
    MessageBoxA(NULL, errorMessage, "Firmware Error", MB_OK | MB_ICONWARNING);
}

// Function to check and handle an error
BOOL CheckAndHandleError(DWORD errorCode, LPCSTR operation)
{
    if (errorCode == ERROR_SUCCESS) {
        return TRUE;
    }
    
    // Determine the type of error and handle it
    switch (errorCode) {
        case ERROR_IO_DEVICE:
        case ERROR_IO_INCOMPLETE:
        case ERROR_IO_PENDING:
            HandleIOError(errorCode, operation);
            break;
            
        case ERROR_OUTOFMEMORY:
        case ERROR_NOT_ENOUGH_MEMORY:
            HandleMemoryError(errorCode, operation);
            break;
            
        case ERROR_INVALID_DATA:
        case ERROR_BAD_FORMAT:
            HandleFirmwareError(errorCode, operation);
            break;
            
        default:
            LogError("Unknown error during %s: %lu", operation ? operation : "operation", errorCode);
            break;
    }
    
    return FALSE;
}

// Function to get the error description
LPCSTR GetErrorDescription(DWORD errorCode)
{
    switch (errorCode) {
        case ERROR_SUCCESS:
            return "Operation completed successfully";
        case ERROR_IO_DEVICE:
            return "IO device error";
        case ERROR_IO_INCOMPLETE:
            return "IO operation incomplete";
        case ERROR_IO_PENDING:
            return "IO operation pending";
        case ERROR_OUTOFMEMORY:
            return "Out of memory";
        case ERROR_NOT_ENOUGH_MEMORY:
            return "Not enough memory";
        case ERROR_INVALID_DATA:
            return "Invalid data";
        case ERROR_BAD_FORMAT:
            return "Bad format";
        case ERROR_FILE_NOT_FOUND:
            return "File not found";
        case ERROR_PATH_NOT_FOUND:
            return "Path not found";
        case ERROR_ACCESS_DENIED:
            return "Access denied";
        case ERROR_INVALID_HANDLE:
            return "Invalid handle";
        case ERROR_INVALID_PARAMETER:
            return "Invalid parameter";
        default:
            return "Unknown error";
    }
}
