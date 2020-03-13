#ifndef UNICODE_HPP
#define UNICODE_HPP

#include <string>


std::string wide_char_to_utf8(std::wstring const & string);
std::wstring utf8_to_wide_char(std::string const & string);

#endif  // UNICODE_HPP
