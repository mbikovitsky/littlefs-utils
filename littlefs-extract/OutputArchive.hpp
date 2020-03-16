#pragma once

#include <memory>

#include <archive.h>

#include "CFile.hpp"


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
};
