##########################################################################

cmake_minimum_required(VERSION 3.10)

project(SGR DESCRIPTION "Simple Graphics Renderer")

#######################################

if (RELEASE AND NOT VERSION)
	message(FATAL_ERROR "Release version was not setted! Use -DVERSION=X.Y.Z")
endif (RELEASE AND NOT VERSION)

if (NOT VERSION)
	message("Build Simple Graphics Renderer in debug mode")
else (NOT VERSION)
	message("Build Simple Graphics Renderer v"${VERSION})
endif (NOT VERSION)

#######################################

# platform detection
if (UNIX AND NOT APPLE)
	set(LINUX TRUE)
endif (UNIX AND NOT APPLE)

if (APPLE)
	set(PLATFORM_NAME MacOS)
endif (APPLE)

if (LINUX)
	set(PLATFORM_NAME Linux)
endif (LINUX)

if (WIN32)
	set(PLATFORM_NAME Windows)
endif (WIN32)

#######################################

# compiler options
set (CMAKE_CXX_STANDARD 17) #standard version is 17 - minimum for "optional" use
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3") #optimization

#######################################

include_directories(include) # SGR include folder

if (NOT RELEASE)
	set(RELEASE false)
	set(CMAKE_BUILD_TYPE RelWithDebInfo)
else (NOT RELEASE)
	
endif (NOT RELEASE)

add_definitions(-DNDBUG=${RELEASE}) # set release or not status

# define all source files - in future need to set by default (all *.cpp files)
SET(SOURCE  src/utils.cpp
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
			src/WindowManager.cpp)

add_library(SGR_ST STATIC ${SOURCE})
add_library(SGR SHARED ${SOURCE})


# include libraries
set(GLM_INC_DIR modules/glm)
set(STB_INC_DIR modules/stb)

# find additional dependecies
find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)

# define if glfw3 founded
if (NOT glfw3_DIR)
	set(GLFW3_FOUND FALSE)
else (NOT glfw3_DIR)
	set(GLFW3_FOUND TRUE)
endif (NOT glfw3_DIR)

# status info about external dependecies
if (VULKAN_FOUND AND GLFW3_FOUND)
    message(STATUS "Found Vulkan, Including and Linking now")
	message(STATUS "Found GLFW3, Including and Linking now")
    include_directories(${Vulkan_INCLUDE_DIRS} ${GLM_INC_DIR} ${STB_INC_DIR})
    target_link_libraries (SGR ${Vulkan_LIBRARIES} glfw)
	target_link_libraries (SGR_ST ${Vulkan_LIBRARIES} glfw)
else (VULKAN_FOUND AND GLFW3_FOUND)
	if (NOT VULKAN_FOUND)
		message(FATAL_ERROR "Vulkan not found, please install Vulkan using library README")
	endif (NOT VULKAN_FOUND)

	if (NOT GLFW3_FOUND)
		message(FATAL_ERROR "GLFW3 not found, please install GLFW3 using library README")
	endif (NOT GLFW3_FOUND)
endif (VULKAN_FOUND AND GLFW3_FOUND)

# linking MacOSX framework libraries
if (APPLE)
	target_link_libraries(SGR "-framework CoreFoundation")
endif (APPLE)

# library prefix based on paltform

set(RELEASE_BUILD_DEST build/SGR-v${VERSION}-${PLATFORM_NAME}) # set release build destination

if (RELEASE)
	file(COPY include DESTINATION ${RELEASE_BUILD_DEST})
	set_target_properties(SGR PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${RELEASE_BUILD_DEST}/lib/shared)
	set_target_properties(SGR_ST PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${RELEASE_BUILD_DEST}/lib/static)
else (RELEASE)
	set_target_properties(SGR PROPERTIES LIBRARY_OUTPUT_DIRECTORY build/shared)
	set_target_properties(SGR_ST PROPERTIES ARCHIVE_OUTPUT_DIRECTORY build/static)
endif (RELEASE)

include(ProcessorCount)
ProcessorCount(N)

message("Ready to build on "${PLATFORM_NAME})
message("Please build using: make -j"  ${N})


##########################################################################