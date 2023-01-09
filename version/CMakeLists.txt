cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD ${CPP_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VERSION_LIB smblob-node-embedded-windows-version)

project(${VERSION_LIB})
set(VERSION_SRC)
set(VERSION_HEADER)

list(APPEND
        VERSION_HEADER include/LibraryVersion.h
        )

list(APPEND
        VERSION_SRC src/LibraryVersion.cpp
        )

list(APPEND
        VERSION_SRC ${VERSION_HEADER}
        )

set(common_libs)

include_directories(
        ./
        ./include
        ./src
)

add_library(${VERSION_LIB} STATIC ${VERSION_SRC})

set_target_properties(${VERSION_LIB}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        )