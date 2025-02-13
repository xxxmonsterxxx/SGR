@echo off
setlocal enabledelayedexpansion

set GLSLC_PATH=%VULKAN_SDK%\Bin\glslc.exe
set INPUT_DIR=ShaderExamples
set OUTPUT_DIR=..\CompiledShaders

if not exist "%GLSLC_PATH%" (
    echo GLSLC compiler not found at %GLSLC_PATH%
    pause
    exit /b 1
)

if exist "%OUTPUT_DIR%" (
    echo Cleaning output directory...
    del /q "%OUTPUT_DIR%\*.*"
) else (
    mkdir "%OUTPUT_DIR%"
)

for %%F in (%INPUT_DIR%\*.vert %INPUT_DIR%\*.frag) do (
    set "INPUT=%%F"
    set "OUTPUT=%OUTPUT_DIR%\%%~nxF.spv"
    echo Compiling !INPUT! to !OUTPUT! ...
    "%GLSLC_PATH%" !INPUT! -o !OUTPUT!
)

echo Compilation finished.
pause