#include <string>
#include "../common.hpp"

_DLLEXPORT std::string base64_encode(unsigned char const* , unsigned int len);
_DLLEXPORT std::string base64_decode(std::string const& s);
