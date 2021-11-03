add_executable(littlefs-format
    main.cpp)

if(NOT LITTLEFS_FORMAT_DEFAULT_VERSION)
    set(LITTLEFS_FORMAT_DEFAULT_VERSION
        2
        CACHE STRING "Default littlefs version" FORCE)
    set_property(CACHE LITTLEFS_FORMAT_DEFAULT_VERSION PROPERTY STRINGS "1" "2")
endif()
if(NOT LITTLEFS_FORMAT_DEFAULT_BLOCK_SIZE)
    set(LITTLEFS_FORMAT_DEFAULT_BLOCK_SIZE
        512
        CACHE STRING "Default littlefs block size" FORCE)
endif()
if(NOT LITTLEFS_FORMAT_DEFAULT_READ_SIZE)
    set(LITTLEFS_FORMAT_DEFAULT_READ_SIZE
        64
        CACHE STRING "Default littlefs read size" FORCE)
endif()
if(NOT LITTLEFS_FORMAT_DEFAULT_PROG_SIZE)
    set(LITTLEFS_FORMAT_DEFAULT_PROG_SIZE
        64
        CACHE STRING "Default littlefs prog size" FORCE)
endif()
configure_file(littlefs_format_config.h.in littlefs_format_config.h @ONLY)
target_include_directories(littlefs-format PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(littlefs-format
    PRIVATE project_options project_warnings
            CONAN_PKG::boost CONAN_PKG::Microsoft.GSL CONAN_PKG::fmt
            common)
