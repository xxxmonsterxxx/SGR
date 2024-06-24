# PowerShell script for requirement libraries installing

Write-Host ""
Write-Host "Welcome to Simple Graphics Renderer (SGR) environment install helper (Windows Version)."
Write-Host ""
Write-Host "Script will install (or skip if installed):"
Write-Host "1. Vulkan SDK"
Write-Host "   a) Vulkan SDK variables export"
Write-Host "   b) Vulkan SDK check"
Write-Host "2. GLFW"
Write-Host ""
$resp = Read-Host "Let's begin? (y/n)"
if ($resp -eq "n") {
    Write-Host "Exiting."
    pause
    exit
}

Write-Host ""

#check vulkan sdk
$vulkan_sdk_installed = $false
Write-Host "1. Checking Vulkan SDK..."

$result = Get-Command vkcube -ErrorAction SilentlyContinue
if ($result) {
    Write-Host "Vulkan SDK already installed."
    $vulkan_sdk_installed = $true
} else {
    $resp = Read-Host "Looks like Vulkan SDK not installed or we cannot find them, install now? (y/n)"
    if ($resp -eq "n") {
        Write-Host "Skipping."
    } else {
        Write-Host "Downloading Vulkan SDK..."
        Invoke-WebRequest -Uri https://sdk.lunarg.com/sdk/download/1.3.283.0/windows/VulkanSDK-1.3.283.0-Installer.exe -OutFile vulkan-sdk.exe
        
        Write-Host "Installing Vulkan SDK..."
        Start-Process vulkan-sdk.exe
        pause

        Write-Host "1a. Export Vulkan SDK variables"
        [System.Environment]::SetEnvironmentVariable("DYLD_LIBRARY_PATH", "${env:VULKAN_SDK}\lib;${env:DYLD_LIBRARY_PATH}", [System.EnvironmentVariableTarget]::User)

        Write-Host "1b. Checking Vulkan"
        $result = Get-Command ${env:VULKAN_SDK}\bin\vkcube -ErrorAction SilentlyContinue
        if ($result) {
            Write-Host "Vulkan SDK was installed correctly"
            $vulkan_sdk_installed = $true
            Remove-Item "vulkan-sdk.exe"
        }
    }
}
Write-Host ""

#check glfw installed

Write-Host "2. Checking GLFW..."
$glfw_installed = $false
$response = Read-Host "Is GLFW installed? (Type 'y' or 'n')"
if ($response.ToLower() -eq "y") {
    $glfw_installed = $true
} elseif ($response.ToLower() -eq "n") {
    Write-Host "Downloading GLFW..."
    $outputFile = "glfw.zip"
    Invoke-WebRequest -Uri https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN64.zip -OutFile $outputfile
    if (Test-Path $outputFile) {
        Expand-Archive -Path $outputFile -DestinationPath "C:\Libs\glfw" -Force
        Move-Item -Path "C:\Libs\glfw\glfw-3.4.bin.WIN64\lib-vc2022" -Destination "C:\Libs\glfw\lib" -Force
        Move-Item -Path "C:\Libs\glfw\glfw-3.4.bin.WIN64\include" -Destination "C:\Libs\glfw\include\GLFW" -Force
        Remove-Item -Path "C:\Libs\glfw\glfw-3.4.bin.WIN64" -Force -Recurse
        Remove-Item $outputFile

        Write-Host "2a. Export Vulkan SDK variables"
        [System.Environment]::SetEnvironmentVariable("GLFW3_LIB", "C:\Libs\glfw", [System.EnvironmentVariableTarget]::User)
        Write-Host "GLFW was installed."
        $glfw_installed = $true
    }
}

Write-Host ""
Write-Host "Environment install helper done:"
Write-Host "Vulkan SDK:   $vulkan_sdk_installed"
Write-Host "GLFW:         $glfw_installed"
pause
exit 1
