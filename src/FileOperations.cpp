#include "FileOperations.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "Utilities.h"

// File creation function (decompiled from FUN_00451e6d)
HANDLE CreateFileWrapper(
    LPCSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile) {
    // Logging the operation
    LogMessage("Creating file: %s", lpFileName);

    // Creating the file via Windows API
    HANDLE hFile = CreateFileA(
        lpFileName,
        dwDesiredAccess,
        dwShareMode,
        lpSecurityAttributes,
        dwCreationDisposition,
        dwFlagsAndAttributes,
        hTemplateFile);

    if(hFile == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        LogError("Failed to create file: %s, Error: %lu", lpFileName, error);
        return INVALID_HANDLE_VALUE;
    }

    LogMessage("File created successfully: %s", lpFileName);
    return hFile;
}

// File reading function (decompiled from FUN_00451ec3)
BOOL ReadFileWrapper(
    HANDLE hFile,
    LPVOID lpBuffer,
    DWORD nNumberOfBytesToRead,
    LPDWORD lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped) {
    if(! hFile || hFile == INVALID_HANDLE_VALUE || ! lpBuffer || ! lpNumberOfBytesRead) {
        LogError("Invalid parameters for ReadFile");
        return FALSE;
    }

    // Reading the file
    BOOL result =
        ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);

    if(! result) {
        DWORD error = GetLastError();
        if(error != ERROR_IO_PENDING) {
            LogError("ReadFile failed with error: %lu", error);
            return FALSE;
        }
    }

    LogMessage("Read %lu bytes from file", *lpNumberOfBytesRead);
    return TRUE;
}

// File writing function (decompiled from FUN_0045f280)
BOOL WriteFileWrapper(
    HANDLE hFile,
    LPCVOID lpBuffer,
    DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped) {
    if(! hFile || hFile == INVALID_HANDLE_VALUE || ! lpBuffer || ! lpNumberOfBytesWritten) {
        LogError("Invalid parameters for WriteFile");
        return FALSE;
    }

    // Writing the file
    BOOL result =
        WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);

    if(! result) {
        DWORD error = GetLastError();
        if(error != ERROR_IO_PENDING) {
            LogError("WriteFile failed with error: %lu", error);
            return FALSE;
        }
    }

    LogMessage("Written %lu bytes to file", *lpNumberOfBytesWritten);
    return TRUE;
}

// File closing function (decompiled from FUN_0045f2b0)
BOOL CloseFileWrapper(HANDLE hFile) {
    if(! hFile || hFile == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    BOOL result = CloseHandle(hFile);
    if(result) {
        LogMessage("File closed successfully");
    } else {
        LogError("Failed to close file");
    }

    return result;
}

// File copying function (decompiled from FUN_00458a8f)
BOOL CopyFileWrapper(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists) {
    LogMessage("Copying file from %s to %s", lpExistingFileName, lpNewFileName);

    BOOL result = CopyFileA(lpExistingFileName, lpNewFileName, bFailIfExists);

    if(! result) {
        DWORD error = GetLastError();
        LogError("CopyFile failed with error: %lu", error);
        return FALSE;
    }

    LogMessage("File copied successfully");
    return TRUE;
}

// File deletion function (decompiled from FUN_00459f98)
BOOL DeleteFileWrapper(LPCSTR lpFileName) {
    LogMessage("Deleting file: %s", lpFileName);

    BOOL result = DeleteFileA(lpFileName);

    if(! result) {
        DWORD error = GetLastError();
        LogError("DeleteFile failed with error: %lu", error);
        return FALSE;
    }

    LogMessage("File deleted successfully");
    return TRUE;
}

// Directory creation function (decompiled from FUN_00459fdd)
BOOL CreateDirectoryWrapper(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes) {
    LogMessage("Creating directory: %s", lpPathName);

    BOOL result = CreateDirectoryA(lpPathName, lpSecurityAttributes);

    if(! result) {
        DWORD error = GetLastError();
        if(error != ERROR_ALREADY_EXISTS) {
            LogError("CreateDirectory failed with error: %lu", error);
            return FALSE;
        }
        LogMessage("Directory already exists");
    } else {
        LogMessage("Directory created successfully");
    }

    return TRUE;
}

// File attributes setting function (decompiled from FUN_004625cc)
BOOL SetFileAttributesWrapper(LPCSTR lpFileName, DWORD dwFileAttributes) {
    LogMessage("Setting attributes for file: %s", lpFileName);

    BOOL result = SetFileAttributesA(lpFileName, dwFileAttributes);

    if(! result) {
        DWORD error = GetLastError();
        LogError("SetFileAttributes failed with error: %lu", error);
        return FALSE;
    }

    LogMessage("File attributes set successfully");
    return TRUE;
}

// File size retrieval function (decompiled from FUN_0045461f)
DWORD GetFileSizeWrapper(HANDLE hFile, LPDWORD lpFileSizeHigh) {
    if(! hFile || hFile == INVALID_HANDLE_VALUE) {
        LogError("Invalid file handle for GetFileSize");
        return INVALID_FILE_SIZE;
    }

    DWORD fileSize = GetFileSize(hFile, lpFileSizeHigh);

    if(fileSize == INVALID_FILE_SIZE) {
        DWORD error = GetLastError();
        LogError("GetFileSize failed with error: %lu", error);
        return INVALID_FILE_SIZE;
    }

    LogMessage("File size: %lu bytes", fileSize);
    return fileSize;
}

// File moving function (decompiled from FUN_00454556)
BOOL MoveFileWrapper(LPCSTR lpExistingFileName, LPCSTR lpNewFileName) {
    LogMessage("Moving file from %s to %s", lpExistingFileName, lpNewFileName);

    BOOL result = MoveFileA(lpExistingFileName, lpNewFileName);

    if(! result) {
        DWORD error = GetLastError();
        LogError("MoveFile failed with error: %lu", error);
        return FALSE;
    }

    LogMessage("File moved successfully");
    return TRUE;
}

// File existence check function (decompiled from FUN_0045459c)
BOOL FileExists(LPCSTR lpFileName) {
    DWORD fileAttributes = GetFileAttributesA(lpFileName);

    if(fileAttributes == INVALID_FILE_ATTRIBUTES) {
        DWORD error = GetLastError();
        if(error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
            return FALSE;
        }
        LogError("GetFileAttributes failed with error: %lu", error);
        return FALSE;
    }

    return TRUE;
}

// Function to create a backup copy of a file
BOOL CreateBackup(LPCSTR lpFileName, LPCSTR lpBackupSuffix) {
    if(! FileExists(lpFileName)) {
        LogError("Source file does not exist: %s", lpFileName);
        return FALSE;
    }

    char backupFileName[MAX_PATH];
    strcpy(backupFileName, lpFileName);
    strcat(backupFileName, lpBackupSuffix);

    return CopyFileWrapper(lpFileName, backupFileName, FALSE);
}

// Function to restore from a backup copy
BOOL RestoreFromBackup(LPCSTR lpFileName, LPCSTR lpBackupSuffix) {
    char backupFileName[MAX_PATH];
    strcpy(backupFileName, lpFileName);
    strcat(backupFileName, lpBackupSuffix);

    if(! FileExists(backupFileName)) {
        LogError("Backup file does not exist: %s", backupFileName);
        return FALSE;
    }

    return CopyFileWrapper(backupFileName, lpFileName, FALSE);
}
