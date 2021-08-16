/* Copyright (C) 2021 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Games.hpp"
#include "bsa/fo4.hpp"
#include "bsa/tes3.hpp"
#include "bsa/tes4.hpp"
#include "detail/Algorithms.hpp"

#include <fstream>

namespace btu::bsa {
[[nodiscard]] auto open_virtual_path(const path &path) -> std::ofstream
{
    std::filesystem::create_directories(path.parent_path());
    std::ofstream out{path, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc};
    out.exceptions(std::ios_base::failbit);
    return out;
}

inline void extract(path const &filePath, bool removeBSA = false, bool overwriteExistingFiles = false)
{
    archive arch(filePath);
    const auto root = filePath.parent_path();
    arch.iterate_files([&root, overwriteExistingFiles](const fs::path &rel, std::span<const std::byte> data) {
        const auto path = root / rel;
        if (fs::exists(path) && !overwriteExistingFiles)
            return;

        auto out = open_virtual_path(path);
        out.write(reinterpret_cast<const char *>(data.data()), data.size());
    });

    if (removeBSA && !fs::remove(filePath))
    {
        throw std::runtime_error("BSA Extract succeeded but failed to delete the extracted BSA");
    }
}

inline void extractAll(path const &dirPath, GameSettings const &sets)
{
    std::vector files(fs::directory_iterator(dirPath), fs::directory_iterator{});
    erase_if(files, [&sets](auto const &file) { return file.path().extension() != sets.extension; });
    std::for_each(files.begin(), files.end(), [](auto const &file) { btu::bsa::extract(file.path()); });
}
} // namespace BethUtil::BSA
