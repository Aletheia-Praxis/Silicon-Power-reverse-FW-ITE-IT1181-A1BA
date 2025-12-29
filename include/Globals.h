#pragma once
#include "WindowsHeaders.h"

// Global variables shared across the application, based on Ghidra analysis

// Application-global variables (mapped from original binary globals)
extern DWORD g_windowsVersion;          // DAT_004ad748
extern DWORD g_windowsVersionBuild;     // Additional Windows version build
extern char g_cmdLineArgs[128];         // DAT_004ad6c0
extern BOOL g_calledFromURescueM;       // DAT_004ad740
extern BOOL g_systemReadyFlag;          // DAT_004ad744
extern char g_tempDirectory[MAX_PATH];  // DAT_004ad74c
