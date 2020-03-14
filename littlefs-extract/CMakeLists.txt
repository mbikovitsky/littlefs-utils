add_executable(littlefs-extract
    main.cpp
    Util.cpp Util.hpp
    FileBlockDevice.cpp FileBlockDevice.hpp)
if (MSVC)
    target_sources(littlefs-extract
        PRIVATE Unicode.cpp Unicode.hpp)
endif (MSVC)

target_link_libraries(littlefs-extract
    PRIVATE project_options project_warnings
            CONAN_PKG::libarchive CONAN_PKG::boost CONAN_PKG::fmt
            littlefs)

if (MSVC)
    target_compile_definitions(littlefs-extract
        PRIVATE -DUNICODE -D_UNICODE)
endif (MSVC)
