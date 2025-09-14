# Test project for URescue Reverse Engineering
project(URescueTests)

# Adding test files
set(TEST_SOURCES
    TestMain.cpp
)

# Creating the test executable
add_executable(URescueTests ${TEST_SOURCES})

# Include directories
target_include_directories(URescueTests PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Linking libraries
target_link_libraries(URescueTests PRIVATE URescueLib)

# Windows compilation settings
if(WIN32)
    target_compile_definitions(URescueTests PRIVATE
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
