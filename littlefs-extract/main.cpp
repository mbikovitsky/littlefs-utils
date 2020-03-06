#include <cstdint>
#include <exception>
#include <iostream>
#include <locale>
#include <optional>
#include <string>

#include <boost/program_options.hpp>

#include <lfs1.h>
#include <lfs2.h>

namespace po = boost::program_options;

struct CommandLineOptions
{
    std::uint32_t version;
    std::uint32_t block_size;
    std::uint32_t read_size;
    std::uint32_t prog_size;
    std::uint32_t filesystem_size;
    std::uint32_t file_offset;
    std::wstring input_file_path;
    std::wstring output_file_path;
};

#if defined(_MSC_VER)
std::optional<CommandLineOptions> parse_command_line(int argc, wchar_t ** argv)
#else
std::optional<CommandLineOptions> parse_command_line(int argc, char ** argv)
#endif
{
    CommandLineOptions options {};

    try
    {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("littlefs-version,l",
                    po::value(&options.version)->required(),
                    "littlefs version to use")
            ("block-size,b",
                    po::value(&options.block_size)->required(),
                    "filesystem block size")
            ("read-size,r",
                    po::value(&options.read_size)->required(),
                    "filesystem read size")
            ("prog-size,p",
                    po::value(&options.prog_size)->required(),
                    "filesystem prog size")
            ("filesystem-size",
                    po::value(&options.filesystem_size)->default_value(0),
                    "filesystem size (default is input file size)")
            ("file-offset",
                    po::value(&options.file_offset)->default_value(0),
                    "offset within the input file where filesystem begins")
            ("input-file,i",
                    po::wvalue(&options.input_file_path)->required(),
                    "littlefs image file")
            ("output-file,o",
                    po::wvalue(&options.output_file_path)->required(),
                    "output tar file")
        ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help"))
        {
            std::cout << desc << "\n";
            return {};
        }

        po::notify(vm);
    }
    catch (std::exception const & exception)
    {
        std::cerr << exception.what() << "\n";
        return {};
    }

    return options;
}

#if defined(_MSC_VER)
int wmain(int argc, wchar_t ** argv)
#else
int main(int argc, char ** argv)
#endif
{
    std::locale::global(std::locale(""));

    auto const options = parse_command_line(argc, argv);
    if (!options)
    {
        return 1;
    }
}
