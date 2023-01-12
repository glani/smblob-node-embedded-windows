if (LINUX)
    set(XDOTOOL_MAJOR 3)
    set(XDOTOOL_MINOR 20211022)
    set(XDOTOOL_PATCH 1)

    set(XDOTOOL_VERSION ${XDOTOOL_MAJOR}.${XDOTOOL_MINOR}.${XDOTOOL_PATCH} CACHE STRING "Build with xdotool")
    set(XDOTOOL_URL https://github.com/jordansissel/xdotool/releases/download/v${XDOTOOL_VERSION}/xdotool-${XDOTOOL_VERSION}.tar.gz)
    if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/xdotool-src)
        message(STATUS "Downloading xdotool sources: ${XDOTOOL_URL}")
        execute_process(COMMAND wget ${XDOTOOL_URL} -O ${CMAKE_CURRENT_SOURCE_DIR}/xdotool-src-${XDOTOOL_VERSION}.tar.gz)
        execute_process(COMMAND tar -xzf ${CMAKE_CURRENT_SOURCE_DIR}/xdotool-src-${XDOTOOL_VERSION}.tar.gz -C ${CMAKE_CURRENT_SOURCE_DIR} --one-top-level=xdotool-src --strip-components 1)
    endif ()
    message(STATUS "Build with xdotool: ${XDOTOOL_VERSION}")
    set(XDOTOOL_SRC_FILES xdo.c xdo.h xdo_util.h xdo_version.h)
    foreach (X IN LISTS XDOTOOL_SRC_FILES)
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/xdotool-src/${X} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/xdotool-wrapper/src)
    endforeach ()
    add_subdirectory(xdotool-wrapper)

    set(LIBUV_MAJOR 1)
    set(LIBUV_MINOR 43)
    set(LIBUV_PATCH 0)

    set(LIBUV_VERSION ${LIBUV_MAJOR}.${LIBUV_MINOR}.${LIBUV_PATCH} CACHE STRING "Build with libuv")
    if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/libuv)
        set(LIBUV_URL https://github.com/libuv/libuv/archive/refs/tags/v${LIBUV_VERSION}.tar.gz)
        message(STATUS "Downloading libuv sources: ${LIBUV_URL}")
        execute_process(COMMAND wget ${LIBUV_URL} -O ${CMAKE_CURRENT_SOURCE_DIR}/libuv-src-${LIBUV_VERSION}.tar.gz)
        execute_process(COMMAND tar -xzf ${CMAKE_CURRENT_SOURCE_DIR}/libuv-src-${LIBUV_VERSION}.tar.gz -C ${CMAKE_CURRENT_SOURCE_DIR} --one-top-level=libuv --strip-components 1)
    endif ()


    set(PROTOBUF_MAJOR 3)
    set(PROTOBUF_MINOR 21)
    set(PROTOBUF_PATCH 12)

    set(PROTOBUF_VERSION ${PROTOBUF_MAJOR}.${PROTOBUF_MINOR}.${PROTOBUF_PATCH} CACHE STRING "Build with protobuf")
    if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/protobuf)
        set(PROTOBUF_URL https://github.com/protocolbuffers/protobuf/releases/download/v${PROTOBUF_MINOR}.${PROTOBUF_PATCH}/protobuf-cpp-${PROTOBUF_VERSION}.tar.gz)
        message(STATUS "Downloading protobuf sources: ${PROTOBUF_URL}")
        execute_process(COMMAND wget ${PROTOBUF_URL} -O ${CMAKE_CURRENT_SOURCE_DIR}/protobuf-src-${PROTOBUF_VERSION}.tar.gz)
        execute_process(COMMAND tar -xzf ${CMAKE_CURRENT_SOURCE_DIR}/protobuf-src-${PROTOBUF_VERSION}.tar.gz -C ${CMAKE_CURRENT_SOURCE_DIR} --one-top-level=protobuf --strip-components 1)
    endif ()

endif ()


set(LIBUVW_VERSION "v2.11.0_libuv_v${LIBUV_MAJOR}.${LIBUV_MINOR}" CACHE STRING "Build with uvw for libuv")

if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/uvw)
    set(LIBUVW_URL https://github.com/skypjack/uvw/archive/refs/tags/${LIBUVW_VERSION}.tar.gz)
    message(STATUS "Downloading uvw for libuv sources: ${LIBUVW_URL}")
    execute_process(COMMAND wget ${LIBUVW_URL} -O ${CMAKE_CURRENT_SOURCE_DIR}/uvw-src-${LIBUVW_VERSION}.tar.gz)
    execute_process(COMMAND tar -xzf ${CMAKE_CURRENT_SOURCE_DIR}/uvw-src-${LIBUVW_VERSION}.tar.gz -C ${CMAKE_CURRENT_SOURCE_DIR} --one-top-level=uvw --strip-components 1)
endif ()


set(PLOG_MAJOR 1)
set(PLOG_MINOR 1)
set(PLOG_PATCH 9)

set(PLOG_VERSION ${PLOG_MAJOR}.${PLOG_MINOR}.${PLOG_PATCH} CACHE STRING "Build with plog")
if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/plog)
    set(PLOG_URL https://github.com/SergiusTheBest/plog/archive/refs/tags/${PLOG_VERSION}.tar.gz)
    message(STATUS "Downloading plog sources: ${PLOG_URL}")
    execute_process(COMMAND wget ${PLOG_URL} -O ${CMAKE_CURRENT_SOURCE_DIR}/plog-src-${PLOG_VERSION}.tar.gz)
    execute_process(COMMAND tar -xzf ${CMAKE_CURRENT_SOURCE_DIR}/plog-src-${PLOG_VERSION}.tar.gz -C ${CMAKE_CURRENT_SOURCE_DIR} --one-top-level=plog --strip-components 1)
endif ()