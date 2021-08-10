# Simple Graphics Renderer

![Image of project](https://i.postimg.cc/52B9yHT0/Logo-5.png)

How to use?

You could build this, but other option is to download release built version as static or shared library.
In additional to this you should to download (or include as submodule of git project) and include to your project follows libraries:
GLM (https://github.com/g-truc/glm)
STB (https://github.com/nothings/stb)
Vulkan (https://www.lunarg.com/vulkan-sdk)
GLFW (https://github.com/glfw/glfw)

How to build?

For build you need only Vulkan and GLFW libraries.

Each build can be multithreaded, after "cmake" command will be generated message with max number of your threads.

MacOS & Linux & Windows build is the same:

:cmake .

:make

For test of engine you can build run and follow:

:cd examplesData

:cmake .

:make