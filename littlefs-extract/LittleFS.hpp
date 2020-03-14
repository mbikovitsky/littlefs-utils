#pragma once

#include <exception>
#include <memory>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>

#include <lfs1.h>
#include <lfs2.h>

#include "IBlockDevice.hpp"

#include "LittleFSErrorCategory.hpp"


struct LittleFS1Traits
{
    using ConfigurationType = lfs1_config;
    using FileSystemType = lfs1_t;
    using SizeType = lfs1_size_t;
    using BlockType = lfs1_block_t;
    using OffsetType = lfs1_off_t;

    static int mount(lfs1_t * lfs1, const struct lfs1_config * config)
    {
        return lfs1_mount(lfs1, config);
    }

    static int unmount(lfs1_t * lfs1)
    {
        return lfs1_unmount(lfs1);
    }
};

struct LittleFS2Traits
{
    using ConfigurationType = lfs2_config;
    using FileSystemType = lfs2_t;
    using SizeType = lfs2_size_t;
    using BlockType = lfs2_block_t;
    using OffsetType = lfs2_off_t;

    static int mount(lfs2_t * lfs2, const struct lfs2_config * config)
    {
        return lfs2_mount(lfs2, config);
    }

    static int unmount(lfs2_t * lfs2)
    {
        return lfs2_unmount(lfs2);
    }
};

static_assert(static_cast<std::underlying_type_t<decltype(LFS1_ERR_IO)>>(LFS1_ERR_IO)
              == static_cast<std::underlying_type_t<decltype(LFS2_ERR_IO)>>(LFS2_ERR_IO));
static_assert(static_cast<std::underlying_type_t<decltype(LFS1_ERR_OK)>>(LFS1_ERR_OK)
              == static_cast<std::underlying_type_t<decltype(LFS2_ERR_OK)>>(LFS2_ERR_OK));

template <typename Traits>
class LittleFS
{
private:
    std::unique_ptr<IBlockDevice> _block_device;
    typename Traits::ConfigurationType _config;
    typename Traits::FileSystemType _filesystem;
    bool _mounted;

public:
    explicit LittleFS(std::unique_ptr<IBlockDevice> block_device,
                      typename Traits::SizeType read_size,
                      typename Traits::SizeType program_size,
                      typename Traits::SizeType lookahead = 128);

    virtual ~LittleFS();

    LittleFS(LittleFS const &) = delete;
    LittleFS & operator=(LittleFS const &) = delete;

private:
    static int _read(typename Traits::ConfigurationType const * config,
                     typename Traits::BlockType block,
                     typename Traits::OffsetType offset,
                     void * buffer,
                     typename Traits::SizeType size) noexcept;

    static int _prog(typename Traits::ConfigurationType const * config,
                     typename Traits::BlockType block,
                     typename Traits::OffsetType offset,
                     void const * buffer,
                     typename Traits::SizeType size) noexcept;

    static int _erase(typename Traits::ConfigurationType const * config,
                      typename Traits::BlockType block) noexcept;

    static int _sync(typename Traits::ConfigurationType const * config) noexcept;
};

template <typename Traits>
LittleFS<Traits>::LittleFS(std::unique_ptr<IBlockDevice> block_device,
                           typename Traits::SizeType read_size,
                           typename Traits::SizeType program_size,
                           typename Traits::SizeType lookahead) :
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

    auto const result = Traits::mount(&_filesystem, &_config);
    if (0 != result)
    {
        throw std::system_error(result, littlefs_category(), "mount");
    }
    _mounted = true;
}

template <typename Traits>
LittleFS<Traits>::~LittleFS()
{
    if (_mounted)
    {
        Traits::unmount(&_filesystem);
        _mounted = false;
    }
}

template <typename Traits>
int LittleFS<Traits>::_read(typename Traits::ConfigurationType const * config,
                            typename Traits::BlockType block,
                            typename Traits::OffsetType offset,
                            void * buffer,
                            typename Traits::SizeType size) noexcept
{
    auto * instance = reinterpret_cast<LittleFS *>(config->context);

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

template <typename Traits>
int LittleFS<Traits>::_prog(typename Traits::ConfigurationType const * config,
                            typename Traits::BlockType block,
                            typename Traits::OffsetType offset,
                            void const * buffer,
                            typename Traits::SizeType size) noexcept
{
    auto * instance = reinterpret_cast<LittleFS *>(config->context);

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

template <typename Traits>
int LittleFS<Traits>::_erase(typename Traits::ConfigurationType const * config,
                             typename Traits::BlockType block) noexcept
{
    auto * instance = reinterpret_cast<LittleFS *>(config->context);

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

template <typename Traits>
int LittleFS<Traits>::_sync(typename Traits::ConfigurationType const * config) noexcept
{
    auto * instance = reinterpret_cast<LittleFS *>(config->context);

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


using LittleFS1 = LittleFS<LittleFS1Traits>;
using LittleFS2 = LittleFS<LittleFS2Traits>;
