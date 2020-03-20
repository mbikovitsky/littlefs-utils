#include "Util.hpp"

#include <cerrno>
#include <cstdint>
#include <string>
#include <system_error>

#if __APPLE__
    #include <sys/stat.h>
#else
    #include <filesystem>
#endif

#if defined(_MSC_VER)
    #include "Unicode.hpp"
#endif


std::uintmax_t file_size(std::string const & path)
{
#if defined(_MSC_VER)
    return std::filesystem::file_size(utf8_to_wide_char(path));
#elif __APPLE__
    struct stat stat_buffer = {};
    auto const result = stat(path.c_str(), &stat_buffer);
    if (-1 == result)
    {
        throw std::system_error(errno, std::system_category(), "stat");
    }
    return static_cast<uintmax_t>(stat_buffer.st_size);
#else
    return std::filesystem::file_size(path);
#endif
}
