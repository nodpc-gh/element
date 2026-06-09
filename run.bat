@echo off
echo Starting Element IDE Server on port 8080...
echo.
start /B output\server.exe
echo Server started. Open http://localhost:8080
echo.
echo To stop: taskkill /f /im server.exe
