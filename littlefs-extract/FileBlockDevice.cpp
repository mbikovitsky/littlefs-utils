#include <vector>
#include <stdexcept>
#include <cstddef>

#if defined(_MSC_VER)
#include "Unicode.hpp"
#endif

#include "FileBlockDevice.hpp"


namespace {

std::fstream open_file(std::string const & path, bool const writable)
{
    std::fstream file_stream {};
    file_stream.exceptions(std::ios_base::badbit | std::ios_base::failbit);

    auto mode = std::ios_base::binary | std::ios_base::in;
    if (writable)
    {
        mode |= std::ios_base::out;
    }

#if defined(_MSC_VER)
    file_stream.open(utf8_to_wide_char(path), mode);
#else
    file_stream.open(path, mode);
#endif

    return file_stream;
}

}


FileBlockDevice::FileBlockDevice(std::string const & path,
                                 bool const writable,
                                 std::uint32_t const block_size,
                                 std::uint32_t const block_count) :
    _filestream(open_file(path, writable)),
    _block_size(block_size),
    _block_count(block_count)
{
}

void FileBlockDevice::read(std::uint32_t block, std::uint32_t offset, void * buffer, std::uint32_t size)
{
    if (block >= _block_count)
    {
        throw std::range_error("Invalid block number");
    }

    if (offset + size >= _block_size)
    {
        throw std::range_error("Invalid read range");
    }

    auto const file_position =
        static_cast<std::size_t>(block) * static_cast<std::size_t>(_block_size) + static_cast<std::size_t>(offset);
    _filestream.seekg(static_cast<std::ifstream::off_type>(file_position), std::ios_base::beg);

    _filestream.read(static_cast<char *>(buffer), size);
}

void FileBlockDevice::program(std::uint32_t block, std::uint32_t offset, void const * buffer, std::uint32_t size)
{
    if (block >= _block_count)
    {
        throw std::range_error("Invalid block number");
    }

    if (offset + size >= _block_size)
    {
        throw std::range_error("Invalid write range");
    }

    auto const file_position =
        static_cast<std::size_t>(block) * static_cast<std::size_t>(_block_size) + static_cast<std::size_t>(offset);
    _filestream.seekp(static_cast<std::ifstream::off_type>(file_position), std::ios_base::beg);

    _filestream.write(static_cast<char const *>(buffer), size);
}

void FileBlockDevice::erase(std::uint32_t block)
{
    std::vector<std::byte> buffer(_block_size);
    program(block, 0, buffer.data(), _block_size);
}

void FileBlockDevice::sync()
{
    _filestream.flush();
}
