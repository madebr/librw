set(SDL2_STATIC_INIT ON)
set(glfw3_STATIC_INIT ON)

function(librw_platform_target TARGET)
    cmake_parse_arguments(LPT "PROVIDES_WINMAIN" "" "" ${ARGN})
    set_target_properties(${TARGET} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${TARGET}>"
    )

    get_target_property(TARGET_TYPE "${TARGET}" TYPE)

    if(MINGW)
        if(LPT_PROVIDES_WINMAIN AND TARGET_TYPE STREQUAL "STATIC_LIBRARY")
            # Start with WinMain marked as undefined.
            # This will prevent the linker ignoring a WinMain symbol from a static library.
            if(CMAKE_SIZEOF_VOID_P EQUAL 4)
                target_link_options(${TARGET} INTERFACE -Wl,--undefined,_WinMain@16)
            endif()
            target_link_options(${TARGET} INTERFACE -Wl,--undefined,WinMain)
        endif()
        if(TARGET_TYPE STREQUAL "EXECUTABLE")
            if(LIBRW_STATIC_LIBGCC)
            endif()
        endif()
    endif()
endfunction()
