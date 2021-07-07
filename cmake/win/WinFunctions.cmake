set(LIBRW_STATIC_RUNTIME_DEFAULT ON)

if(MINGW)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/mingw")
endif()

function(librw_platform_target TARGET)
    cmake_parse_arguments(LPT "PROVIDES_WINMAIN" "" "" ${ARGN})
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
