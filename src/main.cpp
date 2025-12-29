// CLI entry point (no GUI dependencies)

#include "../include/stdafx.h"

#include <cstring>

#include "../include/SystemManager.h"
#include "../include/URescueCore.h"
#include "../include/Utilities.h"

namespace {
constexpr int kExitOk = 0;
constexpr int kExitUsage = 2;
constexpr int kExitFailure = 1;

void PrintUsage(const char* exeName) {
    const char* safeExeName = (exeName && exeName[0] != '\0') ? exeName : "URescue";
    LogMessage(
        "Usage:\n"
        "  %s [--verify] [device_path] [firmware_path]\n\n"
        "If device_path is not provided, reads Device.ini -> [Device] SelectDevice.\n"
        "If firmware_path is not provided, uses the default inferred .bin path.\n",
        safeExeName);
}

bool IsFlag(const char* arg, const char* flag) {
    return arg && flag && std::strcmp(arg, flag) == 0;
}
}  // namespace

int main(int argc, char** argv) {
    const char* devicePath = nullptr;
    const char* firmwarePath = nullptr;
    bool verifyFlag = false;

    for(int i = 1; i < argc; ++i) {
        const char* arg = argv[i];
        if(IsFlag(arg, "--help") || IsFlag(arg, "-h") || IsFlag(arg, "/?")) {
            PrintUsage((argc > 0) ? argv[0] : nullptr);
            return kExitUsage;
        }
        if(IsFlag(arg, "--verify")) {
            verifyFlag = true;
            continue;
        }
        if(! devicePath) {
            devicePath = arg;
            continue;
        }
        if(! firmwarePath) {
            firmwarePath = arg;
            continue;
        }

        LogError("Unexpected argument: %s", arg ? arg : "(null)");
        PrintUsage((argc > 0) ? argv[0] : nullptr);
        return kExitUsage;
    }

    if(! InitializeURescue()) {
        LogError("InitializeURescue failed");
        return kExitFailure;
    }

    PURESCUE_CONTEXT context = GetURescueContext();
    if(context) {
        if(verifyFlag) {
            context->settings.verifyAfterWrite = TRUE;
        }
    }

    char deviceFromIni[MAX_PATH] = { 0 };
    if(! devicePath) {
        if(ReadDeviceSelectionFromIni(deviceFromIni, sizeof(deviceFromIni))) {
            devicePath = deviceFromIni;
        }
    }

    if(! firmwarePath && context && context->binFilePath[0] != '\0') {
        firmwarePath = context->binFilePath;
    }

    if(! devicePath || ! firmwarePath) {
        LogError("device_path and firmware_path are required");
        PrintUsage((argc > 0) ? argv[0] : nullptr);
        DeinitializeURescue();
        return kExitUsage;
    }

    if(! LoadFirmware(firmwarePath)) {
        LogError("LoadFirmware failed");
        DeinitializeURescue();
        return kExitFailure;
    }

    if(! ConnectToDevice(devicePath)) {
        LogError("ConnectToDevice failed");
        DeinitializeURescue();
        return kExitFailure;
    }

    bool ok = true;
    if(! WriteFirmware()) {
        LogError("WriteFirmware failed");
        ok = false;
    } else if(context && context->settings.verifyAfterWrite) {
        if(! VerifyFirmware()) {
            LogError("VerifyFirmware failed");
            ok = false;
        }
    }

    DisconnectFromDevice();
    DeinitializeURescue();

    return ok ? kExitOk : kExitFailure;
}
