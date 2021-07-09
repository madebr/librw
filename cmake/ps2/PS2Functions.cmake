if(NOT COMMAND add_erl_executable)
    message(FATAL_ERROR "The `add_erl_executable` cmake command is not available. Please use an appropriate Playstation 2 toolchain.")
endif()

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -flto")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
#set(CMAKE_DSM_FLAGS "${CMAKE_DSM_FLAGS} -flto")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")

function(librw_platform_target TARGET)
    cmake_parse_arguments(LPT "INSTALL" "" "" ${ARGN})

    get_target_property(TARGET_TYPE "${TARGET}" TYPE)
    if(TARGET_TYPE STREQUAL "EXECUTABLE")
        add_erl_executable(${TARGET} OUTPUT_VAR ERL_FILE)

        if(LIBRW_INSTALL AND LPT_INSTALL)
            install(FILES "${ERL_FILE}"
                DESTINATION "${CMAKE_INSTALL_BINDIR}"
            )
        endif()
    endif()
endfunction()
