cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD ${CPP_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EMBEDDED_WINDOWS_SHARED_LIB smblob-node-embedded-windows-shared)

project(${EMBEDDED_WINDOWS_SHARED_LIB})

set(EMBEDDED_WINDOWS_SHARED_LIB_SRC
        )
set(EMBEDDED_WINDOWS_SHARED_LIB_HEADER
        include/base64.h
        include/ProcessRunner.h
        )

list(APPEND
        EMBEDDED_WINDOWS_SHARED_LIB_SRC
        src/base64.cpp
        src/ProcessRunner.cpp
        )

list(APPEND
        EMBEDDED_WINDOWS_SHARED_LIB_SRC ${EMBEDDED_WINDOWS_SHARED_LIB_HEADER}
)

findLibUV()

include_directories(
        ./
        include
        src
        ../../thirdparties/uvw/src
        ../../thirdparties/plog/include
        ${LIBUV_INCLUDE_PATH}
)

add_library(${EMBEDDED_WINDOWS_SHARED_LIB} STATIC ${EMBEDDED_WINDOWS_SHARED_LIB_SRC})

target_link_libraries(
        ${EMBEDDED_WINDOWS_SHARED_LIB}
        ${DEPEND_THIRDPARTY_LIBS}
        ${LIBUV_LIBRARY}
)


set_target_properties(${EMBEDDED_WINDOWS_SHARED_LIB}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        )

