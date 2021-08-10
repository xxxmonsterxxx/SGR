cmake_minimum_required(VERSION 3.10)

project(SGR DESCRIPTION "Simple Graphics Renderer")

set(LIBRARY_BUILD_TYPE SHARED)

# srg include folder
include_directories(include)

add_library(SGR ${LIBRARY_BUILD_TYPE}
            src/CommandManager.cpp
            src/DescriptorManager.cpp
            src/FileManager.cpp
            src/LogicalDeviceManager.cpp
            src/MemoryManager.cpp
            src/PhysicalDeviceManager.cpp
            src/PipelineManager.cpp
            src/RenderPassManager.cpp
            src/SGR.cpp
            src/ShaderManager.cpp
            src/SwapChainManager.cpp
            src/TextureManager.cpp
            src/WindowManager.cpp
            )

# compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-arc -ObjC++")

# include libraries
set(GLM_INC_DIR modules/glm)
set(STB_INC_DIR modules/stb)

# find additional dependecies
find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)

if (VULKAN_FOUND)
    message(STATUS "Found Vulkan, Including and Linking now")
    include_directories(${Vulkan_INCLUDE_DIRS} ${GLM_INC_DIR} ${STB_INC_DIR})
    target_link_libraries (SGR ${Vulkan_LIBRARIES} glfw)
endif (VULKAN_FOUND)

set_target_properties(SGR PROPERTIES PUBLIC_HEADER SGR.h)
set_target_properties(SGR PROPERTIES LIBRARY_OUTPUT_DIRECTORY build/shared)
set_target_properties(SGR PROPERTIES ARCHIVE_OUTPUT_DIRECTORY build/static)

include(ProcessorCount)
ProcessorCount(N)
message("Please build using: make -j"  ${N})
