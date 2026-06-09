@echo off
echo ==================================
echo   Element IDE - DEFAULT BUILD
echo ==================================
echo.
@"D:\Programms\IDE\FASM\FASM.EXE" src_v2\main_server.asm output\server.exe
if errorlevel 1 goto err
echo Build successful: output\server.exe
echo.
echo For dev build:  build_dev.bat
echo For public:     build_pub.bat
goto end
:err
echo Build failed!
:end
echo ==================================
