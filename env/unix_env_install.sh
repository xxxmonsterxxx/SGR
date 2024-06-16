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

if [ $SYSTEM_TYPE == Darwin ]
then
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
            xcode-select --install
        fi
    else
        echo "CLI already installed"
        CLI_INSTALLED=TRUE
    fi
fi
echo

#check homebrew
HOMEBREW_INSTALLED=FALSE
echo "1. Checking homebrew..."
if [ $(command -v brew) ]
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

        if [ $SYSTEM_TYPE == Linux ]
        then
            sudo apt install curl
            /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
            test -d ~/.linuxbrew && eval "$(~/.linuxbrew/bin/brew shellenv)"
            test -d /home/linuxbrew/.linuxbrew && eval "$(/home/linuxbrew/.linuxbrew/bin/brew shellenv)"
            echo "eval \"\$($(brew --prefix)/bin/brew shellenv)\"" >> ~/.bashrc
            echo "export LD_LIBRARY_PATH=/home/linuxbrew/.linuxbrew/lib:\${LD_LIBRARY_PATH}" >> ~/.bashrc
        else
            /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
        fi

        echo "Homebrew installed"
    fi
fi
echo

#check cmake
CMAKE_INSTALLED=FALSE
echo "2. Checking CMake..."
if [ $(command -v cmake) ]
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
        brew install cmake
        if [ $SYSTEM_TYPE == Linux ]
        then
            sudo apt install make
            sudo apt install g++
        fi
    fi
fi
echo

#check vulkan sdk
VULKAN_SDK_INSTALLED=FALSE
echo "3. Checking Vulkan SDK..."
if [ $(command -v vkvia) ]
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

        if [ $SYSTEM_TYPE == Darwin ]
        then
            curl -O https://sdk.lunarg.com/sdk/download/latest/mac/vulkan-sdk.dmg
            VULKAN_SDK=~/Libs/VulkanSDK/latest

            output=$(hdiutil attach "vulkan-sdk.dmg")
            mount_point=$(echo "$output" | grep -o '/Volumes/[^ ]*')
            sudo $mount_point/InstallVulkan.app/Contents/MacOS/InstallVulkan --root "$VULKAN_SDK" --accept-licenses --default-answer --confirm-command install
            hdiutil detach $mount_point

            echo "3a. Export Vulkan SDK variables"

            echo "" >> ~/.zshrc
            echo "export VULKAN_SDK=$VULKAN_SDK/macOS" >> ~/.zshrc
            echo "PATH="\$PATH:\$VULKAN_SDK/bin"" >> ~/.zshrc
            echo "export PATH" >> ~/.zshrc
            echo "DYLD_LIBRARY_PATH="\$VULKAN_SDK/lib:\${DYLD_LIBRARY_PATH:-}"" >> ~/.zshrc
            echo "export DYLD_LIBRARY_PATH" >> ~/.zshrc
            echo "VK_ADD_LAYER_PATH="\$VULKAN_SDK/share/vulkan/explicit_layer.d"" >> ~/.zshrc
            echo "export VK_ADD_LAYER_PATH" >> ~/.zshrc
            echo "VK_ICD_FILENAMES="\$VULKAN_SDK/share/vulkan/icd.d/MoltenVK_icd.json"" >> ~/.zshrc
            echo "export VK_ICD_FILENAMES" >> ~/.zshrc
            echo "VK_DRIVER_FILES="\$VULKAN_SDK/share/vulkan/icd.d/MoltenVK_icd.json"" >> ~/.zshrc
            echo "export VK_DRIVER_FILES" >> ~/.zshrc

            rm -rf *.dmg
        fi

        if [ $SYSTEM_TYPE == Linux ]
        then
            wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo tee /etc/apt/trusted.gpg.d/lunarg.asc
            sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-jammy.list http://packages.lunarg.com/vulkan/lunarg-vulkan-jammy.list
            sudo apt update
            sudo apt install vulkan-sdk
        fi

        echo "3b. Checking Vulkan"
        if [ !$(command -v vulkanvia &> /dev/null) ]
        then
            echo "Vulkan SDK installed correctly"
        fi
    fi
fi
echo


#check glfw
GLFW_INSTALLED=FALSE
GLFW_INSTALLED_RETURN=$(brew list glfw 2>&1)
echo "4. Checking GLFW..."
if [[ $GLFW_INSTALLED_RETURN != Error:* ]]
then
    echo "GLFW already installed."
    GLFW_INSTALLED=TRUE
else
    read -p "GLFW not installed, install now? (y/n) " RESP
    if [ "$RESP" = "n" ]
    then
        echo "Skipping."
    else
        echo "Installing GLFW..."
        GLFW_INSTALLED=TRUE
        brew install glfw
        brew install glfw
    fi
fi
echo

echo
echo "Environement install helper done:"

if [ $SYSTEM_TYPE == Darwin ]
then
echo "CLI:          $CLI_INSTALLED"
fi

echo "Homebrew:     $HOMEBREW_INSTALLED"
echo "CMake:        $CMAKE_INSTALLED"
echo "Vulkan SDK:   $VULKAN_SDK_INSTALLED"
echo "GLFW:         $GLFW_INSTALLED"
exit 1