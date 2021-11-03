#include "LittleFile2.hpp"

#include <limits>
#include <stdexcept>
#include <system_error>

#include "LittleFSErrorCategory.hpp"


LittleFile2::LittleFile2(lfs2_t & filesystem, std::string const & path, int flags) :
    _filesystem(&filesystem),
    _file(),
    _open(false)
{
    auto const result = lfs2_file_open(_filesystem, &_file, path.c_str(), flags);
    if (result < 0)
    {
        throw std::system_error(result, littlefs_category(), "lfs2_file_open");
    }
    _open = true;
}

LittleFile2::~LittleFile2()
{
    if (_open)
    {
        lfs2_file_close(_filesystem, &_file);
        _open = false;
    }
}

std::size_t LittleFile2::read(gsl::span<std::byte> buffer)
{
    if (buffer.size() > std::numeric_limits<lfs2_size_t>::max())
    {
        throw std::length_error("Read buffer too large");
    }

    auto const read =
        lfs2_file_read(_filesystem, &_file, buffer.data(), static_cast<lfs2_size_t>(buffer.size()));
    if (read < 0)
    {
        throw std::system_error(read, littlefs_category(), "lfs2_file_read");
    }

    return static_cast<std::size_t>(read);
}

std::size_t LittleFile2::write(gsl::span<std::byte const> buffer)
{
    if (buffer.size() > std::numeric_limits<lfs2_size_t>::max())
    {
        throw std::length_error("Write buffer too large");
    }

    auto const read =
        lfs2_file_write(_filesystem, &_file, buffer.data(), static_cast<lfs2_size_t>(buffer.size()));
    if (read < 0)
    {
        throw std::system_error(read, littlefs_category(), "lfs2_file_write");
    }

    return static_cast<std::size_t>(read);
}

std::size_t LittleFile2::size() const
{
    auto const file_size = lfs2_file_size(_filesystem, &_file);
    if (file_size < 0)
    {
        throw std::system_error(file_size, littlefs_category(), "lfs2_file_size");
    }

    return static_cast<std::size_t>(file_size);
}

std::size_t LittleFile2::position() const
{
    auto const file_position = lfs2_file_tell(_filesystem, &_file);
    if (file_position < 0)
    {
        throw std::system_error(file_position, littlefs_category(), "lfs2_file_tell");
    }

    return static_cast<std::size_t>(file_position);
}
