#pragma once

#include <memory>
#include <vector>

#include <lfs1.h>

#include "IBlockDevice.hpp"
#include "LittleFS.hpp"


class LittleFS1 : public LittleFS
{
private:
    std::unique_ptr<IBlockDevice> _block_device;
    lfs1_config _config;
    lfs1_t _filesystem;
    bool _mounted;

public:
    LittleFS1(std::unique_ptr<IBlockDevice> block_device,
              lfs1_size_t read_size,
              lfs1_size_t program_size,
              lfs1_size_t lookahead = 128);

    ~LittleFS1() override;

    LittleFS1(LittleFS1 const &) = delete;
    LittleFS1 & operator=(LittleFS1 const &) = delete;

    std::vector<LittleFS::DirectoryEntry> list_directory(std::string const & path) override;

private:
    static int _read(lfs1_config const * config,
                     lfs1_block_t block,
                     lfs1_off_t offset,
                     void * buffer,
                     lfs1_size_t size) noexcept;

    static int _prog(lfs1_config const * config,
                     lfs1_block_t block,
                     lfs1_off_t offset,
                     void const * buffer,
                     lfs1_size_t size) noexcept;

    static int _erase(lfs1_config const * config, lfs1_block_t block) noexcept;

    static int _sync(lfs1_config const * config) noexcept;
};
