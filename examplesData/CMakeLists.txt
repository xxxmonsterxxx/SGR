##########################################################################

project(SGR_test)
cmake_minimum_required(VERSION 3.10)

# compiler options
set(CMAKE_CXX_STANDARD 17) #standard version is 17 - minimum for "optional" use
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3") #optimization

find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

get_filename_component(SGR_FOLDER ../ ABSOLUTE)

file(GLOB SOURCES ${SGR_FOLDER}/examplesData/test.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

set(GLM_INC_DIR ${SGR_FOLDER}/modules/glm)
set(STB_INC_DIR ${SGR_FOLDER}/modules/stb)

find_library(SGR_LIB SGR ${SGR_FOLDER}/build/shared/)

if (VULKAN_FOUND)
    message(STATUS "Found Vulkan, Including and Linking now")
    include_directories(${Vulkan_INCLUDE_DIRS} ${GLM_INC_DIR} ${STB_INC_DIR} ${SGR_FOLDER}/include)
    target_link_libraries (${PROJECT_NAME} ${Vulkan_LIBRARIES} glfw ${SGR_LIB})
endif (VULKAN_FOUND)

file(COPY Resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

###########################################################################