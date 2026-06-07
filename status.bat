@echo off
echo ==================================
echo   Element IDE Server Status
echo ==================================
echo.

tasklist /FI "IMAGENAME eq element.exe" 2>nul | findstr element.exe >nul
if not errorlevel 1 (
    echo Status: RUNNING
    echo.
    netstat -ano | findstr :8080
) else (
    echo Status: STOPPED
    netstat -ano | findstr :8080 >nul
    if not errorlevel 1 (
        echo [WARNING] Port 8080 is busy!
        netstat -ano | findstr :8080
    )
)
echo.
echo ==================================
