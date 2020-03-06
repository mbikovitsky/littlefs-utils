#include <vector>
#include <filesystem>
#include <stdexcept>
#include <cstddef>

#include "FileBlockDevice.hpp"


FileBlockDevice::FileBlockDevice(std::wstring const & path,
                                 bool writable,
                                 std::uint32_t block_size,
                                 std::uint32_t block_count) :
    _filestream(_open_file(path, writable)),
    _block_size(block_size),
    _block_count(block_count)
{
    // For non-writable files, the block size and count should cover the whole file.
    if (!writable)
    {
        _filestream.seekg(0, std::ios_base::end);
        auto const size = _filestream.tellg();
        if (0 != size % block_size || block_count != size / block_size)
        {
            throw std::runtime_error("Invalid block size");
        }
    }
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

    auto const file_position = (std::size_t)block * (std::size_t)_block_size + (std::size_t)offset;
    _filestream.seekg(file_position, std::ios_base::beg);

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

    auto const file_position = (std::size_t)block * (std::size_t)_block_size + (std::size_t)offset;
    _filestream.seekp(file_position, std::ios_base::beg);

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

std::fstream FileBlockDevice::_open_file(std::wstring const & path, bool writable)
{
    std::fstream file_stream {};
    file_stream.exceptions(std::ios_base::badbit | std::ios_base::failbit);

    auto mode = std::ios_base::binary | std::ios_base::in;
    if (writable)
    {
        mode |= std::ios_base::out;
    }

    file_stream.open(std::filesystem::path(path), mode);

    return file_stream;
}
