#pragma once

#include <cstddef>
#include <vector>

#include <gsl/gsl>


class LittleFile
{
public:
    virtual ~LittleFile() = default;

    virtual std::size_t read(gsl::span<std::byte> buffer) = 0;
    [[nodiscard]] virtual std::size_t size() const = 0;

    std::vector<std::byte> read_to_end();
};
