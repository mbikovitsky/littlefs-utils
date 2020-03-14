#pragma once

#include <string>
#include <system_error>


class LittleFSErrorCategory : public std::error_category
{
public:
    [[nodiscard]] char const * name() const noexcept override;

    [[nodiscard]] std::string message(int condition) const noexcept override;
};

[[nodiscard]] std::error_category const & littlefs_category() noexcept;
