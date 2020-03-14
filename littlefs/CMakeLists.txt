add_library(littlefs
    IBlockDevice.hpp
    LittleFS.hpp
    LittleFSErrorCategory.cpp LittleFSErrorCategory.hpp
    LittleFS1.cpp LittleFS1.hpp)

target_include_directories(littlefs
    INTERFACE .)

target_link_libraries(littlefs
    PRIVATE project_options project_warnings
            CONAN_PKG::fmt
    PUBLIC  littlefs1 littlefs2)
