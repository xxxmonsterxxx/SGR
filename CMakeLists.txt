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


if(CMAKE_BUILD_TYPE STREQUAL "Release")  # set release or not status for source code
	add_definitions(-DNDBUG=false)
else ()
	add_definitions(-DNDBUG=true)
endif ()


if (BUILD_EXAMPLE)
	add_subdirectory(examplesData)
	return()
endif()


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

if(CMAKE_BUILD_TYPE STREQUAL "Release")
	message("Build Simple Graphics Renderer v${VERSION}")
else ()
	message("Build Simple Graphics Renderer v${VERSION} in debug mode")
endif()

#######################################

# compiler options
set (CMAKE_CXX_STANDARD 17) #standard version is 17 - minimum for "optional" use

if (WIN32)
	set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2") #optimization
else ()
	set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3") #optimization
endif ()


#######################################

include_directories(include) # SGR include folder
file(GLOB SOURCES src/*.cpp) #set as source files all files from src/

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(SGR SHARED ${SOURCES})

set_target_properties(SGR PROPERTIES
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

set_target_properties(SGR PROPERTIES
    VERSION ${VERSION}
    SOVERSION ${VMAJOR}.${VMINOR}.0
)

# include libraries
set(GLM_INC_DIR modules/glm)
set(STB_INC_DIR modules/stb)

# find additional dependecies
find_package(Vulkan 1.3.283.0 REQUIRED)

if (WIN32)
	find_library(glfw glfw3 $ENV{GLFW3_LIB}/lib NO_DEFAULT_PATH)
	include_directories($ENV{GLFW3_LIB}/include/GLFW)
	target_link_libraries (SGR ${glfw})
else (WIN32)
	find_package(glfw3 3.3 REQUIRED)
	target_link_libraries (SGR glfw)
	get_target_property(GLFW3_INCLUDE_DIRS glfw INTERFACE_INCLUDE_DIRECTORIES)
	include_directories(${GLFW3_INCLUDE_DIRS}/GLFW)
endif (WIN32)

include_directories(${Vulkan_INCLUDE_DIRS} ${GLM_INC_DIR} ${STB_INC_DIR})
target_link_libraries (SGR ${Vulkan_LIBRARIES})

# linking MacOSX framework libraries
if (APPLE)
	target_link_libraries(SGR "-framework CoreFoundation")
endif (APPLE)

set(BUILD_DEST ${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/SGR-v${VERSION}) # set build destination

set_target_properties(SGR PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BUILD_DEST}/lib OUTPUT_NAME "SGR")

# install process
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/SGR-v${VERSION})
endif ()

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/SGR)
install(TARGETS SGR DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/)

if (NOT WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_custom_command(TARGET SGR
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} --install .
	)
endif ()

if (WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
	install(CODE "
		execute_process(
        COMMAND reg add \"HKCU\\\\Environment\" /v SGR_LIB /t REG_SZ /d \"${CMAKE_INSTALL_PREFIX}/lib\" /f
        OUTPUT_VARIABLE result
        ERROR_VARIABLE result
        RESULT_VARIABLE exit_code
		)
	"
	)

	install(CODE "
		execute_process(
        COMMAND reg add \"HKCU\\\\Environment\" /v CMAKE_PREFIX_PATH /t REG_SZ /d \"${CMAKE_INSTALL_PREFIX}/include/SGR\" /f
        OUTPUT_VARIABLE result
        ERROR_VARIABLE result
        RESULT_VARIABLE exit_code
		)
	"
	)
endif ()

include(ProcessorCount)
ProcessorCount(N)

message("Ready to build on " ${PLATFORM_NAME})

##########################################################################