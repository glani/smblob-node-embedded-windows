if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()

set(XDOTOOL_LIB xdotool-single)

project(xdotool-single C)

include_directories(
        "include"
)

set(CMAKE_C_VISIBILITY_PRESET hidden)

add_library(${XDOTOOL_LIB} src/xdo.c src/xdo.h src/xdo_version.h src/xdo_util.h)


set_target_properties(${XDOTOOL_LIB}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        )
