#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "../include/ITEController.h"
#include "../include/Utilities.h"

// Test logging functionality
void TestLogging() {
    printf("Testing logging functionality...\n");

    LOG_INFO("Test info message");
    LOG_WARNING("Test warning message");
    LOG_ERROR("Test error message");
    LOG_DEBUG("Test debug message");

    printf("Logging test completed.\n");
}

// Test byte formatting function
void TestFormatBytes() {
    printf("Testing byte formatting...\n");

    DWORD testSizes[] = { 1024, 1024 * 1024, 1024 * 1024 * 1024, 1536 };

    for(int i = 0; i < 4; i++) {
        CString formatted = FormatBytes(testSizes[i]);
        printf("Size %d: %s\n", testSizes[i], formatted);
    }

    printf("Byte formatting test completed.\n");
}

// Test CRC32 function
void TestCRC32() {
    printf("Testing CRC32 calculation...\n");

    BYTE testData[] = "Hello, World!";
    DWORD crc = CalculateCRC32(testData, sizeof(testData) - 1);

    printf("CRC32 of 'Hello, World!': 0x%08X\n", crc);

    // Verification with a known value
    if(crc == 0x4A17B156) {
        printf("CRC32 test PASSED\n");
    } else {
        printf("CRC32 test FAILED\n");
    }
}

// Test hex conversion function
void TestHexConversion() {
    printf("Testing hex conversion...\n");

    BYTE testBytes[] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0 };
    CString hexString = BytesToHexString(testBytes, sizeof(testBytes));

    printf("Hex string: %s\n", hexString);

    // Test reverse conversion
    BYTE convertedBytes[8];
    DWORD byteCount;

    if(HexStringToBytes("123456789ABCDEF0", convertedBytes, &byteCount)) {
        if(memcmp(testBytes, convertedBytes, sizeof(testBytes)) == 0) {
            printf("Hex conversion test PASSED\n");
        } else {
            printf("Hex conversion test FAILED\n");
        }
    } else {
        printf("Hex conversion test FAILED\n");
    }
}

// Test file validation function
void TestFileValidation() {
    printf("Testing file validation...\n");

    // Test with a non-existent file
    if(! ValidateFirmwareFile("nonexistent.bin")) {
        printf("File validation test PASSED (non-existent file)\n");
    } else {
        printf("File validation test FAILED (non-existent file)\n");
    }

    // Test with the current executable file
    if(ValidateFirmwareFile("URescue.exe")) {
        printf("File validation test PASSED (existing file)\n");
    } else {
        printf("File validation test FAILED (existing file)\n");
    }
}

// Main testing function
int main() {
    printf("=== URescue Reverse Engineering - Test Suite ===\n\n");

    TestLogging();
    printf("\n");

    TestFormatBytes();
    printf("\n");

    TestCRC32();
    printf("\n");

    TestHexConversion();
    printf("\n");

    TestFileValidation();
    printf("\n");

    printf("=== All tests completed ===\n");

    system("pause");
    return 0;
}
