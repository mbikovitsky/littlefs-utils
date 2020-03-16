#include "OutputArchive.hpp"

#include <memory>
#include <stdexcept>
#include <utility>

#include <archive.h>
#include <archive_entry.h>


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
void OutputArchive::add_file(std::string const & path,
                             gsl::span<std::byte const> buffer,
                             unsigned short permissions)
{
    auto * const entry = archive_entry_new();
    if (nullptr == entry)
    {
        throw std::runtime_error("archive_entry_new");
    }

    try
    {
        archive_entry_set_pathname(entry, path.c_str());
        archive_entry_set_size(entry, buffer.size());
        archive_entry_set_filetype(entry, AE_IFREG);
        archive_entry_set_perm(entry, permissions);

        auto const header_result = archive_write_header(_archive.get(), entry);
        if (header_result < 0)
        {
            throw std::runtime_error(archive_error_string(_archive.get()));
        }

        auto const write_result = archive_write_data(_archive.get(),
                                                     buffer.data(),
                                                     static_cast<std::size_t>(buffer.size()));
        if (write_result < 0)
        {
            throw std::runtime_error(archive_error_string(_archive.get()));
        }
    }
    catch (...)
    {
        archive_entry_free(entry);
    }
    archive_entry_free(entry);
}
