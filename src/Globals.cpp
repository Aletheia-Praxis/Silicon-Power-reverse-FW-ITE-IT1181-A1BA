#include <windows.h>
#include <setupapi.h>
#include <winusb.h>

// Global GUIDs for USB devices
const GUID GUID_DEVCLASS_USB = { 0x36fc9e60, 0xc465, 0x11cf, { 0x80, 0x56, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 } };
const GUID GUID_DEVINTERFACE_USB_DEVICE = { 0xa5dcbfa1, 0x6530, 0x11d2, { 0x90, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x51, 0xed } };

// Constants for ITE controller
const BYTE ITE_COMMAND_PREFIX[] = { 0x55, 0xAA };
const BYTE ITE_RESPONSE_PREFIX[] = { 0xAA, 0x55 };

// ITE controller commands
const BYTE ITE_CMD_INIT = 0x01;
const BYTE ITE_CMD_GET_INFO = 0x02;
const BYTE ITE_CMD_SET_MODE = 0x03;
const BYTE ITE_CMD_GET_STATUS = 0x04;
const BYTE ITE_CMD_RESET = 0xFF;

// ITE controller operating modes
const BYTE ITE_MODE_NORMAL = 0x00;
const BYTE ITE_MODE_FLASH = 0x01;
const BYTE ITE_MODE_RECOVERY = 0x02;

// ITE controller states
const BYTE ITE_STATE_READY = 0x00;
const BYTE ITE_STATE_BUSY = 0x01;
const BYTE ITE_STATE_ERROR = 0xFF;

// Operation timeouts
const DWORD ITE_TIMEOUT_INIT = 100;      // ms
const DWORD ITE_TIMEOUT_COMMAND = 100;   // ms
const DWORD ITE_TIMEOUT_RESET = 1000;    // ms
const DWORD ITE_TIMEOUT_FLASH = 5000;    // ms

// Buffer sizes
const DWORD ITE_BUFFER_SIZE = 4096;
const DWORD ITE_MAX_FIRMWARE_SIZE = 16 * 1024 * 1024; // 16 MB
const DWORD ITE_HEADER_SIZE = 512;

// Signatures and identifiers
const char ITE_SIGNATURE[] = "ITE_FW";
const char ITE_MODEL[] = "IT1181";
const char ITE_VERSION_PREFIX[] = "v81D.2.24.2";

// Error codes
const DWORD ITE_ERROR_NONE = 0x00000000;
const DWORD ITE_ERROR_INIT_FAILED = 0x00000001;
const DWORD ITE_ERROR_INVALID_COMMAND = 0x00000002;
const DWORD ITE_ERROR_DEVICE_BUSY = 0x00000003;
const DWORD ITE_ERROR_FLASH_FAILED = 0x00000004;
const DWORD ITE_ERROR_VERIFY_FAILED = 0x00000005;
const DWORD ITE_ERROR_TIMEOUT = 0x00000006;
const DWORD ITE_ERROR_UNKNOWN = 0xFFFFFFFF;
