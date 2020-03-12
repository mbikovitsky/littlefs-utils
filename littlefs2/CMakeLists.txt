add_library(littlefs2
        littlefs/lfs2.c littlefs/lfs2.h
        littlefs/lfs2_util.c littlefs/lfs2_util.h)
target_include_directories(littlefs2
        INTERFACE littlefs)
target_link_libraries(littlefs2
        PRIVATE project_options project_warnings)
