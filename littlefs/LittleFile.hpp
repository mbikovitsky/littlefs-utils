#pragma once

#include <cstddef>
#include <vector>

#include <gsl/gsl>


class LittleFile
{
public:
    virtual ~LittleFile() = default;

    virtual std::size_t read(gsl::span<std::byte> buffer) = 0;
    virtual std::size_t write(gsl::span<std::byte const> buffer) = 0;

    [[nodiscard]] virtual std::size_t size() const = 0;
    [[nodiscard]] virtual std::size_t position() const = 0;
};
