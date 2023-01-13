cmake_minimum_required(VERSION 3.1)

if (USE_WX_WIDGETS)
    # use wxWidgets implementation
    set(SMBLOB_NODE_EMBEDDED_WINDOWS_USE_WX_WIDGETS true CACHE BOOL "Build with wxWidgets")
else ()
    set(SMBLOB_NODE_EMBEDDED_WINDOWS_USE_QT true CACHE BOOL "Build with Qt")
endif ()


include_directories(
        ./shared/include
        ./consumer/include
)

add_subdirectory(shared)
add_subdirectory(daemon)
add_subdirectory(consumer)
add_subdirectory(example)

#find_package(QT NAMES Qt6 Qt5 OPTIONAL_COMPONENTS Widgets )
#if (QT_FOUND)
#    add_subdirectory(example_qt)
#endif()
