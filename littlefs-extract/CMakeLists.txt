cmake_minimum_required(VERSION 3.15)
project(littlefs-extract)

find_package(Boost
        1.72
        REQUIRED
        COMPONENTS program_options)

find_package(libarchive
        3.4.1
        REQUIRED)

add_executable(littlefs-extract
        main.cpp
        IBlockDevice.hpp
        FileBlockDevice.cpp FileBlockDevice.hpp)
target_include_directories(littlefs-extract
        PRIVATE ${LibArchive_INCLUDE_DIRS})
target_link_libraries(littlefs-extract
        PRIVATE Boost::program_options ${LibArchive_LIBRARIES} littlefs1 littlefs2)
target_compile_features(littlefs-extract PRIVATE cxx_std_17)

if (MSVC)
    target_compile_definitions(littlefs-extract
            PRIVATE -DUNICODE -D_UNICODE)
endif (MSVC)
