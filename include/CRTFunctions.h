#pragma once

#include <windows.h>

// CRT initialization functions (decompiled from URescue_v81D.2.24.2.exe)
// Renamed to avoid conflicts with system CRT

// Heap initialization
int URescue_heap_init(void);

// Multithreading initialization
int URescue_mtinit(void);

// Runtime checks initialization
void URescue_RTC_Initialize(void);

// I/O initialization
int URescue_ioinit(void);

// Argument setup
int URescue_setargv(void);

// Environment setup
int URescue_setenvp(void);

// C runtime initialization
int URescue_cinit(int initterm_e);

// Get command line for Windows
LPTSTR URescue_wincmdln(void);

// Get environment strings
LPSTR URescue_crtGetEnvironmentStringsA(void);

// Exit functions
void URescue_cexit(void);
void URescue_exit(int exitcode);

// Error exit functions
void URescue_fast_error_exit(int exitcode);
void URescue_amsg_exit(int rterrnum);

// Security functions
void URescue_security_init_cookie(void);

// Main CRT startup function (decompiled)
int URescue_tmainCRTStartup(void);

// Global variables (decompiled)
extern LPSTR g_szCmdLine;
extern LPSTR g_pEnvStrs;
