##########################################################################

cmake_minimum_required(VERSION 3.10)

project(SGR DESCRIPTION "Simple Graphics Renderer")

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

if(CMAKE_BUILD_TYPE STREQUAL "Release")
	message("Build Simple Graphics Renderer v${VERSION}")
else ()
	message("Build Simple Game Graphics Renderer v${VERSION} in debug mode")
endif()

#######################################

# compiler options
set (CMAKE_CXX_STANDARD 17) #standard version is 17 - minimum for "optional" use
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -03") #optimization

add_definitions(-DNDBUG=${RELEASE}) # set release or not status

#######################################

include_directories(include) # SGR include folder
file(GLOB SOURCES src/*.cpp) #set as source files all files from src/

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(SGR SHARED ${SOURCES})


# include libraries
set(GLM_INC_DIR modules/glm)
set(STB_INC_DIR modules/stb)

# find additional dependecies
find_package(Vulkan 1.3.283.0 REQUIRED)

if (WIN32)
	SET(glfw $ENV{GLFW3_LIBRARIES})
	include_directories($ENV{GLFW3_INCLUDE_DIRS})
else (WIN32)
	find_package(glfw3 3.3 REQUIRED)
endif (WIN32)

include_directories(${Vulkan_INCLUDE_DIRS} ${GLM_INC_DIR} ${STB_INC_DIR})
target_link_libraries (SGR ${Vulkan_LIBRARIES} ${glfw})

# linking MacOSX framework libraries
if (APPLE)
	target_link_libraries(SGR "-framework CoreFoundation")
endif (APPLE)

set(BUILD_DEST ${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/SGR-v${VERSION}) # set build destination

set_target_properties(SGR PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BUILD_DEST}/lib OUTPUT_NAME "SGR")

# ther is install process
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include DESTINATION ${CMAKE_INSTALL_PREFIX}/SGR-v${VERSION})
install(TARGETS SGR DESTINATION ${CMAKE_INSTALL_PREFIX}/SGR-v${VERSION}/lib)

include(ProcessorCount)
ProcessorCount(N)

message("Ready to build on " ${PLATFORM_NAME})

##########################################################################

if (BUILD_EXAMPLE)
	add_subdirectory(examplesData)
endif()