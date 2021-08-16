/* Copyright (C) 2021 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "BSAType.hpp"
#include "FileTypes.hpp"
#include "btu/bsa/Games.hpp"
#include "common.hpp"

namespace btu::bsa {
class FilePath
{
    static inline const string suffixSeparator = bethutil_bsa_STR(" - ");

public:
    FilePath(path dir, string name, string suffix, path ext, FileTypes type);
    static std::optional<FilePath> make(path const &path, GameSettings const &sets, FileTypes type);

    path fullPath() const;
    path fullName() const;

    path dir_;
    string name_;
    string suffix_;
    path ext_;
    std::optional<uint8_t> counter_;
    FileTypes type_{};

private:
    static std::optional<int> eatDigits(string &str);
    static string eatSuffix(string &str, const GameSettings &sets);

    explicit FilePath() = default;
};

FilePath findBSAName(path const &folderPath, GameSettings const &sets, BSAType type);

void cleanDummyPlugins(path const &folderPath, GameSettings const &sets);
void makeDummyPlugins(path const &folderPath, GameSettings const &sets);
} // namespace btu::bsa
