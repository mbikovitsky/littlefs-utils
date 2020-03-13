add_executable(littlefs-extract
        main.cpp
        IBlockDevice.hpp
        FileBlockDevice.cpp FileBlockDevice.hpp)
if(MSVC)
    target_sources(littlefs-extract
            PRIVATE Unicode.cpp Unicode.hpp)
endif(MSVC)

target_link_libraries(littlefs-extract
        PRIVATE project_options project_warnings
                CONAN_PKG::docopt.cpp CONAN_PKG::libarchive CONAN_PKG::boost
                littlefs1 littlefs2)

if (MSVC)
    target_compile_definitions(littlefs-extract
            PRIVATE -DUNICODE -D_UNICODE)
endif (MSVC)
