@echo off
chcp 65001 >nul
echo ==================================
echo   Element IDE - GitHub Publish
echo ==================================
echo.

cd /d "%~dp0"

REM Проверка git
where git >nul 2>&1
if errorlevel 1 (
    echo ERROR: Git not found!
    echo Install from: https://git-scm.com/download/win
    pause
    exit /b 1
)

echo [1/6] Инициализация репозитория...
git init

echo [2/6] Добавление файлов...
git add .

echo [3/6] Первый коммит...
git commit -m "Release v1.0.0 - Visual IDE with ports and JSON export

Features:
- Visual editor with grid canvas
- UI elements with 4-side ports (in, out, id, data)
- Color-coded connections (data/event/id)
- JSON compiler/export
- Context menus for elements and connections
- Port management (add/remove)
- Element copy function

Built with FASM x86 Assembly
Inspired by HiAsm"

echo.
echo [4/6] Введите имя репозитория на GitHub (например: Element):
set /p REPO_NAME=
if "%REPO_NAME%"=="" set REPO_NAME=Element

echo.
echo [5/6] Введите ваш GitHub username:
set /p GITHUB_USER=

if "%GITHUB_USER%"=="" (
    echo ERROR: Username required!
    pause
    exit /b 1
)

echo.
echo [6/6] Push на GitHub...
echo.
echo Remote URL: https://github.com/%GITHUB_USER%/%REPO_NAME%.git
echo.
git remote add origin https://github.com/%GITHUB_USER%/%REPO_NAME%.git
git branch -M main
git push -u origin main

echo.
echo ==================================
echo   SUCCESS!
echo ==================================
echo.
echo Project published to:
echo https://github.com/%GITHUB_USER%/%REPO_NAME%
echo.
echo Next steps:
echo 1. Add screenshots to screenshots/ folder
echo 2. Update README with your screenshots
echo 3. Create release v1.0.0 on GitHub
echo.
pause

cd /d "%~dp0"
