cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD ${CPP_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EMBEDDED_WINDOWS_LIB smblob-node-embedded-windows)

project(${EMBEDDED_WINDOWS_LIB})
set(EMBEDDED_WINDOWS_SRC)
set(EMBEDDED_WINDOWS_HEADER)

#list(APPEND
#        EMBEDDED_WINDOWS_HEADER include/LibraryVersion.h
#        )
#
list(APPEND
        EMBEDDED_WINDOWS_SRC src/stub.cpp
        )

list(APPEND
        EMBEDDED_WINDOWS_SRC ${EMBEDDED_WINDOWS_HEADER}
        )

include_directories(
        ./
        ./include
        ./src
)

add_library(${EMBEDDED_WINDOWS_LIB} STATIC ${EMBEDDED_WINDOWS_SRC})

set_target_properties(${EMBEDDED_WINDOWS_LIB}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        )


add_subdirectory(example)
