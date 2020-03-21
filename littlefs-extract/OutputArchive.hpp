#pragma once

#include <cstddef>
#include <memory>

#include <archive.h>
#include <gsl/gsl>

#include "CFile.hpp"
#include "IInputStream.hpp"


class OutputArchive
{
private:
    CFile _file;
    std::unique_ptr<archive, decltype(&archive_write_free)> _archive;

public:
    OutputArchive(CFile file, int format);

    virtual ~OutputArchive() = default;

    OutputArchive(OutputArchive const &) = delete;
    OutputArchive & operator=(OutputArchive const &) = delete;

    void add_file(std::string const & path, IInputStream & stream, unsigned short permissions);
};
