#include "LittleFS2.hpp"

#include <cstddef>
#include <system_error>
#include <utility>

#include <gsl/gsl>

#include "LittleFSErrorCategory.hpp"
#include "LittleFile2.hpp"


LittleFS2::LittleFS2(std::unique_ptr<IBlockDevice> block_device,
                     lfs2_size_t const read_size,
                     lfs2_size_t const program_size,
                     std::int32_t const block_cycles,
                     lfs2_size_t const cache_size,
                     lfs2_size_t const lookahead_size,
                     lfs2_size_t const name_max,
                     lfs2_size_t const file_max,
                     lfs2_size_t const attr_max) :
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
    _config.block_cycles = block_cycles;
    _config.cache_size = ((0 == cache_size) ? _config.block_size : cache_size);
    _config.lookahead_size = lookahead_size;
    _config.name_max = name_max;
    _config.file_max = file_max;
    _config.attr_max = attr_max;

    auto const result = lfs2_mount(&_filesystem, &_config);
    if (result < 0)
    {
        throw std::system_error(result, littlefs_category(), "lfs2_mount");
    }
    _mounted = true;
}

LittleFS2::~LittleFS2()
{
    if (_mounted)
    {
        lfs2_unmount(&_filesystem);
        _mounted = false;
    }
}

std::vector<LittleFS::DirectoryEntry> LittleFS2::list_directory(std::string const & path)
{
    std::vector<LittleFS::DirectoryEntry> output {};

    lfs2_dir_t directory {};
    auto result = lfs2_dir_open(&_filesystem, &directory, path.c_str());
    if (result < 0)
    {
        throw std::system_error(result, littlefs_category(), "lfs2_dir_open");
    }
    try
    {
        for (;;)
        {
            lfs2_info info {};
            result = lfs2_dir_read(&_filesystem, &directory, &info);
            if (result < 0)
            {
                throw std::system_error(result, littlefs_category(), "lfs2_dir_read");
            }
            if (0 == result)
            {
                break;
            }

            output.push_back(
                {gsl::span<char>(info.name).data(), info.type == LFS2_TYPE_DIR, info.size});
        }
    }
    catch (...)
    {
        lfs2_dir_close(&_filesystem, &directory);
        throw;
    }
    lfs2_dir_close(&_filesystem, &directory);

    return output;
}

std::unique_ptr<LittleFile> LittleFS2::open_file(std::string const & path,
                                                 LittleFS::OpenFlags flags)
{
    return std::make_unique<LittleFile2>(_filesystem, path, static_cast<int>(flags));
}

void LittleFS2::format(IBlockDevice & block_device,
                       lfs2_size_t const read_size,
                       lfs2_size_t const program_size,
                       std::int32_t const block_cycles,
                       lfs2_size_t const cache_size,
                       lfs2_size_t const lookahead_size,
                       lfs2_size_t const name_max,
                       lfs2_size_t const file_max,
                       lfs2_size_t const attr_max)
{
    lfs2_config config {};
    config.context = &block_device;
    config.read = &_read;
    config.prog = &_prog;
    config.erase = &_erase;
    config.sync = &_sync;
    config.read_size = read_size;
    config.prog_size = program_size;
    config.block_size = block_device.block_size();
    config.block_count = block_device.block_count();
    config.block_cycles = block_cycles;
    config.cache_size = ((0 == cache_size) ? config.block_size : cache_size);
    config.lookahead_size = lookahead_size;
    config.name_max = name_max;
    config.file_max = file_max;
    config.attr_max = attr_max;

    lfs2_t filesystem {};

    auto const result = lfs2_format(&filesystem, &config);
    if (result < 0)
    {
        throw std::system_error(result, littlefs_category(), "lfs2_format");
    }
}

int LittleFS2::_read(lfs2_config const * config,
                     lfs2_block_t block,
                     lfs2_off_t offset,
                     void * buffer,
                     lfs2_size_t size) noexcept
{
    auto * block_device = static_cast<IBlockDevice *>(config->context);

    try
    {
        block_device->read(block, offset, buffer, size);
    }
    catch (std::exception const &)
    {
        return LFS2_ERR_IO;
    }

    return LFS2_ERR_OK;
}

int LittleFS2::_prog(lfs2_config const * config,
                     lfs2_block_t block,
                     lfs2_off_t offset,
                     void const * buffer,
                     lfs2_size_t size) noexcept
{
    auto * block_device = static_cast<IBlockDevice *>(config->context);

    try
    {
        block_device->program(block, offset, buffer, size);
    }
    catch (std::exception const &)
    {
        return LFS2_ERR_IO;
    }

    return LFS2_ERR_OK;
}

int LittleFS2::_erase(lfs2_config const * config, lfs2_block_t block) noexcept
{
    auto * block_device = static_cast<IBlockDevice *>(config->context);

    try
    {
        block_device->erase(block);
    }
    catch (std::exception const &)
    {
        return LFS2_ERR_IO;
    }

    return LFS2_ERR_OK;
}

int LittleFS2::_sync(lfs2_config const * config) noexcept
{
    auto * block_device = static_cast<IBlockDevice *>(config->context);

    try
    {
        block_device->sync();
    }
    catch (std::exception const &)
    {
        return LFS2_ERR_IO;
    }

    return LFS2_ERR_OK;
}
