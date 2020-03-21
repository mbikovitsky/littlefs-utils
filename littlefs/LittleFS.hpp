#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "IBlockDevice.hpp"

#include "LittleFSErrorCategory.hpp"
#include "LittleFile.hpp"


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

    enum class OpenFlags : std::uint32_t
    {
        Read = 1,
        Write = 2,
        CreateIfNotExists = 0x0100,
        FailIfExists = 0x0200,
        Truncate = 0x0400,
        Append = 0x0800,
    };

public:
    virtual ~LittleFS() = default;

    virtual std::vector<DirectoryEntry> list_directory(std::string const & path) = 0;
    virtual std::unique_ptr<LittleFile> open_file(std::string const & path, OpenFlags flags) = 0;

    std::vector<FileInfo> recursive_dirlist(std::string const & path);
};

constexpr LittleFS::OpenFlags operator|(LittleFS::OpenFlags const first,
                                        LittleFS::OpenFlags const second)
{
    using EnumType = std::underlying_type_t<LittleFS::OpenFlags>;

    return static_cast<LittleFS::OpenFlags>(static_cast<EnumType>(first)
                                            | static_cast<EnumType>(second));
}
