#include <cstdint>
#include <exception>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <boost/program_options.hpp>
#include <fmt/core.h>
#include <gsl/gsl>

#include "CFile.hpp"
#include "FileBlockDevice.hpp"
#include "LittleFileInputStream.hpp"
#include "OutputArchive.hpp"
#include "Util.hpp"

#if defined(_MSC_VER)
    #include "Unicode.hpp"
#endif

#include <littlefs_extract_config.h>

#include <LittleFS1.hpp>
#include <LittleFS2.hpp>


struct CommandLineOptions
{
    std::uint32_t version;
    std::uint32_t block_size;
    std::uint32_t read_size;
    std::uint32_t prog_size;
    std::string input_file_path;
    std::string output_file_path;
};


namespace po = boost::program_options;


static constexpr int TAR_FILE_PERMISSIONS = 0644;


std::optional<CommandLineOptions> parse_command_line(std::string const & executable,
                                                     std::vector<std::string> const & args)
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("version,v", "show version")
        ("littlefs-version,l", po::value<std::uint32_t>()->default_value(LITTLEFS_EXTRACT_DEFAULT_VERSION), "littlefs version to use")
        ("block-size,b", po::value<std::uint32_t>()->default_value(LITTLEFS_EXTRACT_DEFAULT_BLOCK_SIZE), "filesystem block size")
        ("read-size,r", po::value<std::uint32_t>()->default_value(LITTLEFS_EXTRACT_DEFAULT_READ_SIZE), "filesystem read size")
        ("prog-size,p", po::value<std::uint32_t>()->default_value(LITTLEFS_EXTRACT_DEFAULT_PROG_SIZE), "filesystem prog size")
        ("input-file,i", po::value<std::string>()->required(), "littlefs image file")
        ("output-file,o", po::value<std::string>()->default_value("-"), "output tar file")
    ;

    po::variables_map vm {};
    po::store(po::basic_command_line_parser(args).options(desc).run(), vm);

    if (args.empty() || 0 != vm.count("help"))
    {
        auto const & usage =
            fmt::format("Usage: {} -i INPUT_FILE [-l LITTLEFS_VERSION] [-b BLOCK_SIZE] "
                        "[-r READ_SIZE] [-p PROG_SIZE] [-o OUTPUT_FILE]\n",
                        executable);

#if _MSC_VER
        std::wcout << utf8_to_wide_char(usage);
#else
        std::cout << usage;
#endif

        std::cout << desc << "\n";
        return {};
    }
    if (0 != vm.count("version"))
    {
        std::cout << "littlefs-extract v0.1\n";
        return {};
    }

    po::notify(vm);

    CommandLineOptions options {};

    options.version = vm["littlefs-version"].as<std::uint32_t>();
    options.block_size = vm["block-size"].as<std::uint32_t>();
    options.read_size = vm["read-size"].as<std::uint32_t>();
    options.prog_size = vm["prog-size"].as<std::uint32_t>();
    options.input_file_path = vm["input-file"].as<std::string>();
    options.output_file_path = vm["output-file"].as<std::string>();

    return options;
}

int entry_point(std::string const & executable, std::vector<std::string> const & args)
{
    auto const options = parse_command_line(executable, args);
    if (!options)
    {
        return 1;
    }

    auto const image_size = file_size(options->input_file_path);
    if (image_size % options->block_size != 0)
    {
        throw std::runtime_error("Invalid block size");
    }

    auto const block_count = image_size / options->block_size;

    if (block_count > std::numeric_limits<std::uint32_t>::max())
    {
        throw std::runtime_error("Image too large");
    }

    auto image_file = std::make_unique<FileBlockDevice>(options->input_file_path,
                                                        false,
                                                        options->block_size,
                                                        static_cast<std::uint32_t>(block_count));

    std::unique_ptr<LittleFS> filesystem {};
    switch (options->version)
    {
    case 1:
        filesystem = std::make_unique<LittleFS1>(std::move(image_file),
                                                 options->read_size,
                                                 options->prog_size);
        break;

    case 2:
        filesystem = std::make_unique<LittleFS2>(std::move(image_file),
                                                 options->read_size,
                                                 options->prog_size);
        break;

    default:
        throw std::runtime_error("Invalid littlefs version specified");
    }

    CFile output_file = options->output_file_path == "-" ? CFile::standard_output()
                                                         : CFile(options->output_file_path, "w");

    // NOLINTNEXTLINE(hicpp-signed-bitwise): There are unsigned literals
    OutputArchive archive(std::move(output_file), ARCHIVE_FORMAT_TAR_PAX_RESTRICTED);

    for (auto const & file_info : filesystem->recursive_dirlist("/"))
    {
        auto const stream = std::make_unique<LittleFileInputStream>(
            filesystem->open_file(file_info.path, LittleFS::OpenFlags::Read));
        archive.add_file(file_info.path.substr(1), *stream, TAR_FILE_PERMISSIONS);
    }

    return 0;
}

#if defined(_MSC_VER)
int wmain(int argc, wchar_t ** argv) noexcept
#else
int main(int argc, char ** argv) noexcept
#endif
{
    try
    {
#if defined(_MSC_VER)
        std::vector<std::string> arguments {};
        arguments.reserve(argc - 1);
        for (int index = 1; index < argc; ++index)
        {
            arguments.push_back(wide_char_to_utf8(argv[index]));
        }

        std::string const executable(wide_char_to_utf8(argv[0]));
#else
        auto const arguments_span = gsl::span<char *>(argv, argc).subspan(1);
        std::vector<std::string> const arguments(arguments_span.cbegin(), arguments_span.cend());

        std::string const executable(argv[0]);
#endif

        return entry_point(executable, arguments);
    }
    catch (std::exception const & exception)
    {
        std::cerr << exception.what() << "\n";
        return -1;
    }
}
