@echo off
echo ================================================
echo   Element IDE - PUBLIC BUILD (gitverse/src)
echo ================================================
echo.
@"D:\Programms\IDE\FASM\FASM.EXE" gitverse\src\main_server.asm output\server_public.exe
if errorlevel 1 goto err
echo Build successful: output\server_public.exe
echo.
echo To run: cd output ^&^& server_public.exe
goto end
:err
echo Build failed!
:end
echo ================================================
