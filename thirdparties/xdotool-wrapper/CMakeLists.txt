if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()

set(XDOTOOL_LIB xdotool-single)

project(xdotool-single C)

include_directories(
        "include"
)

set(CMAKE_C_VISIBILITY_PRESET hidden)

find_package(X11 REQUIRED)

if (!X11_XTest_FOUND)
    message(FATAL_ERROR "X11 extensions are required, but not found!")
endif ()
if (!X11_Xfixes_LIB)
    message(FATAL_ERROR "X11 fixes extension is required, but not found!")
endif ()

if (!X11_xcb_FOUND)
    message(FATAL_ERROR "X11 xcb extensions are required, but not found!")
endif ()
if (!X11_xcb_LIB)
    message(FATAL_ERROR "X11 xcb is required, but not found!")
endif ()

if (!X11_xcb_icccm_FOUND)
    message(FATAL_ERROR "X11 xcb icccm is required, but not found!")
endif ()
if (!X11_xcb_icccm_LIB)
    message(FATAL_ERROR "X11 xcb icccm is required, but not found!")
endif ()

if (!WNCK_Xfixes_LIB)
    message(STATUS "WNCK fixes extension is required, but not found!")
endif ()

include_directories(
        ${X11_INCLUDE_DIR}
        ${X11_xcb_INCLUDE_PATH}
        ${X11_xcb_icccm_INCLUDE_PATH}
        ${X11_XCB_XINERAMA_INCLUDE_PATH}
        ${WNCK_INCLUDE_DIRS}
)


set(${PROJECT_NAME}_PLATFORM_LIBS
        ${X11_LIBRARIES}
        ${X11_Xfixes_LIB}
        ${X11_XTest_LIB}
        ${X11_xcb_LIB}
        xcb-xinerama
        X11-xcb
        Xinerama
        )

add_library(${XDOTOOL_LIB} src/xdo.c src/xdo.h src/xdo_version.h src/xdo_util.h src/xdo_search.c)

target_link_libraries(${XDOTOOL_LIB} PRIVATE ${${PROJECT_NAME}_PLATFORM_LIBS})

set_target_properties(${XDOTOOL_LIB}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        )
