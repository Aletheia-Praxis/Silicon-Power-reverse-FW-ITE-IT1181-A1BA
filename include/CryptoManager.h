#pragma once
#include <cstdint>

#include "WindowsHeaders.h"

/**
 * @class CryptoManager
 * @brief Cryptographic functions extracted from URescue v81D.2.24.2.exe analysis
 *
 * This class implements exact replicas of hash functions and cryptographic utilities
 * found through Ghidra MCP analysis at specific addresses in the original binary.
 *
 * Function addresses from Ghidra decompilation:
 * - HashKey_char:    0x0041fe02
 * - HashKey_wchar:   0x0041fe37
 * - HashKey_BSTR:    0x00420043
 * - HashKey_VARIANT: 0x0043a5aa
 */
class CryptoManager {
public:
    /**
     * @brief Hash function for char* strings (from address 0x0041fe02)
     * @param input Null-terminated char string
     * @return 32-bit hash value using multiplicative hash with constant 0x21 (33)
     */
    static uint32_t HashKey_char(const char* input);

    /**
     * @brief Hash function for wchar_t* strings (from address 0x0041fe37)
     * @param input Null-terminated wide char string
     * @return 32-bit hash value using multiplicative hash with constant 0x21 (33)
     */
    static uint32_t HashKey_wchar(const wchar_t* input);

    /**
     * @brief Hash function for BSTR strings (from address 0x00420043)
     * @param input BSTR (COM string type)
     * @return 32-bit hash value
     */
    static uint32_t HashKey_BSTR(BSTR input);

    /**
     * @brief Hash function for VARIANT values (from address 0x0043a5aa)
     * @param input Pointer to VARIANT structure
     * @return 32-bit hash value based on variant type and data
     */
    static uint32_t HashKey_VARIANT(VARIANT* input);

    /**
     * @brief Wrapper for SystemFunction036 (RtlGenRandom)
     * @param buffer Buffer to fill with random data
     * @param length Number of bytes to generate
     * @return TRUE on success, FALSE on failure
     */
    static BOOL SystemFunction036(void* buffer, ULONG length);

    /**
     * @brief Secure memory allocation with guard bytes
     * @param size Number of bytes to allocate
     * @param purpose Description for logging/debugging
     * @return Pointer to allocated memory or nullptr on failure
     */
    static void* SecureAlloc(SIZE_T size, const char* purpose = nullptr);

    /**
     * @brief Secure memory deallocation with overwrite
     * @param ptr Pointer to memory to free
     * @param size Size of memory block (for secure overwrite)
     */
    static void SecureFree(void* ptr, SIZE_T size);

    /**
     * @brief Validate hash algorithm consistency
     * @return TRUE if hash algorithms work correctly
     */
    static BOOL ValidateHashAlgorithms();

private:
    // Hash algorithm constants from Ghidra analysis
    static constexpr uint32_t HASH_MULTIPLIER = 0x21;  // 33 decimal
    static constexpr uint32_t HASH_INIT_VALUE = 0;

    // Security constants
    static constexpr uint8_t GUARD_BYTE = 0xCC;
    static constexpr SIZE_T GUARD_SIZE = 16;

    /**
     * @brief Internal hash calculation for different data types
     * @param data Pointer to data
     * @param size Size of data in bytes
     * @return 32-bit hash value
     */
    static uint32_t InternalHashCalculation(const void* data, SIZE_T size);

    /**
     * @brief Convert VARIANT to string representation for hashing
     * @param variant Pointer to VARIANT
     * @param buffer Output buffer for string representation
     * @param bufferSize Size of output buffer
     * @return TRUE on success, FALSE on failure
     */
    static BOOL VariantToString(VARIANT* variant, char* buffer, SIZE_T bufferSize);
};

/**
 * @brief Utility macros for secure string hashing
 */
#define SECURE_HASH_STRING(str) ((str) ? CryptoManager::HashKey_char(str) : 0)

#define SECURE_HASH_WSTRING(wstr) ((wstr) ? CryptoManager::HashKey_wchar(wstr) : 0)

/**
 * @brief Hash algorithm verification structure
 */
struct HashTestVector {
    const char* input;
    uint32_t expected_hash;
};

// Test vectors for validation (based on Ghidra analysis)
extern const HashTestVector g_hashTestVectors[];
extern const size_t g_hashTestVectorCount;
