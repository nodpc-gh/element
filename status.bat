@echo off
echo ==================================
echo   Element IDE Server Status
echo ==================================
echo.

tasklist /FI "IMAGENAME eq server.exe" 2>nul | findstr server.exe >nul
if not errorlevel 1 (
    echo Status: RUNNING (server.exe)
    echo.
    netstat -ano | findstr :8080
    goto :eof
)
tasklist /FI "IMAGENAME eq server_public.exe" 2>nul | findstr server_public.exe >nul
if not errorlevel 1 (
    echo Status: RUNNING (server_public.exe)
    echo.
    netstat -ano | findstr :8080
    goto :eof
)
echo Status: STOPPED
netstat -ano | findstr :8080 >nul
if not errorlevel 1 (
    echo [WARNING] Port 8080 is busy by another process!
    netstat -ano | findstr :8080
)
echo.
echo ==================================
