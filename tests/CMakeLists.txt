# Test project for URescue Reverse Engineering

# Including directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Adding test files
set(TEST_SOURCES
    TestMain.cpp
)

# Creating the test executable
add_executable(URescueTests ${TEST_SOURCES})

# Linking libraries
target_link_libraries(URescueTests kernel32 user32)

# Windows compilation settings
if(WIN32)
    target_compile_definitions(URescueTests PRIVATE
        WIN32
        _CONSOLE
        _UNICODE
        UNICODE
    )

    # Visual Studio settings
    if(MSVC)
        set_target_properties(URescueTests PROPERTIES
            COMPILE_FLAGS "/MT"
            LINK_FLAGS "/SUBSYSTEM:CONSOLE"
        )
    endif()
endif()

# Setting output path
set_target_properties(URescueTests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
