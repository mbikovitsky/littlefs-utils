add_library(littlefs
    IBlockDevice.hpp
    LittleFS.cpp LittleFS.hpp
    LittleFile.hpp
    LittleFSErrorCategory.cpp LittleFSErrorCategory.hpp
    LittleFS1.cpp LittleFS1.hpp LittleFile1.cpp LittleFile1.hpp
    LittleFS2.cpp LittleFS2.hpp LittleFile2.cpp LittleFile2.hpp)

target_include_directories(littlefs
    INTERFACE .)

target_link_libraries(littlefs
    PRIVATE project_options project_warnings
            CONAN_PKG::fmt CONAN_PKG::ms-gsl
    PUBLIC  littlefs1 littlefs2)
