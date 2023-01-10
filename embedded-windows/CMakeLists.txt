cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD ${CPP_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EMBEDDED_WINDOWS_LIB smblob-node-embedded-windows)

project(${EMBEDDED_WINDOWS_LIB})
set(EMBEDDED_WINDOWS_SRC
        )
set(EMBEDDED_WINDOWS_HEADER
        include/SMBlobNodeEmbeddedWindows.h
        include/internal/SMBlobNodeEmbeddedWindowsCommon.h
        )

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
set(DEPEND_THIRDPARTY_LIBS)

if (USE_WX_WIDGETS)
    # use wxWidgets implementation
    # TODO implemement
    set(SMBLOB_NODE_EMBEDDED_WINDOWS_USE_WX_WIDGETS true CACHE BOOL "Build with wxWidgets")
else ()
    set(SMBLOB_NODE_EMBEDDED_WINDOWS_USE_QT true CACHE BOOL "Build with Qt")
    # default implementation is Qt
    # dependencies QtCore and QtGui
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    dump_cmake_variables()
    set(QT_INC_DIRS ${Qt${QT_VERSION_MAJOR}Widgets_INCLUDE_DIRS}
            ${Qt${QT_VERSION_MAJOR}Core_INCLUDE_DIRS})
    include_directories(
            ${QT_INC_DIRS}
    )
    message(STATUS "Qt Major:" ${QT_VERSION_MAJOR})
    message(STATUS "Qt Include dirs:" ${QT_INC_DIRS})
    message(STATUS "Qt Definitions:" ${Qt${QT_VERSION_MAJOR}Widgets_DEFINITIONS})

    # We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
    add_definitions(${Qt${QT_VERSION_MAJOR}Widgets_DEFINITIONS})

    list(APPEND DEPEND_THIRDPARTY_LIBS Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Core)

    list(APPEND
            EMBEDDED_WINDOWS_SRC
            src/qt/SMBlobNodeEmbeddedWindows_Qt.cpp
            src/qt/SMBlobAppPrivate.h
            src/qt/SMBlobAppPrivate.cpp
            )
endif ()

add_library(${EMBEDDED_WINDOWS_LIB} STATIC ${EMBEDDED_WINDOWS_SRC})
target_link_libraries(
        ${EMBEDDED_WINDOWS_LIB}
        ${DEPEND_THIRDPARTY_LIBS}
)

set_target_properties(${EMBEDDED_WINDOWS_LIB}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        )


add_subdirectory(example)
