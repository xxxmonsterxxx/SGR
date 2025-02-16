# ***Simple Graphics Renderer ver. 0.10.0*** 
[![SGR autobuild](https://github.com/xxxmonsterxxx/SGR/actions/workflows/ci.yml/badge.svg)](https://github.com/xxxmonsterxxx/SGR/actions/workflows/ci.yml)

![Image of project](https://i.postimg.cc/4xrKg1gS/2022-02-01-14-38-36-1.jpg)

### ***Introduction***

*This library is a wrapper for the Vulkan API, allowing its use without deep diving into Vulkan itself. It handles simple and basic initialization and allows rendering custom 2D/3D objects with basic textures, spritesheets and fonts for text rendering. Designed for cross-platform use (MacOS, Linux, Windows).*

#### ***Supported features:***
* Creation of a simple window application
* Creation of custom 2D/3D shapes/geometry
* Applying any texture to these shapes
* Drawing text objects
* Ability to change shape textures in real-time
* Real-time control of drawn objects (position, orientation, scale)
* Drawing UI interfaces (buttons, text)
* And many others...

### ***How to use?***
___

There is a possibility to build library from source code, but other
option is to [download](https://github.com/xxxmonsterxxx/SGR/releases) release-built version as shared library. For using you should to download following libraries:
- [**Vulkan**](https://www.lunarg.com/vulkan-sdk)
- [**GLFW**](https://github.com/glfw/glfw)

Manual documentation is currently unavailable. :cry:
[:arrow_down:Contacts](#Contacts)

### ***How to build?***
___

Firstly required to download project and its equal to install [git](https://www.git-scm.com/):

MacOS and Linux (terminal):
```
git #for MacOS if was not installed you will got installation process
sudo apt install git #for Linux

#then navigate to folder where you want to install project
git clone --recursive https://github.com/xxxmonsterxxx/SGR.git
cd SGR
#now you are ready
```

For windows you could to use git bash or git desktop or simply download project as ZIP.


#### ***Environement:***
This project contains helpfull scripts to environement install and configure additional dependencies. If you wan't to manually install the libraries, just use the helper scripts:
- Windows: /env/win_env_install.ps1 - Powershell script
- Linux & MacOS: /env/unix_env_install.sh - Bash script

Just run and follow instructions. If you expirience any issues with these scripts, please send me report by email or as an issue on github.

After environement was installed
   

#### ***Build:***

Windows:
```
CMakeLists of this project is well-configured for using Microsoft Visual Studio 2019+. Just open project as local folder and MSVS will confiugre automatically build with all build configurations.
```

Linux and MacOS:
```
./build.sh -h

Use this to see all given options for building and debugging.
```

**In result you should to see something like this**
![Gif of project](https://i.postimg.cc/cLfVFPQ0/0217.gif)

Also are presented release builded example projects for different systems:
- [Windows](https://drive.google.com/file/d/1EHSZGDK2KNPAjeVvmG5eSaR14COx7gP-/view?usp=sharing)
- [MacOS](https://drive.google.com/file/d/1HEEnrqS26Sr7wE-3bwWuPWpwlFXXWDSA/view?usp=sharing)
- [Linux (empty)]()

#### ***References:***
1. [Simple Game Engine](https://github.com/xxxmonsterxxx/SGE) - Game engine based on this SGR library
2. [Vulkan tutorial](https://vulkan-tutorial.com/)
3. [Khronos Vulkan tutorial](https://docs.vulkan.org/tutorial/latest/00_Introduction.html)
4. [More one usefull guide](https://vkguide.dev/)
5. [Sasha's Willems examples](https://www.saschawillems.de/creations/vulkan-examples/)
___

P.S. I am always open to new ideas for this project improvement.

### Contacts
*Mikhail Polestchuk*   
*Email - [arsenal-mihail@mail.ru]*    
*Belarus, Minsk, 2022(c)*
