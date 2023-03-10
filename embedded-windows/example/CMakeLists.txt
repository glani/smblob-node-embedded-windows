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
    #    find_package(X11 REQUIRED)
    #    if(!X11_XTest_FOUND)
    #        message(FATAL_ERROR "X11 extensions are required, but not found!")
    #    endif()
    #    if(!X11_Xfixes_LIB)
    #        message(FATAL_ERROR "X11 fixes extension is required, but not found!")
    #    endif()
    #    message(STATUS "X11_xcb_LIB:" ${X11_xcb_LIB})
    #    message(STATUS "X11_Xfixes_LIB:" ${X11_Xfixes_LIB})
    #    message(STATUS "X11_Xinerama_LIB:" ${X11_Xinerama_LIB})
    #    find_package(Threads REQUIRED)
    #    #	message(FATAL_ERROR "X11 fixes extension is required, but not found!" ${X11_xcb_LIB})
    #    set(${PROJECT_NAME}_PLATFORM_LIBS
    #            ${X11_LIBRARIES}
    #            ${X11_Xfixes_LIB}
    #            ${X11_XTest_LIB}
    #            ${X11_xcb_LIB}
    #            xcb-xinerama
    #            X11-xcb
    #            Xinerama
    #            ${CMAKE_THREAD_LIBS_INIT}
    #            )
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
include_directories(${wxWidgets_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}
        SMBlob_Node_Embedded_Windows_Example.cpp
        )

add_executable(${PROJECT_NAME}-console
        SMBlob_Node_Embedded_Windows_Console_Example.cpp
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
message(STATUS "DEPEND_THIRDPARTY_LIBS: " ${DEPEND_THIRDPARTY_LIBS})

target_link_libraries(${PROJECT_NAME}
        ${wxWidgets_LIBRARIES}
        ${${PROJECT_NAME}_PLATFORM_LIBS}
        ${DEPEND_THIRDPARTY_LIBS}
        smblob-node-embedded-windows
        )

target_link_libraries(${PROJECT_NAME}-console
        ${wxWidgets_LIBRARIES}
        ${${PROJECT_NAME}_PLATFORM_LIBS}
        ${DEPEND_THIRDPARTY_LIBS}
        smblob-node-embedded-windows
        )

set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
        )
