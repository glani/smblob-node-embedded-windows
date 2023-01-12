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
        include/internal/SMBlobNodeEmbeddedWindowsSharedLog.h
        )

list(APPEND
        EMBEDDED_WINDOWS_SRC src/stub.cpp
        src/models/embedded-windows.scheme.pb.cc
        )

list(APPEND
        EMBEDDED_WINDOWS_SRC ${EMBEDDED_WINDOWS_HEADER}
)

include_directories(
        ./
        include
        src
        ../../thirdparties/plog/include
)

set(DEPEND_THIRDPARTY_LIBS)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

add_library(${EMBEDDED_WINDOWS_SHARED_LIB} STATIC ${EMBEDDED_WINDOWS_SRC})
target_link_libraries(
        ${EMBEDDED_WINDOWS_SHARED_LIB}
        ${DEPEND_THIRDPARTY_LIBS}
        ${Protobuf_LIBRARIES}
)

set_target_properties(${EMBEDDED_WINDOWS_SHARED_LIB}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        )

