cmake_minimum_required(VERSION 3.10)

message("Build Simple Graphic Renderer v"${VERSION})

project(SGR DESCRIPTION "Simple Graphics Renderer")

# compiler options
set (CMAKE_CXX_STANDARD 17) #standard version is 17 - minimum for "optional" use
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3") #optimization

# srg include folder
include_directories(include)

if (NOT RELEASE)
	set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif (NOT RELEASE)

SET(SOURCE  src/CommandManager.cpp
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
			src/WindowManager.cpp)

add_library(SGR_ST STATIC ${SOURCE})
add_library(SGR SHARED ${SOURCE})




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
	target_link_libraries (SGR_ST ${Vulkan_LIBRARIES} glfw)
endif (VULKAN_FOUND)

set(BUILD_DEST build/SGR-v${VERSION})

if (RELEASE)
	file(COPY include DESTINATION ${BUILD_DEST})
	set_target_properties(SGR PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BUILD_DEST}/lib/shared)
	set_target_properties(SGR_ST PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DEST}/lib/static)
else (RELEASE)
	set_target_properties(SGR PROPERTIES LIBRARY_OUTPUT_DIRECTORY build/shared)
	set_target_properties(SGR_ST PROPERTIES ARCHIVE_OUTPUT_DIRECTORY build/static)
endif (RELEASE)

include(ProcessorCount)
ProcessorCount(N)
message("Please build using: make -j"  ${N})
