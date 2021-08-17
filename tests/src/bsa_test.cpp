#include <btu/bsa/archive_data.hpp>

#include <string>

auto main() -> int
{
    exported_class e;

    return std::string("bsa") == e.name() ? 0 : 1;
}
