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

# Set output directories for all targets
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)

# Source files for the library
set(LIB_SOURCES
    src/URescueCore.cpp
    src/SystemManager.cpp
    # src/ITEController.cpp    # Temporarily excluded - USB dependency issues
    # src/FirmwareManager.cpp  # Temporarily excluded due to compilation issues
    # src/USBDevice.cpp        # Temporarily excluded - working on USB header conflicts
    src/Utilities.cpp
    src/ErrorHandler.cpp
    src/iTEUFDrs.cpp
    # src/SDKLoader.cpp        # Temporarily excluded due to SDK function dependencies
    src/SDKAPIs.cpp
    # src/FlashSDK.cpp         # Re-disabled - still has winsock conflicts via includes
    src/MemoryManager.cpp
    src/ResourceManager.cpp
    src/FileOperations.cpp
    src/CryptoManager.cpp
    src/Globals.cpp
    src/CRTFunctions.cpp
    src/TempStubsAlt.cpp
)

# Create static library
add_library(URescueLib STATIC ${LIB_SOURCES})

# Set compiler options for the library
target_compile_options(URescueLib PRIVATE
    $<$<CONFIG:Debug>:/MDd /Zi /Ob0 /Od /RTC1>
    $<$<CONFIG:Release>:/MD /O2 /Ob2 /DNDEBUG>
)

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

# Set compiler options for the executable
target_compile_options(URescue PRIVATE
    $<$<CONFIG:Debug>:/MDd /Zi /Ob0 /Od /RTC1>
    $<$<CONFIG:Release>:/MD /O2 /Ob2 /DNDEBUG>
)

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

# Add tests subdirectory - temporarily disabled due to MFC/winsock conflicts
# add_subdirectory(tests)

# Copy resources after build
add_custom_command(TARGET URescue POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:URescue>/resources"
    COMMENT "Copying resources to build directory"
)

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
