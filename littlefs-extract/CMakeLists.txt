set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
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
        PRIVATE project_options project_warnings
                CONAN_PKG::docopt.cpp CONAN_PKG::fmt
                Boost::program_options ${LibArchive_LIBRARIES} littlefs1 littlefs2)

if (MSVC)
    target_compile_definitions(littlefs-extract
            PRIVATE -DUNICODE -D_UNICODE)
endif (MSVC)
