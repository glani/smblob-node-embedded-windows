project(smblob-node-embedded-windows-example)

if(WIN32)
    set(${PROJECT_NAME}_PLATFORM_LIBS Dwmapi)
    add_definitions(-DNOMINMAX)
    if(MINGW)
        list(APPEND ${PROJECT_NAME}_PLATFORM_LIBS Psapi)
    endif()

    MESSAGE(STATUS "Libs: ${${PROJECT_NAME}_PLATFORM_LIBS}")

elseif(APPLE)
    find_package(Threads REQUIRED)
    find_library(corefoundation_lib CoreFoundation REQUIRED)
    find_library(cocoa_lib Cocoa REQUIRED)

    set(${PROJECT_NAME}_PLATFORM_LIBS
            ${CMAKE_THREAD_LIBS_INIT}
            ${corefoundation_lib}
            ${cocoa_lib}
            )
else()
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
endif()

add_executable(${PROJECT_NAME}
        SMBlob_Node_Embedded_Windows_Example.cpp
        )

target_link_libraries(${PROJECT_NAME}

        ${${PROJECT_NAME}_PLATFORM_LIBS}
)