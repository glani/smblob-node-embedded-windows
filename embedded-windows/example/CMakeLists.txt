set(EXAMPLE_APP smblob-node-embedded-windows-example)
project(${EXAMPLE_APP})

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

    set(${PROJECT_NAME}_PLATFORM_LIBS
            ${CMAKE_THREAD_LIBS_INIT}
            ${corefoundation_lib}
            ${cocoa_lib}
            )
else ()
endif ()

if (WIN32)
    # Include a RC file for windows
    list(APPEND SRC_FILES ./example.rc)
elseif (APPLE)
    # Add an icon for the apple .app file
    list(APPEND SRC_FILES ../../src/osx/carbon/wxmac.icns)
endif ()


find_package(wxWidgets REQUIRED COMPONENTS net core base xml xrc)
include(${wxWidgets_USE_FILE})

# Resources
# One or more XRC files containing your resources.
set(xrc_files ${CMAKE_CURRENT_SOURCE_DIR}/resource.xrc)

# Generate this file somewhere under the build directory.
set(resource_cpp ${CMAKE_CURRENT_BINARY_DIR}/resource.cpp)

# Not needed with the installed version, just use "wxrc".
set(wxrc wxrc)
if (DEFINED wxWidgets_CONFIG_EXECUTABLE)
    message(STATUS "wxWidgets_CONFIG_EXECUTABLE:" ${wxWidgets_CONFIG_EXECUTABLE})
    set(wxWidgets_UTILS_PARENT)
    cmake_path(SET pathConfig ${wxWidgets_CONFIG_EXECUTABLE})
    cmake_path(GET pathConfig PARENT_PATH wxWidgets_CONFIG_DIR)
    cmake_path(APPEND wxWidgets_UTILS_WXRC ${wxWidgets_CONFIG_DIR} "wxrc")
    message(STATUS "wxrc:" ${wxWidgets_UTILS_WXRC})
    set(wxrc ${wxWidgets_UTILS_WXRC})
endif ()

add_custom_command(
        OUTPUT ${resource_cpp}
        COMMAND ${wxrc} -c -o ${resource_cpp} ${xrc_files}
        DEPENDS ${xrc_files}
        COMMENT "Compiling XRC resources"
)

# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries

include_directories(
        ${wxWidgets_INCLUDE_DIRS}
        ./
        include
        src
        ../../thirdparties/libuv/include
        ../../thirdparties/uvw/src
        ../../thirdparties/plog/include
)

set(EXAMPLE_APP_SRC)
if (WIN32)
    list(APPEND EXAMPLE_APP_SRC SMBlob_Node_Embedded_Windows_Console_Example_Win.cpp)
else ()
    list(APPEND EXAMPLE_APP_SRC SMBlob_Node_Embedded_Windows_Console_Example_Unix.cpp)
endif ()

add_executable(${PROJECT_NAME}
        SMBlob_Node_Embedded_Windows_Example.cpp)

add_executable(${PROJECT_NAME}-console
        SMBlob_Node_Embedded_Windows_Console_Example.cpp
        SMBlob_Node_Embedded_Windows_Console_Example.h
        ProcessRunner.cpp
        ProcessRunner.h
        ${EXAMPLE_APP_SRC}
        )

target_sources(${PROJECT_NAME} PRIVATE ${resource_cpp})

message(STATUS "wxWidgets_LIBRARIES:" ${wxWidgets_LIBRARIES})
message(STATUS "wxWidgets_INCLUDE_DIRS:" ${wxWidgets_INCLUDE_DIRS})
message(STATUS "wxWidgets_LIBRARY_DIRS:" ${wxWidgets_LIBRARY_DIRS})


set(DEPEND_THIRDPARTY_LIBS)
# wxWidgets is used for example application
# here we determine what we need to use as a core implementation
if (SMBLOB_NODE_EMBEDDED_WINDOWS_USE_WX_WIDGETS)
    message(STATUS "Core system is wxWidgets")

elseif (SMBLOB_NODE_EMBEDDED_WINDOWS_USE_QT)
    message(STATUS "Core system is Qt")
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

    list(APPEND DEPEND_THIRDPARTY_LIBS Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Core)

else ()
    message (FATAL_ERROR "Core system is missed" )
endif ()

link_directories(
        ${wxWidgets_LIBRARY_DIRS}

)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC DAEMON_PATH="${CMAKE_BINARY_DIR}/")
    target_compile_definitions(${PROJECT_NAME}-console PUBLIC DAEMON_PATH="${CMAKE_BINARY_DIR}/")
endif()

find_library(LIBUV_LIBRARY uv_a HINTS "../../thirdparties/libuv/build")

message(STATUS "LIBUV_LIBRARY: " ${LIBUV_LIBRARY})
message(STATUS "DEPEND_THIRDPARTY_LIBS: " ${DEPEND_THIRDPARTY_LIBS})

target_link_libraries(${PROJECT_NAME}
        ${wxWidgets_LIBRARIES}
        ${LIBUV_LIBRARY}
        ${${PROJECT_NAME}_PLATFORM_LIBS}
        ${DEPEND_THIRDPARTY_LIBS}
        smblob-node-embedded-windows-consumer
        )


target_link_libraries(${PROJECT_NAME}-console
        ${wxWidgets_LIBRARIES}
        ${LIBUV_LIBRARY}
        ${${PROJECT_NAME}_PLATFORM_LIBS}
        ${DEPEND_THIRDPARTY_LIBS}
        smblob-node-embedded-windows-shared
        smblob-node-embedded-windows-consumer
        )

set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
        )
