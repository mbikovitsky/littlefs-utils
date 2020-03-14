#pragma once

#include <cstdint>
#include <fstream>
#include <string>

#include <IBlockDevice.hpp>


class FileBlockDevice : public IBlockDevice
{
private:
    std::fstream _filestream;
    std::uint32_t _block_size;
    std::uint32_t _block_count;

public:
    FileBlockDevice(std::string const & path,
                    bool writable,
                    std::uint32_t block_size,
                    std::uint32_t block_count);
    virtual ~FileBlockDevice() = default;

    FileBlockDevice(FileBlockDevice const &) = delete;
    FileBlockDevice & operator=(FileBlockDevice const &) = delete;

    void
        read(std::uint32_t block, std::uint32_t offset, void * buffer, std::uint32_t size) override;
    void program(std::uint32_t block,
                 std::uint32_t offset,
                 void const * buffer,
                 std::uint32_t size) override;
    void erase(std::uint32_t block) override;
    void sync() override;

    [[nodiscard]] std::uint32_t block_size() const noexcept override
    {
        return _block_size;
    }

    [[nodiscard]] std::uint32_t block_count() const noexcept override
    {
        return _block_count;
    }
};
