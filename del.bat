@echo off
for /r %%i in (debug,relese,ipch) do rd /s /q "%%i"
for /r %%i in (*.sdf *.vc.db) do del /s /f /q "%%i"
pause