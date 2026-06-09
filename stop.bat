@echo off
echo ==================================
echo   Element IDE Server Stop
echo ==================================
echo.

taskkill /F /IM server.exe 2>nul
taskkill /F /IM server_public.exe 2>nul
echo Server stopped (if it was running)
echo.
echo ==================================
