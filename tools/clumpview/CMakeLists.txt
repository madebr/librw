add_executable(clumpview
    camera.cpp
    camera.h
    font.cpp
    main.cpp
    tests.cpp
    )

target_link_libraries(clumpview
    PUBLIC
        librw
        librw_skeleton
    )

if(LIBRW_INSTALL)
    install(TARGETS clumpview
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        )
endif()
