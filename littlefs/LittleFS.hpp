#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "IBlockDevice.hpp"

#include "LittleFSErrorCategory.hpp"


class LittleFS
{
public:
    struct FileInfo
    {
        std::string name {};
        bool is_directory {};
        std::uint32_t size {};
    };

public:
    virtual ~LittleFS() = default;

    virtual std::vector<FileInfo> list_directory(std::string const & path) = 0;
};
