add_library(littlefs
    IBlockDevice.hpp
    LittleFS.cpp LittleFS.hpp
    LittleFile.cpp LittleFile.hpp
    LittleFSErrorCategory.cpp LittleFSErrorCategory.hpp
    LittleFS1.cpp LittleFS1.hpp LittleFile1.cpp LittleFile1.hpp)

target_include_directories(littlefs
    INTERFACE .)

target_link_libraries(littlefs
    PRIVATE project_options project_warnings
            CONAN_PKG::fmt CONAN_PKG::ms-gsl
    PUBLIC  littlefs1 littlefs2)
