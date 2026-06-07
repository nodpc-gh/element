@echo off
chcp 65001 >nul
echo ==================================
echo   Element IDE Build Script
echo ==================================
echo.

cd /d "%~dp0src_v2"

if not exist "fasm.exe" (
    echo FASM not found in current folder
    echo Searching for fasm.exe...
    where fasm >nul 2>&1
    if errorlevel 1 (
        echo FASM not found in PATH
        echo Please download from https://flatassembler.net/download.php
        echo or install to D:\Programms\IDE\FASM\
        exit /b 1
    )
)

echo Building main_server.asm (v2 with DB support)...
fasm main_server.asm ..\output\element.exe

if errorlevel 1 (
    echo Build failed!
    exit /b 1
)

echo.
echo ==================================
echo   Build completed successfully!
echo   Output: output\element.exe
echo ==================================
echo.
echo Run: start ..\output\element.exe
echo Open: http://localhost:8080

cd /d "%~dp0"
