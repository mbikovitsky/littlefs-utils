#pragma once

#include <cstddef>
#include <memory>
#include <utility>

#include <gsl/gsl>

#include <LittleFile.hpp>

#include "IInputStream.hpp"


class LittleFileInputStream : public IInputStream
{
private:
    std::unique_ptr<LittleFile> _file;

public:
    explicit LittleFileInputStream(std::unique_ptr<LittleFile> file) : _file(std::move(file))
    {
    }

    std::size_t read(gsl::span<std::byte> buffer) override
    {
        return _file->read(buffer);
    }

    [[nodiscard]] std::size_t remaining() const override
    {
        return _file->size() - _file->position();
    }
};
