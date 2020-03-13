#include <cstdint>
#include <exception>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include <docopt/docopt.h>
#include <boost/lexical_cast.hpp>

#if defined(_MSC_VER)
#include "Unicode.hpp"
#endif


struct CommandLineOptions
{
    std::uint32_t version;
    std::uint32_t block_size;
    std::uint32_t read_size;
    std::uint32_t prog_size;
    std::uint32_t filesystem_size;
    std::uint32_t file_offset;
    std::string input_file_path;
    std::string output_file_path;
};

static constexpr char USAGE[] =
    R"(littlefs-extract.

Usage:
  littlefs-extract [-l VERSION] [-b BLOCK_SIZE] [-r READ_SIZE] [-p PROG_SIZE] [--filesystem-size FS_SIZE] [--file-offset OFFSET] -i INPUT [-o OUTPUT_FILE]
  littlefs-extract -h | --help
  littlefs-extract -h | --version

Options:
  -h --help                              Show this screen.
  -v --version                           Show version.
  -l VERSION --littlefs-version=VERSION  littlefs version to use. [default: 2]
  -b BLOCK_SIZE --block-size=BLOCK_SIZE  filesystem block size. [default: 512]
  -r READ_SIZE --read-size=READ_SIZE     filesystem read size. [default: 64]
  -p PROG_SIZE --prog-size=PROG_SIZE     filesystem prog size. [default: 64]
  --filesystem-size=FS_SIZE              filesystem size. [default: 0]
  --file-offset=OFFSET                   offset within the input file where filesystem begins. [default: 0]
  -i INPUT --input-file=INPUT            littlefs image file.
  -o OUTPUT --output-file=OUTPUT         output tar file. [default: -]
)";

std::optional<CommandLineOptions> parse_command_line(std::vector<std::string> const & argv)
{
    auto args = docopt::docopt(USAGE,
                               argv,
                               true,
                               "littlefs-extract 0.1");

    CommandLineOptions options {};

    options.version = boost::lexical_cast<std::uint32_t>(args.at("--littlefs-version").asString());
    options.block_size = boost::lexical_cast<std::uint32_t>(args.at("--block-size").asString());
    options.read_size = boost::lexical_cast<std::uint32_t>(args.at("--read-size").asString());
    options.prog_size = boost::lexical_cast<std::uint32_t>(args.at("--prog-size").asString());
    options.filesystem_size = boost::lexical_cast<std::uint32_t>(args.at("--filesystem-size").asString());
    options.file_offset = boost::lexical_cast<std::uint32_t>(args.at("--file-offset").asString());
    options.input_file_path = args.at("--input-file").asString();
    options.output_file_path = args.at("--output-file").asString();

    return options;
}

#if defined(_MSC_VER)
int wmain(int argc, wchar_t ** argv)
#else
int main(int argc, char ** argv)
#endif
{
#if defined(_MSC_VER)
    std::vector<std::string> arguments{};
    arguments.reserve(argc - 1);
    for (int index = 1; index < argc; ++index)
    {
        arguments.push_back(wide_char_to_utf8(argv[index]));
    }
#else
    std::vector<std::string> arguments { argv + 1, argv + argc };
#endif

    try
    {
        auto const options = parse_command_line(arguments);
        if (!options)
        {
            return 1;
        }
    }
    catch (std::exception const & exception)
    {
        std::cerr << exception.what() << "\n";
        return -1;
    }
}
