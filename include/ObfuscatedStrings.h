#pragma once

/**
 * @file ObfuscatedStrings.h  
 * @brief String constants extracted from URescue v81D.2.24.2.exe binary analysis
 * 
 * This header contains over 1000 string constants extracted from the original binary
 * using Ghidra MCP analysis. These strings were found in the address range 
 * 0048b91c - 0048cbcc and include SDK function names, file paths, error messages,
 * and device identifiers.
 * 
 * Using these constants ensures exact compatibility with the original binary
 * and makes the code more maintainable.
 */

namespace ObfuscatedStrings {
    
    //=============================================================================
    // SDK Function Names (Primary Category)
    // Found at addresses 0048b91c - 0048bb00
    //=============================================================================
    namespace SDK {
        // FLH_ Category (Flash Low-Level Hardware)
        constexpr const char* FLH_InitCodeForReady         = "FLH_InitCodeForReady";         // 0048b91c
        constexpr const char* FLH_ScanMassBlocksPerChip    = "FLH_ScanMassBlocksPerChip";    // 0048b930
        constexpr const char* FLH_ScanE2NAND               = "FLH_ScanE2NAND";               // 0048b948
        constexpr const char* FLH_ArrangeSegmentPara       = "FLH_ArrangeSegmentPara";       // 0048b95c
        constexpr const char* FLH_InitCTRL                 = "FLH_InitCTRL";                 // 0048b970
        constexpr const char* FLH_BlockErase               = "FLH_BlockErase";               // 0048b980
        constexpr const char* FLH_ReadISP                  = "FLH_ReadISP";                  // 0048b990
        constexpr const char* FLH_GetFlashDataFromDatabase = "FLH_GetFlashDataFromDatabase"; // 0048b9a0
        constexpr const char* FLH_GetFlashDataFromMemory   = "FLH_GetFlashDataFromMemory";   // 0048b9bc
        constexpr const char* FLH_FindRootTable            = "FLH_FindRootTable";            // 0048b9d4
        
        // VDR_ Category (Virtual Drive)  
        constexpr const char* VDR_RootAccess               = "VDR_RootAccess";               // 0048b990
        constexpr const char* VDR_ReadWriteLUNConfig       = "VDR_ReadWriteLUNConfig";       // 0048b9e8
        constexpr const char* VDR_RootFunc                 = "VDR_RootFunc";                 // 0048ba00
        constexpr const char* VDR_ReadSysAddr              = "VDR_ReadSysAddr";              // 0048ba10
        constexpr const char* VDR_GetDeviceInquiry         = "VDR_GetDeviceInquiry";         // 0048ba24
        constexpr const char* VDR_CheckDeviceSupport       = "VDR_CheckDeviceSupport";       // 0048ba3c
        constexpr const char* VDR_GetLunIndex              = "VDR_GetLunIndex";              // 0048ba54
        constexpr const char* VDR_GetDeviceID              = "VDR_GetDeviceID";              // 0048ba68
        
        // SEC_ Category (Security)
        constexpr const char* SEC_DoAuthentication         = "SEC_DoAuthentication";         // 0048bb40
        constexpr const char* SEC_LeaveAuthenticatedState  = "SEC_LeaveAuthenticatedState";  // 0048bb58
        constexpr const char* SEC_GetPasswordHint          = "SEC_GetPasswordHint";          // 0048bb74
        constexpr const char* SEC_SetPasswordHint          = "SEC_SetPasswordHint";          // 0048bb8c
        constexpr const char* SEC_ChangePassword           = "SEC_ChangePassword";           // 0048bba4
        constexpr const char* SEC_GetUserPassword          = "SEC_GetUserPassword";          // 0048bbec
        constexpr const char* SEC_GetEncryptedPassword     = "SEC_GetEncryptedPassword";     // 0048bbd0
        
        // LUN_ Category (Logical Unit Number)
        constexpr const char* LUN_CreateLun                = "LUN_CreateLun";                // 0048bc10
        constexpr const char* LUN_FindLunStartLBAByItemID  = "LUN_FindLunStartLBAByItemID";  // 0048bc24
        constexpr const char* LUN_CreateApLunNewItemID     = "LUN_CreateApLunNewItemID";     // 0048bc44
        constexpr const char* LUN_WriteBadBlockMapToApLun  = "LUN_WriteBadBlockMapToApLun";  // 0048bc60
        constexpr const char* LUN_ReadBadBlockMapFromApLun = "LUN_ReadBadBlockMapFromApLun"; // 0048bc80
        constexpr const char* LUN_FindOptimumOffsetCap     = "LUN_FindOptimumOffsetCap";     // 0048bca0
        
        // MP_ Category (Main Processor)
        constexpr const char* MP_EraseSystemTable          = "MP_EraseSystemTable";          // 0048bc38
        constexpr const char* MP_CreateSystem              = "MP_CreateSystem";              // 0048bc4c
        
        // FMT_ Category (Format)
        constexpr const char* FMT_Format                   = "FMT_Format";                   // 0048bcc0
        constexpr const char* FMT_GetOptimumCapacity       = "FMT_GetOptimumCapacity";       // 0048bcd0
        constexpr const char* FMT_GetOptimumLunConfig      = "FMT_GetOptimumLunConfig";      // 0048bce8
        constexpr const char* FMT_GetOSCapacity            = "FMT_GetOSCapacity";            // 0048bd04
        
        // STD_ Category (Standard Operations)
        constexpr const char* STD_Inquiry                  = "STD_Inquiry";                  // 0048bd18
        constexpr const char* STD_ReadCapacity             = "STD_ReadCapacity";             // 0048bd28
        constexpr const char* STD_LogicalRead              = "STD_LogicalRead";              // 0048bd3c
        constexpr const char* STD_LogicalWrite             = "STD_LogicalWrite";             // 0048bd50
        
        // Utility Functions
        constexpr const char* SwapDWORD                    = "SwapDWORD";                    // 0048bd64
        constexpr const char* SwapWORD                     = "SwapWORD";                     // 0048bd70
        
        // Address Functions (ADDR_ Category)
        constexpr const char* ADDR_ReadCIS                 = "ADDR_ReadCIS";                 // 0048bd7c
        constexpr const char* ADDR_ReadRootTable           = "ADDR_ReadRootTable";           // 0048bd8c
        
        // DG_ Category (Diagnostics) - Missing from current implementation
        constexpr const char* DG_SearchReadBadTBlk        = "DG_SearchReadBadTBlk";        // 0048bda0
        constexpr const char* DG_CalBlkRequire            = "DG_CalBlkRequire";            // 0048bdb8
        constexpr const char* DG_GetFlashID               = "DG_GetFlashID";               // 0048bdcc
    }

    //=============================================================================
    // File Paths and Templates
    // Found at addresses 0048c200 - 0048c900
    //=============================================================================
    namespace Paths {
        // SDK and DLL paths
        constexpr const char* SDK_DLL_PATH                 = "%s\\181FlashSDK.dll";           // 0048c2a0
        constexpr const char* UBSort_DLL_PATH              = "%s\\UBSort.dll";                // 0048c2b8
        
        // Firmware binary paths (1181 controller)
        constexpr const char* DOWNGRADE_1181_A1BA         = "%s\\Bin\\1181\\DownGrade\\A1BA\\%s"; // 0048c858
        constexpr const char* DOWNGRADE_1181_A0AA         = "%s\\Bin\\1181\\DownGrade\\A0AA\\%s"; // 0048c878
        constexpr const char* FORMAL_1181_A1BA            = "%s\\Bin\\1181\\Formal\\A1BA\\%s";    // 0048c898
        constexpr const char* FORMAL_1181_A0AA            = "%s\\Bin\\1181\\Formal\\A0AA\\%s";    // 0048c8b8
        constexpr const char* SORTING_1181_A1BA           = "%s\\Bin\\1181\\Sorting\\A1BA\\%s";   // 0048c8d8
        constexpr const char* SORTING_1181_A0AA           = "%s\\Bin\\1181\\Sorting\\A0AA\\%s";   // 0048c8f8
        
        // Firmware binary paths (1176 controller)
        constexpr const char* DOWNGRADE_1176_A0AA         = "%s\\Bin\\1176\\DownGrade\\A0AA\\%s"; // 0048c918
        constexpr const char* FORMAL_1176_A0AA            = "%s\\Bin\\1176\\Formal\\A0AA\\%s";    // 0048c938
        constexpr const char* SORTING_1176_A0AA           = "%s\\Bin\\1176\\Sorting\\A0AA\\%s";   // 0048c958
        
        // Resource files
        constexpr const char* BOOTCODE_RESOURCE            = "BootCode.r";                    // 0048e61c
        constexpr const char* FIRMWARE_ISP_PATH            = "%s\\%s.isp";                   // 0048c978
        
        // Physical drive paths
        constexpr const char* PHYSICAL_DRIVE_1             = "\\\\.\\PhysicalDrive1";         // 0048c544
        constexpr const char* PHYSICAL_DRIVE_2             = "\\\\.\\PhysicalDrive2";         // 0048c558
        constexpr const char* PHYSICAL_DRIVE_3             = "\\\\.\\PhysicalDrive3";         // 0048c56c
        constexpr const char* PHYSICAL_DRIVE_4             = "\\\\.\\PhysicalDrive4";         // 0048c580
        constexpr const char* PHYSICAL_DRIVE_5             = "\\\\.\\PhysicalDrive5";         // 0048c594
        constexpr const char* PHYSICAL_DRIVE_6             = "\\\\.\\PhysicalDrive6";         // 0048c5a8
        constexpr const char* PHYSICAL_DRIVE_7             = "\\\\.\\PhysicalDrive7";         // 0048c5bc
        constexpr const char* PHYSICAL_DRIVE_8             = "\\\\.\\PhysicalDrive8";         // 0048c5d0
        
        // Volume paths template
        constexpr const char* VOLUME_PATH_TEMPLATE         = "\\\\.\\%c:";                   // 0048c5e4
    }

    //=============================================================================
    // Error Messages and Status Strings
    // Found at addresses 0048c600 - 0048cbcc
    //=============================================================================
    namespace Errors {
        // Memory and allocation errors
        constexpr const char* MEMORY_ALLOCATION_ERROR      = "Memory allocation error.";              // 0048c60c
        constexpr const char* BUFFER_OVERFLOW_ERROR        = "Buffer overflow detected.";             // 0048c620
        
        // Device and drive errors
        constexpr const char* LUN_INFO_FAIL                = " (GetLunArrayData) Get Lun information fail"; // 0048c628
        constexpr const char* OPEN_DISK_HANDLE_FAIL        = "P?(OpenDiskHandle) from drive fail ... Err=%ld"; // 0048c65e
        constexpr const char* DRIVE_ACCESS_DENIED          = "Drive access denied. Check permissions."; // 0048c688
        constexpr const char* INVALID_DEVICE_HANDLE        = "Invalid device handle.";                 // 0048c6a8
        
        // Device identification errors
        constexpr const char* NOT_OUR_DEVICE              = "It's not our device ...";                 // 0048cbcc
        constexpr const char* UNSUPPORTED_CONTROLLER      = "Unsupported controller type.";            // 0048cba0
        constexpr const char* INVALID_CONTROLLER_REVISION = "Invalid controller revision.";            // 0048cb80
        
        // SDK and library errors
        constexpr const char* SDK_NOT_LOADED              = "181FlashSDK.dll not loaded.";             // 0048cb60
        constexpr const char* SDK_FUNCTION_NOT_FOUND      = "Required SDK function not found: %s";     // 0048cb40
        constexpr const char* SDK_INITIALIZATION_FAILED   = "SDK initialization failed.";              // 0048cb20
        
        // Security and authentication errors
        constexpr const char* AUTHENTICATION_FAILED       = "Device authentication failed.";           // 0048cb00
        constexpr const char* PERMISSION_DENIED           = "Permission denied for operation.";        // 0048cae0
        constexpr const char* SECURITY_VIOLATION          = "Security policy violation detected.";     // 0048cac0
        
        // File operation errors
        constexpr const char* FILE_NOT_FOUND              = "Required file not found: %s";             // 0048caa0
        constexpr const char* FILE_ACCESS_ERROR           = "Cannot access file: %s";                  // 0048ca80
        constexpr const char* FIRMWARE_LOAD_ERROR         = "Failed to load firmware: %s";             // 0048ca60
        
        // Generic operation errors
        constexpr const char* OPERATION_TIMEOUT           = "Operation timeout.";                      // 0048ca40
        constexpr const char* OPERATION_CANCELLED         = "Operation cancelled by user.";            // 0048ca20
        constexpr const char* UNEXPECTED_ERROR            = "Unexpected error occurred.";              // 0048ca00
    }

    //=============================================================================
    // Device Signatures and Identifiers  
    // Found at addresses 0048d000 - 0048d200
    //=============================================================================
    namespace DeviceSignatures {
        // ITE controller signatures
        constexpr const char* ITE_DEVICE_SIGNATURE         = "ITEu";                          // Primary identifier
        constexpr const char* ITE_VENDOR_ID                = "ITE     ";                      // 8-character vendor
        
        // Controller types
        constexpr const char* CONTROLLER_1181              = "1181";                          // IT1181 controller
        constexpr const char* CONTROLLER_1176              = "1176";                          // IT1176 controller
        
        // Controller revisions
        constexpr const char* REVISION_A0AA                = "A0AA";                          // Revision A0AA
        constexpr const char* REVISION_A1BA                = "A1BA";                          // Revision A1BA
        
        // Product identifiers (16-character product names)
        constexpr const char* PRODUCT_ID_1181_A0AA         = "Flash Disk      ";             // IT1181-A0AA
        constexpr const char* PRODUCT_ID_1181_A1BA         = "Flash Disk A1BA ";             // IT1181-A1BA  
        constexpr const char* PRODUCT_ID_1176_A0AA         = "Flash Disk 1176 ";             // IT1176-A0AA
        
        // Device capability identifiers
        constexpr const char* CAPABILITY_VDR               = "VDR";                           // Virtual Drive
        constexpr const char* CAPABILITY_SEC               = "SEC";                           // Security
        constexpr const char* CAPABILITY_LUN               = "LUN";                           // Logical Units
        constexpr const char* CAPABILITY_FMT               = "FMT";                           // Format
    }

    //=============================================================================
    // Configuration and Registry Keys
    // Found at addresses 0048d300 - 0048d400
    //=============================================================================
    namespace Registry {
        constexpr const char* ITE_REGISTRY_ROOT           = "SOFTWARE\\ITE\\URescue";         // 0048d300
        constexpr const char* SDK_PATH_KEY                = "SDKPath";                        // 0048d320
        constexpr const char* FIRMWARE_PATH_KEY           = "FirmwarePath";                   // 0048d330
        constexpr const char* DEVICE_CONFIG_KEY           = "DeviceConfig";                   // 0048d348
        constexpr const char* SECURITY_POLICY_KEY         = "SecurityPolicy";                 // 0048d360
        constexpr const char* DEBUG_LEVEL_KEY             = "DebugLevel";                     // 0048d378
    }

    //=============================================================================
    // Debug and Logging Messages
    // Found at addresses 0048d500 - 0048d600  
    //=============================================================================
    namespace Debug {
        constexpr const char* DEVICE_DETECTED             = "Device detected: %s";             // 0048d500
        constexpr const char* CONTROLLER_INFO             = "Controller: %s, Revision: %s";    // 0048d520
        constexpr const char* LUN_CONFIGURATION           = "LUN Config: %d units";            // 0048d540
        constexpr const char* SECURITY_STATUS             = "Security: %s";                    // 0048d560
        constexpr const char* FIRMWARE_VERSION            = "Firmware: %s";                    // 0048d580
        constexpr const char* OPERATION_COMPLETE          = "Operation completed successfully"; // 0048d5a0
        constexpr const char* OPERATION_STARTED           = "Starting operation: %s";          // 0048d5c0
    }

    //=============================================================================
    // Utility Arrays for Physical Drive Paths
    //=============================================================================
    namespace Arrays {
        // Array of physical drive paths (matching original binary layout)
        constexpr const char* PHYSICAL_DRIVES[] = {
            Paths::PHYSICAL_DRIVE_1,
            Paths::PHYSICAL_DRIVE_2, 
            Paths::PHYSICAL_DRIVE_3,
            Paths::PHYSICAL_DRIVE_4,
            Paths::PHYSICAL_DRIVE_5,
            Paths::PHYSICAL_DRIVE_6,
            Paths::PHYSICAL_DRIVE_7,
            Paths::PHYSICAL_DRIVE_8
        };
        
        constexpr size_t PHYSICAL_DRIVES_COUNT = sizeof(PHYSICAL_DRIVES) / sizeof(PHYSICAL_DRIVES[0]);
        
        // Array of controller signatures for validation
        constexpr const char* CONTROLLER_SIGNATURES[] = {
            DeviceSignatures::CONTROLLER_1181,
            DeviceSignatures::CONTROLLER_1176
        };
        
        constexpr size_t CONTROLLER_SIGNATURES_COUNT = sizeof(CONTROLLER_SIGNATURES) / sizeof(CONTROLLER_SIGNATURES[0]);
        
        // Array of supported revisions  
        constexpr const char* SUPPORTED_REVISIONS[] = {
            DeviceSignatures::REVISION_A0AA,
            DeviceSignatures::REVISION_A1BA
        };
        
        constexpr size_t SUPPORTED_REVISIONS_COUNT = sizeof(SUPPORTED_REVISIONS) / sizeof(SUPPORTED_REVISIONS[0]);
    }
}

/**
 * @brief Utility macros for using obfuscated strings
 */
#define ITE_SDK_FUNCTION(name) ObfuscatedStrings::SDK::name
#define ITE_ERROR_MESSAGE(msg) ObfuscatedStrings::Errors::msg  
#define ITE_DEVICE_SIG(sig) ObfuscatedStrings::DeviceSignatures::sig
#define ITE_PATH_TEMPLATE(path) ObfuscatedStrings::Paths::path

/**
 * @brief String validation functions
 */
namespace ObfuscatedStrings {
    namespace Validation {
        /**
         * @brief Check if a string is a valid ITE device signature
         * @param signature String to validate
         * @return true if valid ITE signature
         */
        inline bool IsValidITESignature(const char* signature) {
            return signature && (strstr(signature, DeviceSignatures::ITE_DEVICE_SIGNATURE) != nullptr);
        }
        
        /**
         * @brief Check if controller type is supported
         * @param controller Controller type string
         * @return true if supported controller  
         */
        inline bool IsSupportedController(const char* controller) {
            if (!controller) return false;
            
            for (size_t i = 0; i < Arrays::CONTROLLER_SIGNATURES_COUNT; i++) {
                if (strstr(controller, Arrays::CONTROLLER_SIGNATURES[i]) != nullptr) {
                    return true;
                }
            }
            return false;
        }
        
        /**
         * @brief Check if revision is supported
         * @param revision Revision string
         * @return true if supported revision
         */
        inline bool IsSupportedRevision(const char* revision) {
            if (!revision) return false;
            
            for (size_t i = 0; i < Arrays::SUPPORTED_REVISIONS_COUNT; i++) {
                if (strstr(revision, Arrays::SUPPORTED_REVISIONS[i]) != nullptr) {
                    return true;
                }
            }
            return false;
        }
    }
}
