@echo off
setlocal enabledelayedexpansion

:: Set path
cd /d "%~dp0" || (
    echo Error: Cannot cd to script directory. 1>&2
    exit /b 1
)
set "SCRIPT_DIR=%cd%"

set "CPP_DIR=%~dp0"
set "INCLUDE_DIR=%~dp0\..\..\include"
set "BUILD_DIR=%~dp0\build"
set "SOURCE_DIR=%CPP_DIR%\io.cpp"
set "TARGET_DIR=%BUILD_DIR%\demo.exe"

if not exist "%CPP_DIR%\" (
    echo Error: Source directory not found: %CPP_DIR% 1>&2
    exit /b 1
)
if not exist "%INCLUDE_DIR%\" (
    echo Error: Include directory not found: %INCLUDE_DIR% 1>&2
    exit /b 1
)
if not exist "%BUILD_DIR%\" (
    echo Error: Build directory not found: %BUILD_DIR% 1>&2
    exit /b 1
)

:: Build
call :compile
call :link
exit /b 0
goto :eof

:compile
call :c++ "%SOURCE_DIR%" -c -o "%BUILD_DIR%\demo.o" -iquote "%INCLUDE_DIR%" -std=c++23
if errorlevel 1 (
    echo Error: Failed to compile %SOURCE_DIR% 1>&2
    exit /b 1
)
exit /b 0
goto :eof

:link
call :c++ -static-libstdc++ -static-libgcc -static "%BUILD_DIR%\demo.o" -o "%TARGET_DIR%" -pthread
if errorlevel 1 (
    echo Error: Failed to link %TARGET_DIR% 1>&2
    exit /b 1
)
exit /b 0
goto :eof

:c++
::"<MinGW64_Dir>" %*
c++ %*
if errorlevel 1 (
    exit /b 1
)
exit /b 0
goto :eof
