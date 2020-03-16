#include "CFile.hpp"

#include <cerrno>
#include <cstdio>
#include <system_error>

#if defined(_MSC_VER)
    #include <io.h>
#else
    #include <unistd.h>
#endif

#include <gsl/gsl>

#if defined(_MSC_VER)
    #include "Unicode.hpp"
#endif


#if defined(_MSC_VER)
    #define fileno _fileno
    #define dup    _dup
    #define fdopen _fdopen
    #define close  _close
#endif


namespace {

gsl::owner<FILE *> duplicate_file_handle(gsl::not_null<std::FILE *> handle,
                                         gsl::not_null<gsl::czstring<>> mode)
{
    auto const original_fd = fileno(handle);

    auto const new_fd = dup(original_fd);
    if (-1 == new_fd)
    {
        throw std::system_error(errno, std::system_category(), "dup");
    }

    std::FILE * new_handle = nullptr;
    try
    {
        new_handle = fdopen(new_fd, mode);
        if (nullptr == new_handle)
        {
            throw std::system_error(errno, std::system_category(), "fdopen");
        }
    }
    catch (...)
    {
        close(new_fd);
        throw;
    }

    return new_handle;
}

gsl::owner<FILE *> open_file(std::string const & path, std::string const & mode)
{
    std::FILE * handle = nullptr;

#if defined(_MSC_VER)
    // The mode is not technically UTF-8, just plain ASCII :)
    auto const error =
        _wfopen_s(&handle, utf8_to_wide_char(path).c_str(), utf8_to_wide_char(mode).c_str());
    if (0 != error)
    {
        throw std::system_error(error, std::system_category(), "_wfopen_s");
    }
#else
    handle = std::fopen(path.c_str(), mode.c_str());
    if (nullptr == handle)
    {
        throw std::system_error(errno, std::system_category(), "std::fopen");
    }
#endif

    return handle;
}

}  // namespace


CFile::CFile(gsl::not_null<std::FILE *> handle) : _handle(handle)
{
}

CFile::CFile(std::string const & path, std::string const & mode) : _handle(open_file(path, mode))
{
}

CFile::~CFile()
{
    _destroy();
}

CFile::CFile(CFile && other) noexcept : _handle(other._release())
{
}

CFile & CFile::operator=(CFile && other) noexcept
{
    _destroy();
    _handle = other._release();

    return *this;
}

std::FILE * CFile::handle() const noexcept
{
    return _handle;
}

CFile CFile::standard_input()
{
    return CFile(gsl::not_null(duplicate_file_handle(gsl::not_null(stdin), gsl::not_null("r"))));
}

CFile CFile::standard_output()
{
    return CFile(gsl::not_null(duplicate_file_handle(gsl::not_null(stdout), gsl::not_null("w"))));
}

CFile CFile::standard_error()
{
    return CFile(gsl::not_null(duplicate_file_handle(gsl::not_null(stderr), gsl::not_null("w"))));
}

gsl::owner<std::FILE *> CFile::_release() noexcept
{
    gsl::owner<std::FILE *> const handle = _handle;
    _handle = nullptr;
    return handle;
}

void CFile::_destroy() noexcept
{
    if (nullptr != _handle)
    {
        std::fclose(_handle);
        _handle = nullptr;
    }
}
