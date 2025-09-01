#pragma once

#include <windows.h>

// Load 181FlashSDK.dll from the provided base directory (baseDir + "\\181FlashSDK.dll")
// Returns TRUE on success and sets outModule.
BOOL Load181FlashSDK(LPCSTR baseDir, HMODULE* outModule);

// Unload previously loaded SDK module
void Unload181FlashSDK(HMODULE hModule);
