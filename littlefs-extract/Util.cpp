#include "Util.hpp"

#include <cerrno>
#include <cstdint>
#include <string>
#include <system_error>

#if defined(_MSC_VER)
    #include <fcntl.h>
    #include <io.h>
#else
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <unistd.h>
#endif

#if defined(_MSC_VER)
    #include "Unicode.hpp"
#endif


#if defined(_MSC_VER)
    #define lseek _lseeki64
    #define close _close
#endif


std::uintmax_t file_size(std::string const & path)
{
#if defined(_MSC_VER)
    int fd = -1;
    auto const open_result = _wsopen_s(&fd,
                                       utf8_to_wide_char(path).c_str(),
                                       _O_RDONLY | _O_BINARY,
                                       _SH_DENYNO,
                                       _S_IREAD | _S_IWRITE);
    if (0 != open_result)
    {
        throw std::system_error(errno, std::system_category(), "_wsopen_s");
    }
#else
    auto const fd = open(path.c_str(), O_RDONLY);
    if (-1 == fd)
    {
        throw std::system_error(errno, std::system_category(), "open");
    }
#endif

    std::uintmax_t size = 0;

    try
    {
        auto const lseek_result = lseek(fd, 0, SEEK_END);
        if (-1 == lseek_result)
        {
            throw std::system_error(errno, std::system_category(), "lseek");
        }
        size = static_cast<std::uintmax_t>(lseek_result);
    }
    catch (...)
    {
        close(fd);
        throw;
    }
    close(fd);

    return size;
}
