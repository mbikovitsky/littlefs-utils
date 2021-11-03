#include "Unicode.hpp"

#include <stdexcept>
#include <vector>

#include <Windows.h>


std::string wide_char_to_utf8(std::wstring const & string)
{
    auto const required_size = WideCharToMultiByte(CP_UTF8,
                                                   WC_ERR_INVALID_CHARS,
                                                   string.c_str(),
                                                   -1,
                                                   nullptr,
                                                   0,
                                                   nullptr,
                                                   nullptr);
    if (0 == required_size)
    {
        throw std::runtime_error("WideCharToMultiByte");
    }

    std::vector<char> buffer(required_size);

    auto const written = WideCharToMultiByte(CP_UTF8,
                                             WC_ERR_INVALID_CHARS,
                                             string.c_str(),
                                             -1,
                                             buffer.data(),
                                             static_cast<int>(buffer.size() * sizeof(buffer[0])),
                                             nullptr,
                                             nullptr);
    if (0 == written)
    {
        throw std::runtime_error("WideCharToMultiByte");
    }

    return buffer.data();
}

std::wstring utf8_to_wide_char(std::string const & string)
{
    auto const required_length =
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, string.c_str(), -1, nullptr, 0);
    if (0 == required_length)
    {
        throw std::runtime_error("MultiByteToWideChar");
    }

    std::vector<wchar_t> buffer(required_length);

    auto const written = MultiByteToWideChar(CP_UTF8,
                                             MB_ERR_INVALID_CHARS,
                                             string.c_str(),
                                             -1,
                                             buffer.data(),
                                             static_cast<int>(buffer.size()));
    if (0 == written)
    {
        throw std::runtime_error("WideCharToMultiByte");
    }

    return buffer.data();
}
