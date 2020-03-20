#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include <gsl/gsl>

#include <lfs2.h>

#include "LittleFile.hpp"


class LittleFile2 : public LittleFile
{
private:
    lfs2_t * _filesystem;
    mutable lfs2_file_t _file;
    bool _open;

public:
    LittleFile2(lfs2_t & filesystem, std::string const & path, int flags);

    ~LittleFile2() override;

    LittleFile2(LittleFile2 const &) = delete;
    LittleFile2 & operator=(LittleFile2 const &) = delete;

    std::size_t read(gsl::span<std::byte> buffer) override;

    [[nodiscard]] std::size_t size() const override;
    [[nodiscard]] std::size_t position() const override;
};
