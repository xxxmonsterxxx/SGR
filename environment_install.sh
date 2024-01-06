#!/bin/bash

# Ubuntu or MacOS (Warning!!! For Windows use environment_install.ps (powershell script))
# shell script for requirement librariess installing

echo
echo "Welcome to Simple Graphics Renderer (SGR) environement install helper."
echo
echo "Script will install (or skip if installed):"
echo "0. Check OS (different OS requires different libs and tools)"
echo "   a) If You're using MacOS (Darwin system) - Command Line Tools (CLI)"
echo "1. Homebrew (tools for easy to install different tools and library)"
echo "2. CMake"
echo "3. Vulkan SDK"
echo "   a) Vulkan SDK variables export"
echo "   b) Vulkan SDK check"
echo "4. glfw"
echo
read -p "Let's begin? (y/n) " RESP
if [ "$RESP" = "n" ]
then
    echo "Exiting."
    exit 0
fi

# Check OS
SYSTEM_TYPE="$(uname -s)"

echo
echo "0. Your system is $SYSTEM_TYPE"
echo
# Check OS


#IF MACOS

#install command line tools
echo "0a. Checking CLI..."
CLI_INSTALLED=FALSE
CLI_INSTALLED_RETURN=$(xcode-select -p)
if [[ $CLI_INSTALLED_RETURN == *"xcode-select: error:"* ]]
then
    read -p "CLI not installed, install now? (y/n) " RESP
    if [ "$RESP" = "n" ]
    then
        echo "Skipping."
    else
        echo "Installing..."
        CLI_INSTALLED=TRUE
        #xcode-select --install
    fi
else
    echo "CLI already installed"
    CLI_INSTALLED=TRUE
fi
echo

#check homebrew
HOMEBREW_INSTALLED=FALSE
echo "1. Checking homebrew..."
if [ !$(command -v brew &> /dev/null) ]
then
    echo "Homebrew already installed."
    HOMEBREW_INSTALLED=TRUE
else
    read -p "Homebrew not installed, install now? (y/n) " RESP
    if [ "$RESP" = "n" ]
    then
        echo "Skipping."
    else
        echo "Installing homebrew..."
        HOMEBREW_INSTALLED=TRUE
        #/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    fi
fi
echo

#check cmake
CMAKE_INSTALLED=FALSE
echo "2. Checking CMake..."
if [ !$(command -v CMake &> /dev/null) ]
then
    echo "CMake already installed."
    CMAKE_INSTALLED=TRUE
else
    read -p "CMake not installed, install now? (y/n) " RESP
    if [ "$RESP" = "n" ]
    then
        echo "Skipping."
    else
        echo "Installing CMake..."
        CMAKE_INSTALLED=TRUE
        #brew install cmake
    fi
fi
echo

#check cmake
VULKAN_SDK_INSTALLED=FALSE
echo "3. Checking Vulkan SDK..."
if [ !$(command -v vulkanvia &> /dev/null) ]
then
    echo "Vulkan SDK already installed."
    VULKAN_SDK_INSTALLED=TRUE
else
    read -p "Looks like Vulkan SDK not installed or we cannot find them, install now? (y/n) " RESP
    if [ "$RESP" = "n" ]
    then
        echo "Skipping."
    else
        echo "Installing Vulkan SDK..."
        VULKAN_SDK_INSTALLED=TRUE
        # wget https://sdk.lunarg.com/sdk/download/latest/mac/vulkan-sdk.dmg


        # VULKAN_SDK=~/Libs/VulkanSDK/1.3.268.1/macOS
        # mount dmg
        # sudo ./InstallVulkan.app/Contents/MacOS/InstallVulkan --root "$VULKAN_SDK" --accept-licenses --default-answer --confirm-command install


        echo "3a. Export Vulkan SDK variables"
        # #add to end of bashrc
        # export VULKAN_SDK
        # PATH="$PATH:$VULKAN_SDK/bin"
        # export PATH
        # DYLD_LIBRARY_PATH="$VULKAN_SDK/lib:${DYLD_LIBRARY_PATH:-}"
        # export DYLD_LIBRARY_PATH
        # #echo "This script is now using VK_ADD_LAYER_PATH instead of VK_LAYER_PATH"
        # VK_ADD_LAYER_PATH="$VULKAN_SDK/share/vulkan/explicit_layer.d"
        # export VK_ADD_LAYER_PATH
        # VK_ICD_FILENAMES="$VULKAN_SDK/share/vulkan/icd.d/MoltenVK_icd.json"
        # export VK_ICD_FILENAMES
        # VK_DRIVER_FILES="$VULKAN_SDK/share/vulkan/icd.d/MoltenVK_icd.json"
        # export VK_DRIVER_FILES

        echo "3b. Checking Vulkan"
        if [ !$(command -v vulkanvia &> /dev/null) ]
        then
            echo "Vulkan SDK installed correctly"
        fi

        #rm -rf *.dmg
        # remove
    fi
fi
echo


#check glfw
# GLFW_INSTALLED=FALSE
# GLFW_INSTALLED_RETURN=$(command -v $(find / -name glfw) &> /dev/null)
# echo "4. Checking GLFW..."
# if [[ $GLFW_INSTALLED_RETURN == *"/glfw"* ]]
# then
#     echo "GLFW already installed."
#     GLFW_INSTALLED=TRUE
# else
#     read -p "GLFW not installed, install now? (y/n) " RESP
#     if [ "$RESP" = "n" ]
#     then
#         echo "Skipping."
#     else
#         echo "Installing GLFW..."
#         GLFW_INSTALLED=TRUE
#         #brew install glfw
#     fi
# fi
# echo

echo
echo "Environement install helper done:"
echo "CLI:          $CLI_INSTALLED"
echo "Homebrew:     $HOMEBREW_INSTALLED"
echo "CMake:        $CMAKE_INSTALLED"
echo "Vulkan SDK:   $VULKAN_SDK_INSTALLED"
echo "GLFW:         $GLFW_INSTALLED"
exit 1