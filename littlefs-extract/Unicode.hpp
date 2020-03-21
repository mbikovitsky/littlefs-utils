#pragma once

#include <string>


std::string wide_char_to_utf8(std::wstring const & string);
std::wstring utf8_to_wide_char(std::string const & string);
