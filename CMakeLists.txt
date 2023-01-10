cmake_minimum_required(VERSION 3.22)
message("CMAKE_VERSION: ${CMAKE_VERSION}")
set(CPP_STANDARD 17)

set(CMAKE_CXX_STANDARD ${CPP_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(smblob-node-embedded-windows)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/)
message(STATUS "CMAKE_MODULE_PATH:" ${CMAKE_MODULE_PATH})

# Include AppsOptions
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/CompileOptions.cmake)

set(DEPEND_THIRD_PARTY_LIBS)
set(LIB_PLATFORM ${CMAKE_MODULE_PATH})


if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-D_DEBUG)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-D_RELEASE)
endif (CMAKE_BUILD_TYPE STREQUAL "Release")


if (WIN32)
elseif (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14")
    find_library(CFLIB CoreFoundation)
    list(APPEND DEPEND_THIRD_PARTY_LIBS ${CFLIB})
else ()

endif ()

add_subdirectory(thirdparties)
add_subdirectory(version)
add_subdirectory(embedded-windows)

execute_process(COMMAND python3
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/generate-version.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/version/include"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})