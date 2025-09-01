#pragma once

#include <windows.h>

// CRT initialization functions (decompiled from URescue_v81D.2.24.2.exe)

// Heap initialization
int __heap_init(void);

// Multithreading initialization  
int __mtinit(void);

// Runtime checks initialization
void __RTC_Initialize(void);

// I/O initialization
int __ioinit(void);

// Argument setup
int __setargv(void);

// Environment setup
int __setenvp(void);

// C runtime initialization
int __cinit(int initterm_e);

// Get command line for Windows
LPTSTR __wincmdln(void);

// Get environment strings
LPSTR ___crtGetEnvironmentStringsA(void);

// Exit functions
void __cexit(void);
void _exit(int exitcode);

// Error exit functions
void fast_error_exit(int exitcode);
void __amsg_exit(int rterrnum);

// Security functions
void ___security_init_cookie(void);
