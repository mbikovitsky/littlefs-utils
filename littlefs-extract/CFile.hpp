#pragma once

#include <cstdio>
#include <string>
#include <type_traits>
#include <memory>

#include <gsl/gsl>


class CFile final
{
private:
    std::unique_ptr<std::FILE, decltype(&std::fclose)> _handle;

public:
    CFile(std::string const & path, std::string const & mode);

    template <typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
    std::size_t read(gsl::span<T> buffer) noexcept
    {
        return std::fread(buffer.data(), sizeof(T), buffer.size(), _handle);
    }

    template <typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
    std::size_t write(gsl::span<T> buffer) noexcept
    {
        return std::fwrite(buffer.data(), sizeof(T), buffer.size(), _handle);
    }

    [[nodiscard]] std::FILE * handle() const noexcept;

    [[nodiscard]] static CFile standard_input();
    [[nodiscard]] static CFile standard_output();
    [[nodiscard]] static CFile standard_error();

private:
    explicit CFile(std::unique_ptr<std::FILE, decltype(&std::fclose)> handle);
};
