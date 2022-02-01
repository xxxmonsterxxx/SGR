##########################################################################

project(SGR_test)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++17")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-arc -ObjC++")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")

find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

get_filename_component(SGR_FOLDER ../ ABSOLUTE)

file(GLOB SOURCES ${SGR_FOLDER}/examplesData/test.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

set(GLM_INC_DIR ~/Programming/Library/glm)
set(STB_INC_DIR ~/repos/stb)

find_library(SGR_LIB SGR ${SGR_FOLDER}/build/shared/)

if (VULKAN_FOUND)
    message(STATUS "Found Vulkan, Including and Linking now")
    include_directories(${Vulkan_INCLUDE_DIRS} ${GLM_INC_DIR} ${STB_INC_DIR} ${SGR_FOLDER}/include)
    target_link_libraries (${PROJECT_NAME} ${Vulkan_LIBRARIES} glfw ${SGR_LIB})
endif (VULKAN_FOUND)

file(COPY Resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

###########################################################################