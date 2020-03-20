add_executable(littlefs-extract
    main.cpp
    Util.cpp Util.hpp
    FileBlockDevice.cpp FileBlockDevice.hpp
    CFile.cpp CFile.hpp
    IInputStream.hpp
    OutputArchive.cpp OutputArchive.hpp
    LittleFileInputStream.hpp)
if (MSVC)
    target_sources(littlefs-extract
        PRIVATE Unicode.cpp Unicode.hpp)
endif (MSVC)

if(NOT LITTLEFS_EXTRACT_DEFAULT_VERSION)
    set(LITTLEFS_EXTRACT_DEFAULT_VERSION
        2
        CACHE STRING "Default littlefs version" FORCE)
    set_property(CACHE LITTLEFS_EXTRACT_DEFAULT_VERSION PROPERTY STRINGS "1" "2")
endif()
if(NOT LITTLEFS_EXTRACT_DEFAULT_BLOCK_SIZE)
    set(LITTLEFS_EXTRACT_DEFAULT_BLOCK_SIZE
        512
        CACHE STRING "Default littlefs block size" FORCE)
endif()
if(NOT LITTLEFS_EXTRACT_DEFAULT_READ_SIZE)
    set(LITTLEFS_EXTRACT_DEFAULT_READ_SIZE
        64
        CACHE STRING "Default littlefs read size" FORCE)
endif()
if(NOT LITTLEFS_EXTRACT_DEFAULT_PROG_SIZE)
    set(LITTLEFS_EXTRACT_DEFAULT_PROG_SIZE
        64
        CACHE STRING "Default littlefs prog size" FORCE)
endif()
configure_file(littlefs_extract_config.h.in littlefs_extract_config.h @ONLY)
target_include_directories(littlefs-extract PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(littlefs-extract
    PRIVATE project_options project_warnings
            CONAN_PKG::libarchive CONAN_PKG::boost CONAN_PKG::ms-gsl
            littlefs)

if (MSVC)
    target_compile_definitions(littlefs-extract
        PRIVATE -DUNICODE -D_UNICODE)
endif (MSVC)
