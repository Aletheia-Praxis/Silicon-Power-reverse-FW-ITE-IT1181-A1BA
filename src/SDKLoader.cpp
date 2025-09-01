#include <windows.h>
#include <stdio.h>
#include "SDKLoader.h"
#include "Utilities.h"

BOOL Load181FlashSDK(LPCSTR baseDir, HMODULE* outModule)
{
    if (!baseDir || !outModule) return FALSE;
    CHAR path[MAX_PATH] = {0};
    if (!JoinPathA(path, sizeof(path), baseDir, "181FlashSDK.dll")) return FALSE;
    HMODULE h = LoadLibraryA(path);
    if (!h) {
        LogMessage("Failed to load 181FlashSDK.dll from: %s", path);
        return FALSE;
    }
    *outModule = h;
    LogMessage("Loaded 181FlashSDK.dll from: %s", path);
    return TRUE;
}

void Unload181FlashSDK(HMODULE hModule)
{
    if (hModule) {
        FreeLibrary(hModule);
        LogMessage("Unloaded 181FlashSDK.dll");
    }
}
