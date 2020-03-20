#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include <gsl/gsl>

#include <lfs1.h>

#include "LittleFile.hpp"


class LittleFile1 : public LittleFile
{
private:
    lfs1_t * _filesystem;
    mutable lfs1_file_t _file;
    bool _open;

public:
    LittleFile1(lfs1_t & filesystem, std::string const & path, int flags);

    ~LittleFile1() override;

    LittleFile1(LittleFile1 const &) = delete;
    LittleFile1 & operator=(LittleFile1 const &) = delete;

    std::size_t read(gsl::span<std::byte> buffer) override;
    [[nodiscard]] std::size_t size() const override;
};
