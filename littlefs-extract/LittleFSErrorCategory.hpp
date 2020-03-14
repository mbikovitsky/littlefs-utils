#pragma once

#include <system_error>

#include <fmt/core.h>

#include <lfs1.h>
#include <lfs2.h>


class LittleFSErrorCategory : public std::error_category
{
public:
    [[nodiscard]] char const * name() const noexcept override
    {
        return "littlefs";
    }

    [[nodiscard]] std::string message(int condition) const noexcept override
    {
        switch (condition)
        {
        case LFS1_ERR_IO:
            return "Error during device operation";

        case LFS1_ERR_CORRUPT:
        case LFS2_ERR_CORRUPT:
            return "Corrupted";

        case LFS1_ERR_NOENT:
            return "No directory entry";

        case LFS1_ERR_EXIST:
            return "Entry already exists";

        case LFS1_ERR_NOTDIR:
            return "Entry is not a dir";

        case LFS1_ERR_ISDIR:
            return "Entry is a dir";

        case LFS1_ERR_NOTEMPTY:
            return "Dir is not empty";

        case LFS1_ERR_BADF:
            return "Bad file number";

        case LFS1_ERR_FBIG:
            return "File too large";

        case LFS1_ERR_INVAL:
            return "Invalid parameter";

        case LFS1_ERR_NOSPC:
            return "No space left on device";

        case LFS1_ERR_NOMEM:
            return "No more memory available";

        case LFS2_ERR_NOATTR:
            return "No data/attr available";

        case LFS2_ERR_NAMETOOLONG:
            return "File name too long";

        default:
            return fmt::format("Error: {}", condition);
        }
    }
};

[[nodiscard]] inline std::error_category const & littlefs_category() noexcept
{
    static LittleFSErrorCategory category {};
    return category;
}
