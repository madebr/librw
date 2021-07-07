if(LIBRW_STATIC_RUNTIME)
    set(_glfw3lib_names libglfw3.a)
else()
    set(_glfw3lib_names libglfw3.dll.a)
endif()

find_path(GLFW3_INCLUDE_DIR glfw3.h PATH_SUFFIXES GLFW)
find_library(GLFW3_LIBRARY ${_glfw3lib_names})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(glfw3
    REQUIRED_VARS GLFW3_LIBRARY GLFW3_INCLUDE_DIR
)

if(LIBRW_STATIC_RUNTIME)
    if(WIN32)
        set(GLFW3_SUPPORT_LIBRARIES )#imm32 setupapi version winmm)
    else()
        set(GLFW3_SUPPORT_LIBRARIES )
    endif()
else()
    set(GLFW3_SUPPORT_LIBRARIES )
endif()

if(glfw3_FOUND AND NOT TARGET glfw)
    add_library(glfw UNKNOWN IMPORTED)
    set_target_properties(glfw PROPERTIES
        IMPORTED_LOCATION "${GLFW3_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GLFW3_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "${GLFW3_SUPPORT_LIBRARIES}"
    )
endif()
