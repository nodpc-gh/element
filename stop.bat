@echo off
echo ==================================
echo   Element IDE Server Stop
echo ==================================
echo.

taskkill /F /IM element.exe 2>nul
if errorlevel 1 (
    echo Server not running
) else (
    echo Server stopped
)
echo.
echo ==================================
