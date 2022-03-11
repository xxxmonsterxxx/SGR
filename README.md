# ***Simple Graphics Renderer***

![Image of project](https://i.postimg.cc/4xrKg1gS/2022-02-01-14-38-36-1.jpg)

### *How to use?*
___

There is possibility to build library from source code, but other
option is to download release built version as static or shared
[library](https://github.com/xxxmonsterxxx/SGR/releases). It's necessary to download (or
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

### *How to build?*
___

For build need to install **Vulkan** and **GLFW** libraries.
Each build can be multithreaded, after "cmake"
command will be generated message with max number
of your threads.

#### *Environement:*

**Linux** (Check if your package system provides GLFW 3.3.6 or another version new enough to be useful to you)
- install vulkan [SDK](https://vulkan.lunarg.com/doc/view/latest/linux/getting_started_ubuntu.html)
- `sudo apt install cmake` - install [CMake](https://cmake.org/) tools
- `sudo apt-get install libglfw3-dev` - install [GLFW](https://www.glfw.org/)

**MacOS**
- install1
- install2

**Windows**
- install1
- install2

MacOS & Linux build is the same:
```bash
./build.sh -option #for options descriptions use -h
```

Windows:
```
```

For test of engine run build and launch the application:
```bash
cd examplesData
cmake .
make
cd build
./SGR_test
```
___

### Contacts
*Mikhail Polestchuk*    
*Email - [arsenal-mihail@mail.ru]*    
*Belarus, Minsk, 2022(c)*