# This script supports shared and static libglfw3 simultaneously.

if(NOT DEFINED glfw3_STATIC_INIT)
    set(glfw3_STATIC_INIT OFF)
endif()
option(glfw3_STATIC "By default, use static glfw3." ${glfw3_STATIC_INIT})

if(MSVC)
    set(_lib_prefix )
    set(_lib_static_suffix .lib)
    set(_lib_shared_suffix .dll.lib)
else()
    set(_lib_prefix lib)
    if(WIN32)
        set(_lib_static_suffix .a)
        set(_lib_shared_suffix .dll.a)
    else()
        set(_lib_static_suffix .a)
        if(APPLE)
            set(_lib_shared_suffix .dylib)
        else()
            set(_lib_shared_suffix .so)
        endif()
    endif()
endif()

find_path(GLFW3_INCLUDE_DIR glfw3.h PATH_SUFFIXES GLFW)
find_library(GLFW3_STATIC_LIBRARY ${_lib_prefix}glfw3${_lib_static_suffix})
find_library(GLFW3_SHARED_LIBRARY ${_lib_prefix}glfw3${_lib_shared_suffix})

set(_glfw3_required_variables )
if(glfw3_STATIC)
    list(APPEND _glfw3_required_variables GLFW3_STATIC_LIBRARY)
else()
    list(APPEND _glfw3_required_variables GLFW3_SHARED_LIBRARY)
endif()
list(APPEND _glfw3_required_variables GLFW3_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(glfw3
    REQUIRED_VARS ${_glfw3_required_variables}
)

if(GLFW3_INCLUDE_DIR AND GLFW3_STATIC_LIBRARY AND NOT TARGET glfw_static)
    add_library(glfw_static UNKNOWN IMPORTED)
    set_target_properties(glfw_static PROPERTIES
        IMPORTED_LOCATION "${GLFW3_STATIC_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GLFW3_INCLUDE_DIR}"
    )
endif()

if(GLFW3_INCLUDE_DIR AND GLFW3_SHARED_LIBRARY AND NOT TARGET glfw_shared)
    add_library(glfw_shared UNKNOWN IMPORTED)
    set_target_properties(glfw_shared PROPERTIES
        IMPORTED_LOCATION "${GLFW3_SHARED_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GLFW3_INCLUDE_DIR}"
    )
endif()

if(glfw3_FOUND AND NOT TARGET glfw)
    add_library(glfw INTERFACE IMPORTED)
    if(glfw3_STATIC)
        set_target_properties(glfw PROPERTIES
            INTERFACE_LINK_LIBRARIES "glfw_static"
        )
    else()
        set_target_properties(glfw PROPERTIES
            INTERFACE_LINK_LIBRARIES "glfw_shared"
        )
    endif()
endif()
