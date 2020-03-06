cmake_minimum_required(VERSION 3.15)
project(littlefs)

set(CMAKE_C_STANDARD 99)

add_library(littlefs
        littlefs/lfs.c littlefs/lfs.h
        littlefs/lfs_util.c littlefs/lfs_util.h)
target_include_directories(littlefs
        INTERFACE littlefs)
