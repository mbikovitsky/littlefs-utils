#include "LittleFS1.hpp"

#include <system_error>

#include <gsl/span>

#include "LittleFSErrorCategory.hpp"


LittleFS1::LittleFS1(std::unique_ptr<IBlockDevice> block_device,
                     lfs1_size_t read_size,
                     lfs1_size_t program_size,
                     lfs1_size_t lookahead) :
    _block_device(std::move(block_device)),
    _config(),
    _filesystem(),
    _mounted(false)
{
    _config.context = this;
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
        throw std::system_error(result, littlefs_category(), "mount");
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
        throw std::system_error(result, littlefs_category(), "dir_open");
    }
    try
    {
        for (;;)
        {
            lfs1_info info {};
            result = lfs1_dir_read(&_filesystem, &directory, &info);
            if (result < 0)
            {
                throw std::system_error(result, littlefs_category(), "dir_read");
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

int LittleFS1::_read(lfs1_config const * config,
                     lfs1_block_t block,
                     lfs1_off_t offset,
                     void * buffer,
                     lfs1_size_t size) noexcept
{
    auto * instance = reinterpret_cast<LittleFS1 *>(config->context);

    try
    {
        instance->_block_device->read(block, offset, buffer, size);
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
    auto * instance = reinterpret_cast<LittleFS1 *>(config->context);

    try
    {
        instance->_block_device->program(block, offset, buffer, size);
    }
    catch (std::exception const &)
    {
        return LFS1_ERR_IO;
    }

    return LFS1_ERR_OK;
}

int LittleFS1::_erase(lfs1_config const * config, lfs1_block_t block) noexcept
{
    auto * instance = reinterpret_cast<LittleFS1 *>(config->context);

    try
    {
        instance->_block_device->erase(block);
    }
    catch (std::exception const &)
    {
        return LFS1_ERR_IO;
    }

    return LFS1_ERR_OK;
}

int LittleFS1::_sync(lfs1_config const * config) noexcept
{
    auto * instance = reinterpret_cast<LittleFS1 *>(config->context);

    try
    {
        instance->_block_device->sync();
    }
    catch (std::exception const &)
    {
        return LFS1_ERR_IO;
    }

    return LFS1_ERR_OK;
}
