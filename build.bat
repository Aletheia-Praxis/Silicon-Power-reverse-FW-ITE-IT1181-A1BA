@echo off
REM ============================================================================
REM URescue Build Script
REM ============================================================================

echo.
echo ========================================
echo URescue Build
echo ========================================
echo.

REM Check if CMakeLists.txt exists
if not exist "CMakeLists.txt" (
    echo ERROR: CMakeLists.txt not found!
    echo Please run this script from the project root directory.
    pause
    exit /b 1
)

echo [1/3] Creating build directory...
if not exist "build" mkdir build
echo       Build directory: build

echo.
echo [2/3] Configuring CMake...
cmake -S . -B build -G "Visual Studio 17 2022" -A Win32
if errorlevel 1 (
    echo.
    echo ERROR: CMake configuration failed!
    echo Please check your CMake installation and Visual Studio setup.
    pause
    exit /b 1
)

echo.
echo [3/3] Building project (Debug)...
cmake --build build --config Debug
if errorlevel 1 (
    echo.
    echo ERROR: Build failed!
    echo Please check the compiler errors above.
    pause
    exit /b 1
)

echo.
echo ========================================
echo SUCCESS! Build completed!
echo ========================================
echo.
echo Executable: build\bin\Debug\URescue_v81D.2.24.2.exe
echo.
echo Run with: start.bat [arguments]
echo.
pause
