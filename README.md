# ***Simple Graphics Renderer***

![Image of project](https://i.postimg.cc/4xrKg1gS/2022-02-01-14-38-36-1.jpg)

### ***Introduction***

*This library is a wrapper for the Vulkan API, allowing its use without deep diving into Vulkan itself. It handles simple and basic initialization and allows rendering custom 2D/3D objects with basic textures, spritesheets, and fonts for text rendering. Designed for cross-platform use.*

### ***How to use?***
___

There is possibility to build library from source code, but other
option is to [download](https://github.com/xxxmonsterxxx/SGR/releases) release built version as static or shared
library. It's necessary to download (or
include as submodule of git project) and include to your
project follows libraries:    
- **GLM** (https://github.com/g-truc/glm)
- **STB** (https://github.com/nothings/stb)
- **Vulkan** (https://www.lunarg.com/vulkan-sdk)
- **GLFW** (https://github.com/glfw/glfw)

Set path to `SGR-vX.X.X/include` as include path and
`SGR-vX.X.X/lib/static` or shared as external libraries
for your projects.

Manual documentation is currently unavailable. :cry:
[:arrow_down:Contacts](#Contacts)

### ***How to build?***
___

#### ***Using helpers:***
There is a environement install script - [environement_install.sh](https://github.com/xxxmonsterxxx/SGR/blob/master/environement_install.sh). It will install: Brew, Vulkan, GLFW. Then you can run [build.sh](https://github.com/xxxmonsterxxx/SGR/blob/master/build.sh) script to building release, install, debug and example application.

#### ***Environement:***
Unix-like systems:

Just use helper scripts.

Windows
- install1
- install2 
   
#### ***Build:***

For build library you should to export these paths.
```
export CMAKE_PREFIX_PATH=/usr/local/include/SGR  #for cmake package_find command
export SGR_LIB=/usr/local/lib/libSGR.dylib
```

Windows:
```
```

For test of engine run build and launch the application:
```bash
./build.sh -de
```

#### ***References:***
1. [Simple Game Engine](https://github.com/xxxmonsterxxx/SGE) - Game engine based on this SGR library
2. [Vulkan tutorial](https://vulkan-tutorial.com/)
___

### Contacts
*Mikhail Polestchuk*   
*Email - [arsenal-mihail@mail.ru]*    
*Belarus, Minsk, 2022(c)*