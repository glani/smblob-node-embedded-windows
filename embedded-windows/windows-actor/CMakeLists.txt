cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD ${CPP_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EMBEDDED_WINDOWS_WINDOWS_ACTOR_LIB smblob-node-embedded-windows-actor)

project(${EMBEDDED_WINDOWS_WINDOWS_ACTOR_LIB})
set(EMBEDDED_WINDOWS_WINDOWS_ACTOR_LIB_SRC
        )
set(EMBEDDED_WINDOWS_WINDOWS_ACTOR_LIB_HEADER
        )

list(APPEND
        EMBEDDED_WINDOWS_WINDOWS_ACTOR_LIB_SRC
        )

list(APPEND
        EMBEDDED_WINDOWS_WINDOWS_ACTOR_LIB_SRC ${EMBEDDED_WINDOWS_WINDOWS_ACTOR_LIB_HEADER}
        )



set({${PROJECT_NAME}_PLATFORM_INC})

set(SCREEN_CAPTURE_PLATFORM_INC
        include/linux
        ${X11_INCLUDE_DIR}
        ${X11_xcb_INCLUDE_PATH}
        ${X11_xcb_icccm_INCLUDE_PATH}
        ${X11_XCB_XINERAMA_INCLUDE_PATH}
        ${WNCK_INCLUDE_DIRS}
        )

if (LINUX)
    include_directories(
            ../../thirdparties/xdotool-wrapper/src
    )

    list(APPEND EMBEDDED_WINDOWS_WINDOWS_ACTOR_LIB_SRC
            linux/XcbInitializer.cpp
            include/linux/XcbInitializer.h
            include/linux/XcbAtom.h
            linux/XcbAtom.cpp
            )
    FIND_PACKAGE(PkgConfig REQUIRED)
    find_package(X11 REQUIRED)

    list(APPEND ${PROJECT_NAME}_PLATFORM_INC
            ${X11_INCLUDE_DIR}
            ${X11_xcb_INCLUDE_PATH}
            ${X11_xcb_icccm_INCLUDE_PATH}
            ${X11_XCB_XINERAMA_INCLUDE_PATH}
            )
    set(${PROJECT_NAME}_PLATFORM_LIBS
            xdotool-single
            ${X11_LIBRARIES}
            ${X11_Xfixes_LIB}
            ${X11_XTest_LIB}
            ${X11_xcb_LIB}
            xcb-xinerama
            X11-xcb
            Xinerama
            )
endif ()

message(STATUS "PLATFORM_INC: " ${${PROJECT_NAME}_PLATFORM_INC})


include_directories(
        ./
        include
        src
        ../../thirdparties/plog/include
        ${PROJECT_NAME}_PLATFORM_INC
)


add_library(${EMBEDDED_WINDOWS_WINDOWS_ACTOR_LIB} STATIC ${EMBEDDED_WINDOWS_WINDOWS_ACTOR_LIB_SRC})

addPipeNameDefinition(${EMBEDDED_WINDOWS_WINDOWS_ACTOR_LIB})

target_link_libraries(
        ${EMBEDDED_WINDOWS_WINDOWS_ACTOR_LIB}
        PRIVATE
        ${DEPEND_THIRDPARTY_LIBS}
        PRIVATE
        ${${PROJECT_NAME}_PLATFORM_LIBS}
)

set_target_properties(${EMBEDDED_WINDOWS_WINDOWS_ACTOR_LIB}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        )
#

