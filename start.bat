@echo off
REM URescue Launcher

if not exist "build\bin\Debug\URescue_v81D.2.24.2.exe" (
    echo ERROR: URescue not found!
    echo Please run build.bat first.
    pause
    exit /b 1
)

cd build\bin\Debug
URescue_v81D.2.24.2.exe %*
cd ..\..\..
