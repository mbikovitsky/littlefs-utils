cmake_minimum_required(VERSION 3.15)
project(littlefs2)

add_library(littlefs2
        littlefs/lfs2.c littlefs/lfs2.h
        littlefs/lfs2_util.c littlefs/lfs2_util.h)
target_include_directories(littlefs2
        INTERFACE littlefs)
target_compile_features(littlefs2 PRIVATE c_std_99)
