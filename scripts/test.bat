@echo off
setlocal

rem Quick test runner for Windows: builds the engine and runs every fixture.

set "ROOT=%~dp0.."
cd /d "%ROOT%"

if not exist build\CMakeCache.txt (
    cmake -S . -B build -G Ninja || exit /b 1
) else (
    echo Reusing existing build configuration.
)
cmake --build build || exit /b 1

set FAILED=0
for %%F in (delimited_text_experiment multiple_extractors_experiment placeholder_text_experiment) do (
    echo.
    echo === %%F ===
    build\doe_engine.exe build\tests\fixtures\%%F\experiment.json
    if errorlevel 1 set FAILED=1
)

echo.
if %FAILED%==1 (
    echo Some fixtures failed.
    exit /b 1
)
echo All fixtures passed.
exit /b 0
