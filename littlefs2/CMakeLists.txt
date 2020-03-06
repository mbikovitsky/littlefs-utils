cmake_minimum_required(VERSION 3.15)
project(littlefs)

set(CMAKE_C_STANDARD 99)

add_library(littlefs
        littlefs/lfs2.c littlefs/lfs2.h
        littlefs/lfs2_util.c littlefs/lfs2_util.h)
target_include_directories(littlefs
        INTERFACE littlefs)
