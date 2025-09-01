#pragma once

#include <windows.h>
#include "SDKLoader.h"
#include "Utilities.h"

// Main application class (decompiled from iTEUFDrs constructor FUN_0040d690)
class iTEUFDrs {
public:
    // Constructor (decompiled from FUN_0040d690)
    iTEUFDrs(LPCSTR basePath);
    
    // Destructor
    ~iTEUFDrs();
    
    // Status getters
    BOOL IsInitialized() const { return m_isInitialized; }
    DWORD GetLastError() const { return m_lastError; }
    
    // SDK handle getter
    HMODULE GetSDKHandle() const { return m_hSDK; }
    
private:
    // Virtual function table pointer (first member)
    void** m_vtable;
    
    // Status flags
    BOOL m_isInitialized;        // offset +4 (param_1 + 1)
    BYTE m_errorCode;            // offset +5 (param_1 + 5) 
    BYTE m_initialized;          // offset +6
    BYTE m_reserved1;            // offset +7
    
    // Base path for SDK (offset +0x46c = 1132)
    CHAR m_basePath[260];        // param_1 + 0x11b = path buffer
    
    // SDK handle  
    HMODULE m_hSDK;              // param_1[0x21f] = SDK handle
    
    // Large data structures (from memory layout analysis)
    BYTE m_reserved2[0x200];     // param_1 + 2, size 0x200
    BYTE m_reserved3[0x40];      // offset 0x6aca, size 0x40  
    BYTE m_reserved4[0x40];      // param_1 + 0x82, size 0x40
    BYTE m_reserved5[0x800];     // param_1 + 0x41ace, size 0x800
    BYTE m_reserved6[0x100000];  // param_1 + 0x41cd0, size 0x100000
    
    // Status and configuration
    DWORD m_config1;             // param_1[0x97]
    DWORD m_config2;             // param_1[0x98] 
    DWORD m_config3;             // param_1[0x99]
    DWORD m_config4;             // param_1[0x9a]
    DWORD m_settings;            // param_1[0x81cd2] = 4
    
    // Device status flags
    BOOL m_deviceReady;          // param_1 + 0x881
    BOOL m_deviceConnected;      // param_1 + 0x882
    BOOL m_flashReady;           // param_1 + 0x885
    BOOL m_systemReady;          // param_1 + 0x886 = 1
    BOOL m_bufferReady;          // param_1 + 0x6b37
    BOOL m_controllerReady;      // param_1 + 0x81cd0

    DWORD m_lastError;
    
    // Private initialization methods
    BOOL InitializeSDK();
    BOOL GetDeviceInfo();
    void InitializeMemory();
};

// Error codes for iTEUFDrs
#define ITEUFDRS_ERROR_NONE          0
#define ITEUFDRS_ERROR_SDK_LOAD      1  // Failed to load 181FlashSDK.dll
#define ITEUFDRS_ERROR_API_BIND      2  // Failed to bind API functions 
#define ITEUFDRS_ERROR_DEVICE_INFO   3  // Failed to get device information
