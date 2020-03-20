#include "LittleFile.hpp"


std::vector<std::byte> LittleFile::read_to_end()
{
    std::vector<std::byte> buffer(size());
    auto const returned = read(buffer);
    buffer.resize(returned);
    return buffer;
}
