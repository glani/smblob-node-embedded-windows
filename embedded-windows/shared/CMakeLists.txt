cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD ${CPP_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EMBEDDED_WINDOWS_SHARED_LIB smblob-node-embedded-windows-shared)

project(${EMBEDDED_WINDOWS_SHARED_LIB})

set(EMBEDDED_WINDOWS_SRC
        )
set(EMBEDDED_WINDOWS_HEADER
        include/SMBlobNodeEmbeddedWindowsShared.h
        include/internal/SMBlobNodeEmbeddedWindowsSharedInternal.h
        )

list(APPEND
        EMBEDDED_WINDOWS_SRC src/stub.cpp
        )

list(APPEND
        EMBEDDED_WINDOWS_SRC ${EMBEDDED_WINDOWS_HEADER}
)

include_directories(
        ./
        include
        src
)

set(DEPEND_THIRDPARTY_LIBS)

add_library(${EMBEDDED_WINDOWS_SHARED_LIB} STATIC ${EMBEDDED_WINDOWS_SRC})
target_link_libraries(
        ${EMBEDDED_WINDOWS_SHARED_LIB}
        ${DEPEND_THIRDPARTY_LIBS}
)

set_target_properties(${EMBEDDED_WINDOWS_SHARED_LIB}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        )

