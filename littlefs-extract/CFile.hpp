#pragma once

#include <cstdio>
#include <type_traits>

#include <gsl/gsl>


class CFile
{
private:
    gsl::owner<std::FILE *> _handle;

public:
    explicit CFile(gsl::not_null<std::FILE *> handle);

    virtual ~CFile();

    CFile(CFile const &) = delete;
    CFile & operator=(CFile const &) = delete;

    CFile(CFile && other) noexcept;
    CFile & operator=(CFile && other) noexcept;

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

    static CFile standard_input();
    static CFile standard_output();
    static CFile standard_error();

private:
    [[nodiscard]] gsl::owner<std::FILE *> _release() noexcept;
    void _destroy() noexcept;
};
