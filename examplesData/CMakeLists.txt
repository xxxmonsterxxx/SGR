##########################################################################

cmake_minimum_required(VERSION 3.10)

project(SGR_test)

# compiler options
set(CMAKE_CXX_STANDARD 17) #standard version is 17 - minimum for "optional" use

find_package(Vulkan 1.3.283.0 REQUIRED)

if (WIN32)
	SET(glfw $ENV{GLFW3_LIBRARIES})
	include_directories($ENV{GLFW3_INCLUDE_DIRS})
else (WIN32)
	find_package(glfw3 3.3 REQUIRED)
endif (WIN32)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/exampleApplication)

file(GLOB SOURCES test.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

set(GLM_INC_DIR ../modules/glm)
set(STB_INC_DIR ../modules/stb)

find_library(SGR_LIB SGR ${CMAKE_SOURCE_DIR}/build/SGR-v${VERSION}/lib/ NO_DEFAULT_PATH)

include_directories(${Vulkan_INCLUDE_DIRS} ${GLM_INC_DIR} ${STB_INC_DIR} ${SGR_FOLDER}/include)
target_link_libraries (${PROJECT_NAME} ${Vulkan_LIBRARIES} ${glfw} ${SGR_LIB})

file(COPY Resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(DLL_FILES ${CMAKE_SOURCE_DIR}/build/SGR-v${VERSION}/lib/SGR.dll)

add_custom_target(CopyDlls ALL
    COMMENT "Copying dynamic libraries to output directory"
    DEPENDS ${DLL_FILES}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${DLL_FILES}
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

add_dependencies(${PROJECT_NAME} CopyDlls)


###########################################################################