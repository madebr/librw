function(librw_platform_target TARGET)
    cmake_parse_arguments(LPT "PROVIDES_WINMAIN" "" "" ${ARGN})
    get_target_property(TARGET_TYPE "${TARGET}" TYPE)
    if(MINGW AND LPT_PROVIDES_WINMAIN AND TARGET_TYPE STREQUAL "STATIC_LIBRARY")
        # Start with WinMain marked as undefined.
        # This makes the linker keep a WinMain symbol from a static library.
        if(CMAKE_SIZEOF_VOID_P EQUAL 4)
            target_link_options(${TARGET} INTERFACE -Wl,--undefined,WinMain@16)
        else()
            target_link_options(${TARGET} INTERFACE -Wl,--undefined,WinMain)
        endif()
    endif()
endfunction()
