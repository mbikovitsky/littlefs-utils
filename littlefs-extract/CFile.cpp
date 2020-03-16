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
    #define fileno _fileno
    #define dup    _dup
    #define fdopen _fdopen
    #define close  _close
#endif


namespace {

FILE * duplicate_file_handle(gsl::not_null<std::FILE *> handle, gsl::not_null<gsl::czstring<>> mode)
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

}  // namespace


CFile::CFile(gsl::not_null<std::FILE *> handle) : _handle(handle)
{
}

CFile::~CFile()
{
    std::fclose(_handle);
    _handle = nullptr;
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
