#!/bin/bash

# Ubuntu or MacOS (Warning!!! For Windows use environment_install.ps (powershell script))
# shell script for requirement librariess installing

echo
echo "Welcome to Simple Graphics Renderer (SGR) environement install helper."
echo
echo "Script will install (or skip if installed):"
echo "0.  Check OS (different OS requires different libs and tools)"
echo "0a. If You're using MacOS (Darwin system) - Command Line Tools (CLI)"
echo "1.  Homebrew (tools for easy to install different tools and library)"
echo "2.  CMake"
echo "3.  Vulkan SDK"
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
echo "0a. Checking CLI Tools installed..."
CLI_INSTALLED=FALSE
CLI_INSTALLED_RETURN=$(xcode-select -p)
if [[ $CLI_INSTALLED == *"xcode-select: error:"* ]]
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

#check homebrew
HOMEBREW_INSTALLED=FALSE
echo "1. Checking homebrew..."
if [ !$(command -v brew &> /dev/null) ]
then
    echo "Heombrew already installed."
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

#install cmake
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
echo "Environement install helper done:"
echo "CLI:      $CLI_INSTALLED"
echo "Homebrew: $HOMEBREW_INSTALLED"
echo "CMake:    $CMAKE_INSTALLED"
exit 1