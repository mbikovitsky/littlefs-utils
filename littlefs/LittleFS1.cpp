#include "LittleFS1.hpp"

#include <cstddef>
#include <system_error>
#include <utility>

#include <gsl/gsl>

#include "LittleFSErrorCategory.hpp"
#include "LittleFile1.hpp"


LittleFS1::LittleFS1(std::unique_ptr<IBlockDevice> block_device,
                     lfs1_size_t const read_size,
                     lfs1_size_t const program_size,
                     lfs1_size_t const lookahead) :
    _block_device(std::move(block_device)),
    _config(),
    _filesystem(),
    _mounted(false)
{
    _config.context = _block_device.get();
    _config.read = &_read;
    _config.prog = &_prog;
    _config.erase = &_erase;
    _config.sync = &_sync;
    _config.read_size = read_size;
    _config.prog_size = program_size;
    _config.block_size = _block_device->block_size();
    _config.block_count = _block_device->block_count();
    _config.lookahead = lookahead;

    auto const result = lfs1_mount(&_filesystem, &_config);
    if (result < 0)
    {
        throw std::system_error(result, littlefs_category(), "lfs1_mount");
    }
    _mounted = true;
}

LittleFS1::~LittleFS1()
{
    if (_mounted)
    {
        lfs1_unmount(&_filesystem);
        _mounted = false;
    }
}

std::vector<LittleFS::DirectoryEntry> LittleFS1::list_directory(std::string const & path)
{
    std::vector<LittleFS::DirectoryEntry> output {};

    lfs1_dir_t directory {};
    auto result = lfs1_dir_open(&_filesystem, &directory, path.c_str());
    if (result < 0)
    {
        throw std::system_error(result, littlefs_category(), "lfs1_dir_open");
    }
    try
    {
        for (;;)
        {
            lfs1_info info {};
            result = lfs1_dir_read(&_filesystem, &directory, &info);
            if (result < 0)
            {
                throw std::system_error(result, littlefs_category(), "lfs1_dir_read");
            }
            if (0 == result)
            {
                break;
            }

            output.push_back(
                {gsl::span<char>(info.name).data(), info.type == LFS1_TYPE_DIR, info.size});
        }
    }
    catch (...)
    {
        lfs1_dir_close(&_filesystem, &directory);
        throw;
    }
    lfs1_dir_close(&_filesystem, &directory);

    return output;
}

std::unique_ptr<LittleFile> LittleFS1::open_file(std::string const & path,
                                                 LittleFS::OpenFlags flags)
{
    return std::make_unique<LittleFile1>(_filesystem, path, static_cast<int>(flags));
}

void LittleFS1::format(IBlockDevice & block_device,
                       lfs1_size_t const read_size,
                       lfs1_size_t const program_size,
                       lfs1_size_t const lookahead)
{
    lfs1_config config {};
    config.context = &block_device;
    config.read = &_read;
    config.prog = &_prog;
    config.erase = &_erase;
    config.sync = &_sync;
    config.read_size = read_size;
    config.prog_size = program_size;
    config.block_size = block_device.block_size();
    config.block_count = block_device.block_count();
    config.lookahead = lookahead;

    lfs1_t filesystem {};

    auto const result = lfs1_format(&filesystem, &config);
    if (result < 0)
    {
        throw std::system_error(result, littlefs_category(), "lfs1_format");
    }
}

int LittleFS1::_read(lfs1_config const * config,
                     lfs1_block_t block,
                     lfs1_off_t offset,
                     void * buffer,
                     lfs1_size_t size) noexcept
{
    auto * block_device = static_cast<IBlockDevice *>(config->context);

    try
    {
        block_device->read(block, offset, buffer, size);
    }
    catch (std::exception const &)
    {
        return LFS1_ERR_IO;
    }

    return LFS1_ERR_OK;
}

int LittleFS1::_prog(lfs1_config const * config,
                     lfs1_block_t block,
                     lfs1_off_t offset,
                     void const * buffer,
                     lfs1_size_t size) noexcept
{
    auto * block_device = static_cast<IBlockDevice *>(config->context);

    try
    {
        block_device->program(block, offset, buffer, size);
    }
    catch (std::exception const &)
    {
        return LFS1_ERR_IO;
    }

    return LFS1_ERR_OK;
}

int LittleFS1::_erase(lfs1_config const * config, lfs1_block_t block) noexcept
{
    auto * block_device = static_cast<IBlockDevice *>(config->context);

    try
    {
        block_device->erase(block);
    }
    catch (std::exception const &)
    {
        return LFS1_ERR_IO;
    }

    return LFS1_ERR_OK;
}

int LittleFS1::_sync(lfs1_config const * config) noexcept
{
    auto * block_device = static_cast<IBlockDevice *>(config->context);

    try
    {
        block_device->sync();
    }
    catch (std::exception const &)
    {
        return LFS1_ERR_IO;
    }

    return LFS1_ERR_OK;
}
