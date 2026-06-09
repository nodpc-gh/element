@echo off
echo Starting Element IDE Public Server on port 8080...
echo.
start /B output\server_public.exe
echo Server started. Open http://localhost:8080
echo.
echo To stop: taskkill /f /im server_public.exe
