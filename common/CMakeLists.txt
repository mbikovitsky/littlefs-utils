add_library(common
    Util.cpp Util.hpp
    FileBlockDevice.cpp FileBlockDevice.hpp
    CFile.cpp CFile.hpp
    IInputStream.hpp
    OutputArchive.cpp OutputArchive.hpp
    LittleFileInputStream.hpp)
if (MSVC)
    target_sources(common
        PRIVATE Unicode.cpp Unicode.hpp)
endif (MSVC)

target_include_directories(common
    INTERFACE .)

target_link_libraries(common
    PRIVATE project_options project_warnings
    PUBLIC  CONAN_PKG::libarchive CONAN_PKG::Microsoft.GSL littlefs)
