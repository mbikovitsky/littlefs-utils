#pragma once

#include <cstdint>


class IBlockDevice
{
public:
    virtual ~IBlockDevice() = default;

    virtual void
        read(std::uint32_t block, std::uint32_t offset, void * buffer, std::uint32_t size) = 0;
    virtual void program(std::uint32_t block,
                         std::uint32_t offset,
                         void const * buffer,
                         std::uint32_t size) = 0;
    virtual void erase(std::uint32_t block) = 0;
    virtual void sync() = 0;

    [[nodiscard]] virtual std::uint32_t block_size() const noexcept = 0;
    [[nodiscard]] virtual std::uint32_t block_count() const noexcept = 0;
};
