@echo off
setlocal enabledelayedexpansion

:: Set path
cd /d "%~dp0" || (
    echo Error: Cannot cd to script directory. 1>&2
    exit /b 1
)
set "SCRIPT_DIR=%cd%"

set "TEST_DIR=%~dp0"

:: Compile unit test
call "%TEST_DIR%/../acyclic/make.bat"
call "%TEST_DIR%/../io/make.bat"
call "%TEST_DIR%/../meta/make.bat"
call "%TEST_DIR%/../print/make.bat"
call "%TEST_DIR%/../tuple/make.bat"
call "%TEST_DIR%/../view/make.bat"

:: Run unit test
call :invoke "%TEST_DIR%/../acyclic/build/demo.exe"
call :invoke "%TEST_DIR%/../io/build/demo.exe"
call :invoke "%TEST_DIR%/../meta/build/demo.exe"
call :invoke "%TEST_DIR%/../print/build/demo.exe"
call :invoke "%TEST_DIR%/../tuple/build/demo.exe"
call :invoke "%TEST_DIR%/../view/build/demo.exe"

echo "All tests passed."
exit /b 0
goto :eof

:invoke
"%~1"
if errorlevel 1 (
	echo Error: Failed to run "%~1" 1>&2
    exit /b 1
)
exit /b 0
goto :eof
