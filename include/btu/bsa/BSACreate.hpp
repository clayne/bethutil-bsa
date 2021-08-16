/* Copyright (C) 2021 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "detail/Algorithms.hpp"
#include "detail/BSA.hpp"
#include "detail/BSASplit.hpp"
#include "detail/archive.hpp"

#include <fstream>
#include <ranges>

namespace btu::bsa {
template<class UnaryFunction>
void for_each_file(const std::filesystem::path &a_root, UnaryFunction a_func)
{
    auto it = std::filesystem::recursive_directory_iterator(a_root);
    for (const auto &entry : it | std::ranges::views::filter([](auto &&e) { return e.is_regular_file(); }))
    {
        a_func(entry.path());
    }
}

template<class... Keys>
[[nodiscard]] auto open_virtual_path(const std::filesystem::path &a_root,
                                     const std::filesystem::path &relative) -> std::ofstream
{
    const auto path = a_root / relative;
    std::filesystem::create_directories(path.parent_path());
    std::ofstream out{path, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc};
    out.exceptions(std::ios_base::failbit);
    return out;
}

inline void create(path const &dir, BSA bsa, bool allowCompression, GameSettings const &sets)
{
    bsa.name(dir, sets);
    bool const compressed = allowCompression && bsa.type_ != BSAType::Incompressible;

    archive arch(bsa.format_, compressed);
    for_each_file(dir, [&](const std::filesystem::path &path) { arch.add_file(dir, path); });
    arch.write(bsa.path_);
}

} // namespace BethUtil::BSA
