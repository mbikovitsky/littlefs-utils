#include "OutputArchive.hpp"

#include <memory>
#include <stdexcept>
#include <utility>

#include <archive.h>


namespace {

std::unique_ptr<archive, decltype(&archive_write_free)> create_archive_object()
{
    auto const object = archive_write_new();
    if (nullptr == object)
    {
        throw std::runtime_error("archive_write_new");
    }
    return {object, &archive_write_free};
}

}  // namespace


OutputArchive::OutputArchive(CFile file, int format) :
    _file(std::move(file)),
    _archive(create_archive_object())
{
    auto result = archive_write_set_format(_archive.get(), format);
    if (result < 0)
    {
        throw std::runtime_error(archive_error_string(_archive.get()));
    }

    result = archive_write_open_FILE(_archive.get(), _file.handle());
    if (result < 0)
    {
        throw std::runtime_error(archive_error_string(_archive.get()));
    }
}
