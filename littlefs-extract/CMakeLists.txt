add_executable(littlefs-extract
        main.cpp
        IBlockDevice.hpp
        FileBlockDevice.cpp FileBlockDevice.hpp)
target_link_libraries(littlefs-extract
        PRIVATE project_options project_warnings
                CONAN_PKG::docopt.cpp CONAN_PKG::fmt CONAN_PKG::libarchive
                littlefs1 littlefs2)

if (MSVC)
    target_compile_definitions(littlefs-extract
            PRIVATE -DUNICODE -D_UNICODE)
endif (MSVC)
