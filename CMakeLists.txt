cmake_minimum_required(VERSION 3.8)

project(librw C CXX)

if(PS2DEV AND EE)
    include("${CMAKE_CURRENT_LIST_DIR}/cmake/ee.cmake")
endif()

set(LIBRW_PLATFORMS_AVAILABLE "NULL")
if(PS2DEV)
    list(APPEND LIBRW_PLATFORMS_AVAILABLE "PS2")
else()
    list(APPEND LIBRW_PLATFORMS_AVAILABLE "GL3")
endif()
if(WIN32)
    list(APPEND LIBRW_PLATFORMS_AVAILABLE "D3D9")
endif()
set(LIBRW_PLATFORMS "NULL" CACHE STRING "Platforms that librw will support")
if(NOT LIBRW_PLATFORMS)
    message(FATAL_ERROR "librw must support at least one platform (choices=${LIBRW_PLATFORMS_AVAILABLE})")
endif()
foreach(LIBRW_PLATFORM ${LIBRW_PLATFORMS})
    if(NOT LIBRW_PLATFORM IN_LIST LIBRW_PLATFORMS_AVAILABLE)
        message(FATAL_ERROR "Illegal LIBRW_PLATFORM=${LIBRW_PLATFORM}")
    endif()
    set("LIBRW_PLATFORM_${LIBRW_PLATFORM}" ON)
endforeach()
message(STATUS "librw will support these platforms: ${LIBRW_PLATFORMS}")

if(LIBRW_PLATFORM_GL3)
    set(OpenGL_GL_PREFERENCE GLVND)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(glfw3 REQUIRED)
endif()

option(LIBRW_TOOLS "Build librw tools" OFF)
option(LIBRW_INSTALL "Install librw files" OFF)

if(LIBRW_INSTALL)
    include(GNUInstallDirs)
    set(LIBRW_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}/librw")
endif()

add_subdirectory(src)

if(LIBRW_TOOLS)
    if(LIBRW_PLATFORM_PS2)
        enable_language(DSM)
    endif()

    add_subdirectory(skeleton)
    add_subdirectory(tools)
endif()

if(LIBRW_INSTALL)
    include(CMakePackageConfigHelpers)
    configure_package_config_file(librw-config.cmake.in librw-config.cmake
        INSTALL_DESTINATION "${CMAKE_INSTALL_PREFIX}"
        )
    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/librw-config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
        )
    install(
        EXPORT librw-targets
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}"
        )

    include(CMakeCPack.cmake)
endif()
