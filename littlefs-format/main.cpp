#include <algorithm>
#include <cstdint>
#include <exception>
#include <iostream>
#include <iterator>
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

#include <FileBlockDevice.hpp>
#include <Util.hpp>

#if defined(_MSC_VER)
    #include <Unicode.hpp>
#endif

#include <littlefs_format_config.h>
#include <littlefs_utils_config.h>

#include <LittleFS1.hpp>
#include <LittleFS2.hpp>


struct CommandLineOptions
{
    std::uint32_t version;
    std::uint32_t block_size;
    std::optional<std::uint32_t> block_count;
    std::uint32_t read_size;
    std::uint32_t prog_size;
    std::string input_file_path;
};


namespace po = boost::program_options;


std::optional<CommandLineOptions> parse_command_line(std::string const & executable,
                                                     std::vector<std::string> const & args)
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("version,v", "show version")
        ("littlefs-version,l", po::value<std::uint32_t>()->default_value(LITTLEFS_FORMAT_DEFAULT_VERSION), "littlefs version to use")
        ("block-size,b", po::value<std::uint32_t>()->default_value(LITTLEFS_FORMAT_DEFAULT_BLOCK_SIZE), "filesystem block size")
        ("block-count,c", po::value<std::uint32_t>(), "filesystem block count")
        ("read-size,r", po::value<std::uint32_t>()->default_value(LITTLEFS_FORMAT_DEFAULT_READ_SIZE), "filesystem read size")
        ("prog-size,p", po::value<std::uint32_t>()->default_value(LITTLEFS_FORMAT_DEFAULT_PROG_SIZE), "filesystem prog size")
        ("input-file,i", po::value<std::string>()->required(), "littlefs image file")
    ;

    po::variables_map vm {};
    po::store(po::basic_command_line_parser(args).options(desc).run(), vm);

    if (args.empty() || 0 != vm.count("help"))
    {
        auto const & usage =
            fmt::format("Usage: {} -i INPUT_FILE [-l LITTLEFS_VERSION] [-b BLOCK_SIZE] "
                        "[-c BLOCK_COUNT] [-r READ_SIZE] [-p PROG_SIZE]\n",
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
        std::cout << fmt::format("littlefs-extract {}\n", LITTLEFS_UTILS_VERSION);
        return {};
    }

    po::notify(vm);

    CommandLineOptions options {};

    options.version = vm["littlefs-version"].as<std::uint32_t>();
    options.block_size = vm["block-size"].as<std::uint32_t>();
    options.read_size = vm["read-size"].as<std::uint32_t>();
    options.prog_size = vm["prog-size"].as<std::uint32_t>();
    options.input_file_path = vm["input-file"].as<std::string>();

    if (0 != vm.count("block-count"))
    {
        options.block_count = vm["block-count"].as<std::uint32_t>();
    }

    return options;
}

int entry_point(std::string const & executable, std::vector<std::string> const & args)
{
    auto options = parse_command_line(executable, args);
    if (!options)
    {
        return 1;
    }

    if (!options->block_count)
    {
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

        options->block_count = static_cast<std::uint32_t>(block_count);
    }

    auto image_file = FileBlockDevice(options->input_file_path,
                                      true,
                                      options->block_size,
                                      options->block_count.value());

    std::unique_ptr<LittleFS> filesystem {};
    switch (options->version)
    {
    case 1:
        LittleFS1::format(image_file, options->read_size, options->prog_size);
        break;

    case 2:
        LittleFS2::format(image_file, options->read_size, options->prog_size);
        break;

    default:
        throw std::runtime_error("Invalid littlefs version specified");
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
        gsl::span<wchar_t *> argv_span(argv, argc);

        auto const arguments_span = argv_span.subspan(1);

        std::vector<std::string> arguments {};
        arguments.reserve(arguments_span.size());
        std::transform(std::begin(arguments_span),
                       std::end(arguments_span),
                       std::back_inserter(arguments),
                       wide_char_to_utf8);

        std::string const executable(wide_char_to_utf8(argv_span.at(0)));
#else
        gsl::span<char *> argv_span(argv, argc);

        auto const arguments_span = argv_span.subspan(1);
        std::vector<std::string> const arguments(arguments_span.cbegin(), arguments_span.cend());

        std::string const executable(argv_span.at(0));
#endif

        return entry_point(executable, arguments);
    }
    catch (std::exception const & exception)
    {
        std::cerr << exception.what() << "\n";
        return -1;
    }
}
