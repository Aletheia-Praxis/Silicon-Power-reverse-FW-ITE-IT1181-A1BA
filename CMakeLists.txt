cmake_minimum_required(VERSION 3.16)
project(URescue_Reverse)

# Setting C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Searching for MFC
find_package(MFC REQUIRED)

# Setting paths
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Including directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Adding source files
set(SOURCES
    src/main.cpp
    src/URescueMain.cpp
    src/USBDevice.cpp
    src/FirmwareManager.cpp
    src/ITEController.cpp
    src/URescueGUI.cpp
    src/Dialogs.cpp
    src/Globals.cpp
    src/Utilities.cpp
    src/FileOperations.cpp
    src/ResourceManager.cpp
    src/MemoryManager.cpp
    src/SystemManager.cpp
    src/ErrorHandler.cpp
    src/URescueCore.cpp
)

# Creating executable file
add_executable(URescue ${SOURCES})

# Linking MFC
target_link_libraries(URescue mfc140u mfcs140u)

# Windows compilation settings
if(WIN32)
    target_compile_definitions(URescue PRIVATE
        WIN32
        _WINDOWS
        _AFXDLL
        _UNICODE
        UNICODE
    )
    
    # Visual Studio settings
    if(MSVC)
        set_target_properties(URescue PROPERTIES
            COMPILE_FLAGS "/MT"
            LINK_FLAGS "/SUBSYSTEM:WINDOWS"
        )
    endif()
endif()

# Copying resources
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR}/bin)

# Adding tests
add_subdirectory(tests)
