#include "../include/FirmwareManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/Globals.h"
#include "../include/USBDevice.h"
#include "../include/Utilities.h"
#include "../include/WindowsHeaders.h"

// Firmware loading function
BOOL LoadFirmware(LPCSTR firmwarePath, LPVOID* ppBuffer, DWORD* pSize) {
    // Opening the firmware file
    HANDLE hFile = CreateFileA(
        firmwarePath,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if(hFile == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    // Getting the file size
    DWORD fileSize = GetFileSize(hFile, NULL);
    if(fileSize == INVALID_FILE_SIZE) {
        CloseHandle(hFile);
        return FALSE;
    }

    // Allocating memory for the firmware
    LPVOID pBuffer = malloc(fileSize);
    if(! pBuffer) {
        CloseHandle(hFile);
        return FALSE;
    }

    // Reading the firmware file
    DWORD bytesRead;
    BOOL result = ReadFile(hFile, pBuffer, fileSize, &bytesRead, NULL);

    CloseHandle(hFile);

    if(! result || bytesRead != fileSize) {
        free(pBuffer);
        return FALSE;
    }

    *ppBuffer = pBuffer;
    *pSize = fileSize;
    return TRUE;
}

// Build SetDBPath / GetBinFilePath equivalents
BOOL BuildDatabasePathsA(
    LPCSTR baseDir,
    LPSTR outFlashFdb,
    DWORD outFlashFdbSize,
    LPSTR outCtrlCdb,
    DWORD outCtrlCdbSize) {
    if(! baseDir || ! outFlashFdb || ! outCtrlCdb)
        return FALSE;
    if(! JoinPathA(outFlashFdb, outFlashFdbSize, baseDir, "Bin\\FlashSSD_D.fdb"))
        return FALSE;
    if(! JoinPathA(outCtrlCdb, outCtrlCdbSize, baseDir, "Bin\\CtrlSSD.cdb"))
        return FALSE;
    return TRUE;
}

BOOL BuildBinPathA(
    LPCSTR baseDir,
    BYTE familyHint,
    BYTE binIndex,
    BOOL isA1BA,
    LPSTR outBin,
    DWORD outBinSize) {
    if(! baseDir || ! outBin)
        return FALSE;
    CHAR subdir[64] = { 0 };
    // familyHint: 0x76 -> 1176, else 1181; variant: A0AA or A1BA
    if(isA1BA) {
        strcpy_s(subdir, sizeof(subdir), "1181\\DownGrade\\A1BA");
    } else {
        if(familyHint == 0x76)
            strcpy_s(subdir, sizeof(subdir), "1176\\DownGrade\\A0AA");
        else
            strcpy_s(subdir, sizeof(subdir), "1181\\DownGrade\\A0AA");
    }
    CHAR binName[64] = { 0 };
    if(binIndex == 0xFF)
        strcpy_s(binName, sizeof(binName), "u181s00.bin");
    else
        sprintf_s(binName, sizeof(binName), "u181s%02x.bin", (unsigned) binIndex);
    CHAR dir[MAX_PATH] = { 0 };
    if(! JoinPathA(dir, sizeof(dir), baseDir, "Bin"))
        return FALSE;
    if(! JoinPathA(dir, sizeof(dir), dir, subdir))
        return FALSE;
    return JoinPathA(outBin, outBinSize, dir, binName);
}

// Firmware writing function to the device
BOOL WriteFirmware(HANDLE hDevice, LPCVOID pFirmware, DWORD firmwareSize) {
    // Parameter check
    if(! hDevice || ! pFirmware || firmwareSize == 0) {
        return FALSE;
    }

    // Preparing the buffer for writing
    DWORD bytesWritten = 0;
    DWORD totalWritten = 0;
    const BYTE* pData = (const BYTE*) pFirmware;

    // Writing firmware in blocks
    while(totalWritten < firmwareSize) {
        DWORD blockSize = (USB_BUFFER_SIZE < (firmwareSize - totalWritten))
                              ? USB_BUFFER_SIZE
                              : (firmwareSize - totalWritten);

        BOOL result = Write(hDevice, pData + totalWritten, blockSize, &bytesWritten, NULL);
        if(! result) {
            return FALSE;
        }

        totalWritten += bytesWritten;

        // Delay between blocks
        Sleep(10);
    }

    return TRUE;
}

// Firmware verification function
BOOL VerifyFirmware(HANDLE hDevice, LPCVOID pFirmware, DWORD firmwareSize) {
    // Parameter check
    if(! hDevice || ! pFirmware || firmwareSize == 0) {
        return FALSE;
    }

    // Allocating a buffer for reading
    BYTE* pReadBuffer = (BYTE*) malloc(firmwareSize);
    if(! pReadBuffer) {
        return FALSE;
    }

    // Reading firmware from the device
    DWORD bytesRead = 0;
    DWORD totalRead = 0;

    while(totalRead < firmwareSize) {
        DWORD blockSize = (USB_BUFFER_SIZE < (firmwareSize - totalRead))
                              ? USB_BUFFER_SIZE
                              : (firmwareSize - totalRead);

        BOOL result = Read(hDevice, pReadBuffer + totalRead, blockSize, &bytesRead, NULL);
        if(! result) {
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
void CleanupFirmware(LPVOID pBuffer) {
    if(pBuffer) {
        free(pBuffer);
    }
}

/**
 * Background monitor thread - reconstructed from function at 0x00413fd0
 *
 * This thread is responsible for handling boot code files. It performs the following steps:
 * 1. Deletes old "181FlashSDK.dll" and "BootCode.r" files from the temp directory.
 * 2. Extracts new "181FlashSDK.dll" and "BootCode.r" from the executable's resources.
 * 3. If "BootCode.r" exists, it is deleted using SHFileOperationA.
 * 4. Opens the "BootCode.r" archive and processes its contents.
 * 5. On success, sets a global flag (g_systemReadyFlag) to indicate completion.
 * 6. Handles errors by showing message boxes to the user.
 */
UINT WINAPI BackgroundMonitorThread(LPVOID pParam) {
    // This is a high-level reconstruction. The actual implementation uses
    // custom string classes and resource handling mechanisms (ATL::CSimpleStringT, etc.)

    LogMessage("BackgroundMonitorThread - Starting boot code file handling");

    char tempDir[MAX_PATH];
    strcpy_s(tempDir, sizeof(tempDir), g_tempDirectory);  // Assuming g_tempDirectory is set

    char flashSdkPath[MAX_PATH];
    char bootCodePath[MAX_PATH];

    // Build paths for the files in the temp directory
    PathCombineA(flashSdkPath, tempDir, "181FlashSDK.dll");
    PathCombineA(bootCodePath, tempDir, "BootCode.r");

    // --- Delete old files ---
    if(PathFileExistsA(flashSdkPath)) {
        SetFileAttributesA(flashSdkPath, FILE_ATTRIBUTE_NORMAL);
        if(! DeleteFileA(flashSdkPath)) {
            LogError("BackgroundMonitorThread - Failed to delete old 181FlashSDK.dll");
        } else {
            LogMessage("BackgroundMonitorThread - Deleted old 181FlashSDK.dll");
        }
    }

    if(PathFileExistsA(bootCodePath)) {
        SetFileAttributesA(bootCodePath, FILE_ATTRIBUTE_NORMAL);
        // The original uses SHFileOperationA for deletion, which can handle in-use files better
        SHFILEOPSTRUCTA fileOp = { 0 };
        fileOp.wFunc = FO_DELETE;
        fileOp.pFrom = bootCodePath;
        fileOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
        if(SHFileOperationA(&fileOp) != 0) {
            LogError(
                "BackgroundMonitorThread - Failed to delete old BootCode.r using SHFileOperation");
        } else {
            LogMessage("BackgroundMonitorThread - Deleted old BootCode.r");
        }
    }

    // --- Extract new files from resources ---
    // The original uses a custom resource handling mechanism. We'll simulate it.
    // Resource "BINARYFILE", ID 97 -> 181FlashSDK.dll
    // Resource "BINARYFILE", ID 106 -> BootCode.r

    if(! ExtractResourceToFile("BINARYFILE", 97, flashSdkPath)) {
        AfxMessageBox("Cannot open the flash SDK file", MB_OK | MB_ICONERROR);
        LogError("BackgroundMonitorThread - Failed to extract 181FlashSDK.dll");
        return 1;  // Indicate failure
    }
    LogMessage("BackgroundMonitorThread - Extracted 181FlashSDK.dll");

    if(! ExtractResourceToFile("BINARYFILE", 106, bootCodePath)) {
        AfxMessageBox("Cannot open the boot code file", MB_OK | MB_ICONERROR);
        LogError("BackgroundMonitorThread - Failed to extract BootCode.r");
        return 1;  // Indicate failure
    }
    LogMessage("BackgroundMonitorThread - Extracted BootCode.r");

    // --- Process the BootCode.r archive ---
    // The original code contains complex logic to parse this file.
    // This involves custom CArchive-like classes and decompression.
    // We will represent this as a placeholder function call.

    BOOL bootCodeProcessed = ProcessBootCodeArchive(bootCodePath);

    if(! bootCodeProcessed) {
        AfxMessageBox(
            "Pointer reset failed", MB_OK | MB_ICONERROR);  // Corresponds to "Pointer reset failed"
        LogError("BackgroundMonitorThread - Failed to process BootCode.r archive");
        return 1;  // Indicate failure
    }
    LogMessage("BackgroundMonitorThread - Successfully processed BootCode.r archive");

    // Set the global ready flag on success
    g_systemReadyFlag = TRUE;
    LogMessage("BackgroundMonitorThread - System is ready");

    return 0;  // Indicate success
}

/**
 * Placeholder for the complex logic of processing the BootCode.r archive.
 * In the original code, this involves creating an archive object, seeking,
 * reading, and decompressing data.
 */
BOOL ProcessBootCodeArchive(LPCSTR archivePath) {
    // Simulate the complex processing steps
    LogMessage("ProcessBootCodeArchive - Simulating processing of %s", archivePath);
    // In a real implementation, this would involve file I/O, decompression, etc.
    // For now, we just return success.
    return TRUE;
}

/**
 * Placeholder for extracting an embedded resource to a file.
 */
BOOL ExtractResourceToFile(LPCSTR resourceType, UINT resourceID, LPCSTR outputPath) {
    HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(resourceID), resourceType);
    if(! hRes)
        return FALSE;

    HGLOBAL hResLoad = LoadResource(NULL, hRes);
    if(! hResLoad)
        return FALSE;

    LPVOID pResLock = LockResource(hResLoad);
    if(! pResLock)
        return FALSE;

    DWORD dwSize = SizeofResource(NULL, hRes);

    HANDLE hFile =
        CreateFile(outputPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE)
        return FALSE;

    DWORD bytesWritten;
    BOOL bSuccess = WriteFile(hFile, pResLock, dwSize, &bytesWritten, NULL);

    CloseHandle(hFile);

    return bSuccess && (bytesWritten == dwSize);
}
