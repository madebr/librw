#set(CMAKE_LINK_SEARCH_START_STATIC ON)

if(CMAKE_C_COMPILER_ID MATCHES "^MSVC$")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

if(CMAKE_C_COMPILER_ID MATCHES "^GNU$")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
endif()

if(WIN32)
    list(REMOVE_ITEM CMAKE_FIND_LIBRARY_SUFFIXES .dll.a .dll.lib)
endif()
