#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "IBlockDevice.hpp"

#include "LittleFSErrorCategory.hpp"


class LittleFS
{
public:
    struct DirectoryEntry
    {
        std::string name {};
        bool is_directory {};
        std::uint32_t size {};
    };

    struct FileInfo
    {
        std::string path {};
        std::uint32_t size {};
    };

public:
    virtual ~LittleFS() = default;

    virtual std::vector<DirectoryEntry> list_directory(std::string const & path) = 0;

    std::vector<FileInfo> recursive_dirlist(std::string const & path);
};
