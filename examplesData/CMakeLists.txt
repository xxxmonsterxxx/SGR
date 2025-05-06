##########################################################################

cmake_minimum_required(VERSION 3.16)

project(SGR_test)

# compiler options
set(CMAKE_CXX_STANDARD 17) #standard version is 17 - minimum for "optional" use

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/exampleApplication)

file(GLOB SOURCES test.cpp)

if (WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES} Resources/logo/icon.rc)
elseif (APPLE AND CMAKE_BUILD_TYPE STREQUAL "Release")

    file(GLOB_RECURSE RESOURCE_FILES "Resources/*")
    foreach (FILE ${RESOURCE_FILES})
        file(RELATIVE_PATH NEW_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Resources ${FILE})
        get_filename_component(NEW_FILE_PATH ${NEW_FILE} DIRECTORY)
        set_source_files_properties(${FILE}
                                    PROPERTIES
                                    MACOSX_PACKAGE_LOCATION "Resources/${NEW_FILE_PATH}"
                                    HEADER_FILE_ONLY ON)
    endforeach()

    set(MACOSX_BUNDLE_ICON_FILE "logo/Logo.icns")

    # install rpath for distributing
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH "@executable_path/../Frameworks")

    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES}
                                                 ${RESOURCE_FILES})

    set_target_properties(${PROJECT_NAME} PROPERTIES
                                          MACOSX_BUNDLE TRUE
                                          MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
                                          MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION}
                                          MACOSX_BUNDLE_SHORT_VERSION_STRING ${VMAJOR}.${VMINOR}
                                          MACOSX_BUNDLE_COPYRIGHT "© 2022 SimpleGraphicsFam. All rights reserved."
    )

    set(BUNDLE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app)
    set(FRAMEWORKS_PATH ${BUNDLE_PATH}/Contents/Frameworks)

    # Creating Resources/Vulkan/icd.d
    file(MAKE_DIRECTORY ${BUNDLE_PATH}/Contents/Resources/vulkan/icd.d)

    # JSON for Vulkan loader
    set(MOLTENVK_ICD_JSON_CONTENT 
    "{
        \"file_format_version\" : \"1.0.0\",
        \"ICD\": {
            \"library_path\": \"../../../Frameworks/libMoltenVK.dylib\",
            \"api_version\" : \"1.2.0\",
            \"is_portability_driver\" : true
        }
    }")

    # Creating MoltenVK_icd.json
    file(WRITE ${BUNDLE_PATH}/Contents/Resources/vulkan/icd.d/MoltenVK_icd.json 
            "${MOLTENVK_ICD_JSON_CONTENT}")

    file(COPY ${Vulkan_LIBRARIES} DESTINATION ${FRAMEWORKS_PATH} FOLLOW_SYMLINK_CHAIN)
    file(COPY $ENV{VULKAN_SDK}/lib/libMoltenVK.dylib DESTINATION ${FRAMEWORKS_PATH} FOLLOW_SYMLINK_CHAIN)
    file(COPY ${BUILD_DEST}/lib/libSGR.dylib DESTINATION ${FRAMEWORKS_PATH} FOLLOW_SYMLINK_CHAIN)
    file(COPY ${GLFW3_INCLUDE_DIRS}/../lib/libglfw.dylib DESTINATION ${FRAMEWORKS_PATH} FOLLOW_SYMLINK_CHAIN)

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD                    
                    COMMAND codesign --deep --force --verbose --sign - ${BUNDLE_PATH}
                    COMMENT "Signing the application bundle...")
else ()
    add_executable(${PROJECT_NAME} ${SOURCES})
    file(COPY Resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

set(GLM_INC_DIR ../modules/glm)
set(STB_INC_DIR ../modules/stb)
set(TINY_OBJ_LOADER_INC_DIR ../modules/tiny_obj_loader)
set(TINY_GLTF_LOADER_INC_DIR ../modules/tiny_gltf_loader)

set(SGR_FOLDER ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/SGR-v${VERSION}-${CMAKE_SYSTEM_NAME})

include_directories(${GLM_INC_DIR} ${STB_INC_DIR} ${TINY_OBJ_LOADER_INC_DIR} ${TINY_GLTF_LOADER_INC_DIR} ${SGR_FOLDER}/include/SGR)
target_link_libraries (${PROJECT_NAME} SGR)

if (WIN32)
    set(DLL_FILES ${BUILD_DEST}/lib/SGR.dll $ENV{GLFW3_LIB}/lib/glfw3.dll)

    add_custom_target(CopyDlls ALL
        COMMENT "Copying dynamic libraries to output directory"
        DEPENDS ${DLL_FILES}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${DLL_FILES}
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )

    add_dependencies(${PROJECT_NAME} CopyDlls)

    if (MAKE_INSTALL_PACKAGE)
	#    include(CPack)
    #
	#    set(CPACK_GENERATOR "WIX")
	#    set(CPACK_PACKAGE_NAME "SGR installer")
	#    set(CPACK_PACKAGE_VERSION ${VERSION})
	#    set(CPACK_PACKAGE_INSTALL_DIRECTORY "MyApp")
    #
	#    add_custom_command(
	#    	TARGET ${PROJECT_NAME}
	#    	POST_BUILD
	#    	COMMAND ${CMAKE_COMMAND} -E echo "Generating installer..."
	#    	COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR} cpack --config CPackConfig.cmake
	#    )
    endif()

endif()

###########################################################################