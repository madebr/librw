if(LIBRW_PLATFORM_GL3 OR LIBRW_PLATFORM_D3D9)
    add_subdirectory(clumpview)
endif()

if(LIBRW_PLATFORM_NULL)
    add_subdirectory(dumprwtree)
endif()

if(TARGET librw_skeleton_imgui)
    add_subdirectory(imguitest)
endif()

if(LIBRW_PLATFORM_PS2)
    add_subdirectory(ps2test)
endif()
