#include <filesystem>

#if defined(_MSC_VER)
    #include "Unicode.hpp"
#endif

#include "Util.hpp"


std::uintmax_t file_size(std::string const & path)
{
#if defined(_MSC_VER)
    return std::filesystem::file_size(utf8_to_wide_char(path));
#else
    return std::filesystem::file_size(path);
#endif
}
