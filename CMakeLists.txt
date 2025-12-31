cmake_minimum_required(VERSION 3.16)
project(URescue VERSION 81.2.24.2 LANGUAGES CXX)

# ============================================================================
# PROJECT CONFIGURATION
# ============================================================================

# Target architecture: x86 (32-bit)
if(CMAKE_GENERATOR_PLATFORM)
    # Platform set via -A flag
elseif(CMAKE_GENERATOR MATCHES "Visual Studio")
    set(CMAKE_GENERATOR_PLATFORM Win32)
endif()

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)

# ============================================================================
# CORE LIBRARY
# ============================================================================

# Precompiled header
set(PCH_HEADER include/stdafx.h)
set(PCH_SOURCE src/stdafx.cpp)

set(CORE_LIB_SOURCES
    # Core functionality
    src/URescueCore.cpp
    src/ITEController.cpp
    src/FirmwareManager.cpp
    src/USBDevice.cpp

    # SDK integration
    src/SDKLoader.cpp
    src/SDKAPIs.cpp
    src/FlashSDK.cpp

    # System operations
    src/MemoryManager.cpp
    src/FileOperations.cpp
    src/CryptoManager.cpp
    src/SystemManager.cpp

    # Utilities
    src/Utilities.cpp
    src/ErrorHandler.cpp
    src/Globals.cpp
    src/iTEUFDrs.cpp
)

# Core library target
add_library(URescueCore STATIC ${CORE_LIB_SOURCES} ${PCH_SOURCE})

# Enable precompiled headers
target_precompile_headers(URescueCore PUBLIC ${PCH_HEADER})

target_compile_options(URescueCore PRIVATE
    /W4
    /WX-
    $<$<CONFIG:Debug>:/MDd /Zi /Ob0 /Od /RTC1>
    $<$<CONFIG:Release>:/MD /O2 /Ob2 /DNDEBUG>
)

target_compile_definitions(URescueCore PRIVATE
    WIN32
    _WINDOWS
    UNICODE
    _UNICODE
    _CRT_SECURE_NO_WARNINGS
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

target_include_directories(URescueCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ============================================================================
# EXECUTABLE
# ============================================================================

set(EXECUTABLE_SOURCES
    src/main.cpp
)

add_executable(URescue ${EXECUTABLE_SOURCES})

# Use precompiled headers from core library
target_precompile_headers(URescue REUSE_FROM URescueCore)

target_compile_options(URescue PRIVATE
    /W4
    /WX-
    $<$<CONFIG:Debug>:/MDd /Zi /Ob0 /Od /RTC1>
    $<$<CONFIG:Release>:/MD /O2 /Ob2 /DNDEBUG>
)

target_compile_definitions(URescue PRIVATE
    WIN32
    _WINDOWS
    _CONSOLE
    UNICODE
    _UNICODE
    _CRT_SECURE_NO_WARNINGS
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

target_include_directories(URescue PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(URescue PRIVATE
    URescueCore
    kernel32
    advapi32
    version
    setupapi
    winusb
)

set_target_properties(URescue PROPERTIES
    WIN32_EXECUTABLE FALSE
    OUTPUT_NAME "URescue_v81D.2.24.2"
)

# Set console subsystem for linker
target_link_options(URescue PRIVATE
    $<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>
    $<$<CONFIG:Release>:/SUBSYSTEM:CONSOLE>
)

# ============================================================================
# INSTALLATION
# ============================================================================

install(TARGETS URescue URescueCore
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# ============================================================================
# BUILD SUMMARY
# ============================================================================

message(STATUS "")
message(STATUS "========================================")
message(STATUS "URescue Build Configuration")
message(STATUS "========================================")
message(STATUS "Version:    ${PROJECT_VERSION}")
message(STATUS "Platform:   ${CMAKE_GENERATOR_PLATFORM}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Interface:  CLI (Console)")
message(STATUS "========================================")
message(STATUS "")
