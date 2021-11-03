#pragma once

#include <cstddef>

#include <gsl/gsl>


class IInputStream
{
public:
    virtual ~IInputStream() = default;

    virtual std::size_t read(gsl::span<std::byte> buffer) = 0;

    [[nodiscard]] virtual std::size_t remaining() const = 0;
};
