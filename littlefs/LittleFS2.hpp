#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include <lfs2.h>

#include "IBlockDevice.hpp"
#include "LittleFS.hpp"


class LittleFS2 : public LittleFS
{
private:
    std::unique_ptr<IBlockDevice> _block_device;
    lfs2_config _config;
    lfs2_t _filesystem;
    bool _mounted;

public:
    LittleFS2(std::unique_ptr<IBlockDevice> block_device,
              lfs2_size_t read_size,
              lfs2_size_t program_size,
              std::int32_t block_cycles = 100,
              lfs2_size_t cache_size = 0,
              lfs2_size_t lookahead_size = 128,
              lfs2_size_t name_max = LFS2_NAME_MAX,
              lfs2_size_t file_max = LFS2_FILE_MAX,
              lfs2_size_t attr_max = LFS2_ATTR_MAX);

    ~LittleFS2() override;

    LittleFS2(LittleFS2 const &) = delete;
    LittleFS2 & operator=(LittleFS2 const &) = delete;

    std::vector<LittleFS::DirectoryEntry> list_directory(std::string const & path) override;
    std::unique_ptr<LittleFile> open_file(std::string const & path, OpenFlags flags) override;

    static void format(IBlockDevice & block_device,
                       lfs2_size_t read_size,
                       lfs2_size_t program_size,
                       std::int32_t block_cycles = 100,
                       lfs2_size_t cache_size = 0,
                       lfs2_size_t lookahead_size = 128,
                       lfs2_size_t name_max = LFS2_NAME_MAX,
                       lfs2_size_t file_max = LFS2_FILE_MAX,
                       lfs2_size_t attr_max = LFS2_ATTR_MAX);

private:
    static int _read(lfs2_config const * config,
                     lfs2_block_t block,
                     lfs2_off_t offset,
                     void * buffer,
                     lfs2_size_t size) noexcept;

    static int _prog(lfs2_config const * config,
                     lfs2_block_t block,
                     lfs2_off_t offset,
                     void const * buffer,
                     lfs2_size_t size) noexcept;

    static int _erase(lfs2_config const * config, lfs2_block_t block) noexcept;

    static int _sync(lfs2_config const * config) noexcept;
};
