#pragma once

#include <windows.h>

// File operations (decompiled from Ghidra)
HANDLE CreateFileWrapper(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, 
                        LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, 
                        DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);

BOOL ReadFileWrapper(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, 
                    LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);

BOOL WriteFileWrapper(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, 
                     LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);

BOOL CloseFileWrapper(HANDLE hFile);

BOOL CopyFileWrapper(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists);

BOOL DeleteFileWrapper(LPCSTR lpFileName);

BOOL CreateDirectoryWrapper(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);

BOOL SetFileAttributesWrapper(LPCSTR lpFileName, DWORD dwFileAttributes);

DWORD GetFileSizeWrapper(HANDLE hFile, LPDWORD lpFileSizeHigh);

BOOL MoveFileWrapper(LPCSTR lpExistingFileName, LPCSTR lpNewFileName);

BOOL FileExists(LPCSTR lpFileName);

// Additional file operations
BOOL CreateBackup(LPCSTR lpFileName, LPCSTR lpBackupSuffix);

BOOL RestoreFromBackup(LPCSTR lpFileName, LPCSTR lpBackupSuffix);

// Constants for file operations
#define DEFAULT_BACKUP_SUFFIX ".bak"
#define TEMP_FILE_SUFFIX ".tmp"
#define MAX_FILE_PATH 260
#define MAX_FILE_NAME 256
