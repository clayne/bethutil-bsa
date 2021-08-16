#pragma once

#include "btu/bsa/Games.hpp"
#include "btu/bsa/detail/Algorithms.hpp"
#include "btu/bsa/detail/common.hpp"

#include <array>

namespace btu::bsa {
enum class FileTypes
{
    Standard,
    Texture,
    Incompressible,
    Blacklist,
    Plugin,
    BSA
};

inline FileTypes getFileType(path const &filepath, path const &root, GameSettings const &sets)
{
    auto check = [ext = to_lower(filepath.extension()), &filepath, &root](auto const &vec) {
        using std::cbegin, std::cend;
        auto it = std::find_if(cbegin(vec), cend(vec), [&ext, &filepath, &root](auto &&val) {
            if constexpr (std::is_same_v<std::decay_t<decltype(val)>, AllowedPath>)
                return val.check(filepath, root);
            else
                return val == ext;
        });
        return it != cend(vec);
    };

    if (check(sets.standardFiles))
        return FileTypes::Standard;
    if (check(sets.textureFiles))
        return FileTypes::Texture;
    if (check(sets.incompressibleFiles))
        return FileTypes::Incompressible;
    if (check(sets.pluginExtensions))
        return FileTypes::Plugin;

    path plugExt[] = {sets.extension};
    if (check(plugExt))
        return FileTypes::BSA;

    return FileTypes::Blacklist;
}

} // namespace BethUtil::BSA
