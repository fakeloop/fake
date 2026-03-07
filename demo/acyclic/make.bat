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
set "list=aspect;awaited;awaiter;branch;coroutine;demo;exception;nested;sequence;simple"

for %%i in (%list:;= %) do (
    call :compile %%i
)
call :link
exit /b 0
goto :eof

:compile
set "module=%~1"
call :c++ "%CPP_DIR%\%module%.cpp" -c -o "%BUILD_DIR%\%module%.o" -iquote "%INCLUDE_DIR%" -std=c++23
if errorlevel 1 (
    echo Error: Failed to compile %module% 1>&2
    exit /b 1
)
exit /b 0
goto :eof

:link
set "obj_list="
for %%i in (%list:;= %) do (
    set "obj_list=!obj_list! %BUILD_DIR%\%%i.o"
)
call :c++ -static-libstdc++ -static-libgcc -static !obj_list! -o "%TARGET_DIR%" -pthread
if errorlevel 1 (
    echo Error: Failed to link acyclic demo 1>&2
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
