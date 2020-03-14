add_library(littlefs1
    littlefs/lfs1.c littlefs/lfs1.h
    littlefs/lfs1_util.c littlefs/lfs1_util.h)
target_include_directories(littlefs1
    INTERFACE littlefs)
target_link_libraries(littlefs1
    PRIVATE project_options)

if (MSVC)
    target_compile_definitions(littlefs1
        PRIVATE -Drestrict=__restrict)
endif (MSVC)
