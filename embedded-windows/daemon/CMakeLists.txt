set(DAEMON_APP smblob-node-embedded-windows-daemon)
project(${DAEMON_APP} VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

findLibUV()

include_directories(
        ./
        include
        src
        ../../thirdparties/uvw/src
        ../../thirdparties/plog/include
        ${LIBUV_INCLUDE_PATH}
)


set(PROJECT_SOURCES
        ProcessorPrivate.cpp
        ProcessorPrivate.h
        BaseProcessor.h
        BaseProcessor.cpp
        )

if (WIN32)
    set(${PROJECT_NAME}_PLATFORM_LIBS Dwmapi)
    add_definitions(-DNOMINMAX)
    if (MINGW)
        list(APPEND ${PROJECT_NAME}_PLATFORM_LIBS Psapi)
    endif ()

    MESSAGE(STATUS "Libs: ${${PROJECT_NAME}_PLATFORM_LIBS}")

elseif (APPLE)
    find_package(Threads REQUIRED)
    find_library(corefoundation_lib CoreFoundation REQUIRED)
    find_library(cocoa_lib Cocoa REQUIRED)

    # todo fix
    link_directories(
            "/usr/local/Cellar/pcre2/10.42/lib/"
    )

    set(${PROJECT_NAME}_PLATFORM_LIBS
            ${CMAKE_THREAD_LIBS_INIT}
            ${corefoundation_lib}
            ${cocoa_lib}
            )
elseif (LINUX)
    set(${PROJECT_NAME}_PLATFORM_LIBS
            xdotool-single
            )
    include_directories(
            ../../thirdparties/xdotool-wrapper/src
    )
endif ()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

list(APPEND PROJECT_SOURCES
        qt/application.cpp
        qt/application.h
        qt/main.cpp
        qt/mainwindow.cpp
        qt/embedded-window.cpp
        qt/embedded-window.h
        qt/mainwindow.h
        qt/mainwindow.ui
        qt/shared.h
        )

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
            )
else ()
    add_executable(${PROJECT_NAME}
            ${PROJECT_SOURCES}
            )
endif ()


addPipeNameDefinition(${PROJECT_NAME})

message(STATUS "Qt${QT_VERSION_MAJOR} Widgets:" Qt${QT_VERSION_MAJOR}::Widgets)


target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        ${${PROJECT_NAME}_PLATFORM_LIBS}
        ${LIBUV_LIBRARY}
        smblob-node-embedded-windows-shared
        )

install(TARGETS ${PROJECT_NAME}
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()