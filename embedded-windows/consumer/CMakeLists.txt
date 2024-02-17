cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD ${CPP_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EMBEDDED_WINDOWS_CONSUMER_LIB smblob-node-embedded-windows-consumer)

project(${EMBEDDED_WINDOWS_CONSUMER_LIB})
set(EMBEDDED_WINDOWS_CONSUMER_LIB_SRC
        )
set(EMBEDDED_WINDOWS_CONSUMER_LIB_HEADER
        include/SMBlobNodeEmbeddedWindows.h
        )

list(APPEND
        EMBEDDED_WINDOWS_CONSUMER_LIB_SRC
        src/Consumer.cpp
        src/ConsumerPrivate.cpp
        src/ConsumerPrivate.h
        )

list(APPEND
        EMBEDDED_WINDOWS_CONSUMER_LIB_SRC ${EMBEDDED_WINDOWS_CONSUMER_LIB_HEADER}

        )

findLibUV()

include_directories(
        ./
        include
        src
        ${LIBUV_INCLUDE_PATH}
        ../../thirdparties/uvw/src
        ../../thirdparties/plog/include
        ../windows-actor/include
        ../windows-actor
)


find_package(Protobuf REQUIRED)

set(DEPEND_THIRDPARTY_LIBS)

include_directories(
        ../shared/include
        ../models-shared/include
)

add_library(${EMBEDDED_WINDOWS_CONSUMER_LIB} STATIC ${EMBEDDED_WINDOWS_CONSUMER_LIB_SRC})

addPipeNameDefinition(${EMBEDDED_WINDOWS_CONSUMER_LIB})

target_link_libraries(
        ${EMBEDDED_WINDOWS_CONSUMER_LIB}
        PRIVATE
        ${LIBUV_LIBRARY}
        ${DEPEND_THIRDPARTY_LIBS}
        smblob-node-embedded-windows-actor
        smblob-node-embedded-windows-shared
        smblob-node-embedded-windows-models-shared
)

set_target_properties(${EMBEDDED_WINDOWS_CONSUMER_LIB}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        )
#

