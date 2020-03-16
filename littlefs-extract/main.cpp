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

#include "FileBlockDevice.hpp"
#include "Util.hpp"

#if defined(_MSC_VER)
    #include "Unicode.hpp"
#endif

#include <LittleFS1.hpp>


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


std::optional<CommandLineOptions> parse_command_line(std::vector<std::string> const & argv)
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("version,v", "show version")
        ("littlefs-version,l", po::value<std::uint32_t>()->default_value(2), "littlefs version to use")
        ("block-size,b", po::value<std::uint32_t>()->default_value(512), "filesystem block size")
        ("read-size,r", po::value<std::uint32_t>()->default_value(64), "filesystem read size")
        ("prog-size,p", po::value<std::uint32_t>()->default_value(64), "filesystem prog size")
        ("input-file,i", po::value<std::string>()->required(), "littlefs image file")
        ("output-file,o", po::value<std::string>()->default_value("-"), "output tar file")
    ;

    po::variables_map vm {};
    po::store(po::basic_command_line_parser(argv).options(desc).run(), vm);

    if (0 != vm.count("help"))
    {
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

int entry_point(std::vector<std::string> const & argv)
{
    auto const options = parse_command_line(argv);
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

    LittleFS1 filesystem(std::move(image_file), options->read_size, options->prog_size);

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
#else
        std::vector<std::string> arguments {argv + 1, argv + argc};
#endif

        return entry_point(arguments);
    }
    catch (std::exception const & exception)
    {
        std::cerr << exception.what() << "\n";
        return -1;
    }
}
