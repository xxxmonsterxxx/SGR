##########################################################################

project(SGR DESCRIPTION "Simple Graphics Renderer")

cmake_minimum_required(VERSION 3.10)

##########################################################################

file(STRINGS "./include/version.h" ver)

string(REGEX MATCH "SGR_VERSION_MAJOR ([0-9]*)" _ ${ver})
set(VMAJOR ${CMAKE_MATCH_1})

string(REGEX MATCH "SGR_VERSION_MINOR ([0-9]*)" _ ${ver})
set(VMINOR ${CMAKE_MATCH_1})

string(REGEX MATCH "SGR_VERSION_PATCH ([0-9]*)" _ ${ver})
set(VPATCH ${CMAKE_MATCH_1})

set(VERSION ${VMAJOR}.${VMINOR}.${VPATCH})


# Config version file generate 
set(SGR_CONFIG_VERSION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/include/SGRConfigVersion.cmake")
set(SGR_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/include/SGRConfig.cmake")

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/SGRConfigVersion.cmake.in"
  "${SGR_CONFIG_VERSION_FILE}"
  @ONLY
)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/SGRConfig.cmake.in"
  "${SGR_CONFIG_FILE}"
  @ONLY
)

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

if (RELEASE)
	message("Build Simple Graphics Renderer v${VERSION}")
else (RELEASE)
	message("Build Simple Game Graphics Renderer in debug mode")
	set(DEBUG TRUE)
endif (RELEASE)

#######################################

# compiler options
set (CMAKE_CXX_STANDARD 17) #standard version is 17 - minimum for "optional" use
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3") #optimization

if (DEBUG)
	set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif (DEBUG)

add_definitions(-DNDBUG=${RELEASE}) # set release or not status

#######################################

add_definitions(-DNDBUG=${RELEASE}) # set release or not status

#######################################

include_directories(include) # SGR include folder
file(GLOB SOURCES src/*.cpp) #set as source files all files from src/

add_library(SGR_ST STATIC ${SOURCES})
add_library(SGR SHARED ${SOURCES})


# include libraries
set(GLM_INC_DIR modules/glm)
set(STB_INC_DIR modules/stb)

# find additional dependecies
find_package(Vulkan 1.3.283.0 REQUIRED)
find_package(glfw3 3.3 REQUIRED)

include_directories(${Vulkan_INCLUDE_DIRS} ${GLM_INC_DIR} ${STB_INC_DIR})
target_link_libraries (SGR ${Vulkan_LIBRARIES} glfw)
target_link_libraries (SGR_ST ${Vulkan_LIBRARIES} glfw)

# linking MacOSX framework libraries
if (APPLE)
	target_link_libraries(SGR "-framework CoreFoundation")
endif (APPLE)

# library prefix based on paltform
set(RELEASE_BUILD_DEST release/SGR-v${VERSION}-${PLATFORM_NAME}) # set release build destination
set(DEBUG_BUILD_DEST debug) # set release build destination

if (RELEASE)
	file(COPY include DESTINATION ${RELEASE_BUILD_DEST})
	set_target_properties(SGR PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${RELEASE_BUILD_DEST}/lib/shared)
	set_target_properties(SGR_ST PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${RELEASE_BUILD_DEST}/lib/static)
else (RELEASE)
	set_target_properties(SGR PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${DEBUG_BUILD_DEST}/shared)
	set_target_properties(SGR_ST PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${DEBUG_BUILD_DEST}/static)
endif (RELEASE)

include(ProcessorCount)
ProcessorCount(N)

message("Ready to build on " ${PLATFORM_NAME})

##########################################################################