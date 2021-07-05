function(librw_platform_target TARGET)
    cmake_parse_arguments(LPT "PROVIDES_WINMAIN" "" "" ${ARGN})
    get_target_property(TARGET_TYPE "${TARGET}" TYPE)
    if(MINGW AND LPT_PROVIDES_WINMAIN AND TARGET_TYPE STREQUAL "STATIC_LIBRARY")
        # When consumers link to this target, make the linker require WinMain to be defined.
        # This causes WinMain to not be dropped.
        target_link_options(${TARGET} INTERFACE -Wl,--require-defined=WinMain)
    endif()
endfunction()
