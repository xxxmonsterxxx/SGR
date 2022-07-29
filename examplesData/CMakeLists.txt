##########################################################################

cmake_minimum_required(VERSION 3.10)

project(SGR_test)

# compiler options
set(CMAKE_CXX_STANDARD 17) #standard version is 17 - minimum for "optional" use
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3") #optimization

find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/exampleApplication)

get_filename_component(SGR_FOLDER ../ ABSOLUTE)

file(GLOB SOURCES test.cpp)

if (NOT RELEASE)
	set(RELEASE false)
	set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif (NOT RELEASE)

add_definitions(-DNDBUG=${RELEASE})

if (APPLE AND RELEASE)
	add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES})
	set_target_properties(${PROJECT_NAME} PROPERTIES
											BUNDLE True
											MACOSX_BUNDLE_ICON_FILE "ico.icns"
											MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
											MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
											MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
	)
else (APPLE AND RELEASE)
	add_executable(${PROJECT_NAME} ${SOURCES})
endif(APPLE AND RELEASE)

set(GLM_INC_DIR ${SGR_FOLDER}/modules/glm)
set(STB_INC_DIR ${SGR_FOLDER}/modules/stb)

if (NOT RELEASE)
	find_library(SGR_LIB SGR ${SGR_FOLDER}/build/debug/shared/ NO_DEFAULT_PATH)
else (NOT RELEASE)
	find_library(SGR_LIB SGR ${SGR_FOLDER}/build/release/SGR-v0.7.0-MacOS/lib/shared/ NO_DEFAULT_PATH)
endif (NOT RELEASE)

if (NOT SGR_LIB)
	message(FATAL_ERROR "Builded SGR library in parrent folder not found")
endif (NOT SGR_LIB)

if (VULKAN_FOUND)
    message(STATUS "Found Vulkan, Including and Linking now")
    include_directories(${Vulkan_INCLUDE_DIRS} ${GLM_INC_DIR} ${STB_INC_DIR} ${SGR_FOLDER}/include)
    target_link_libraries (${PROJECT_NAME} ${Vulkan_LIBRARIES} glfw ${SGR_LIB})
endif (VULKAN_FOUND)

if (APPLE AND RELEASE)
	file(COPY Resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/)
	file(COPY "Resources/textures/ico.icns" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/)
else (APPLE AND RELEASE)
	file(COPY Resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif(APPLE AND RELEASE)

###########################################################################