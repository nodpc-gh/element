@echo off
chcp 65001 >nul
echo ==================================
echo   Element IDE - Load Project
echo ==================================
echo.

if "%~1"=="" (
    echo Usage: load_project.bat ^<project_file.json^>
    echo Example: load_project.bat example_project.json
    goto :eof
)

if not exist "%~1" (
    echo Error: File "%~1" not found!
    goto :eof
)

echo Copying project to www folder...
copy "%~1" www\project_to_load.json /Y

echo.
echo Project file copied to www\project_to_load.json
echo.
echo Open http://localhost:8080 in browser
echo Then open Developer Console (F12) and run:
echo   fetch('/project_to_load.json').then(r=>r.json()).then(p=>Compiler.apply(p))
echo.
