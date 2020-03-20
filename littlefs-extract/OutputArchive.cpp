#include "OutputArchive.hpp"

#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>

#include <archive.h>
#include <archive_entry.h>


namespace {

constexpr std::size_t STREAM_READ_BUFFER_SIZE = 4 * 1024;

std::unique_ptr<archive, decltype(&archive_write_free)> create_archive_object()
{
    auto const object = archive_write_new();
    if (nullptr == object)
    {
        throw std::runtime_error("archive_write_new");
    }
    return {object, &archive_write_free};
}

std::unique_ptr<archive_entry, decltype(&archive_entry_free)> create_archive_entry()
{
    auto const entry = archive_entry_new();
    if (nullptr == entry)
    {
        throw std::runtime_error("archive_entry_new");
    }
    return {entry, &archive_entry_free};
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
                             IInputStream & stream,
                             unsigned short permissions)
{
    auto const stream_size = stream.remaining();

    if (stream_size > std::numeric_limits<la_int64_t>::max())
    {
        throw std::length_error("Stream too large");
    }

    auto const entry = create_archive_entry();

    archive_entry_set_pathname(entry.get(), path.c_str());
    archive_entry_set_size(entry.get(), static_cast<la_int64_t>(stream_size));
    archive_entry_set_filetype(entry.get(), AE_IFREG);
    archive_entry_set_perm(entry.get(), permissions);

    std::vector<std::byte> buffer(STREAM_READ_BUFFER_SIZE);

    auto const header_result = archive_write_header(_archive.get(), entry.get());
    if (header_result < 0)
    {
        throw std::runtime_error(archive_error_string(_archive.get()));
    }

    auto to_read = stream_size;
    while (to_read > 0)
    {
        auto const bytes_read = stream.read(buffer);
        auto const write_result = archive_write_data(_archive.get(), buffer.data(), bytes_read);
        if (write_result < 0)
        {
            throw std::runtime_error(archive_error_string(_archive.get()));
        }
        to_read -= bytes_read;
    }
}
