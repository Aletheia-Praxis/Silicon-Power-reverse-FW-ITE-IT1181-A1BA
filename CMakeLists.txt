cmake_minimum_required(VERSION 3.16)
project(URescue VERSION 81.2.24.2)

# Set architecture to x86 for compatibility with original binary
if(CMAKE_GENERATOR_PLATFORM)
    # This is set automatically when using -A flag
elseif(CMAKE_GENERATOR MATCHES "Visual Studio")
    set(CMAKE_GENERATOR_PLATFORM Win32)
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable MFC support with shared DLL
set(CMAKE_MFC_FLAG 2)  # Use MFC in a Shared DLL

# Set runtime library to MD for MFC
set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Zi /Ob0 /Od /RTC1")
set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Ob2 /DNDEBUG")

# Set output directories for all targets
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)

# Source files for the library
set(LIB_SOURCES
    src/URescueCore.cpp
    src/SystemManager.cpp
    src/ITEController.cpp
    src/FirmwareManager.cpp
    src/USBDevice.cpp
    src/Utilities.cpp
    src/ErrorHandler.cpp
    src/iTEUFDrs.cpp
    src/SDKLoader.cpp
    src/SDKAPIs.cpp
    src/FlashSDK.cpp
    src/MemoryManager.cpp
    src/ResourceManager.cpp
    src/FileOperations.cpp
    src/CryptoManager.cpp
    src/Globals.cpp
    src/CRTFunctions.cpp
)

# Create static library
add_library(URescueLib STATIC ${LIB_SOURCES})

# Include directories for the library
target_include_directories(URescueLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
)

# Source files for the executable
set(APP_SOURCES
    src/main.cpp
    src/URescueMain.cpp
    src/URescueGUI.cpp
)

# Create executable
add_executable(URescue ${APP_SOURCES})

# Link library to executable
target_link_libraries(URescue PRIVATE URescueLib)

# Include directories for the executable
target_include_directories(URescue PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
)

# Link libraries (with MFC support)
target_link_libraries(URescue PRIVATE
    kernel32
    user32
    gdi32
    winspool
    comdlg32
    advapi32
    shell32
    ole32
    oleaut32
    uuid
    odbc32
    odbccp32
    setupapi
    winmm
)

# Set properties
set_target_properties(URescue PROPERTIES
    WIN32_EXECUTABLE TRUE
    OUTPUT_NAME "URescue_v81D.2.24.2"
)

# Add tests subdirectory
add_subdirectory(tests)

# Copy resources
file(COPY resources DESTINATION ${CMAKE_BINARY_DIR}/bin)

# Install rules
install(TARGETS URescue
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY resources/
    DESTINATION share/urescue
)
