#pragma once

#ifdef _MSC_VER // Only implementation that uses wchar for path::string_type
#define bethutil_bsa_STR(x) L##x
#else
#define bethutil_bsa_STR(x) x
#endif

#include "btu/bsa/fo4.hpp"
#include "btu/bsa/tes3.hpp"
#include "btu/bsa/tes4.hpp"

#include <filesystem>

namespace btu::bsa {
namespace fs = std::filesystem;
using fs::path;
using string = path::string_type;
using charT  = string::value_type;
} // namespace btu::bsa
