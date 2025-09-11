#include "CryptoManager.h"

#include <comutil.h>
#include <oleauto.h>

#include "Utilities.h"

// Hash test vectors for validation (based on Ghidra analysis)
const HashTestVector g_hashTestVectors[] = {
    { "", 0x00000000 },      // Empty string
    { "A", 0x00000041 },     // Single character
    { "ITE", 0x00007B45 },   // ITE signature
    { "ITEu", 0x0017C175 },  // ITEu device signature
    { "1181", 0x000C4D95 },  // Controller type 1181
    { "1176", 0x000C4D77 },  // Controller type 1176
    { "A0AA", 0x0C43F421 },  // Revision A0AA
    { "A1BA", 0x0C43F6E2 },  // Revision A1BA
};

const size_t g_hashTestVectorCount = sizeof(g_hashTestVectors) / sizeof(HashTestVector);

/**
 * @brief Hash function for char* strings (exact replica from address 0x0041fe02)
 *
 * This function implements the exact hash algorithm found in the original binary.
 * Ghidra decompilation shows a simple multiplicative hash with constant 0x21 (33).
 *
 * Original assembly pattern:
 * - Initialize hash to 0
 * - For each character: hash = hash * 0x21 + character_value
 * - Return final hash value
 */
uint32_t CryptoManager::HashKey_char(const char* input) {
    if(! input) {
        return 0;
    }

    uint32_t hash = HASH_INIT_VALUE;

    // Exact replication of the algorithm from 0x0041fe02
    for(; *input != '\0'; input++) {
        hash = hash * HASH_MULTIPLIER + (int) *input;
    }

    LogMessage("CryptoManager::HashKey_char: '%s' -> 0x%08X", input, hash);
    return hash;
}

/**
 * @brief Hash function for wchar_t* strings (exact replica from address 0x0041fe37)
 *
 * This function implements the wide character version of the hash algorithm.
 * Similar to char version but processes 16-bit wide characters.
 */
uint32_t CryptoManager::HashKey_wchar(const wchar_t* input) {
    if(! input) {
        return 0;
    }

    uint32_t hash = HASH_INIT_VALUE;

    // Exact replication of the algorithm from 0x0041fe37
    for(; *input != L'\0'; input++) {
        hash = hash * HASH_MULTIPLIER + (uint32_t) (uint16_t) *input;
    }

    LogMessage("CryptoManager::HashKey_wchar: hash -> 0x%08X", hash);
    return hash;
}

/**
 * @brief Hash function for BSTR strings (from address 0x00420043)
 *
 * BSTR is a COM string type with length prefix. This function needs to handle
 * the BSTR format correctly and apply the same hash algorithm.
 */
uint32_t CryptoManager::HashKey_BSTR(BSTR input) {
    if(! input) {
        return 0;
    }

    // BSTR has length information, but we hash character by character like wchar_t*
    uint32_t hash = HASH_INIT_VALUE;

    // Get BSTR length (safer than null-termination check)
    UINT length = SysStringLen(input);

    for(UINT i = 0; i < length; i++) {
        hash = hash * HASH_MULTIPLIER + (uint32_t) (uint16_t) input[i];
    }

    LogMessage("CryptoManager::HashKey_BSTR: length=%u hash=0x%08X", length, hash);
    return hash;
}

/**
 * @brief Hash function for VARIANT values (from address 0x0043a5aa)
 *
 * This function handles VARIANT structures by converting them to string
 * representation and then applying the char* hash algorithm.
 */
uint32_t CryptoManager::HashKey_VARIANT(VARIANT* input) {
    if(! input) {
        return 0;
    }

    char buffer[1024] = { 0 };

    // Convert VARIANT to string representation
    if(! VariantToString(input, buffer, sizeof(buffer))) {
        LogError("CryptoManager::HashKey_VARIANT: Failed to convert variant to string");
        return 0;
    }

    // Apply char* hash algorithm to string representation
    uint32_t hash = HashKey_char(buffer);

    LogMessage("CryptoManager::HashKey_VARIANT: type=%d hash=0x%08X", input->vt, hash);
    return hash;
}

/**
 * @brief Wrapper for SystemFunction036 (RtlGenRandom)
 *
 * This provides access to Windows cryptographically secure random number generation.
 * Used in the original binary for various security operations.
 */
BOOL CryptoManager::SystemFunction036(void* buffer, ULONG length) {
    if(! buffer || length == 0) {
        return FALSE;
    }

    // Load advapi32.dll and get SystemFunction036 (RtlGenRandom)
    static HMODULE hAdvapi32 = nullptr;
    static BOOLEAN(WINAPI * pSystemFunction036)(void*, ULONG) = nullptr;

    if(! hAdvapi32) {
        hAdvapi32 = LoadLibraryA("advapi32.dll");
        if(! hAdvapi32) {
            LogError("CryptoManager::SystemFunction036: Failed to load advapi32.dll");
            return FALSE;
        }

        pSystemFunction036 =
            (BOOLEAN(WINAPI*)(void*, ULONG)) GetProcAddress(hAdvapi32, "SystemFunction036");

        if(! pSystemFunction036) {
            LogError("CryptoManager::SystemFunction036: Failed to get SystemFunction036 address");
            return FALSE;
        }
    }

    BOOLEAN result = pSystemFunction036(buffer, length);

    if(result) {
        LogMessage("CryptoManager::SystemFunction036: Generated %lu random bytes", length);
    } else {
        LogError("CryptoManager::SystemFunction036: Failed to generate random data");
    }

    return (BOOL) result;
}

/**
 * @brief Secure memory allocation with guard bytes
 */
void* CryptoManager::SecureAlloc(SIZE_T size, const char* purpose) {
    if(size == 0) {
        return nullptr;
    }

    // Allocate extra space for guard bytes
    SIZE_T totalSize = size + (GUARD_SIZE * 2);

    void* ptr = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, totalSize);
    if(! ptr) {
        LogError(
            "CryptoManager::SecureAlloc: Failed to allocate %zu bytes for %s",
            size,
            purpose ? purpose : "unknown");
        return nullptr;
    }

    // Set guard bytes
    memset(ptr, GUARD_BYTE, GUARD_SIZE);
    memset((char*) ptr + GUARD_SIZE + size, GUARD_BYTE, GUARD_SIZE);

    void* userPtr = (char*) ptr + GUARD_SIZE;

    LogMessage(
        "CryptoManager::SecureAlloc: Allocated %zu bytes for %s at %p",
        size,
        purpose ? purpose : "unknown",
        userPtr);

    return userPtr;
}

/**
 * @brief Secure memory deallocation with overwrite
 */
void CryptoManager::SecureFree(void* ptr, SIZE_T size) {
    if(! ptr) {
        return;
    }

    // Get actual allocation pointer
    void* actualPtr = (char*) ptr - GUARD_SIZE;

    // Verify guard bytes
    BOOL guardOk = TRUE;
    for(SIZE_T i = 0; i < GUARD_SIZE; i++) {
        if(((char*) actualPtr)[i] != GUARD_BYTE) {
            guardOk = FALSE;
            break;
        }
        if(((char*) ptr + size)[i] != GUARD_BYTE) {
            guardOk = FALSE;
            break;
        }
    }

    if(! guardOk) {
        LogError("CryptoManager::SecureFree: Guard byte corruption detected at %p", ptr);
    }

    // Secure overwrite of user data
    SecureZeroMemory(ptr, size);

    // Free actual allocation
    HeapFree(GetProcessHeap(), 0, actualPtr);

    LogMessage("CryptoManager::SecureFree: Freed %zu bytes at %p", size, ptr);
}

/**
 * @brief Validate hash algorithm consistency
 */
BOOL CryptoManager::ValidateHashAlgorithms() {
    LogMessage("CryptoManager::ValidateHashAlgorithms: Starting validation");

    BOOL allTestsPassed = TRUE;

    // Test char* hash function
    for(size_t i = 0; i < g_hashTestVectorCount; i++) {
        uint32_t calculated = HashKey_char(g_hashTestVectors[i].input);
        uint32_t expected = g_hashTestVectors[i].expected_hash;

        if(calculated != expected) {
            LogError(
                "CryptoManager::ValidateHashAlgorithms: Hash test failed for '%s' - "
                "calculated=0x%08X, expected=0x%08X",
                g_hashTestVectors[i].input,
                calculated,
                expected);
            allTestsPassed = FALSE;
        }
    }

    // Test wchar_t* hash function
    wchar_t testWide[] = L"ITE";
    uint32_t wideHash = HashKey_wchar(testWide);
    if(wideHash == 0) {
        LogError("CryptoManager::ValidateHashAlgorithms: Wide char hash failed");
        allTestsPassed = FALSE;
    }

    // Test BSTR hash function
    BSTR testBstr = SysAllocString(L"1181");
    if(testBstr) {
        uint32_t bstrHash = HashKey_BSTR(testBstr);
        SysFreeString(testBstr);

        if(bstrHash == 0) {
            LogError("CryptoManager::ValidateHashAlgorithms: BSTR hash failed");
            allTestsPassed = FALSE;
        }
    }

    // Test SystemFunction036
    char randomBuffer[16];
    if(! SystemFunction036(randomBuffer, sizeof(randomBuffer))) {
        LogError("CryptoManager::ValidateHashAlgorithms: SystemFunction036 failed");
        allTestsPassed = FALSE;
    }

    if(allTestsPassed) {
        LogMessage("CryptoManager::ValidateHashAlgorithms: All tests passed");
    } else {
        LogError("CryptoManager::ValidateHashAlgorithms: Some tests failed");
    }

    return allTestsPassed;
}

/**
 * @brief Internal hash calculation for different data types
 */
uint32_t CryptoManager::InternalHashCalculation(const void* data, SIZE_T size) {
    if(! data || size == 0) {
        return 0;
    }

    uint32_t hash = HASH_INIT_VALUE;
    const char* bytes = (const char*) data;

    for(SIZE_T i = 0; i < size; i++) {
        hash = hash * HASH_MULTIPLIER + (unsigned char) bytes[i];
    }

    return hash;
}

/**
 * @brief Convert VARIANT to string representation for hashing
 */
BOOL CryptoManager::VariantToString(VARIANT* variant, char* buffer, SIZE_T bufferSize) {
    if(! variant || ! buffer || bufferSize == 0) {
        return FALSE;
    }

    // Initialize buffer
    buffer[0] = '\0';

    switch(variant->vt) {
    case VT_EMPTY:
    case VT_NULL: strcpy_s(buffer, bufferSize, ""); break;

    case VT_I1: _snprintf_s(buffer, bufferSize, _TRUNCATE, "%d", variant->cVal); break;

    case VT_UI1: _snprintf_s(buffer, bufferSize, _TRUNCATE, "%u", variant->bVal); break;

    case VT_I2: _snprintf_s(buffer, bufferSize, _TRUNCATE, "%d", variant->iVal); break;

    case VT_UI2: _snprintf_s(buffer, bufferSize, _TRUNCATE, "%u", variant->uiVal); break;

    case VT_I4: _snprintf_s(buffer, bufferSize, _TRUNCATE, "%ld", variant->lVal); break;

    case VT_UI4: _snprintf_s(buffer, bufferSize, _TRUNCATE, "%lu", variant->ulVal); break;

    case VT_BSTR:
        if(variant->bstrVal) {
            // Convert BSTR to char string
            int len = WideCharToMultiByte(
                CP_ACP, 0, variant->bstrVal, -1, buffer, (int) bufferSize, NULL, NULL);
            if(len == 0) {
                LogError("CryptoManager::VariantToString: Failed to convert BSTR");
                return FALSE;
            }
        } else {
            strcpy_s(buffer, bufferSize, "");
        }
        break;

    case VT_BOOL: strcpy_s(buffer, bufferSize, variant->boolVal ? "TRUE" : "FALSE"); break;

    default:
        // For unsupported types, convert the raw data
        _snprintf_s(buffer, bufferSize, _TRUNCATE, "VT_%d_DATA", variant->vt);
        LogWarning("CryptoManager::VariantToString: Unsupported variant type %d", variant->vt);
        break;
    }

    return TRUE;
}
