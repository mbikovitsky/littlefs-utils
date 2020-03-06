cmake_minimum_required(VERSION 3.15)
project(littlefs-extract)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost
        1.72
        REQUIRED
        COMPONENTS program_options)

find_package(libarchive
        3.4.1
        REQUIRED)

add_executable(littlefs-extract
        main.cpp)
target_include_directories(littlefs-extract
        PRIVATE ${LibArchive_INCLUDE_DIRS})
target_link_libraries(littlefs-extract
        PRIVATE Boost::program_options ${LibArchive_LIBRARIES} littlefs1 littlefs2)

if (MSVC)
    target_compile_definitions(littlefs-extract
            PRIVATE -DUNICODE -D_UNICODE)
endif (MSVC)
